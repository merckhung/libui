#!/usr/bin/python

# pylint: disable=C0301
"""
Copyright 2014 Google Inc.

Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.

Utilities for accessing Google Cloud Storage, using the boto library (wrapper
for the XML API).

API/library references:
- https://developers.google.com/storage/docs/reference-guide
- http://googlecloudstorage.blogspot.com/2012/09/google-cloud-storage-tutorial-using-boto.html
"""
# pylint: enable=C0301

# System-level imports
import errno
import hashlib
import os
import posixpath
import Queue
import re
import sys
import threading

# Imports from third-party code
TRUNK_DIRECTORY = os.path.abspath(os.path.join(
    os.path.dirname(__file__), os.pardir, os.pardir))
for import_subdir in ['boto']:
  import_dirpath = os.path.join(
      TRUNK_DIRECTORY, 'third_party', 'externals', import_subdir)
  if import_dirpath not in sys.path:
    # We need to insert at the beginning of the path, to make sure that our
    # imported versions are favored over others that might be in the path.
    sys.path.insert(0, import_dirpath)
from boto.exception import BotoServerError
from boto.gs import acl
from boto.gs.bucket import Bucket
from boto.gs.connection import GSConnection
from boto.gs.key import Key
from boto.s3.bucketlistresultset import BucketListResultSet
from boto.s3.connection import SubdomainCallingFormat
from boto.s3.prefix import Prefix

# How many files to upload at once, by default.
# TODO(epoger): Is there a way to compute this intelligently?  To some extent
# it is a function of how many cores are on the machine, and how many other
# processes it is running; but it's probably more a function of how much time
# each core sits idle waiting for network I/O to complete.
DEFAULT_UPLOAD_THREADS = 10

GS_PREFIX = 'gs://'


class AnonymousGSConnection(GSConnection):
  """GSConnection class that allows anonymous connections.

  The GSConnection class constructor in
  https://github.com/boto/boto/blob/develop/boto/gs/connection.py doesn't allow
  for anonymous connections (connections without credentials), so we have to
  override it.
  """
  def __init__(self):
    super(GSConnection, self).__init__(
        # This is the important bit we need to add...
        anon=True,
        # ...and these are just copied in from GSConnection.__init__()
        bucket_class=Bucket,
        calling_format=SubdomainCallingFormat(),
        host=GSConnection.DefaultHost,
        provider='google')


class GSUtils(object):
  """Utilities for accessing Google Cloud Storage, using the boto library."""

  class Permission:
    """Fine-grained permissions that may be set per user/group on each file.

    See SupportedPermissions in
    https://github.com/boto/boto/blob/develop/boto/gs/acl.py
    Also see https://developers.google.com/storage/docs/accesscontrol
    """
    EMPTY = None
    OWNER = 'FULL_CONTROL'
    READ  = 'READ'
    WRITE = 'WRITE'

  class PredefinedACL:
    """Canned ACLs that provide a "base coat" of permissions for each file.

    See CannedACLStrings in
    https://github.com/boto/boto/blob/develop/boto/gs/acl.py
    Also see https://developers.google.com/storage/docs/accesscontrol
    """
    AUTHENTICATED_READ        = 'authenticated-read'
    BUCKET_OWNER_FULL_CONTROL = 'bucket-owner-full-control'
    BUCKET_OWNER_READ         = 'bucket-owner-read'
    PRIVATE                   = 'private'
    PROJECT_PRIVATE           = 'project-private'
    PUBLIC_READ               = 'public-read'
    PUBLIC_READ_WRITE         = 'public-read-write'

  class IdType:
    """Types of identifiers we can use to set "fine-grained" ACLs."""
    GROUP_BY_DOMAIN = acl.GROUP_BY_DOMAIN
    GROUP_BY_EMAIL  = acl.GROUP_BY_EMAIL
    GROUP_BY_ID     = acl.GROUP_BY_ID
    USER_BY_EMAIL   = acl.USER_BY_EMAIL
    USER_BY_ID      = acl.USER_BY_ID

  class UploadIf:
    """Cases in which we will upload a file.

    Beware of performance tradeoffs.  E.g., if you are uploading just one small
    file, the extra round trip to check for file existence and/or checksum may
    take longer than just uploading the file.
    See http://skbug.com/2778 ('gs_utils: when uploading IF_NEW, batch up
    checks for existing files within a single remote directory')
    """
    ALWAYS = 1      # always upload the file
    IF_NEW = 2      # if there is an existing file with the same name,
                    # leave it alone
    IF_MODIFIED = 3 # if there is an existing file with the same name and
                    # contents, leave it alone

  def __init__(self, boto_file_path=None):
    """Constructor.

    Params:
      boto_file_path: full path (local-OS-style) on local disk where .boto
          credentials file can be found.  If None, fall back on the
          AWS_CREDENTIAL_FILE environment variable, then look in a set of
          common paths for the .boto file.  If no .boto file is found, then the
          GSUtils object created will be able to access only public files in
          Google Storage.

    Raises an exception if no file is found at boto_file_path, or if the file
    found there is malformed.
    """
    self._gs_access_key_id = None
    self._gs_secret_access_key = None
    if not boto_file_path:
      if os.environ.get('AWS_CREDENTIAL_FILE'):
        boto_file_path = os.path.expanduser(os.environ['AWS_CREDENTIAL_FILE'])
    if not boto_file_path:
      for path in (os.path.join(os.path.expanduser('~'), '.boto'),):
        if os.path.isfile(path):
          boto_file_path = path
          break

    if boto_file_path:
      print ('Reading boto file from %s' % boto_file_path)
      boto_dict = _config_file_as_dict(filepath=boto_file_path)
      self._gs_access_key_id = boto_dict['gs_access_key_id']
      self._gs_secret_access_key = boto_dict['gs_secret_access_key']
    else:
      print >> sys.stderr, 'Warning: no .boto file found.'

    # Which field we get/set in ACL entries, depending on IdType.
    self._field_by_id_type = {
        self.IdType.GROUP_BY_DOMAIN: 'domain',
        self.IdType.GROUP_BY_EMAIL:  'email_address',
        self.IdType.GROUP_BY_ID:     'id',
        self.IdType.USER_BY_EMAIL:   'email_address',
        self.IdType.USER_BY_ID:      'id',
    }

  def delete_file(self, bucket, path):
    """Delete a single file within a GS bucket.

    TODO(epoger): what if bucket or path does not exist?  Should probably raise
    an exception.  Implement, and add a test to exercise this.

    Params:
      bucket: GS bucket to delete a file from
      path: full path (Posix-style) of the file within the bucket to delete
    """
    b = self._connect_to_bucket(bucket=bucket)
    key = Key(b)
    key.name = path
    try:
      key.delete()
    except BotoServerError, e:
      e.body = (repr(e.body) +
                ' while deleting gs://%s/%s' % (b.name, path))
      raise

  def get_last_modified_time(self, bucket, path):
    """Gets the timestamp of when this file was last modified.

    Params:
      bucket: GS bucket in which to look for the file
      path: full path (Posix-style) of the file within the bucket to check

    Returns the last modified time, as a freeform string.  If the file was not
    found, returns None.
    """
    b = self._connect_to_bucket(bucket=bucket)
    try:
      key = b.get_key(key_name=path)
      if not key:
        return None
      return key.last_modified
    except BotoServerError, e:
      e.body = (repr(e.body) +
                ' while getting attributes of gs://%s/%s' % (b.name, path))
      raise

  def upload_file(self, source_path, dest_bucket, dest_path,
                  upload_if=UploadIf.ALWAYS,
                  predefined_acl=None,
                  fine_grained_acl_list=None):
    """Upload contents of a local file to Google Storage.

    params:
      source_path: full path (local-OS-style) on local disk to read from
      dest_bucket: GS bucket to copy the file to
      dest_path: full path (Posix-style) within that bucket
      upload_if: one of the UploadIf values, describing in which cases we should
          upload the file
      predefined_acl: which predefined ACL to apply to the file on Google
          Storage; must be one of the PredefinedACL values defined above.
          If None, inherits dest_bucket's default object ACL.
      fine_grained_acl_list: list of (id_type, id_value, permission) tuples
          to apply to the uploaded file (on top of the predefined_acl),
          or None if predefined_acl is sufficient

    TODO(epoger): Consider adding a do_compress parameter that would compress
    the file using gzip before upload, and add a "Content-Encoding:gzip" header
    so that HTTP downloads of the file would be unzipped automatically.
    See https://developers.google.com/storage/docs/gsutil/addlhelp/
        WorkingWithObjectMetadata#content-encoding
    """
    b = self._connect_to_bucket(bucket=dest_bucket)
    local_md5 = None  # filled in lazily

    if upload_if == self.UploadIf.IF_NEW:
      old_key = b.get_key(key_name=dest_path)
      if old_key:
        print ('Skipping upload of existing file gs://%s/%s' % (
            b.name, dest_path))
        return
    elif upload_if == self.UploadIf.IF_MODIFIED:
      old_key = b.get_key(key_name=dest_path)
      if old_key:
        if not local_md5:
          local_md5 = _get_local_md5(path=source_path)
        if ('"%s"' % local_md5) == old_key.etag:
          print (
              'Skipping upload of unmodified file gs://%s/%s : %s' % (
                  b.name, dest_path, local_md5))
          return
    elif upload_if != self.UploadIf.ALWAYS:
      raise Exception('unknown value of upload_if: %s' % upload_if)

    # Upload the file using a temporary name at first, in case the transfer
    # is interrupted partway through.
    if not local_md5:
      local_md5 = _get_local_md5(path=source_path)
    initial_key = Key(b)
    initial_key.name = dest_path + '-uploading-' + local_md5
    try:
      initial_key.set_contents_from_filename(filename=source_path,
                                             policy=predefined_acl)
    except BotoServerError, e:
      e.body = (repr(e.body) +
                ' while uploading source_path=%s to gs://%s/%s' % (
                    source_path, b.name, initial_key.name))
      raise

    # Verify that the file contents were uploaded successfully.
    #
    # TODO(epoger): Check whether the boto library or XML API already do this...
    # if so, we may be duplicating effort here, and maybe we don't need to do
    # the whole "upload using temporary filename, then rename" thing.
    #
    # TODO(epoger): Confirm that the etag is set on the server side...
    # otherwise, we may just be validating another MD5 hash that was generated
    # on the client side before the file was uploaded!
    validate_key = b.get_key(key_name=initial_key.name)
    if validate_key.etag != ('"%s"' % local_md5):
      raise Exception('found wrong MD5 after uploading gs://%s/%s' % (
          b.name, validate_key.name))

    # Rename the file to its real name.
    #
    # TODO(epoger): I don't know how long this takes.  I wish we could rename
    # the key instead, but AFAICT you can't do that.
    # Perhaps we could use Key.compose() to create a composite object pointing
    # at the original key?
    # See https://developers.google.com/storage/docs/composite-objects
    final_key = b.copy_key(
        new_key_name=dest_path, src_key_name=initial_key.name,
        src_bucket_name=b.name, preserve_acl=False)
    initial_key.delete()

    # Set ACLs on the file.
    # We do this *after* copy_key(), because copy_key's preserve_acl
    # functionality would incur a performance hit.
    for (id_type, id_value, permission) in fine_grained_acl_list or []:
      self.set_acl(
          bucket=b, path=final_key.name,
          id_type=id_type, id_value=id_value, permission=permission)

  def upload_dir_contents(self, source_dir, dest_bucket, dest_dir,
                          num_threads=DEFAULT_UPLOAD_THREADS,
                          upload_if=UploadIf.ALWAYS, **kwargs):
    """Recursively upload contents of a local directory to Google Storage.

    params:
      source_dir: full path (local-OS-style) on local disk of directory to copy
          contents of
      dest_bucket: GS bucket to copy the files into
      dest_dir: full path (Posix-style) within that bucket; write the files into
          this directory.  If None, write into the root directory of the bucket.
      num_threads: how many files to upload at once
      upload_if: one of the UploadIf values, describing in which cases we should
          upload the file
      kwargs: any additional keyword arguments "inherited" from upload_file()

    The copy operates as a merge: any files in source_dir will be "overlaid" on
    top of the existing content in dest_dir.  Existing files with the same names
    may or may not be overwritten, depending on the value of upload_if.

    TODO(epoger): Upload multiple files simultaneously to reduce latency.
    """
    b = self._connect_to_bucket(bucket=dest_bucket)
    if not dest_dir:
      dest_dir = ''

    # Create a set of all files within source_dir.
    source_fileset = set()
    prefix_length = len(source_dir)+1
    for dirpath, _, filenames in os.walk(source_dir):
      relative_dirpath = dirpath[prefix_length:]
      for filename in filenames:
        source_fileset.add(os.path.join(relative_dirpath, filename))
    num_files_total = len(source_fileset)

    # If we are only uploading files conditionally, remove any unnecessary
    # files from source_fileset.
    if upload_if == self.UploadIf.ALWAYS:
      pass  # there are no shortcuts... upload them all
    else:
      # Create a mapping of filename to Key for existing files within dest_dir
      existing_dest_filemap = {}
      prefix = dest_dir
      if prefix and not prefix.endswith('/'):
        prefix += '/'
      prefix_length = len(prefix)
      items = BucketListResultSet(bucket=b, prefix=prefix)
      for item in items:
        if type(item) is Key:
          existing_dest_filemap[item.name[prefix_length:]] = item

      # Now, depending on upload_if, trim files we should skip uploading.
      files_in_common = source_fileset.intersection(
          existing_dest_filemap.keys())
      if upload_if == self.UploadIf.IF_NEW:
        source_fileset -= files_in_common
      elif upload_if == self.UploadIf.IF_MODIFIED:
        for rel_path in files_in_common:
          local_md5 = '"%s"' % _get_local_md5(path=os.path.join(
              source_dir, rel_path))
          key = existing_dest_filemap[rel_path]
          if local_md5 == key.etag:
            source_fileset.remove(rel_path)
      else:
        raise Exception('unknown value of upload_if: %s' % upload_if)

    # Upload any files still in source_fileset.
    num_files_to_upload = len(source_fileset)
    print ('Uploading %d files, skipping %d ...' % (
        num_files_to_upload, num_files_total - num_files_to_upload))
    if num_files_to_upload == 0:
      return
    if num_threads > num_files_to_upload:
      num_threads = num_files_to_upload

    # Create a work queue with all files that need to be uploaded.
    q = Queue.Queue(maxsize=num_files_to_upload)
    for rel_path in source_fileset:
      q.put(rel_path)

    # Spin up worker threads to read from the task queue.
    def worker():
      while True:
        try:
          rel_path = q.get(block=False)
        except Queue.Empty:
          return  # no more tasks in the queue, so exit
        print (' Uploading file %d/%d: %s' % (
            num_files_to_upload - q.qsize(), num_files_to_upload, rel_path))
        self.upload_file(
            source_path=os.path.join(source_dir, rel_path),
            dest_bucket=b,
            dest_path=posixpath.join(dest_dir, rel_path),
            upload_if=self.UploadIf.ALWAYS,
            **kwargs)
        q.task_done()
    for _ in range(num_threads):
      t = threading.Thread(target=worker)
      t.daemon = True
      t.start()

    # Block until all files have been uploaded and all workers have exited.
    q.join()

  def download_file(self, source_bucket, source_path, dest_path,
                    create_subdirs_if_needed=False, source_generation=None):
    """Downloads a single file from Google Cloud Storage to local disk.

    Args:
      source_bucket: GS bucket to download the file from
      source_path: full path (Posix-style) within that bucket
      dest_path: full path (local-OS-style) on local disk to copy the file to
      create_subdirs_if_needed: boolean; whether to create subdirectories as
          needed to create dest_path
      source_generation: the generation version of the source
    """
    b = self._connect_to_bucket(bucket=source_bucket)
    key = Key(b)
    key.name = source_path
    if source_generation:
      key.generation = source_generation
    if create_subdirs_if_needed:
      _makedirs_if_needed(os.path.dirname(dest_path))
    with open(dest_path, 'w') as f:
      try:
        key.get_contents_to_file(fp=f)
      except BotoServerError, e:
        e.body = (repr(e.body) +
                  ' while downloading gs://%s/%s to local_path=%s' % (
                      b.name, source_path, dest_path))
        raise

  def download_dir_contents(self, source_bucket, source_dir, dest_dir):
    """Recursively download contents of a Google Storage directory to local disk

    params:
      source_bucket: GS bucket to copy the files from
      source_dir: full path (Posix-style) within that bucket; read the files
          from this directory
      dest_dir: full path (local-OS-style) on local disk of directory to copy
          the files into

    The copy operates as a "merge with overwrite": any files in source_dir will
    be "overlaid" on top of the existing content in dest_dir.  Existing files
    with the same names will be overwritten.

    TODO(epoger): Download multiple files simultaneously to reduce latency.
    """
    _makedirs_if_needed(dest_dir)
    b = self._connect_to_bucket(bucket=source_bucket)
    (dirs, files) = self.list_bucket_contents(
        bucket=source_bucket, subdir=source_dir)

    for filename in files:
      key = Key(b)
      key.name = posixpath.join(source_dir, filename)
      dest_path = os.path.join(dest_dir, filename)
      with open(dest_path, 'w') as f:
        try:
          key.get_contents_to_file(fp=f)
        except BotoServerError, e:
          e.body = (repr(e.body) +
                    ' while downloading gs://%s/%s to local_path=%s' % (
                        b.name, key.name, dest_path))
          raise

    for dirname in dirs:
      self.download_dir_contents(  # recurse
          source_bucket=source_bucket,
          source_dir=posixpath.join(source_dir, dirname),
          dest_dir=os.path.join(dest_dir, dirname))

  def get_acl(self, bucket, path, id_type, id_value):
    """Retrieve partial access permissions on a single file in Google Storage.

    Various users who match this id_type/id_value pair may have access rights
    other than that returned by this call, if they have been granted those
    rights based on *other* id_types (e.g., perhaps they have group access
    rights, beyond their individual access rights).

    TODO(epoger): What if the remote file does not exist?  This should probably
    raise an exception in that case.

    Params:
      bucket: GS bucket
      path: full path (Posix-style) to the file within that bucket
      id_type: must be one of the IdType values defined above
      id_value: get permissions for users whose id_type field contains this
          value

    Returns: the Permission value which has been set for users matching
        this id_type/id_value, on this file; or Permission.EMPTY if no such
        permissions have been set.
    """
    field = self._field_by_id_type[id_type]
    b = self._connect_to_bucket(bucket=bucket)
    acls = b.get_acl(key_name=path)
    matching_entries = [entry for entry in acls.entries.entry_list
                        if (entry.scope.type == id_type) and
                        (getattr(entry.scope, field) == id_value)]
    if matching_entries:
      assert len(matching_entries) == 1, '%d == 1' % len(matching_entries)
      return matching_entries[0].permission
    else:
      return self.Permission.EMPTY

  def set_acl(self, bucket, path, id_type, id_value, permission):
    """Set partial access permissions on a single file in Google Storage.

    Note that a single set_acl() call will not guarantee what access rights any
    given user will have on a given file, because permissions are additive.
    (E.g., if you set READ permission for a group, but a member of that group
    already has WRITE permission, that member will still have WRITE permission.)
    TODO(epoger): Do we know that for sure?  I *think* that's how it works...

    If there is already a permission set on this file for this id_type/id_value
    combination, this call will overwrite it.

    TODO(epoger): What if the remote file does not exist?  This should probably
    raise an exception in that case.

    Params:
      bucket: GS bucket
      path: full path (Posix-style) to the file within that bucket
      id_type: must be one of the IdType values defined above
      id_value: add permission for users whose id_type field contains this value
      permission: permission to add for users matching id_type/id_value;
          must be one of the Permission values defined above.
          If Permission.EMPTY, then any permissions will be granted to this
          particular id_type/id_value will be removed... but, given that
          permissions are additive, specific users may still have access rights
          based on permissions given to *other* id_type/id_value pairs.

    Example Code:
      bucket = 'gs://bucket-name'
      path = 'path/to/file'
      id_type = IdType.USER_BY_EMAIL
      id_value = 'epoger@google.com'
      set_acl(bucket, path, id_type, id_value, Permission.READ)
      assert Permission.READ == get_acl(bucket, path, id_type, id_value)
      set_acl(bucket, path, id_type, id_value, Permission.WRITE)
      assert Permission.WRITE == get_acl(bucket, path, id_type, id_value)
    """
    field = self._field_by_id_type[id_type]
    b = self._connect_to_bucket(bucket=bucket)
    acls = b.get_acl(key_name=path)

    # Remove any existing entries that refer to the same id_type/id_value,
    # because the API will fail if we try to set more than one.
    matching_entries = [entry for entry in acls.entries.entry_list
                        if (entry.scope.type == id_type) and
                        (getattr(entry.scope, field) == id_value)]
    if matching_entries:
      assert len(matching_entries) == 1, '%d == 1' % len(matching_entries)
      acls.entries.entry_list.remove(matching_entries[0])

    # Add a new entry to the ACLs.
    if permission != self.Permission.EMPTY:
      args = {'type': id_type, 'permission': permission}
      args[field] = id_value
      new_entry = acl.Entry(**args)
      acls.entries.entry_list.append(new_entry)

    # Finally, write back the modified ACLs.
    b.set_acl(acl_or_str=acls, key_name=path)

  def list_bucket_contents(self, bucket, subdir=None):
    """Returns files in the Google Storage bucket as a (dirs, files) tuple.

    TODO(epoger): This should raise an exception if subdir does not exist in
    Google Storage; right now, it just returns empty contents.

    Args:
      bucket: name of the Google Storage bucket
      subdir: directory within the bucket to list, or None for root directory
    """
    # The GS command relies on the prefix (if any) ending with a slash.
    prefix = subdir or ''
    if prefix and not prefix.endswith('/'):
      prefix += '/'
    prefix_length = len(prefix) if prefix else 0

    b = self._connect_to_bucket(bucket=bucket)
    items = BucketListResultSet(bucket=b, prefix=prefix, delimiter='/')
    dirs = []
    files = []
    for item in items:
      t = type(item)
      if t is Key:
        files.append(item.name[prefix_length:])
      elif t is Prefix:
        dirs.append(item.name[prefix_length:-1])
    return (dirs, files)

  def does_storage_object_exist(self, bucket, object_name):
    """Determines whether an object exists in Google Storage.

    Returns True if it exists else returns False.
    """
    b = self._connect_to_bucket(bucket=bucket)
    if object_name in b:
      return True
    dirs, files = self.list_bucket_contents(bucket, object_name)
    return bool(dirs or files)

  @staticmethod
  def is_gs_url(url):
    """Returns True if url is a legal Google Storage URL ("gs://bucket/file").
    """
    try:
      if url.lower().startswith(GS_PREFIX) and len(url) > len(GS_PREFIX):
        return url[len(GS_PREFIX)].isalnum()
      else:
        return False
    except AttributeError:
      return False

  @staticmethod
  def split_gs_url(url):
    """Returns (bucket, filepath) corresponding to a legal Google Storage URL.

    Raises AttributeError if the input URL is not a legal Google Storage URL.
    """
    if not GSUtils.is_gs_url(url):
      raise AttributeError('"%s" is not a legal Google Storage URL' % url)
    prefix_removed = url[len(GS_PREFIX):]
    pathsep_index = prefix_removed.find('/')
    if pathsep_index < 0:
      return (prefix_removed, '')
    else:
      return (prefix_removed[:pathsep_index],
              prefix_removed[pathsep_index+1:].strip('/'))

  def _connect_to_bucket(self, bucket):
    """Returns a Bucket object we can use to access a particular bucket in GS.

    Params:
      bucket: name of the bucket (e.g., 'chromium-skia-gm'), or a Bucket
          object (in which case this param is just returned as-is)
    """
    if type(bucket) is Bucket:
      return bucket
    try:
      return self._create_connection().get_bucket(bucket_name=bucket)
    except BotoServerError, e:
      e.body = repr(e.body) + ' while connecting to bucket=%s' % bucket
      raise

  def _create_connection(self):
    """Returns a GSConnection object we can use to access Google Storage."""
    if self._gs_access_key_id:
      return GSConnection(
          gs_access_key_id=self._gs_access_key_id,
          gs_secret_access_key=self._gs_secret_access_key)
    else:
      return AnonymousGSConnection()


def _config_file_as_dict(filepath):
  """Reads a boto-style config file into a dict.

  Parses all lines from the file of this form: key = value
  TODO(epoger): Create unittest.

  Params:
    filepath: path to config file on local disk

  Returns: contents of the config file, as a dictionary

  Raises exception if file not found.
  """
  dic = {}
  line_regex = re.compile('^\s*(\S+)\s*=\s*(\S+)\s*$')
  with open(filepath) as f:
    for line in f:
      match = line_regex.match(line)
      if match:
        (key, value) = match.groups()
        dic[key] = value
  return dic


def _makedirs_if_needed(path):
  """Creates a directory (and any parent directories needed), if it does not
  exist yet.

  Args:
    path: full path of directory to create
  """
  try:
    os.makedirs(path)
  except OSError as e:
    if e.errno != errno.EEXIST:
      raise


def _get_local_md5(path):
  """Returns the MD5 hash of a file on local disk."""
  hasher = hashlib.md5()
  with open(path, 'rb') as f:
    while True:
      data = f.read(64*1024)
      if not data:
        return hasher.hexdigest()
      hasher.update(data)

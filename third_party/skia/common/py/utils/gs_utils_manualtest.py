#!/usr/bin/python

"""Tests for gs_utils.py.

TODO(epoger): How should we exercise these self-tests? See http://skbug.com/2751
"""

# System-level imports.
import os
import posixpath
import random
import shutil
import sys
import tempfile
import time

# Local imports.
import gs_utils

TEST_BUCKET = 'chromium-skia-testing'


def _get_authenticated_gs_handle():
  """Returns an instance of GSUtils using ~/.boto for authentication."""
  try:
    return gs_utils.GSUtils(
        boto_file_path=os.path.expanduser(os.path.join('~','.boto')))
  except:
    print """
Failed to instantiate GSUtils object with default .boto file path.
Do you have a ~/.boto file that provides the credentials needed to write
into gs://%s?
""" % TEST_BUCKET
    raise


def _get_unique_posix_dir():
  """Returns a unique directory name suitable for use in Google Storage."""
  return 'gs_utils_manualtest/%d' % random.randint(0, sys.maxint)


def _test_static_methods():
  """Test all static methods."""
  gs = gs_utils.GSUtils

  # (input url,  output bucket, output path) for each test case
  testcases = [
      (None,  None, None),
      (5,  None, None),
      ('',  None, None),
      ('/one/two',  None, None),
      ('http://one/two',  None, None),
      ('gs:',  None, None),
      ('gs://',  None, None),
      ('gs:///',  None, None),
      ('gs://???',  None, None),
      ('gs:///bucket',  None, None),
      ('gs://bucket',  'bucket', ''),
      ('GS://bucket/',  'bucket', ''),
      ('gs://bucket//',  'bucket', ''),
      ('gs://bucket/path1',  'bucket', 'path1'),
      ('gs://bucket/path1/path2',  'bucket', 'path1/path2'),
      ('gs://bucket/path1/path2/',  'bucket', 'path1/path2'),
      ('gs://bucket///path1/path2///',  'bucket', 'path1/path2'),
      ('gs://bucket///path1//path2///',  'bucket', 'path1//path2'),
  ]
  for (url, bucket, path) in testcases:
    is_legal_url = (bucket != None)
    assert gs.is_gs_url(url) == is_legal_url, 'gs.is_gs_url("%s") == %s' % (
        url, is_legal_url)
    if is_legal_url:
      assert gs.split_gs_url(url) == (bucket, path), (
          'gs.split_gs_url("%s") == ("%s", "%s")' % (url, bucket, path))


def _test_public_read():
  """Make sure we can read from public files without .boto file credentials."""
  gs = gs_utils.GSUtils()
  gs.list_bucket_contents(bucket=TEST_BUCKET, subdir=None)


def _test_upload_if_one_file():
  """Test upload_if param within upload_file()."""
  gs = _get_authenticated_gs_handle()
  filename = 'filename'
  remote_dir = _get_unique_posix_dir()
  dest_path = posixpath.join(remote_dir, filename)
  local_dir = tempfile.mkdtemp()
  try:
    # Create a file on local disk, and upload it for the first time.
    local_path = os.path.join(local_dir, filename)
    with open(local_path, 'w') as f:
      f.write('original contents')
    gs.upload_file(source_path=local_path, dest_bucket=TEST_BUCKET,
                   dest_path=dest_path, upload_if=gs.UploadIf.IF_NEW)
    try:
      # Re-upload the same file, with upload_if=gs.UploadIf.ALWAYS;
      # the timestamp should change.
      old_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=dest_path)
      time.sleep(2)
      gs.upload_file(source_path=local_path, dest_bucket=TEST_BUCKET,
                     dest_path=dest_path, upload_if=gs.UploadIf.ALWAYS)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=dest_path)
      assert old_timestamp != new_timestamp, '%s != %s' % (
          old_timestamp, new_timestamp)

      # Re-upload the same file, with upload_if=gs.UploadIf.IF_MODIFIED;
      # the timestamp should NOT change.
      old_timestamp = new_timestamp
      time.sleep(2)
      gs.upload_file(source_path=local_path, dest_bucket=TEST_BUCKET,
                     dest_path=dest_path, upload_if=gs.UploadIf.IF_MODIFIED)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=dest_path)
      assert old_timestamp == new_timestamp, '%s == %s' % (
          old_timestamp, new_timestamp)

      # Modify and re-upload the file, with upload_if=gs.UploadIf.IF_NEW;
      # the timestamp should still not change.
      old_timestamp = new_timestamp
      with open(local_path, 'w') as f:
        f.write('modified contents')
      time.sleep(2)
      gs.upload_file(source_path=local_path, dest_bucket=TEST_BUCKET,
                     dest_path=dest_path, upload_if=gs.UploadIf.IF_NEW)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=dest_path)
      assert old_timestamp == new_timestamp, '%s == %s' % (
          old_timestamp, new_timestamp)

      # Re-upload the modified file, with upload_if=gs.UploadIf.IF_MODIFIED;
      # now the timestamp SHOULD change.
      old_timestamp = new_timestamp
      time.sleep(2)
      gs.upload_file(source_path=local_path, dest_bucket=TEST_BUCKET,
                     dest_path=dest_path, upload_if=gs.UploadIf.IF_MODIFIED)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=dest_path)
      assert old_timestamp != new_timestamp, '%s != %s' % (
          old_timestamp, new_timestamp)
    finally:
      # Clean up the remote_dir.
      gs.delete_file(bucket=TEST_BUCKET, path=dest_path)
  finally:
    # Clean up the local dir.
    shutil.rmtree(local_dir)


def _test_upload_if_multiple_files():
  """Test upload_if param within upload_dir_contents()."""
  gs = _get_authenticated_gs_handle()
  subdir = 'subdir'
  filenames = ['file1', 'file2']
  local_dir = tempfile.mkdtemp()
  remote_dir = _get_unique_posix_dir()
  sample_file_local_path = os.path.join(local_dir, subdir, filenames[0])
  sample_file_remote_path = posixpath.join(remote_dir, subdir, filenames[0])
  try:
    # Create files on local disk, and upload them for the first time.
    os.mkdir(os.path.join(local_dir, subdir))
    for filename in filenames:
      with open(os.path.join(local_dir, subdir, filename), 'w') as f:
        f.write('original contents of %s' % filename)
    gs.upload_dir_contents(
        source_dir=local_dir, dest_bucket=TEST_BUCKET,
        dest_dir=remote_dir, upload_if=gs.UploadIf.IF_NEW)
    try:
      # Re-upload the same files, with upload_if=gs.UploadIf.ALWAYS;
      # the timestamps should change.
      old_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=sample_file_remote_path)
      time.sleep(2)
      gs.upload_dir_contents(
          source_dir=local_dir, dest_bucket=TEST_BUCKET,
          dest_dir=remote_dir, upload_if=gs.UploadIf.ALWAYS)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=sample_file_remote_path)
      assert old_timestamp != new_timestamp, '%s != %s' % (
          old_timestamp, new_timestamp)

      # Re-upload the same files, with upload_if=gs.UploadIf.IF_MODIFIED;
      # the timestamps should NOT change.
      old_timestamp = new_timestamp
      time.sleep(2)
      gs.upload_dir_contents(
          source_dir=local_dir, dest_bucket=TEST_BUCKET,
          dest_dir=remote_dir, upload_if=gs.UploadIf.IF_MODIFIED)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=sample_file_remote_path)
      assert old_timestamp == new_timestamp, '%s == %s' % (
          old_timestamp, new_timestamp)

      # Modify and re-upload the files, with upload_if=gs.UploadIf.IF_NEW;
      # the timestamps should still not change.
      old_timestamp = new_timestamp
      with open(sample_file_local_path, 'w') as f:
        f.write('modified contents of sample file')
      time.sleep(2)
      gs.upload_dir_contents(
          source_dir=local_dir, dest_bucket=TEST_BUCKET,
          dest_dir=remote_dir, upload_if=gs.UploadIf.IF_NEW)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=sample_file_remote_path)
      assert old_timestamp == new_timestamp, '%s == %s' % (
          old_timestamp, new_timestamp)

      # Re-upload the modified file, with upload_if=gs.UploadIf.IF_MODIFIED;
      # now the timestamp SHOULD change.
      old_timestamp = new_timestamp
      time.sleep(2)
      gs.upload_dir_contents(
          source_dir=local_dir, dest_bucket=TEST_BUCKET,
          dest_dir=remote_dir, upload_if=gs.UploadIf.IF_MODIFIED)
      new_timestamp = gs.get_last_modified_time(
          bucket=TEST_BUCKET, path=sample_file_remote_path)
      assert old_timestamp != new_timestamp, '%s != %s' % (
          old_timestamp, new_timestamp)
    finally:
      # Delete all the files we uploaded to Google Storage.
      for filename in filenames:
        gs.delete_file(bucket=TEST_BUCKET,
                       path=posixpath.join(remote_dir, subdir, filename))
  finally:
    # Clean up the local dir.
    shutil.rmtree(local_dir)


def _test_authenticated_round_trip():
  gs = _get_authenticated_gs_handle()
  remote_dir = _get_unique_posix_dir()
  subdir = 'subdir'
  filenames_to_upload = ['file1', 'file2']

  # Upload test files to Google Storage, checking that their fine-grained
  # ACLs were set correctly.
  id_type = gs.IdType.GROUP_BY_DOMAIN
  id_value = 'chromium.org'
  set_permission = gs.Permission.READ
  local_src_dir = tempfile.mkdtemp()
  os.mkdir(os.path.join(local_src_dir, subdir))
  try:
    for filename in filenames_to_upload:
      with open(os.path.join(local_src_dir, subdir, filename), 'w') as f:
        f.write('contents of %s\n' % filename)
      dest_path = posixpath.join(remote_dir, subdir, filename)
      gs.upload_file(
          source_path=os.path.join(local_src_dir, subdir, filename),
          dest_bucket=TEST_BUCKET, dest_path=dest_path,
          fine_grained_acl_list=[(id_type, id_value, set_permission)])
      got_permission = gs.get_acl(bucket=TEST_BUCKET, path=dest_path,
                                  id_type=id_type, id_value=id_value)
      assert got_permission == set_permission, '%s == %s' % (
          got_permission, set_permission)
  finally:
    shutil.rmtree(local_src_dir)

  # Get a list of the files we uploaded to Google Storage.
  (dirs, files) = gs.list_bucket_contents(
      bucket=TEST_BUCKET, subdir=remote_dir)
  assert dirs == [subdir], '%s == [%s]' % (dirs, subdir)
  assert files == [], '%s == []' % files
  (dirs, files) = gs.list_bucket_contents(
      bucket=TEST_BUCKET, subdir=posixpath.join(remote_dir, subdir))
  assert dirs == [], '%s == []' % dirs
  assert files == filenames_to_upload, '%s == %s' % (files, filenames_to_upload)

  # Manipulate ACLs on one of those files, and verify them.
  # TODO(epoger): Test IdTypes other than GROUP_BY_DOMAIN ?
  # TODO(epoger): Test setting multiple ACLs on the same file?
  id_type = gs.IdType.GROUP_BY_DOMAIN
  id_value = 'google.com'
  fullpath = posixpath.join(remote_dir, subdir, filenames_to_upload[0])
  # Make sure ACL is empty to start with ...
  gs.set_acl(bucket=TEST_BUCKET, path=fullpath,
             id_type=id_type, id_value=id_value, permission=gs.Permission.EMPTY)
  permission = gs.get_acl(bucket=TEST_BUCKET, path=fullpath,
                          id_type=id_type, id_value=id_value)
  assert permission == gs.Permission.EMPTY, '%s == %s' % (
      permission, gs.Permission.EMPTY)
  # ... set it to OWNER ...
  gs.set_acl(bucket=TEST_BUCKET, path=fullpath,
             id_type=id_type, id_value=id_value, permission=gs.Permission.OWNER)
  permission = gs.get_acl(bucket=TEST_BUCKET, path=fullpath,
                          id_type=id_type, id_value=id_value)
  assert permission == gs.Permission.OWNER, '%s == %s' % (
      permission, gs.Permission.OWNER)
  # ... now set it to READ ...
  gs.set_acl(bucket=TEST_BUCKET, path=fullpath,
             id_type=id_type, id_value=id_value, permission=gs.Permission.READ)
  permission = gs.get_acl(bucket=TEST_BUCKET, path=fullpath,
                          id_type=id_type, id_value=id_value)
  assert permission == gs.Permission.READ, '%s == %s' % (
      permission, gs.Permission.READ)
  # ... and clear it again to finish.
  gs.set_acl(bucket=TEST_BUCKET, path=fullpath,
             id_type=id_type, id_value=id_value, permission=gs.Permission.EMPTY)
  permission = gs.get_acl(bucket=TEST_BUCKET, path=fullpath,
                          id_type=id_type, id_value=id_value)
  assert permission == gs.Permission.EMPTY, '%s == %s' % (
      permission, gs.Permission.EMPTY)

  # Download the files we uploaded to Google Storage, and validate contents.
  local_dest_dir = tempfile.mkdtemp()
  try:
    for filename in filenames_to_upload:
      gs.download_file(source_bucket=TEST_BUCKET,
                       source_path=posixpath.join(remote_dir, subdir, filename),
                       dest_path=os.path.join(local_dest_dir, subdir, filename),
                       create_subdirs_if_needed=True)
      with open(os.path.join(local_dest_dir, subdir, filename)) as f:
        file_contents = f.read()
      assert file_contents == 'contents of %s\n' % filename, (
          '%s == "contents of %s\n"' % (file_contents, filename))
  finally:
    shutil.rmtree(local_dest_dir)

  # Delete all the files we uploaded to Google Storage.
  for filename in filenames_to_upload:
    gs.delete_file(bucket=TEST_BUCKET,
                   path=posixpath.join(remote_dir, subdir, filename))

  # Confirm that we deleted all the files we uploaded to Google Storage.
  (dirs, files) = gs.list_bucket_contents(
      bucket=TEST_BUCKET, subdir=posixpath.join(remote_dir, subdir))
  assert dirs == [], '%s == []' % dirs
  assert files == [], '%s == []' % files


def _test_dir_upload_and_download():
  """Test upload_dir_contents() and download_dir_contents()."""
  gs = _get_authenticated_gs_handle()
  remote_dir = _get_unique_posix_dir()
  subdir = 'subdir'
  filenames = ['file1', 'file2']

  # Create directory tree on local disk and upload it.
  id_type = gs.IdType.GROUP_BY_DOMAIN
  id_value = 'chromium.org'
  set_permission = gs.Permission.READ
  local_src_dir = tempfile.mkdtemp()
  os.mkdir(os.path.join(local_src_dir, subdir))
  try:
    for filename in filenames:
      with open(os.path.join(local_src_dir, subdir, filename), 'w') as f:
        f.write('contents of %s\n' % filename)
    gs.upload_dir_contents(
        source_dir=local_src_dir, dest_bucket=TEST_BUCKET, dest_dir=remote_dir,
        predefined_acl=gs.PredefinedACL.PRIVATE,
        fine_grained_acl_list=[(id_type, id_value, set_permission)])
  finally:
    shutil.rmtree(local_src_dir)

  # Validate the list of the files we uploaded to Google Storage.
  (dirs, files) = gs.list_bucket_contents(
      bucket=TEST_BUCKET, subdir=remote_dir)
  assert dirs == [subdir], '%s == [%s]' % (dirs, subdir)
  assert files == [], '%s == []' % files
  (dirs, files) = gs.list_bucket_contents(
      bucket=TEST_BUCKET, subdir=posixpath.join(remote_dir, subdir))
  assert dirs == [], '%s == []' % dirs
  assert files == filenames, '%s == %s' % (files, filenames)

  # Check the fine-grained ACLs we set in Google Storage.
  for filename in filenames:
    got_permission = gs.get_acl(
        bucket=TEST_BUCKET, path=posixpath.join(remote_dir, subdir, filename),
        id_type=id_type, id_value=id_value)
    assert got_permission == set_permission, '%s == %s' % (
        got_permission, set_permission)

  # Download the directory tree we just uploaded, make sure its contents
  # are what we expect, and then delete the tree in Google Storage.
  local_dest_dir = tempfile.mkdtemp()
  try:
    gs.download_dir_contents(source_bucket=TEST_BUCKET, source_dir=remote_dir,
                             dest_dir=local_dest_dir)
    for filename in filenames:
      with open(os.path.join(local_dest_dir, subdir, filename)) as f:
        file_contents = f.read()
      assert file_contents == 'contents of %s\n' % filename, (
          '%s == "contents of %s\n"' % (file_contents, filename))
  finally:
    shutil.rmtree(local_dest_dir)
    for filename in filenames:
      gs.delete_file(bucket=TEST_BUCKET,
                     path=posixpath.join(remote_dir, subdir, filename))


def _test_does_storage_object_exist():
  gs = gs_utils.GSUtils()
  cases = (('gs_utils_manualtest/4843645167453050644/subdir/file1', True),
           ('gs_utils_manualtest', True),
           ('bogus', False))
  for obj, expect in cases:
    msg = 'expected exists(%s) to be %s' % (
        '/'.join(('gs:/', TEST_BUCKET, obj)), expect)
    assert gs.does_storage_object_exist(TEST_BUCKET, obj) == expect, msg


if __name__ == '__main__':
  _test_static_methods()
  _test_upload_if_multiple_files()
  _test_upload_if_one_file()
  _test_public_read()
  _test_authenticated_round_trip()
  _test_dir_upload_and_download()
  _test_does_storage_object_exist()
  # TODO(epoger): Add _test_unauthenticated_access() to make sure we raise
  # an exception when we try to access without needed credentials.

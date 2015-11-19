/*
 * Copyright (C) 2015 UI project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "libbase/ThreadList.h"

namespace merck {

ThreadList::ThreadList() {
}

ThreadList::~ThreadList() {
}

void ThreadList::Register(Thread* self) {
  threads_.push_back(self);
}

void ThreadList::Unregister(Thread* self) {
  for (Thread* thread : threads_) {
    if (thread == self) {
      threads_.remove(thread);
    }
  }
}

void ThreadList::ForEach(void (*callback)(Thread*, void*), void* context) {
  for (const auto& thread : threads_) {
    callback(thread, context);
  }
}

void ThreadList::DestroyAllThreads() {
  for (const auto& thread : threads_) {
    delete thread;
  }
  threads_.empty();
}

void ThreadList::Dump(std::ostream& os) const {
  for (const auto& thread : threads_) {
    os << *thread;
  }
}

std::ostream& operator<<(std::ostream& os, const ThreadList& thread_list) {
  thread_list.Dump(os);
  return os;
}

}  // namespace merck

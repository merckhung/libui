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

#ifndef UI_LIBBASE_THREADLIST_H_
#define UI_LIBBASE_THREADLIST_H_

#include "libbase/macros.h"
#include "libbase/Thread.h"
#include "libbase/Mutex.h"

#include <list>
#include <iosfwd>
#include <iostream>

namespace merck {

class ThreadList {
 public:
  ThreadList();
  ~ThreadList();

  void Register(Thread* self);
  void Unregister(Thread* self);
  void ForEach(void (*callback)(Thread*, void*), void* context);
  void DestroyAllThreads();
  void Dump(std::ostream& os) const;

 private:
  std::list<Thread*> threads_;

  DISALLOW_COPY_AND_ASSIGN(ThreadList);
};

std::ostream& operator<<(std::ostream& os, const ThreadList& thread_list);

}  // namespace merck

#endif  // UI_LIBBASE_THREADLIST_H_

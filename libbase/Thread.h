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

#ifndef UI_LIBBASE_THREAD_H_
#define UI_LIBBASE_THREAD_H_

#include "libbase/macros.h"

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <iosfwd>
#include <iostream>

namespace merck {

class Thread {
 public:
  enum class ThreadState : uint8_t {
    THDSTS_INIT = 0,
    THDSTS_READY,
    THDSTS_RUNNING,
    THDSTS_SUSPEND,
    THDSTS_EXIT,
  };

 public:
  ~Thread();
  uint32_t GetTid() const;
  ThreadState GetThreadState() const;
  void SetName(uint8_t* name);
  uint8_t* GetName() const;
  void Dump(std::ostream& os) const;

  static Thread* CreateNativeThread(void* func,
                                    void* arg,
                                    const uint8_t* name,
                                    size_t stack_size = 4096);
  static void Startup();
  static void Shutdown();

 private:
  explicit Thread(void* func, const uint8_t* name);
  bool Init();
  void Destroy();

  static void* CreateCallback(void* arg);
  static void ExitCallback(void* arg);

  static pthread_key_t pthread_key_self_;

  void* (*func_)(void*);
  void* arg_;
  uint32_t tid_;
  pthread_t pthread_self_;
  ThreadState thread_state_;
  const uint8_t* name_;

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

std::ostream& operator<<(std::ostream& os, const Thread& thread);
std::ostream& operator<<(std::ostream& os, const Thread::ThreadState state);

}  // namespace merck

#endif  // UI_LIBBASE_THREAD_H_

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

#include "libbase/Thread.h"

namespace merck {

pthread_key_t Thread::pthread_key_self_;

Thread::~Thread() {
  if (thread_state_ >= ThreadState::THDSTS_RUNNING) {
    pthread_cancel(pthread_self_);
    pthread_join(pthread_self_, nullptr);
  }
}

uint32_t Thread::GetTid() const {
  return tid_;
}

Thread::ThreadState Thread::GetThreadState() const {
  return thread_state_;
}

void Thread::Dump(std::ostream& os) const {
  os << "[Thread name=\"" << name_ << "\", tid=" << tid_ << ", state=";
  os << thread_state_;
  os << "]\n";
}

std::ostream& operator<<(std::ostream& os, const Thread& thread) {
  thread.Dump(os);
  return os;
}

std::ostream& operator<<(std::ostream& os, const Thread::ThreadState state) {
  switch (state) {
    case Thread::ThreadState::THDSTS_INIT:
      os << "INIT";
      break;
    case Thread::ThreadState::THDSTS_READY:
      os << "READY";
      break;
    case Thread::ThreadState::THDSTS_RUNNING:
      os << "RUNNING";
      break;
    case Thread::ThreadState::THDSTS_SUSPEND:
      os << "SUSPEND";
      break;
    case Thread::ThreadState::THDSTS_EXIT:
      os << "EXIT";
      break;
    default:
      break;
  }
  return os;
}

Thread* Thread::CreateNativeThread(void* func,
                                   void* arg,
                                   const uint8_t* name,
                                   size_t stack_size) {
  Thread* child_thread = new Thread(func, name);
  child_thread->arg_ = arg;

  pthread_t new_pthread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(&attr, stack_size);
  pthread_create(&new_pthread, &attr, Thread::CreateCallback, child_thread);
  pthread_attr_destroy(&attr);
  return child_thread;
}

void Thread::Startup() {
  pthread_key_create(&Thread::pthread_key_self_, Thread::ExitCallback);
  if (pthread_getspecific(Thread::pthread_key_self_) != nullptr) {
    // FIXME: FATAL error
  }
}

void Thread::Shutdown() {
  pthread_key_delete(Thread::pthread_key_self_);
}

void* Thread::CreateCallback(void* arg) {
  Thread* thread = reinterpret_cast<Thread*>(arg);
  // Initialize the thread data
  thread->Init();
  // Execute the function body
  thread->thread_state_ = ThreadState::THDSTS_RUNNING;
  void* ret = thread->func_(thread->arg_);
  // Destroy
  thread->Destroy();
  // Return
  return ret;
}

void Thread::ExitCallback(void* arg) {
}

Thread::Thread(void* func, const uint8_t* name)
  : arg_(nullptr),
    tid_(0),
    pthread_self_(0),
    thread_state_(ThreadState::THDSTS_INIT),
    name_(name) {
  func_ = reinterpret_cast<void* (*)(void*)>(func);
}

bool Thread::Init() {
  pthread_self_ = pthread_self();
#ifdef __APPLE__
  uint64_t owner = 0;
  pthread_threadid_np(nullptr, &owner);
  tid_ = static_cast<uint32_t>(owner);
#else
  tid_ = static_cast<uint32_t>(getpid());
#endif
  pthread_setspecific(Thread::pthread_key_self_, this);
  thread_state_ = ThreadState::THDSTS_READY;
  return true;
}

void Thread::Destroy() {
  thread_state_ = ThreadState::THDSTS_EXIT;
}

}  // namespace merck

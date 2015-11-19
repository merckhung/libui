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

#include "libbase/Mutex.h"

#include <stdint.h>
#include <errno.h>

namespace merck {

Mutex::Mutex() {
  pthread_mutex_init(&mutex_, nullptr);
}

Mutex::~Mutex() {
  pthread_mutex_destroy(&mutex_);
}

void Mutex::ExclusiveLock() {
  pthread_mutex_lock(&mutex_);
}

bool Mutex::ExclusiveTryLock() {
  int32_t result = pthread_mutex_trylock(&mutex_);
  if (result == EBUSY) {
    return false;
  }
  if (result != 0) {
    errno = result;
  }
  return true;
}

void Mutex::ExclusiveUnlock() {
  pthread_mutex_unlock(&mutex_);
}

}  // namespace merck

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

#ifndef UI_LIBUI_DEBUGGER_H_
#define UI_LIBUI_DEBUGGER_H_

#include "libbase/macros.h"

#include <stdint.h>

namespace merck {
namespace ui {

class Debugger {
 public:
  enum class DebugCmd : uint8_t {
    CMD_HELP = 0,
    CMD_LIST_THREADS,
    CMD_LIST_VIEWS,
  };

 public:
  Debugger();
  ~Debugger();

  static void* DebuggerMain(void* arg);

 private:
  DISALLOW_COPY_AND_ASSIGN(Debugger);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_DEBUGGER_H_

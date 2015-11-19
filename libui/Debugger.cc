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

#include "libui/Debugger.h"

#include "libbase/Thread.h"
#include "libbase/Mutex.h"
#include "libbase/ThreadList.h"
#include "libui/Window.h"

namespace merck {
namespace ui {

Debugger::Debugger() {
}

Debugger::~Debugger() {
}

static void cmd_help() {
  fprintf(stderr, "COMMANDS:\n");
  fprintf(stderr, "  help           - display this help message\n");
  fprintf(stderr, "  list threads   - display all running threads\n");
  fprintf(stderr, "  list views     - display all views hierarchically\n");
}

static void cmd_list_threads(Window* wind) {
  std::cerr << wind->GetThreadList();
}

static Debugger::DebugCmd ParseCmd(uint8_t* cmd) {
  if (!strcmp(reinterpret_cast<const char *>(cmd), "help")) {
    return Debugger::DebugCmd::CMD_HELP;
  } else if (!strcmp(reinterpret_cast<const char *>(cmd), "list threads")) {
    return Debugger::DebugCmd::CMD_LIST_THREADS;
  } else if (!strcmp(reinterpret_cast<const char *>(cmd), "list views")) {
    return Debugger::DebugCmd::CMD_LIST_VIEWS;
  }
  return Debugger::DebugCmd::CMD_HELP;
}

static bool ExecuteCmd(uint8_t* cmd, Window* wind) {
  Debugger::DebugCmd type = ParseCmd(cmd);

  switch (type) {
    case Debugger::DebugCmd::CMD_LIST_THREADS:
      cmd_list_threads(wind);
      return true;

    case Debugger::DebugCmd::CMD_LIST_VIEWS:
      // Dump views
      std::cerr << *wind;
      return true;

    case Debugger::DebugCmd::CMD_HELP:
    default:
      cmd_help();
      return true;
  }

  return false;
}

void* Debugger::DebuggerMain(void* arg) {
  Window* wind = reinterpret_cast<Window*>(arg);
  uint8_t cmd[256];
  size_t rbytes;

  while (true) {
    // Print out the prompt
    write(1, "DEBUG> ", 7);

    // Read a line
    rbytes = read(0, &cmd, sizeof(cmd));
    cmd[rbytes - 1] = '\0';

    // Execute the command
    ExecuteCmd(cmd, wind);

    // Sleep for a while
    usleep(500);
  }
  return nullptr;
}

}  // namespace ui
}  // namespace merck

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

#include "libui/os/OSWindow.h"
#include "libui/os/linux/LinuxOSWindow.h"

int main(int argc, char** argv) {
  // Create an instance os
  merck::ui::OSWindow* os_window = merck::ui::LinuxOSWindow::Create(argc,
                                  reinterpret_cast<int8_t**>(argv));
  if (os_window == nullptr) {
    return -1;
  }

  // Initialize the window instance
  os_window->Init(1024, 768);

  // Enter the main loop
  os_window->MainLoop();

  // Release the resource
  delete os_window;
  return 0;
}

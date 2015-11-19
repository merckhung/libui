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

#include "VisualDemo/VisualDemo.h"
#include "libui/UiMain.h"
#include "libbase/debug.h"

namespace merck {
namespace ui {

bool UiMain(Window* wind, int32_t argc, int8_t** argv) {
  // Set window title
  std::string title("Visual Demo");
  wind->SetTitle(title);

  // Set window background color
  wind->SetBgColor(0xFFFFFFFF);
  wind->EnBgColor();

  // Create a layout
  wind->SetLayout(new LinearLayout(wind->GetBound()));

  // Left
  std::string left_str = "<";
  DirectButton* lb = new DirectButton(nullptr, left_str, 40, 30);
  lb->EnVisible();
  lb->EnEnable();
  wind->AddChildView(lb);

  // Right
  std::string right_str = ">";
  DirectButton* rb = new DirectButton(nullptr, right_str, 40, 30);
  rb->EnVisible();
  rb->EnEnable();
  wind->AddChildView(rb);

  // Label - IP address
  std::string str_ip = "IP:";
  Label* lb_ip = new Label(str_ip);
  lb_ip->EnVisible();
  lb_ip->EnEnable();
  wind->AddChildView(lb_ip);

  // TextField - IP address
  std::string str_addr = "192.168.1.1";
  TextField* tf_ip = new TextField(str_addr);
  tf_ip->EnVisible();
  tf_ip->EnEnable();
  wind->AddChildView(tf_ip);

  // Select - PID
  std::string str_proc = "com.test.app";
  SelectBox* sb_pid = new SelectBox(str_proc, 400, 30);
  sb_pid->EnVisible();
  sb_pid->EnEnable();

  // Button - connect
  std::string str_conn = "Connect";
  Button* b_conn = new Button(nullptr, str_conn, 110, 30);
  b_conn->EnVisible();
  b_conn->EnEnable();
  wind->AddChildView(b_conn);

  // Label - PID
  std::string str_pid = "PID:";
  Label* lb_pid = new Label(str_pid);
  lb_pid->EnVisible();
  lb_pid->EnEnable();
  wind->AddChildView(lb_pid);

  // Select - PID -- Assign
  wind->AddChildView(sb_pid);

  // Button - watch
  std::string str_watch = "Watch";
  Button* b_watch = new Button(nullptr, str_watch, 80, 30);
  b_watch->EnVisible();
  b_watch->EnEnable();
  wind->AddChildView(b_watch);

  return true;
}

void UiExit(Window* wind, int32_t argc, int8_t** argv) {
}

}  // namespace ui
}  // namespace merck

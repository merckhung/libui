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

#ifndef UI_LIBUI_UIMAIN_H_
#define UI_LIBUI_UIMAIN_H_

#include "libui/Window.h"
#include "libui/LinearLayout.h"
#include "libui/StackLayout.h"
#include "libui/Label.h"
#include "libui/TextField.h"
#include "libui/SelectBox.h"
#include "libui/Button.h"
#include "libui/DirectButton.h"
#include "libui/ImageBox.h"

namespace merck {
namespace ui {

bool UiMain(Window* wind, int32_t argc, int8_t** argv);
void UiExit(Window* wind, int32_t argc, int8_t** argv);

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_UIMAIN_H_

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

/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "X11/Xlib.h"
#include "X11/keysym.h"

#include "SkKey.h"

#ifndef UI_LIBUI_OS_LINUX_XKEYSTOSKKEYS_H_
#define UI_LIBUI_OS_LINUX_XKEYSTOSKKEYS_H_

SkKey XKeyToSkKey(KeySym keysym) {
  switch (keysym) {
    case XK_BackSpace:
      return kBack_SkKey;
    case XK_Return:
      return kOK_SkKey;
    case XK_Home:
      return kHome_SkKey;
    case XK_End:
      return kEnd_SkKey;
    case XK_Right:
      return kRight_SkKey;
    case XK_Left:
      return kLeft_SkKey;
    case XK_Down:
      return kDown_SkKey;
    case XK_Up:
      return kUp_SkKey;
    default:
      return kNONE_SkKey;
  }
}

#endif  // UI_LIBUI_OS_LINUX_XKEYSTOSKKEYS_H_

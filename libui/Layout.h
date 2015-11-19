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

#ifndef UI_LIBUI_LAYOUT_H_
#define UI_LIBUI_LAYOUT_H_

#include "libbase/macros.h"

#include "SkRect.h"

namespace merck {
namespace ui {

class View;

class Layout {
 public:
  Layout();
  explicit Layout(const SkRect& margin);

  virtual ~Layout();

  virtual void SetMargins(int32_t thickness);
  virtual void SetMargins(const SkRect& margin);

  virtual void OnLayout(View* view) = 0;

 private:
  SkRect margin_;

  DISALLOW_COPY_AND_ASSIGN(Layout);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_LAYOUT_H_

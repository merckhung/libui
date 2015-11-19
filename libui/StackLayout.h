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

#ifndef UI_LIBUI_STACKLAYOUT_H_
#define UI_LIBUI_STACKLAYOUT_H_

#include "libui/Layout.h"
#include "libui/View.h"

namespace merck {
namespace ui {

class StackLayout : public Layout {
 public:
  static constexpr int32_t DELIMITER = 5;

  StackLayout();
  explicit StackLayout(const SkRect& margin);
  virtual ~StackLayout();

  virtual void OnLayout(View* view) override;

 private:
  typedef Layout INHERITED;
  DISALLOW_COPY_AND_ASSIGN(StackLayout);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_STACKLAYOUT_H_

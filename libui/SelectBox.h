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

#ifndef UI_LIBUI_SELECTBOX_H_
#define UI_LIBUI_SELECTBOX_H_

#include "Widget.h"

namespace merck {
namespace ui {

class SelectBox : public Widget {
 public:
  static constexpr const char* VIEW_NAME = "SelectBox";

 public:
  explicit SelectBox();
  explicit SelectBox(std::string& text);
  explicit SelectBox(std::string& text, int32_t w, int32_t h);
  virtual ~SelectBox();

  virtual void OnDraw(SkCanvas* canvas) override;

  virtual const SkSize& GetPreferSize() override;

 private:
  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(SelectBox);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_SELECTBOX_H_

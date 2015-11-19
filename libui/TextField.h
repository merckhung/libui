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

#ifndef UI_LIBUI_TEXTFIELD_H_
#define UI_LIBUI_TEXTFIELD_H_

#include "Widget.h"

namespace merck {
namespace ui {

class TextField : public Widget {
 public:
  static constexpr const char* VIEW_NAME = "TextField";

 public:
  explicit TextField();
  explicit TextField(std::string& text);
  explicit TextField(std::string& text, int32_t width, int32_t height);
  virtual ~TextField();

  virtual void OnDraw(SkCanvas* canvas) override;

  virtual const SkSize& GetPreferSize() override;

 private:
  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(TextField);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_TEXTFIELD_H_

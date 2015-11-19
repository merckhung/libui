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

#ifndef UI_LIBUI_WIDGET_H_
#define UI_LIBUI_WIDGET_H_

#include "libui/View.h"

namespace merck {
namespace ui {

class Widget : public View {
 public:
  static constexpr const char* VIEW_NAME = "Widget";

 public:
  Widget();
  explicit Widget(std::string& text);
  explicit Widget(std::string& text, int32_t width, int32_t height);
  explicit Widget(std::string& text, SkRect& bound);
  explicit Widget(int32_t width, int32_t height);

  virtual ~Widget();

  virtual void Invalidate(SkRect* bound) override;

  virtual void SetText(std::string& text);
  virtual const std::string& GetText() const;

 protected:
  std::string text_;
  int32_t font_size_;

 private:
  typedef View INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Widget);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_WIDGET_H_

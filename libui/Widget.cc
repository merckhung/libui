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

#include "libui/Widget.h"

namespace merck {
namespace ui {

Widget::Widget()
  : INHERITED(),
    text_(std::string()),
    font_size_(DefaultFontSize) {
  internal_name_ = VIEW_NAME;
}

Widget::Widget(std::string& text)
  : Widget() {
  SetText(text);
}

Widget::Widget(std::string& text, int32_t width, int32_t height)
  : Widget(text) {
  bound_.set(SkIntToScalar(0), SkIntToScalar(0), SkIntToScalar(width), SkIntToScalar(height));
}

Widget::Widget(std::string& text, SkRect& bound)
  : Widget(text) {
  SetBound(bound);
}

Widget::Widget(int32_t width, int32_t height)
  : Widget() {
  bound_.set(SkIntToScalar(0), SkIntToScalar(0), SkIntToScalar(width), SkIntToScalar(height));
}

Widget::~Widget() {
}

void Widget::Invalidate(SkRect* bound) {
  if (parent_ != nullptr) {
    parent_->Invalidate(bound);
  }
}

void Widget::SetText(std::string& text) {
  text_ = text;
}

const std::string& Widget::GetText() const {
  return text_;
}

}  // namespace ui
}  // namespace merck

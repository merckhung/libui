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

#include "libui/Label.h"

namespace merck {
namespace ui {

Label::Label()
  : INHERITED() {
  internal_name_ = VIEW_NAME;
}

Label::Label(std::string& text)
  : INHERITED(text) {
  SetBound(0, 0, text_.size() * (GetFontSize() - 4), (GetFontSize() * 2));
  internal_name_ = VIEW_NAME;
}

Label::Label(std::string& text, int32_t w, int32_t h)
  : INHERITED(text) {
  SetBound(0, 0, w, h);
  internal_name_ = VIEW_NAME;
}

Label::~Label() {
}

void Label::OnDraw(SkCanvas* canvas) {
  // Text
  SkPaint p;
  float x = GetX();
  float y = GetY();
  float w = GetWidth();
  float h = GetHeight();
  int32_t d_x = (w * 0.2f) + x;
  int32_t d_y = (h * 0.65f) + y;

  p.setFlags(
      SkPaint::kAntiAlias_Flag |
      SkPaint::kSubpixelText_Flag);
  p.setStyle(SkPaint::kFill_Style);
  p.setColor(SK_ColorBLACK);
  p.setTextSize(GetFontSize());
  p.setFakeBoldText(true);

  canvas->save();
  canvas->drawText(text_.c_str(), text_.length(), SkIntToScalar(d_x), SkIntToScalar(d_y), p);
  canvas->restore();

  // Debug
  if (IsDebug() == true) {
    SkPaint pd;
    pd.setStrokeWidth(SkIntToScalar(1));
    pd.setColor(SK_ColorRED);
    pd.setStyle(SkPaint::Style::kStroke_Style);
    SkRect rd = SkRect::MakeXYWH(
                  SkIntToScalar(GetX()),
                  SkIntToScalar(GetY()),
                  SkIntToScalar(GetWidth()),
                  SkIntToScalar(GetHeight()));
    canvas->drawRect(rd, pd);
  }
}

const SkSize& Label::GetPreferSize() {
  prefer_size_.set(GetWidth(), GetHeight());
  return prefer_size_;
}

}  // namespace ui
}  // namespace merck

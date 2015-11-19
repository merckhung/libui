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

#include "TextField.h"

namespace merck {
namespace ui {

TextField::TextField()
  : INHERITED() {
  internal_name_ = VIEW_NAME;
}

TextField::TextField(std::string& text)
  : INHERITED(text) {
  SetBound(0, 0, text_.size() * (GetFontSize() - 4), (GetFontSize() * 2));
  internal_name_ = VIEW_NAME;
}

TextField::TextField(std::string& text, int32_t width, int32_t height)
  : INHERITED(text, width, height) {
  internal_name_ = VIEW_NAME;
}

TextField::~TextField() {
}

void TextField::OnDraw(SkCanvas* canvas) {
  SkPaint p;
  int32_t MARGIN = 10;
  float x = GetX();
  float y = GetY();
  float w = GetWidth();
  float h = GetHeight();
  float d_x = MARGIN + x;
  float d_y = (h * 0.65f) + y;

  // Background
  p.setColor(0xFFE0E0E0);
  p.setStyle(SkPaint::Style::kFill_Style);
  SkRect r1 = SkRect::MakeXYWH(
                SkIntToScalar(x),
                SkIntToScalar(y),
                SkIntToScalar(w),
                SkIntToScalar(h));
  canvas->drawRect(r1, p);

  // Border
  p.setColor(0xFF000000);
  p.setStrokeWidth(SkIntToScalar(1));
  p.setStyle(SkPaint::Style::kStroke_Style);
  canvas->drawRect(r1, p);

  // Text
  p.setFlags(
      SkPaint::kAntiAlias_Flag |
      SkPaint::kSubpixelText_Flag);
  p.setStyle(SkPaint::kFill_Style);
  p.setColor(SK_ColorBLACK);
  p.setTextSize(GetFontSize());
  p.setFakeBoldText(true);

  canvas->save();
  canvas->drawText(text_.c_str(), text_.length(), d_x, d_y, p);
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

const SkSize& TextField::GetPreferSize() {
  prefer_size_.set(GetWidth() + 3, GetHeight() + 3);
  return prefer_size_;
}

}  // namespace ui
}  // namespace merck

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

#include "libui/DirectButton.h"

namespace merck {
namespace ui {

DirectButton::DirectButton(ButtonListener* listener, std::string& text)
  : INHERITED(listener, text) {
  internal_name_ = VIEW_NAME;
}

DirectButton::DirectButton(ButtonListener* listener, std::string& text, int32_t w, int32_t h)
  : INHERITED(listener, text, w, h) {
  internal_name_ = VIEW_NAME;
}

DirectButton::~DirectButton() {
}

void DirectButton::OnDraw(SkCanvas* canvas) {
  if ((GetWidth() == 0) || (GetHeight() == 0)) {
    return;
  }

  // Debug
  if (IsDebug() == true) {
    SkPaint pd;
    pd.setStrokeWidth(SkIntToScalar(1));
    pd.setColor(SK_ColorRED);
    pd.setStyle(SkPaint::Style::kStroke_Style);
    SkRect r1 = SkRect::MakeXYWH(
                  SkIntToScalar(GetX()),
                  SkIntToScalar(GetY()),
                  SkIntToScalar(GetWidth()),
                  SkIntToScalar(GetHeight()));
    canvas->drawRect(r1, pd);
  }

  SkPaint p;
  int32_t x = GetX();
  int32_t y = GetY();
  int32_t w = GetWidth();
  int32_t h = GetHeight();
  int32_t d_x = x + 12;
  int32_t d_y = y + 21;

  // Draw the shadow
  p.setColor(0xFFB0B0B0);
  p.setStyle(SkPaint::kStroke_Style);
  SkRect r2 = SkRect::MakeXYWH(
                SkIntToScalar(x + 1),
                SkIntToScalar(y),
                SkIntToScalar(w),
                SkIntToScalar(h));
  canvas->drawRect(r2, p);

  // Draw the button
  p.setStyle(SkPaint::kFill_Style);
  if (state_ == Button::STATE_PRESSED) {
    // Pressed
    p.setColor(0xFF909090);
  } else {
    // Normal
    p.setColor(0xFFE0E0E0);
  }
  SkRect r1 = SkRect::MakeXYWH(
                SkIntToScalar(x),
                SkIntToScalar(y),
                SkIntToScalar(w),
                SkIntToScalar(h));
  canvas->drawRect(r1, p);

  // Text label
  p.setFlags(SkPaint::kAntiAlias_Flag
             | SkPaint::kSubpixelText_Flag);
  if (state_ == Button::STATE_PRESSED) {
    // Pressed
    p.setColor(0xFFF0F0F0);
  } else {
    // Normal
    p.setColor(0xFF909090);
  }
  p.setTextSize(SkIntToScalar(GetFontSize()));
  p.setStyle(SkPaint::kFill_Style);
  canvas->save();
  canvas->drawText(text_.c_str(),
                   strlen(text_.c_str()),
                   SkIntToScalar(d_x),
                   SkIntToScalar(d_y), p);
  canvas->restore();
}

const SkSize& DirectButton::GetPreferSize() {
  prefer_size_.set(GetWidth(), GetHeight());
  return prefer_size_;
}

}  // namespace ui
}  // namespace merck

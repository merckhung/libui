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

#include "libui/Button.h"

namespace merck {
namespace ui {

Button::Button(ButtonListener* listener, std::string& text)
  : INHERITED(text, text.size() * GetFontSize(), (GetFontSize() * 3)),
    state_(Button::State::STATE_NORMAL),
    style_(Button::Style::STYLE_STANDARD),
    listener_(listener) {
  internal_name_ = VIEW_NAME;
}

Button::Button(ButtonListener* listener, std::string& text, int32_t w, int32_t h)
  : INHERITED(text, w, h),
    state_(Button::State::STATE_NORMAL),
    style_(Button::Style::STYLE_STANDARD),
    listener_(listener) {
  if (w == 0 && h == 0) {
    SetBound(0, 0, text_.size() * GetFontSize(), (GetFontSize() * 3));
  } else if (w != 0 && h == 0) {
    SetBound(0, 0, w, (GetFontSize() * 3));
  } else if (w == 0 && h != 0) {
    SetBound(0, 0, text_.size() * GetFontSize(), h);
  } else {
    SetBound(0, 0, w, h);
  }
}

Button::~Button() {
}

void Button::NotifyClick(const Event& evt) {
  if (listener_) {
    listener_->ButtonPressed(this, evt);
  }
}

void Button::NotifyUnclick(const Event& evt) {
  if (listener_) {
    listener_->ButtonReleased(this, evt);
  }
}

Button::State Button::GetState() const {
  return state_;
}

Button::Style Button::GetStyle() const {
  return style_;
}

void Button::OnDraw(SkCanvas* canvas) {
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

bool Button::OnMousePress(Event& evt) {
  state_ = Button::STATE_PRESSED;
  NotifyClick(evt);
  // Notify the bound to redraw
  Invalidate(&bound_);
  return true;
}

bool Button::OnMouseRelease(Event& evt) {
  state_ = Button::STATE_NORMAL;
  NotifyUnclick(evt);
  // Notify the bound to redraw
  Invalidate(&bound_);
  return true;
}

bool Button::OnMouseMotion(Event& evt) {
  return true;
}

const SkSize& Button::GetPreferSize() {
  prefer_size_.set(GetWidth() + 3, GetHeight() + 3);
  return prefer_size_;
}

}  // namespace ui
}  // namespace merck

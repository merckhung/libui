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

#include "Chart.h"

namespace merck {
namespace ui {

Chart::Chart(int32_t width, int32_t height) {
  bound_.set(SkIntToScalar(0), SkIntToScalar(0), SkIntToScalar(width), SkIntToScalar(height));
  data_lock_.reset(new Mutex());
  internal_name_ = VIEW_NAME;
}

Chart::~Chart() {
}

void Chart::OnDraw(SkCanvas* canvas) {
  int width = GetWidth();
  int height = GetHeight();
  int x = GetX();
  int y = GetY();
  int left = 40;
  int h_mid = y + (height / 2);
  int h_80p = y + (height / 5) * 1;
  int start_left = x + left;

  SkPaint p;
  p.setStrokeWidth(SkIntToScalar(1));

  // Background
  p.setColor(0xFFE0E0E0);
  p.setStyle(SkPaint::Style::kFill_Style);
  SkRect r1 = SkRect::MakeXYWH(
                SkIntToScalar(x),
                SkIntToScalar(y),
                SkIntToScalar(width),
                SkIntToScalar(height));
  canvas->drawRect(r1, p);

  // Outline
  p.setColor(0xFF000000);
  p.setStyle(SkPaint::Style::kStroke_Style);
  SkRect r2 = SkRect::MakeXYWH(
                SkIntToScalar(x + left),
                SkIntToScalar(y),
                SkIntToScalar(width - left),
                SkIntToScalar(height));
  canvas->drawRect(r2, p);

  // 100
  char title_100[] = "100";
  canvas->drawText(title_100, strlen(title_100), x, h_mid, p);
  canvas->drawLine(SkIntToScalar(start_left),
                   SkIntToScalar(h_mid),
                   SkIntToScalar(start_left + (width - left)),
                   SkIntToScalar(h_mid),
                   p);

  // 200
  char title_200[] = "200";
  canvas->drawText(title_200, strlen(title_200), x, h_80p, p);
  canvas->drawLine(SkIntToScalar(start_left),
                   SkIntToScalar(h_80p),
                   SkIntToScalar(start_left + (width - left)),
                   SkIntToScalar(h_80p),
                   p);

  // Data
  p.setColor(0xFFFF0000);
  p.setStyle(SkPaint::Style::kStrokeAndFill_Style);
  int nr_cells = MAX_COLUMNS;
  int margin = 2;
  int cell_w = ((width - left) / nr_cells) - margin;
  int cell_h = (height) / 100;
  int bottom = y + height;

  size_t i = MAX_COLUMNS - data_.size();
  std::list<ChartData>::iterator iter = data_.begin();

  for (; i < MAX_COLUMNS; ++i, ++iter) {
    printf("Percentage = %f\n", (*iter).GetValue());
    int s_l = (i * (cell_w + margin)) + start_left + 1;
    int s_t_len = cell_h * (*iter).GetValue();
    int s_t = bottom - s_t_len;
    SkRect r3 = SkRect::MakeXYWH(
                    SkIntToScalar(s_l),
                    SkIntToScalar(s_t),
                    SkIntToScalar(cell_w),
                    SkIntToScalar(s_t_len));
    canvas->drawRect(r3, p);
  }
}

bool Chart::OnMousePress(Event& evt) {
  return true;
}

bool Chart::OnMouseRelease(Event& evt) {
  return true;
}

bool Chart::OnMouseMotion(Event& evt) {
  return true;
}

const SkSize& Chart::GetPreferSize() {
  prefer_size_.set(GetWidth() + 3, GetHeight() + 3);
  return prefer_size_;
}

void Chart::AddValue(ChartData& cd) {
  {
    MutexLock mu(*data_lock_);
    if (data_.size() == MAX_COLUMNS) {
      data_.pop_front();
    }
    data_.push_back(cd);
  }
}

void Chart::Clear() {
  {
    MutexLock mu(*data_lock_);
    data_.clear();
  }
}

}  // namespace ui
}  // namespace merck

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

#include "libui/LinearLayout.h"

namespace merck {
namespace ui {

LinearLayout::LinearLayout()
  : INHERITED() {;
}

LinearLayout::LinearLayout(const SkRect& margin)
  : INHERITED(margin) {
}

LinearLayout::~LinearLayout() {
}

void LinearLayout::OnLayout(View* view) {
  int32_t x = view->GetX() + DELIMITER;
  int32_t y = view->GetY() + DELIMITER;
  int32_t w = view->GetWidth();
  int32_t h = view->GetHeight();
  int32_t e_x = x + w;
  int32_t e_y = y + h;
  int32_t dx = x;
  int32_t dy = y;
  int32_t max_height = 0;

  for (auto& child : view->children_) {
    // Get its prefer size
    SkSize size = child->GetPreferSize();

    // Check the required size of the view, must be smaller/equal to,
    // and then skip it
    if (size.width() > w || size.height() > h) {
      child->SetBound(0, 0, 0, 0);
      fprintf(stderr, "Size is too large, shrink to 0\n");
      continue;
    }

    // If not wide enough, move to next line
    if (dx + size.width() > e_x) {
      dx = x;
      dy += (max_height + DELIMITER);
    }

    // If not height enough, shrink it to 0 size, and then skip it
    if (dy + size.height() > e_y) {
      child->SetBound(0, 0, 0, 0);
      fprintf(stderr, "Out of Y, shrink to 0\n");
      continue;
    }

    // Maintain the maximum height of this line
    if (size.height() > max_height) {
      max_height = size.height();
    }

    // Set the view to a XY position
    child->SetPos(dx, dy);

#if 0
    // Debug
    int tmpw = size.width();
    int tmph = size.height();
    fprintf(stderr, "At X:%d, W:%d, Y:%d, H:%d\n", dx, tmpw, dy, tmph);
#endif

    // Move to next X position
    dx += (size.width() + DELIMITER);

    // If X has exceeded the boundary, then move to next line
    if (dx >= e_x) {
      dx = x;
      dy += (max_height + DELIMITER);
      max_height = 0;
    }
  }
}

}  // namespace ui
}  // namespace merck

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

#include "libui/Layout.h"

namespace merck {
namespace ui {

Layout::Layout() {
  margin_.set(0, 0, 0, 0);
}

Layout::Layout(const SkRect& margin) {
  margin_ = margin;
}

Layout::~Layout() {
}

void Layout::SetMargins(int32_t thickness) {
  margin_.set(thickness, thickness, thickness, thickness);
}

void Layout::SetMargins(const SkRect& margin) {
  margin_ = margin;
}

}  // namespace ui
}  // namespace merck

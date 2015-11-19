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

#ifndef UI_LIBUI_IMAGEBOX_H_
#define UI_LIBUI_IMAGEBOX_H_

#include "libui/Widget.h"

#include "SkBitmap.h"
#include "SkPicture.h"

namespace merck {
namespace ui {

class ImageBox : public Widget {
 public:
  static constexpr const char* VIEW_NAME = "ImageBox";

 public:
  explicit ImageBox(std::string& path);
  explicit ImageBox(std::string& path, int32_t w, int32_t h);
  virtual ~ImageBox();

  virtual void OnDraw(SkCanvas* canvas) override;
  virtual const SkSize& GetPreferSize() override;

 private:
  virtual void Init(std::string& path, int32_t w, int32_t h);

  std::string path_;
  SkBitmap b_;
  SkCanvas* can_;
  SkAutoTUnref<SkPicture> pic_;

  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(ImageBox);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_IMAGEBOX_H_

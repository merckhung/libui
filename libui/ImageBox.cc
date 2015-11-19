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

#include "libui/ImageBox.h"

#include "SkPictureRecorder.h"
#include "SkImageDecoder.h"

namespace merck {
namespace ui {

void ImageBox::Init(std::string& path, int32_t w, int32_t h) {
  SkPictureRecorder recorder;
  if (SkImageDecoder::DecodeFile(path_.c_str(), &b_) == false) {
    return;
  }
  can_ = recorder.beginRecording(SkIntToScalar(b_.width()),
                                 SkIntToScalar(b_.height()),
                                 nullptr, 0);
  can_->drawBitmap(b_, 0, 0, nullptr);
  pic_.reset(recorder.endRecording());

  if (w == 0 && h == 0) {
    SetBound(0, 0, b_.width(), b_.height());
  } else if (w == 0 && h != 0) {
    SetBound(0, 0, b_.width(), h);
  } else if (w !=0 && h == 0) {
    SetBound(0, 0, w, b_.height());
  } else {
    SetBound(0, 0, w, h);
  }
}

ImageBox::ImageBox(std::string& path)
  : INHERITED(),
    path_(path) {
  Init(path, 0, 0);
  internal_name_ = VIEW_NAME;
}

ImageBox::ImageBox(std::string& path, int32_t w, int32_t h)
  : INHERITED(),
    path_(path) {
  Init(path, w, h);
  internal_name_ = VIEW_NAME;
}

ImageBox::~ImageBox() {
}

void ImageBox::OnDraw(SkCanvas* canvas) {
  if (pic_.get() == nullptr) {
    return;
  }
  canvas->drawPicture(pic_);
}

const SkSize& ImageBox::GetPreferSize() {
  prefer_size_.set(GetWidth(), GetHeight());
  return prefer_size_;
}

}  // namespace ui
}  // namespace merck

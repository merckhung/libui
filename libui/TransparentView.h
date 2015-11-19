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

#ifndef UI_LIBUI_TRANSPARENTVIEW_H_
#define UI_LIBUI_TRANSPARENTVIEW_H_

#include "libui/View.h"

namespace merck {
namespace ui {

class TransparentView : public View {
 public:
  static constexpr const char* VIEW_NAME = "TransparentView";

 public:
  TransparentView();
  virtual ~TransparentView();

  virtual void OnDraw(SkCanvas* canvas) override;
  virtual void OnResize(SkSize& size) override;
  virtual void Invalidate(SkRect* bound) override;
  virtual void OnAnimate() override;

  virtual bool OnMousePress(Event& evt) override;
  virtual bool OnMouseRelease(Event& evt) override;
  virtual bool OnMouseMotion(Event& evt) override;
  virtual bool OnKeyPress(Event& evt) override;
  virtual bool OnKeyRelease(Event& evt) override;
  virtual bool OnKeyChar(Event& evt) override;

 private:
  typedef View INHERITED;
  DISALLOW_COPY_AND_ASSIGN(TransparentView);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_TRANSPARENTVIEW_H_

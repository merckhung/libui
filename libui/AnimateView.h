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

#ifndef UI_LIBUI_ANIMATEVIEW_H_
#define UI_LIBUI_ANIMATEVIEW_H_

#include "libui/View.h"
#include "libbase/Mutex.h"

namespace merck {
namespace ui {

class AnimateView : public View {
 public:
  static constexpr const char* VIEW_NAME = "AnimateView";

 public:
  AnimateView();
  explicit AnimateView(uint32_t id);
  explicit AnimateView(uint32_t id, View* parent);
  virtual ~AnimateView();

  // Painting events
  virtual void OnPreDraw() override;
  virtual void OnPostDraw() override;

  // Animation
  static void* AnimatorMain(void* arg);

  virtual void OnAnimate() override;
  virtual bool OnAnimateStart();
  virtual AnimState OnAnimateProgress();
  virtual bool OnAnimateCancelStart();
  virtual AnimState OnAnimateCancelProgress();
  virtual void OnAnimateEnd();

 protected:
  Mutex anim_lock_;
  bool anim_end_;

 private:
  typedef View INHERITED;
  DISALLOW_COPY_AND_ASSIGN(AnimateView);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_ANIMATEVIEW_H_

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

#ifndef UI_LIBUI_WINDOW_H_
#define UI_LIBUI_WINDOW_H_

#include "libui/TransparentView.h"
#include "libui/WindowManager.h"
#include "libbase/ThreadList.h"

#include "SkApplication.h"
#include "SkDraw.h"
#include "SkGradientShader.h"
#include "SkGraphics.h"
#include "SkWindow.h"

namespace merck {
namespace ui {

class Window : public TransparentView {
 public:
  virtual ~Window();
  virtual void SetTitle(std::string& title);
  virtual void Invalidate(SkRect* bound) override;
  virtual void OnDraw(SkCanvas *canvas) override;
  virtual ThreadList& GetThreadList();

  static Window* Create(void* init,
                        void* destroy,
                        uint32_t width,
                        uint32_t height,
                        WindowManager* wm);

 private:
  explicit Window(void* init,
                  void* destroy,
                  uint32_t width,
                  uint32_t height,
                  WindowManager* wm);

  WindowManager* wm_;
  void (*destroy_)(Window*, int32_t, int8_t**);
  bool dirty_;
  ThreadList thread_list_;

  typedef TransparentView INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Window);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_WINDOW_H_

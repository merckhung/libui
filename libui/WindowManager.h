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

#ifndef UI_LIBUI_WINDOWMANAGER_H_
#define UI_LIBUI_WINDOWMANAGER_H_

#include "libbase/macros.h"
#include "libui/View.h"

#include "SkCanvas.h"
#include "SkBitmap.h"

#include <vector>
#include <stdint.h>

namespace merck {
namespace ui {

class WindowManager {
 public:
  virtual ~WindowManager();

  virtual void OnResize(uint32_t width, uint32_t height);
  virtual void OnReposition(uint32_t x, uint32_t y);
  virtual void OnBtnPress(uint32_t x, uint32_t , uint32_t modi);
  virtual void OnBtnRelease(uint32_t x, uint32_t , uint32_t modi);
  virtual void OnMotion(uint32_t x, uint32_t y, uint32_t modi);
  virtual void OnKeyPress(uint32_t key);
  virtual void OnKeyRelease(uint32_t key);
  virtual void OnKeyChar(uint32_t key);
  virtual void OnDraw(SkCanvas* canvas);
  virtual void Invalidate();

  virtual uint32_t GetX() const;
  virtual uint32_t GetY() const;
  virtual uint32_t GetWidth() const;
  virtual uint32_t GetHeight() const;
  virtual SkBitmap& GetBitmap();
  virtual int32_t GetArgc() const;
  virtual int8_t** GetArgv() const;

  virtual void SetTitle(std::string& title);

  static WindowManager* Create(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
  static WindowManager* Current();

 private:
  WindowManager(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

  static WindowManager* instance_;

  uint32_t x_;
  uint32_t y_;
  uint32_t width_;
  uint32_t height_;
  SkBitmap bitmap_;
  SkColorType color_type_;
  std::vector<View*> windows_;

  DISALLOW_COPY_AND_ASSIGN(WindowManager);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_WINDOWMANAGER_H_

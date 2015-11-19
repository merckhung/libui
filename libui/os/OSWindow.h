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

#ifndef UI_LIBUI_OS_OSWINDOW_H_
#define UI_LIBUI_OS_OSWINDOW_H_

#include "libbase/macros.h"
#include "libui/WindowManager.h"

#include <stdint.h>

namespace merck {
namespace ui {

class OSWindow {
 public:
  OSWindow();
  virtual ~OSWindow();

  virtual int32_t MainLoop() = 0;
  virtual bool Init(uint32_t width, uint32_t height) = 0;
  virtual void Destroy() = 0;
  virtual void SetTitle(std::string& title) = 0;
  virtual void Update(SkRect* area) = 0;
  virtual void Present() = 0;

  virtual void Resize(uint32_t width, uint32_t height);
  virtual uint32_t GetWidth() const;
  virtual uint32_t GetHeight() const;
  virtual bool InitWindowManager();
  virtual void DestroyWindowManager();
  virtual WindowManager* GetWindowManager() const;
  virtual int32_t GetArgc() const;
  virtual int8_t** GetArgv() const;
  virtual void SetDirty();
  virtual void ClearDirty();
  virtual bool IsDirty() const;

  static OSWindow* Current();

 protected:
  uint32_t x_;
  uint32_t y_;
  uint32_t width_;
  uint32_t height_;
  WindowManager* wm_;
  int32_t argc_;
  int8_t** argv_;
  bool is_dirty_;

  // Singleton
  static OSWindow* instance_;

 private:
  DISALLOW_COPY_AND_ASSIGN(OSWindow);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_OS_OSWINDOW_H_

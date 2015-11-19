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

#ifndef UI_LIBUI_OS_LINUX_LINUXOSWINDOW_H_
#define UI_LIBUI_OS_LINUX_LINUXOSWINDOW_H_

#include "libui/os/OSWindow.h"

#include <memory>
#include <GL/glx.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

#include "SkGraphics.h"
#include "SkBitmap.h"
#include "SkScalar.h"
#include "SkSurface.h"
#include "SkGr.h"
#include "GrRenderTarget.h"
#include "gl/GrGLInterface.h"
#include "gl/GrGLUtil.h"

namespace merck {
namespace ui {

static GLint gl_attr[] = {
  GLX_RGBA,
  GLX_DEPTH_SIZE, 24,
  GLX_DOUBLEBUFFER,
  GLX_STENCIL_SIZE, 8,
  None
};
static constexpr GLint kAttCount = SK_ARRAY_COUNT(gl_attr);
static constexpr uint32_t x_event_mask = (StructureNotifyMask
                                          | ButtonPressMask
                                          | ButtonReleaseMask
                                          | ExposureMask
                                          | PointerMotionMask
                                          | KeyPressMask
                                          | KeyReleaseMask);
static constexpr uint32_t requestedMSAASampleCount = 0;

class LinuxOSWindow : public OSWindow {
 public:
  enum NextXEventResult {
    kContinue,
    kQuitRequest,
    kPaintRequest,
  };
  enum SkModifierKeys {
    kShift_SkModifierKey    = 1 << 0,
    kControl_SkModifierKey  = 1 << 1,
    kOption_SkModifierKey   = 1 << 2,   // same as ALT
    kCommand_SkModifierKey  = 1 << 3,
  };

 public:
  virtual ~LinuxOSWindow() override;
  virtual int32_t MainLoop() override;
  virtual bool Init(uint32_t width, uint32_t height) override;
  virtual void Destroy() override;
  virtual void SetTitle(std::string& title) override;
  virtual void Resize(uint32_t width, uint32_t height) override;
  virtual void Update(SkRect* area) override;
  virtual void Present() override;

  static LinuxOSWindow* Create(int32_t argc, int8_t** argv);
  static LinuxOSWindow* Current();

 private:
  explicit LinuxOSWindow(int32_t argc, int8_t** argv);
  void MapWindowAndWait();
  bool GlAttach();
  void GlDetach();
  GrRenderTarget* GlRenderTarget();
  bool GlSetup();
  void GlTeardown();
  SkSurface* CreateSurface();
  bool Paint(SkBitmap& bitmap);

  NextXEventResult NextXEvent();
  bool NextXEventWithDelay();

  // X window variables
  Display* display_;
  XVisualInfo* x_visual_info_;
  Window x_window_;
  GC x_gc_;
  XEvent evt_;

  // OpenGL device variables
  uint32_t msaa_sample_cnt_;
  GLXContext glx_context_;

  // OpenGL SKIA variables
  const GrGLInterface* gr_gl_ifce_;
  GrContext* gr_context_;
  GrRenderTarget* gl_render_tgt;

  // SKIA surface
  SkSurfaceProps props_;

  // X window paint
  XImage image_;

  typedef OSWindow INHERITED;
  DISALLOW_COPY_AND_ASSIGN(LinuxOSWindow);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_OS_LINUX_LINUXOSWINDOW_H_

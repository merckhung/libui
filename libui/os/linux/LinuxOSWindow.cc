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

#include "libui/os/linux/LinuxOSWindow.h"
#include "libui/os/linux/keysym2ucs.h"
#include "libui/os/linux/XkeysToSkKeys.h"

namespace merck {
namespace ui {

static Atom wm_delete_window_message;
static SkMSec gTimerDelay = 1;

LinuxOSWindow::LinuxOSWindow(int32_t argc, int8_t** argv)
  : INHERITED(),
    display_(nullptr),
    x_visual_info_(nullptr),
    x_gc_(nullptr),
    msaa_sample_cnt_(0),
    glx_context_(nullptr),
    gr_gl_ifce_(nullptr),
    gr_context_(nullptr),
    gl_render_tgt(nullptr),
    props_(SkSurfaceProps::kLegacyFontHost_InitType) {
  // Initialize SKIA
  SkGraphics::Init();
  // Keep the input arguments for later uses
  argc_ = argc;
  argv_ = argv;
}

LinuxOSWindow::~LinuxOSWindow() {
  // Terminate Window Manager
  INHERITED::DestroyWindowManager();
  // Terminate X windows & OpenGL
  Destroy();
  // Terminate SKIA
  SkGraphics::Term();
}

bool LinuxOSWindow::NextXEventWithDelay() {
  // Check for pending events before entering the select loop. There might
  // be events in the in-memory queue but not processed yet.
  if (XPending(display_)) {
    XNextEvent(display_, &evt_);
    return true;
  }
  SkMSec ms = gTimerDelay;
  if (ms > 0) {
    int x11_fd = ConnectionNumber(display_);
    fd_set input_fds;
    FD_ZERO(&input_fds);
    FD_SET(x11_fd, &input_fds);

    timeval tv;
    tv.tv_sec = ms / 1000;              // seconds
    tv.tv_usec = (ms % 1000) * 1000;    // microseconds

    if (!select(x11_fd + 1, &input_fds, NULL, NULL, &tv)) {
      if (!XPending(display_)) {
        return false;
      }
    }
  }
  XNextEvent(display_, &evt_);
  return true;
}

static uint32_t GetModi(const XEvent& evt) {
  static const struct {
    unsigned fXMask;
    unsigned fSkMask;
  } gModi[] = {
      // X values found by experiment. Is there a better way?
      {1, LinuxOSWindow::kShift_SkModifierKey},
      {4, LinuxOSWindow::kControl_SkModifierKey},
      {8, LinuxOSWindow::kOption_SkModifierKey},
    };
  uint32_t modi = 0;
  for (size_t i = 0; i < SK_ARRAY_COUNT(gModi); ++i) {
    if (evt.xkey.state & gModi[i].fXMask) {
      modi |= gModi[i].fSkMask;
    }
  }
  return modi;
}

LinuxOSWindow::NextXEventResult LinuxOSWindow::NextXEvent() {
  if (NextXEventWithDelay() == false) {
    return kContinue;
  }

  switch (evt_.type) {
    case Expose:
      if (0 == evt_.xexpose.count) {
        return kPaintRequest;
      }
      break;

    case ConfigureNotify:
    {
      XConfigureEvent xcfg = evt_.xconfigure;
      uint32_t w = static_cast<uint32_t>(xcfg.width);
      uint32_t h = static_cast<uint32_t>(xcfg.height);
      uint32_t x = static_cast<uint32_t>(xcfg.x);
      uint32_t y = static_cast<uint32_t>(xcfg.y);
      if (x != x_ || y != y_) {
        x_ = x;
        y_ = y;
        wm_->OnReposition(x_, y_);
      }
      if (w != width_ || h != height_) {
        Resize(w, h);
        wm_->OnResize(w, h);
      }
      break;
    }

    case ButtonPress:
      if (evt_.xbutton.button == Button1) {
        uint32_t x = static_cast<uint32_t>(evt_.xbutton.x);
        uint32_t y = static_cast<uint32_t>(evt_.xbutton.y);
        uint32_t modi = GetModi(evt_);
        wm_->OnBtnPress(x, y, modi);
      }
      break;

    case ButtonRelease:
      if (evt_.xbutton.button == Button1) {
        uint32_t x = static_cast<uint32_t>(evt_.xbutton.x);
        uint32_t y = static_cast<uint32_t>(evt_.xbutton.y);
        uint32_t modi = GetModi(evt_);
        wm_->OnBtnRelease(x, y, modi);
      }
      break;

    case MotionNotify:
    {
      uint32_t x = static_cast<uint32_t>(evt_.xmotion.x);
      uint32_t y = static_cast<uint32_t>(evt_.xmotion.y);
      uint32_t modi = GetModi(evt_);
      wm_->OnMotion(x, y, modi);
      break;
    }

    case KeyPress:
    {
      wm_->OnKeyPress(XKeyToSkKey(XkbKeycodeToKeysym(display_, evt_.xkey.keycode, 0, 0)));
      int32_t shiftLevel = (evt_.xkey.state & ShiftMask) ? 1 : 0;
      KeySym keysym = XkbKeycodeToKeysym(display_, evt_.xkey.keycode,
                                         0, shiftLevel);
      int32_t uni = keysym2ucs(keysym);
      if (uni != -1) {
        wm_->OnKeyChar(uni);
      }
      break;
    }

    case KeyRelease:
      wm_->OnKeyRelease(XKeyToSkKey(XkbKeycodeToKeysym(display_, evt_.xkey.keycode, 0, 0)));
      break;

    case ClientMessage:
      if ((Atom)evt_.xclient.data.l[0] == wm_delete_window_message) {
        INHERITED::DestroyWindowManager();
        return kQuitRequest;
      }
      // fallthrough
    default:
      // Do nothing for other events
      break;
  }

  return kContinue;
}

int32_t LinuxOSWindow::MainLoop() {
  // Register interest in the delete window message
  wm_delete_window_message = XInternAtom(display_, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display_, x_window_, &wm_delete_window_message, 1);

  // Select input X events
  XSelectInput(display_, x_window_, x_event_mask);

  for (;;) {
    if (IsDirty() == true) {
      ClearDirty();
      XEvent evt;
      sk_bzero(&evt, sizeof(evt));
      evt.type = Expose;
      evt.xexpose.display = display_;
      XSendEvent(display_, x_window_, false, ExposureMask, &evt);
    }

    if (!XPending(display_)) {
      continue;
    }

    switch (NextXEvent()) {
      case kContinue:
        break;
      case kPaintRequest:
        Update(nullptr);
        break;
      case kQuitRequest:
        return 0;
    }
  }
  return 0;
}

bool LinuxOSWindow::Init(uint32_t width, uint32_t height) {
  if (width == 0 || height == 0) {
    return false;
  }
  width_ = width;
  height_ = height;
  // Only 1 display instance is allowed
  if (display_ != nullptr) {
    return true;
  }
  // Create an instance of the X display
  display_ = XOpenDisplay(NULL);
  if (display_ == nullptr) {
    return false;
  }
  // Configure OpenGL MSSA sample count
  if (requestedMSAASampleCount > 0) {
    GLint msaaAtt[kAttCount + 4];
    memcpy(msaaAtt, gl_attr, sizeof(gl_attr));
    SkASSERT(None == msaaAtt[kAttCount - 1]);
    msaaAtt[kAttCount - 1] = GLX_SAMPLE_BUFFERS_ARB;
    msaaAtt[kAttCount + 0] = 1;
    msaaAtt[kAttCount + 1] = GLX_SAMPLES_ARB;
    msaaAtt[kAttCount + 2] = requestedMSAASampleCount;
    msaaAtt[kAttCount + 3] = None;
    x_visual_info_ = glXChooseVisual(display_, DefaultScreen(display_), msaaAtt);
    msaa_sample_cnt_ = requestedMSAASampleCount;
  }
  if (x_visual_info_ == nullptr) {
    x_visual_info_ = glXChooseVisual(display_,
                                     DefaultScreen(display_),
                                     reinterpret_cast<int*>(gl_attr));
  }
  if (x_visual_info_ == nullptr) {
    // Create a simple window instead.  We will not be able to show GL
    x_window_ = XCreateSimpleWindow(display_,
                                    DefaultRootWindow(display_),
                                    0, 0,  // x, y
                                    width, height,
                                    0,     // border width
                                    0,     // border value
                                    0);    // background value
  } else {
    Colormap colorMap = XCreateColormap(display_,
                                        RootWindow(display_, x_visual_info_->screen),
                                        x_visual_info_->visual,
                                        AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = colorMap;
    swa.event_mask = x_event_mask;
    x_window_ = XCreateWindow(display_,
                              RootWindow(display_, x_visual_info_->screen),
                              0, 0,  // x, y
                              width, height,
                              0,  // border width
                              x_visual_info_->depth,
                              InputOutput,
                              x_visual_info_->visual,
                              CWEventMask | CWColormap,
                              &swa);
  }

  MapWindowAndWait();
  x_gc_ = XCreateGC(display_, x_window_, 0, nullptr);
  if (x_gc_ == nullptr) {
    Destroy();
    return false;
  }

  if (GlAttach() == false) {
    Destroy();
    return false;
  }

  if (GlSetup() == false) {
    Destroy();
    return false;
  }

  // Initialize Window Manager
  INHERITED::InitWindowManager();
  return true;
}

void LinuxOSWindow::Destroy() {
  GlTeardown();
  GlDetach();

  if (display_) {
    GlDetach();
    if (x_gc_) {
      XFreeGC(display_, x_gc_);
    }
    x_gc_ = nullptr;
    XDestroyWindow(display_, x_window_);
    x_visual_info_ = nullptr;
    XCloseDisplay(display_);
    display_ = nullptr;
    msaa_sample_cnt_ = 0;
  }
}

void LinuxOSWindow::SetTitle(std::string& title) {
  if (display_ == nullptr) {
    return;
  }
  unsigned char* new_title = new unsigned char[title.size() + 1];
  strncpy(reinterpret_cast<char*>(new_title), title.c_str(), title.size());
  XTextProperty textProp;
  textProp.value = new_title;
  textProp.format = 8;
  textProp.nitems = strlen(reinterpret_cast<char*>(textProp.value));
  textProp.encoding = XA_STRING;
  XSetWMName(display_, x_window_, &textProp);
  free(new_title);
}

LinuxOSWindow* LinuxOSWindow::Create(int32_t argc, int8_t** argv) {
  if (instance_) {
    return reinterpret_cast<LinuxOSWindow*>(instance_);
  }
  instance_ = new LinuxOSWindow(argc, argv);
  return reinterpret_cast<LinuxOSWindow*>(instance_);
}

LinuxOSWindow* LinuxOSWindow::Current() {
  return reinterpret_cast<LinuxOSWindow*>(instance_);
}

void LinuxOSWindow::MapWindowAndWait() {
  XMapWindow(display_, x_window_);
  uint32_t eventMask = StructureNotifyMask;
  XSelectInput(display_, x_window_, eventMask);
  // Wait until screen is ready.
  XEvent evt;
  do {
    XNextEvent(display_, &evt);
  } while (evt.type != MapNotify);
}

bool LinuxOSWindow::GlAttach() {
  if (display_ == nullptr) {
    return false;
  }
  if (glx_context_ == nullptr) {
    if (x_visual_info_ == nullptr) {
      return false;
    }
    glx_context_ = glXCreateContext(display_,
                                   x_visual_info_,
                                   nullptr,
                                   GL_TRUE);
    if (glx_context_ == nullptr) {
      return false;
    }
  }
  glXMakeCurrent(display_,
                 x_window_,
                 glx_context_);
  glViewport(0, 0,
             SkScalarRoundToInt(width_),
             SkScalarRoundToInt(height_));
  glClearColor(0, 0, 0, 0);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  return true;
}

void LinuxOSWindow::GlDetach() {
  if (display_ == nullptr || glx_context_ == nullptr) {
    return;
  }
  glXMakeCurrent(display_, None, NULL);
  glXDestroyContext(display_, glx_context_);
  glx_context_ = nullptr;
}

GrRenderTarget* LinuxOSWindow::GlRenderTarget() {
  int32_t sample_count;
  int32_t stencil_bits;
  glXGetConfig(display_, x_visual_info_, GLX_SAMPLES_ARB, &sample_count);
  glXGetConfig(display_, x_visual_info_, GLX_STENCIL_SIZE, &stencil_bits);

  GrBackendRenderTargetDesc desc;
  desc.fWidth = SkScalarRoundToInt(width_);
  desc.fHeight = SkScalarRoundToInt(height_);
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
  desc.fSampleCnt = sample_count;
  desc.fStencilBits = stencil_bits;

  GrGLint buffer;
  GR_GL_GetIntegerv(gr_gl_ifce_, GR_GL_FRAMEBUFFER_BINDING, &buffer);

  desc.fRenderTargetHandle = buffer;
  return gr_context_->wrapBackendRenderTarget(desc);
}

bool LinuxOSWindow::GlSetup() {
  gr_gl_ifce_ = GrGLCreateNativeInterface();
  if (gr_gl_ifce_ == nullptr) {
    return false;
  }
  gr_context_ = GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)gr_gl_ifce_);
  if (gr_context_ == nullptr) {
    return false;
  }
  SkSafeUnref(gl_render_tgt);
  gl_render_tgt = GlRenderTarget();
  if (gl_render_tgt == nullptr) {
    return false;
  }
  return true;
}

void LinuxOSWindow::GlTeardown() {
  SkSafeUnref(gr_context_);
  gr_context_ = nullptr;

  SkSafeUnref(gr_gl_ifce_);
  gr_gl_ifce_ = nullptr;

  SkSafeUnref(gl_render_tgt);
  gl_render_tgt = nullptr;

  GlDetach();
}

static bool ConvertSkBitmapToXImage(XImage& image, const SkBitmap& bitmap) {
  sk_bzero(&image, sizeof(image));
  int bitsPerPixel = bitmap.bytesPerPixel() * 8;
  image.width = bitmap.width();
  image.height = bitmap.height();
  image.format = ZPixmap;
  image.data = reinterpret_cast<char*>(bitmap.getPixels());
  image.byte_order = LSBFirst;
  image.bitmap_unit = bitsPerPixel;
  image.bitmap_bit_order = LSBFirst;
  image.bitmap_pad = bitsPerPixel;
  image.depth = 24;
  image.bytes_per_line = bitmap.rowBytes() - bitmap.width() * 4;
  image.bits_per_pixel = bitsPerPixel;
  return XInitImage(&image);
}

bool LinuxOSWindow::Paint(SkBitmap& bitmap) {
  if (glx_context_ != nullptr) {
    return true;
  }
  if (display_ == nullptr) {
    return false;
  }
  if (!ConvertSkBitmapToXImage(image_, bitmap)) {
    return false;
  }
  XPutImage(display_,
            x_window_,
            x_gc_,
            &image_,
            0, 0,     // src x,y
            0, 0,     // dst x,y
            bitmap.width(),
            bitmap.height());
  return true;
}

void LinuxOSWindow::Resize(uint32_t width, uint32_t height) {
  INHERITED::Resize(width, height);
  GlAttach();
  SkSafeUnref(gl_render_tgt);
  gl_render_tgt = GlRenderTarget();
}

void LinuxOSWindow::Update(SkRect* area) {
  // Acquire a SkCanvas
  std::unique_ptr<SkSurface> surface(SkSurface::NewRenderTargetDirect(gl_render_tgt, &props_));
  SkCanvas* canvas = surface->getCanvas();

  // Call all children to draw on this canvas
  wm_->OnDraw(canvas);

  // Flush and present to GL
  gr_context_->flush();
  Present();
}

void LinuxOSWindow::Present() {
  if (display_ != nullptr && glx_context_ != nullptr) {
    glXSwapBuffers(display_, x_window_);
  }
}

}  // namespace ui
}  // namespace merck

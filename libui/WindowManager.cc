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

#include "libui/WindowManager.h"
#include "libui/os/OSWindow.h"
#include "libui/UiMain.h"

namespace merck {
namespace ui {

// Initialize the instance of the singleton to nullptr
WindowManager* WindowManager::instance_ = nullptr;

WindowManager::WindowManager(uint32_t x,
                             uint32_t y,
                             uint32_t width,
                             uint32_t height)
  : x_(x),
    y_(y),
    width_(width),

    height_(height),
    color_type_(kRGBA_8888_SkColorType) {
  // Allocate bitmap
  bitmap_.allocPixels(SkImageInfo::Make(width_, height_,
                                        color_type_, kPremul_SkAlphaType));

  // Start Threads
  Thread::Startup();

  // Install windows
  Window* main_app = Window::Create(reinterpret_cast<void*>(UiMain),
                                    reinterpret_cast<void*>(UiExit),
                                    width_,
                                    height_,
                                    this);
  if (main_app != nullptr) {
    main_app->SetFocus();
    windows_.push_back(main_app);
  }
}

WindowManager::~WindowManager() {
  // Shutdown threads
  Thread::Shutdown();

  // Delete all windows
  for (auto wind : windows_) {
    delete wind;
  }
}

void WindowManager::OnResize(uint32_t width, uint32_t height) {
  width_ = width;
  height_ = height;
  bitmap_.allocPixels(SkImageInfo::Make(width_, height_,
                                        color_type_, kPremul_SkAlphaType));
  SkSize new_size = SkSize::Make(width_, height_);
  // Notify all children
  for (auto wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnResize(new_size);
    }
  }
}

void WindowManager::OnReposition(uint32_t x, uint32_t y) {
}

void WindowManager::OnBtnPress(uint32_t x, uint32_t y, uint32_t modi) {
  Event evt = Event::Make(Event::Type::M_PRESS, x, y, modi);
  // Notify the focused child
  for (auto& wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnMousePress(evt);
    }
  }
}

void WindowManager::OnBtnRelease(uint32_t x, uint32_t y, uint32_t modi) {
  Event evt = Event::Make(Event::Type::M_RELEASE, x, y, modi);
  // Notify the focused child
  for (auto& wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnMouseRelease(evt);
    }
  }
}

void WindowManager::OnMotion(uint32_t x, uint32_t y, uint32_t modi) {
  Event evt = Event::Make(Event::Type::M_MOTION, x, y, modi);
  // Notify the focused child
  for (auto& wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnMouseMotion(evt);
    }
  }
}

void WindowManager::OnKeyPress(uint32_t key) {
  Event evt = Event::Make(Event::Type::K_DOWN, key);
  // Notify the focused child
  for (auto wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnKeyPress(evt);
    }
  }
}

void WindowManager::OnKeyRelease(uint32_t key) {
  Event evt = Event::Make(Event::Type::K_UP, key);
  // Notify the focused child
  for (auto wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnKeyRelease(evt);
    }
  }
}

void WindowManager::OnKeyChar(uint32_t key) {
  Event evt = Event::Make(Event::Type::K_CHAR, key);
  // Notify the focused child
  for (auto wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnKeyChar(evt);
    }
  }
}

void WindowManager::OnDraw(SkCanvas* canvas) {
  // Notify the focused child to draw on this canvas
  for (auto wind : windows_) {
    if (wind->IsFocus() == true) {
      wind->OnDraw(canvas);
    }
  }
}

void WindowManager::Invalidate() {
  OSWindow::Current()->SetDirty();
}

uint32_t WindowManager::GetX() const {
  return x_;
}

uint32_t WindowManager::GetY() const {
  return y_;
}

uint32_t WindowManager::GetWidth() const {
  return width_;
}

uint32_t WindowManager::GetHeight() const {
  return height_;
}

SkBitmap& WindowManager::GetBitmap() {
  return bitmap_;
}

void WindowManager::SetTitle(std::string& title) {
  OSWindow::Current()->SetTitle(title);
}

int32_t WindowManager::GetArgc() const {
  return OSWindow::Current()->GetArgc();
}

int8_t** WindowManager::GetArgv() const {
  return OSWindow::Current()->GetArgv();
}

WindowManager* WindowManager::Create(uint32_t x,
                                     uint32_t y,
                                     uint32_t width,
                                     uint32_t height) {
  if (instance_ != nullptr) {
    return instance_;
  }
  instance_ = new WindowManager(x, y, width, height);
  return instance_;
}

WindowManager* WindowManager::Current() {
  return instance_;
}

}  // namespace ui
}  // namespace merck

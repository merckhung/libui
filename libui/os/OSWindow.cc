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

#include "libui/os/OSWindow.h"

namespace merck {
namespace ui {

// Initialize the instance of the singleton to nullptr
OSWindow* OSWindow::instance_ = nullptr;

OSWindow::OSWindow()
  : x_(0),
    y_(0),
    width_(0),
    height_(0),
    wm_(nullptr),
    argc_(0),
    argv_(nullptr),
    is_dirty_(true) {
}

OSWindow::~OSWindow() {
}

void OSWindow::Resize(uint32_t width, uint32_t height) {
  width_ = width;
  height_ = height;
  // Notify the WindowManager about the resizing
  wm_->OnResize(width_, height_);
}

uint32_t OSWindow::GetWidth() const {
  return width_;
}

uint32_t OSWindow::GetHeight() const {
  return height_;
}

bool OSWindow::InitWindowManager() {
  wm_ = WindowManager::Create(x_, y_, width_, height_);
  if (wm_ == nullptr) {
    return false;
  }
  return true;
}

void OSWindow::DestroyWindowManager() {
  if (wm_ != nullptr) {
    delete wm_;
    wm_ = nullptr;
  }
}

WindowManager* OSWindow::GetWindowManager() const {
  return wm_;
}

int32_t OSWindow::GetArgc() const {
  return argc_;
}

int8_t** OSWindow::GetArgv() const {
  return argv_;
}

OSWindow* OSWindow::Current() {
  return instance_;
}

void OSWindow::SetDirty() {
  is_dirty_ = true;
}

void OSWindow::ClearDirty() {
  is_dirty_ = false;
}

bool OSWindow::IsDirty() const {
  return is_dirty_;
}

}  // namespace ui
}  // namespace merck

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

#include "libui/Window.h"
#include "libui/AnimateView.h"
#include "libui/Debugger.h"

namespace merck {
namespace ui {

Window* Window::Create(void* init,
                       void* destroy,
                       uint32_t width,
                       uint32_t height,
                       WindowManager* wm) {
  // Call the constructor to generate a new instance
  return new Window(init, destroy, width, height, wm);
}

Window::Window(void* init,
               void* destroy,
               uint32_t width,
               uint32_t height,
               WindowManager* wm)
  : INHERITED(),
    wm_(wm),
    destroy_(nullptr),
    dirty_(true) {
  // Set the boundary
  SetBound(0, 0, width, height);

  // Set focusable flag
  SetFlag(Attr::ATTR_FOCUSABLE);

  // Cast to the installer pointer
  bool (*installer)(Window*, int32_t, int8_t**)
      = reinterpret_cast<bool (*)(Window*, int32_t, int8_t**)>(init);

  // Cast to the un-installer pointer and register it
  destroy_ = reinterpret_cast<void (*)(Window*, int32_t, int8_t**)>(destroy);

  // Install Widgets by calling the installer
  installer(this, wm_->GetArgc(), wm_->GetArgv());

  // Do the resizing at first time in order to invoke the OnLayout
  SkSize size;
  size.set(SkIntToScalar(width), SkIntToScalar(height));
  OnResize(size);

  // Start the animator thread
  Thread* thread = Thread::CreateNativeThread(
      reinterpret_cast<void*>(AnimateView::AnimatorMain),
      this,
      reinterpret_cast<const uint8_t*>("Animator thread"));
  thread_list_.Register(thread);

  // Start the debugger thread
  thread = Thread::CreateNativeThread(
      reinterpret_cast<void*>(Debugger::DebuggerMain),
      this,
      reinterpret_cast<const uint8_t*>("Debugger thread"));
  thread_list_.Register(thread);
}

Window::~Window() {
  // Stop the animating thread
  thread_list_.DestroyAllThreads();

  if (destroy_ != nullptr) {
    // Call the destroy function for exiting the program
    destroy_(this, wm_->GetArgc(), wm_->GetArgv());
  }
}

void Window::SetTitle(std::string& title) {
  // Notify the window manager to set the title
  wm_->SetTitle(title);
}

void Window::OnDraw(SkCanvas *canvas) {
  if (dirty_ == true) {
    dirty_ = false;
    INHERITED::OnDraw(canvas);
  }
}

void Window::Invalidate(SkRect* bound) {
  dirty_ = true;
  wm_->Invalidate();
}

ThreadList& Window::GetThreadList() {
  return thread_list_;
}

}  // namespace ui
}  // namespace merck

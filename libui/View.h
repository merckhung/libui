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

#ifndef UI_LIBUI_VIEW_H_
#define UI_LIBUI_VIEW_H_

#include "libbase/macros.h"
#include "libui/Event.h"
#include "libui/Layout.h"

#include "SkCanvas.h"
#include "SkRect.h"
#include "SkPoint.h"

#include <memory>
#include <vector>
#include <iosfwd>
#include <iostream>

namespace merck {
namespace ui {

class View {
  friend class LinearLayout;  // For accessing children
  friend class StackLayout;   // For accessing children
  friend class AnimateView;   // For accessing AnimState

 public:
  static constexpr const char* VIEW_NAME = "View";

  static constexpr SkColor DefaultFgColor = 0xFF000000;
  static constexpr SkColor DefaultBgColor = 0xFFFFFFFF;
  static constexpr int32_t DefaultFontSize = 16;

  enum class Attr : uint32_t {
    ATTR_NONE         = 0x0000,
    ATTR_ENABLE       = 0x0001,
    ATTR_VISIBLE      = 0x0002,
    ATTR_FOCUSABLE    = 0x0004,
    ATTR_FOCUSED      = 0x0008,
    ATTR_BGCOLOR      = 0x0010,
    ATTR_TRANSPARENT  = 0x0020,
    ATTR_DEBUG        = 0x8000,
  };

  enum class AnimState : uint8_t {
    ANIM_NONE = 0,
    ANIM_INITIAL,
    ANIM_PROGRESS,
    ANIM_CANCEL,
    ANIM_CANCEL_PROGRESS,
    ANIM_END,
  };

 public:
  View();
  explicit View(uint32_t id);
  explicit View(uint32_t id, View* parent);
  virtual ~View();

  virtual void SetId(uint32_t id);
  virtual uint32_t GetId() const;

  // Attribute
  virtual void SetFlag(Attr flag);
  virtual void ClearFlag(Attr flag);
  virtual void ResetFlag();
  virtual bool IsFlag(Attr flag) const;

  // Enable & Visible
  virtual void EnVisible();
  virtual void DisVisible();
  virtual bool IsVisible() const;
  virtual void EnEnable();
  virtual void DisEnable();
  virtual bool IsEnable() const;

  // Debug
  virtual void EnDebug();
  virtual void DisDebug();
  virtual bool IsDebug() const;

  // Foreground color
  virtual void SetFgColor(SkColor color);
  virtual SkColor GetFgColor() const;

  // Background color
  virtual void SetBgColor(SkColor color);
  virtual SkColor GetBgColor() const;
  virtual void EnBgColor();
  virtual void DisBgColor();
  virtual bool IsBgColor() const;

  // Focus
  virtual bool SetFocus();
  virtual void ClearFocus();
  virtual bool IsFocus() const;

  // Coordinator
  virtual void SetBound(const SkRect& bound);
  virtual void SetBound(int32_t x, int32_t y, int32_t w, int32_t h);
  virtual const SkRect& GetBound() const;
  virtual int32_t GetWidth() const;
  virtual int32_t GetHeight() const;
  virtual void SetPos(SkPoint& pos);
  virtual void SetPos(int32_t x, int32_t y);
  virtual SkPoint GetPos() const;
  virtual int32_t GetX() const;
  virtual int32_t GetY() const;

  // Intersection
  virtual bool Intersect(const SkRect& bound);
  virtual bool Intersect(SkPoint& pos) const;
  virtual bool Intersect(int32_t x, int32_t y) const;
  virtual const SkSize& GetPreferSize();

  // Focus
  virtual bool AcceptFocus();

  // Parent and children views
  virtual void SetParentView(View* parent);
  virtual View* GetParentView() const;
  virtual bool IsParentView(View* guest) const;
  virtual void ResetParentView();

  virtual void AddChildView(View* view);
  virtual void AddChildView(View* view, size_t idx);
  virtual View* RemoveChildView();
  virtual View* RemoveChildView(View* view);
  virtual View* RemoveChildView(size_t idx);
  virtual View* GetChildView(size_t idx) const;
  virtual bool IsChildView(View* view) const;
  virtual void ResetChildViews();
  virtual size_t GetNrChildViews() const;

  // Mouse events
  virtual bool OnMousePress(Event& evt);
  virtual bool OnMouseRelease(Event& evt);
  virtual bool OnMouseMotion(Event& evt);

  // Keyboard events
  virtual bool OnKeyPress(Event& evt);
  virtual bool OnKeyRelease(Event& evt);
  virtual bool OnKeyChar(Event& evt);

  // Painting events
  virtual void OnPreDraw();
  virtual void OnDraw(SkCanvas* canvas);
  virtual void OnPostDraw();
  virtual void OnRedraw(SkCanvas* canvas, SkRect* bound);
  virtual void OnResize(SkSize& size);
  virtual void Invalidate(SkRect* bound);

  // AnimateState
  virtual AnimState GetAnimateState();
  virtual void OnAnimate();

  // Font size
  virtual int32_t GetFontSize() const;
  virtual void SetFontSize(int32_t sz);

  // Layout
  virtual void SetLayout(Layout* layout);
  virtual Layout* GetLayout() const;
  virtual void OnLayout();

  // Dump tools
  virtual void Dump(std::ostream& os, uint32_t level) const;

 protected:
  // Animation
  virtual void SetAnimateState(AnimState state);

  uint32_t id_;
  View* parent_;
  std::vector<View*> children_;
  std::unique_ptr<Layout> layout_;

  SkRect bound_;
  SkSize prefer_size_;
  SkColor fg_color_;
  SkColor bg_color_;
  Attr attr_;
  AnimState anim_state_;
  int32_t font_size_;
  const char* internal_name_;

 private:
  DISALLOW_COPY_AND_ASSIGN(View);
};

std::ostream& operator<<(std::ostream& os, const View& view);
std::ostream& operator<<(std::ostream& os, const View::Attr& attr);

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_VIEW_H_

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

#include "libui/View.h"

namespace merck {
namespace ui {

View::View()
  : id_(0),
    parent_(nullptr),
    fg_color_(DefaultFgColor),
    bg_color_(DefaultBgColor),
    attr_(Attr::ATTR_NONE),
    anim_state_(AnimState::ANIM_NONE),
    font_size_(DefaultFontSize),
    internal_name_(VIEW_NAME) {
  layout_.reset(nullptr);
  bound_.set(SkIntToScalar(0), SkIntToScalar(0), SkIntToScalar(0), SkIntToScalar(0));
  prefer_size_.set(SkIntToScalar(0), SkIntToScalar(0));
}

View::View(uint32_t id)
  : View() {
  id_ = id;
}

View::View(uint32_t id, View* parent)
  : View(id) {
  parent_ = parent;
}

View::~View() {
  // Free all children
  View* pView;
  while (true) {
    // Retrieve the pointer of one of children
    pView = RemoveChildView();
    if (pView == nullptr) {
      // If none left, mean all cleared
      return;
    }
    // Free this child
    delete pView;
  }
}

void View::SetId(uint32_t id) {
  id_ = id;
}

uint32_t View::GetId() const {
  return id_;
}

void View::SetFlag(Attr flag) {
  attr_ = static_cast<Attr>(static_cast<uint32_t>(attr_) | static_cast<uint32_t>(flag));
}

void View::ClearFlag(Attr flag) {
  attr_ = static_cast<Attr>(static_cast<uint32_t>(attr_) & ~static_cast<uint32_t>(flag));
}

void View::ResetFlag() {
  attr_ = Attr::ATTR_NONE;
}

bool View::IsFlag(Attr flag) const {
  if (static_cast<uint32_t>(attr_) & static_cast<uint32_t>(flag)) {
    return true;
  }
  return false;
}

void View::EnVisible() {
  SetFlag(Attr::ATTR_VISIBLE);
}

void View::DisVisible() {
  ClearFlag(Attr::ATTR_VISIBLE);
}

bool View::IsVisible() const {
  return IsFlag(Attr::ATTR_VISIBLE);
}

void View::EnEnable() {
  SetFlag(Attr::ATTR_ENABLE);
}

void View::DisEnable() {
  ClearFlag(Attr::ATTR_ENABLE);
}

bool View::IsEnable() const {
  return IsFlag(Attr::ATTR_ENABLE);
}

void View::EnDebug() {
  SetFlag(Attr::ATTR_DEBUG);
}

void View::DisDebug() {
  ClearFlag(Attr::ATTR_DEBUG);
}

bool View::IsDebug() const {
  return IsFlag(Attr::ATTR_DEBUG);
}

void View::SetFgColor(SkColor color) {
  fg_color_ = color;
}

SkColor View::GetFgColor() const {
  return fg_color_;
}

void View::SetBgColor(SkColor color) {
  bg_color_ = color;
}

SkColor View::GetBgColor() const {
  return bg_color_;
}

void View::EnBgColor() {
  SetFlag(Attr::ATTR_BGCOLOR);
}

void View::DisBgColor() {
  ClearFlag(Attr::ATTR_BGCOLOR);
}

bool View::IsBgColor() const {
  return IsFlag(Attr::ATTR_BGCOLOR);
}

void View::SetBound(const SkRect& bound) {
  bound_ = bound;
}

bool View::SetFocus() {
  if (IsFlag(Attr::ATTR_FOCUSABLE) == true) {
    SetFlag(Attr::ATTR_FOCUSED);
    return true;
  }
  return false;
}

void View::ClearFocus() {
  ClearFlag(Attr::ATTR_FOCUSED);
}

bool View::IsFocus() const {
  return IsFlag(Attr::ATTR_FOCUSED);
}

void View::SetBound(int32_t x, int32_t y, int32_t w, int32_t h) {
  bound_.set(SkIntToScalar(x), SkIntToScalar(y), SkIntToScalar(w), SkIntToScalar(h));
}

const SkRect& View::GetBound() const {
  return bound_;
}

int32_t View::GetWidth() const {
  return bound_.width();
}

int32_t View::GetHeight() const {
  return bound_.height();
}

void View::SetPos(SkPoint& pos) {
  bound_.set(pos.x(), pos.y(), pos.x() + bound_.width(), pos.y() + bound_.height());
}

void View::SetPos(int32_t x, int32_t y) {
  bound_.set(SkIntToScalar(x),
             SkIntToScalar(y),
             SkIntToScalar(x) + bound_.width(),
             SkIntToScalar(y) + bound_.height());
}

SkPoint View::GetPos() const {
  SkPoint pos = SkPoint::Make(bound_.left(), bound_.top());
  return pos;
}

int32_t View::GetX() const {
  return SkScalarTruncToInt(bound_.left());
}

int32_t View::GetY() const {
  return SkScalarTruncToInt(bound_.top());
}

bool View::Intersect(const SkRect& bound) {
  return bound_.intersect(bound);
}

bool View::Intersect(SkPoint& pos) const {
  if ((pos.x() >= bound_.left())
      && (pos.x() <= (bound_.left() + bound_.width()))
      && (pos.y() >= bound_.top())
      && (pos.y() <= (bound_.top() + bound_.height()))) {
    return true;
  }
  return false;
}

bool View::Intersect(int32_t x, int32_t y) const {
  if ((SkIntToScalar(x) >= bound_.left())
      && (SkIntToScalar(x) <= (bound_.left() + bound_.width()))
      && (SkIntToScalar(y) >= bound_.top())
      && (SkIntToScalar(y) <= (bound_.top() + bound_.height()))) {
    return true;
  }
  return false;
}

const SkSize& View::GetPreferSize() {
  return prefer_size_;
}

bool View::AcceptFocus() {
  return false;
}

void View::SetParentView(View* parent) {
  parent_ = parent;
}

View* View::GetParentView() const {
  return parent_;
}

bool View::IsParentView(View* guest) const {
  if (parent_ == guest) {
    return true;
  }
  return false;
}

void View::ResetParentView() {
  parent_ = nullptr;
}

void View::AddChildView(View* view) {
  view->SetParentView(this);
  children_.push_back(view);
}

void View::AddChildView(View* view, size_t idx) {
  std::vector<View*>::iterator it = children_.begin() + idx;
  view->SetParentView(this);
  children_.insert(it, view);
}

View* View::RemoveChildView() {
  if (children_.size() <= 0) {
    return nullptr;
  }
  // Retrieve the last child
  View* pView = children_.back();
  // Remove it from the list
  children_.pop_back();
  // Return it
  return pView;
}

View* View::RemoveChildView(View* view) {
  for (std::vector<View*>::iterator it = children_.begin();
       it != children_.end();
       ++it) {
    if (*it == view) {
      View* pView = *it;
      // Found it and then erase it
      children_.erase(it);
      return pView;
    }
  }
  // Doesn't see it in the vector container
  return nullptr;
}

View* View::RemoveChildView(size_t idx) {
  View* pView = GetChildView(idx);
  return RemoveChildView(pView);
}

View* View::GetChildView(size_t idx) const {
  if (idx >= children_.size()) {
    return nullptr;
  }
  return children_.at(idx);
}

bool View::IsChildView(View* view) const {
  for (auto child : children_) {
    if (child == view) {
      // Already have one in the vector container
      return true;
    }
  }
  // Doesn't exist in the vector container
  return false;
}

void View::ResetChildViews() {
  children_.empty();
}

size_t View::GetNrChildViews() const {
  return children_.size();
}

bool View::OnMousePress(Event& evt) {
  return false;
}

bool View::OnMouseRelease(Event& evt) {
  return false;
}

bool View::OnMouseMotion(Event& evt) {
  return false;
}

bool View::OnKeyPress(Event& evt) {
  return false;
}

bool View::OnKeyRelease(Event& evt) {
  return false;
}

bool View::OnKeyChar(Event& evt) {
  return false;
}

void View::OnPreDraw() {
}

void View::OnDraw(SkCanvas* canvas) {
}

void View::OnPostDraw() {
}

void View::OnRedraw(SkCanvas* canvas, SkRect* bound) {
}

void View::OnResize(SkSize& size) {
  OnLayout();
}

void View::Invalidate(SkRect* bound) {
}

void View::SetAnimateState(AnimState state) {
  anim_state_ = state;
}

View::AnimState View::GetAnimateState() {
  return anim_state_;
}

void View::OnAnimate() {
}

int32_t View::GetFontSize() const {
  return font_size_;
}

void View::SetFontSize(int32_t sz) {
  font_size_ = sz;
}

void View::SetLayout(Layout* layout) {
  layout_.reset(layout);
}

Layout* View::GetLayout() const {
  return layout_.get();
}

void View::OnLayout() {
  if (layout_.get() != nullptr) {
    layout_->OnLayout(this);
  }
}

void View::Dump(std::ostream& os, uint32_t level) const {
  for (uint32_t i = 0; i < level; ++i) {
    os << "|";
  }
  os << "View[";
  os << "id=" << GetId();
  os << ",name=" << internal_name_;
  os << ",parent=" << GetParentView();
  os << ",layout=" << GetLayout();
  const SkRect& b = GetBound();
  os << ",bound=" << "{" << b.x() << "," << b.y() << "," << b.width() <<"," << b.height() << "}";
  os << ",fg_color=" << GetFgColor();
  os << ",bg_color=" << GetBgColor();
  os << ",attribute=" << attr_ << "\n";
  for (uint32_t i = 0; i < level; ++i) {
    os << "|";
  }
  os << "\t,child=" << GetNrChildViews() << "]\n";
  if (GetNrChildViews() > 0) {
    for (std::vector<View*>::const_iterator it = children_.begin();
         it != children_.end();
         ++it) {
      (*it)->Dump(os, level + 1);
    }
  }
}

std::ostream& operator<<(std::ostream& os, const View& view) {
  view.Dump(os, 0);
  return os;
}

std::ostream& operator<<(std::ostream& os, const View::Attr& attr) {
  os << static_cast<uint32_t>(attr);
  return os;
}

}  // namespace ui
}  // namespace merck

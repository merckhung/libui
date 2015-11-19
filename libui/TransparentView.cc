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

#include "libui/TransparentView.h"

namespace merck {
namespace ui {

TransparentView::TransparentView()
  : INHERITED() {
  SetFlag(Attr::ATTR_TRANSPARENT);
  SetFlag(Attr::ATTR_ENABLE);
  internal_name_ = VIEW_NAME;
}

TransparentView::~TransparentView() {
}

void TransparentView::OnDraw(SkCanvas* canvas) {
  // Draw the background color, if enabled
  if (IsBgColor() == true) {
    canvas->drawColor(bg_color_);
  }
  // Literally call all children to draw on this canvas
  for (auto child : children_) {
    // If it's either a LAYOUT or visible WIDGET,
    // distribute this drawing event to it.
    // If not, ignore it.
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true
        || child->IsVisible() == true) {
      if (child->GetAnimateState() == AnimState::ANIM_NONE) {
        // If it's not an AnimateView, simply call the OnDraw()
        child->OnDraw(canvas);
      } else {
        // If it's an AnimateView, has to call Pre/Post additionally
        // In order to keep synchronization
        child->OnPreDraw();
        child->OnDraw(canvas);
        child->OnPostDraw();
      }
    }
  }
}

void TransparentView::Invalidate(SkRect* bound) {
  if (parent_ != nullptr) {
    parent_->Invalidate(bound);
  }
}

void TransparentView::OnAnimate() {
  for (auto child : children_) {
    child->OnAnimate();
  }
}

void TransparentView::OnResize(SkSize& size) {
  for (auto child : children_) {
    // Notify all children to do resizing
    if (layout_ != nullptr) {
      // Invoke the layout
      layout_->OnLayout(this);
    }
    // Call child's OnResize();
    child->OnResize(size);
  }
}

bool TransparentView::OnMousePress(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    // Check if it's intersected, if not, just ignore this view
    if (child->Intersect(evt.GetX(), evt.GetY()) == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnMousePress(evt);
      continue;
    }
    if (child->OnMousePress(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

bool TransparentView::OnMouseRelease(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    // Check if it's intersected, if not, just ignore this view
    if (child->Intersect(evt.GetX(), evt.GetY()) == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnMouseRelease(evt);
      continue;
    }
    if (child->OnMouseRelease(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

bool TransparentView::OnMouseMotion(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    // Check if it's intersected, if not, just ignore this view
    if (child->Intersect(evt.GetX(), evt.GetY()) == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnMouseMotion(evt);
      continue;
    }
    if (child->OnMouseMotion(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

bool TransparentView::OnKeyPress(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnKeyPress(evt);
      continue;
    }
    if (child->OnKeyPress(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

bool TransparentView::OnKeyRelease(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnKeyRelease(evt);
      continue;
    }
    if (child->OnKeyRelease(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

bool TransparentView::OnKeyChar(Event& evt) {
  // Iterate each child to find out which one to distribute this event
  for (auto child : children_) {
    // Check if it's enabled
    if (child->IsEnable() == false) {
      continue;
    }
    if (child->IsFlag(Attr::ATTR_TRANSPARENT) == true) {
      // If it's a LAYOUT view, distribute this event to it
      // And let it do the rest
      child->OnKeyChar(evt);
      continue;
    }
    if (child->OnKeyChar(evt) == true) {
      // If it's not a LAYOUT view, but a WIDGET one.
      // Try to call its handler to see whether it will be handled.
      // If so, terminate this process.
      // If not, continue to check out next WIDGET view.
      return true;
    }
  }
  return false;
}

}  // namespace ui
}  // namespace merck

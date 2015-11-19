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

#include "libui/AnimateView.h"
#include "libui/Window.h"
#include "libbase/Mutex.h"

namespace merck {
namespace ui {

AnimateView::AnimateView()
  : INHERITED(),
    anim_end_(false) {
  SetAnimateState(AnimState::ANIM_INITIAL);
  internal_name_ = VIEW_NAME;
}

AnimateView::AnimateView(uint32_t id)
  : INHERITED(id) {
}

AnimateView::AnimateView(uint32_t id, View* parent)
  : INHERITED(id, parent) {
}

AnimateView::~AnimateView() {
}

void AnimateView::OnPreDraw() {
  anim_lock_.ExclusiveLock();
}

void AnimateView::OnPostDraw() {
  anim_lock_.ExclusiveUnlock();
}

void* AnimateView::AnimatorMain(void* arg) {
  Window* window = reinterpret_cast<Window*>(arg);
  for (;;) {
    for (auto child : window->children_) {
      child->OnAnimate();
    }
    usleep(10);
  }
  return nullptr;
}

void AnimateView::OnAnimate() {
  for (auto it : children_) {
    // If its Animate State if NONE, then it means that it's not
    // an AnimateView, simply ignore this child
    if (it->GetAnimateState() == AnimState::ANIM_NONE) {
      continue;
    }

    // Case this child from View* to AnimateView*
    AnimateView* child = reinterpret_cast<AnimateView*>(it);

    // Standard animate state machine
    if (child->GetAnimateState() == AnimState::ANIM_INITIAL) {
      // If it's in INITIAL state, call OnAnimateStart() and then
      // if succeed, turn it to PROGRESS state.
      anim_end_ = false;
      if (child->OnAnimateStart() == false) {
        // If not succeed, turn it to END state and prevent OnAnimateEnd()
        // from being called.
        anim_end_ = true;
        SetAnimateState(AnimState::ANIM_END);
      } else {
        SetAnimateState(AnimState::ANIM_PROGRESS);
      }
    }
    // If it's in PROGRESS state, continue to call OnAnimateProgress()
    // until it reaches a new state
    if (child->GetAnimateState() == AnimState::ANIM_PROGRESS) {
      child->OnAnimateProgress();
    }
    // If it's in CANCAL state, call the OnAnimateCancelStart() first,
    // then turn it to CANCEL_PROGRESS state.
    if (child->GetAnimateState() == AnimState::ANIM_CANCEL) {
      if (child->OnAnimateCancelStart() == false) {
        // If not succeed, turn it to END state and prevent OnAnimateEnd()
        // from being called.
        anim_end_ = true;
        SetAnimateState(AnimState::ANIM_END);
      } else {
        // If succeed, turn it to CANCEL_PROGRESS state
        SetAnimateState(AnimState::ANIM_CANCEL_PROGRESS);
      }
    }
    // If it's in CANCEL_PROGRESS state, continue to call the
    // OnAnimateCancelProgress() until it reaches a new state
    if (child->GetAnimateState() == AnimState::ANIM_CANCEL_PROGRESS) {
      child->OnAnimateCancelProgress();
    }
    // If it's first time in END state, call the OnAnimateEnd()
    if (anim_end_ == false && child->GetAnimateState() == AnimState::ANIM_END) {
      anim_end_ = true;
      child->OnAnimateEnd();
    }
  }
}

bool AnimateView::OnAnimateStart() {
  return false;
}

AnimateView::AnimState AnimateView::OnAnimateProgress() {
  return AnimState::ANIM_END;
}

bool AnimateView::OnAnimateCancelStart() {
  return false;
}

AnimateView::AnimState AnimateView::OnAnimateCancelProgress() {
  return AnimState::ANIM_END;
}

void AnimateView::OnAnimateEnd() {
}

}  // namespace ui
}  // namespace merck

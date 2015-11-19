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

#include "libui/Event.h"

namespace merck {
namespace ui {

Event::Event()
  : type_(Event::Type::NONE),
    modi_(0) {
  pos_.set(SkIntToScalar(0), SkIntToScalar(0));
}

Event::Event(Event::Type type, int32_t x, int32_t y, int32_t modi)
  : type_(type) {
  pos_.set(SkIntToScalar(x), SkIntToScalar(y));
  modi_ = modi;
}

Event::~Event() {
}

void Event::Set(Event::Type type, int32_t x, int32_t y, int32_t modi) {
  type_ = type;
  pos_.set(SkIntToScalar(x), SkIntToScalar(y));
  modi_ = modi;
}

void Event::Set(Event::Type type, SkPoint& point, int32_t modi) {
  type_ = type;
  pos_ = point;
  modi_ = modi;
}

void Event::Set(Event::Type type) {
  type_ = type;
}

void Event::Set(int32_t x, int32_t y, int32_t modi) {
  pos_.set(SkIntToScalar(x), SkIntToScalar(y));
  modi_ = modi;
}

void Event::Set(SkPoint& pos, int32_t modi) {
  pos_ = pos;
  modi_ = modi;
}

Event::Type Event::GetType() const {
  return type_;
}

int32_t Event::GetX() const {
  return pos_.x();
}

int32_t Event::GetY() const {
  return pos_.y();
}

int32_t Event::GetModi() const {
  return modi_;
}

const SkPoint& Event::GetPoint() const {
  return pos_;
}

void Event::Set(Event::Type type, uint32_t key) {
  type_ = type;
  key_ = key;
}

uint32_t Event::GetKey() const {
  return key_;
}

Event Event::Make(Event::Type type, int32_t x, int32_t y, int32_t modi) {
  Event evt;
  evt.Set(type, x, y, modi);
  return evt;
}

Event Event::Make(Event::Type type, uint32_t key) {
  Event evt;
  evt.Set(type, key);
  return evt;
}

}  // namespace ui
}  // namespace merck

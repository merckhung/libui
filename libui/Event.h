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

#ifndef UI_LIBUI_EVENT_H_
#define UI_LIBUI_EVENT_H_

#include "libbase/macros.h"

#include "SkPoint.h"

namespace merck {
namespace ui {

class Event {
 public:
  enum Type {
    NONE = 0,
    K_UP,
    K_DOWN,
    K_CHAR,
    M_PRESS,
    M_RELEASE,
    M_MOTION,
    M_DRAG,
  };

 public:
  Event();
  explicit Event(Type type, int32_t x, int32_t y, int32_t modi);
  virtual ~Event();

  virtual void Set(Type type, int32_t x, int32_t y, int32_t modi);
  virtual void Set(Type type, SkPoint& point, int32_t modi);
  virtual void Set(Type type);
  virtual void Set(int32_t x, int32_t y, int32_t modi);
  virtual void Set(SkPoint& point, int32_t modi);
  virtual Type GetType() const;
  virtual int32_t GetX() const;
  virtual int32_t GetY() const;
  virtual int32_t GetModi() const;
  virtual const SkPoint& GetPoint() const;
  virtual void Set(Type type, uint32_t key);
  virtual uint32_t GetKey() const;

  static Event Make(Event::Type type, int32_t x, int32_t y, int32_t modi);
  static Event Make(Type type, uint32_t key);

 protected:
  Type type_;
  SkPoint pos_;
  int32_t modi_;
  uint32_t key_;
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_EVENT_H_

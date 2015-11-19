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

#ifndef UI_LIBUI_BUTTON_H_
#define UI_LIBUI_BUTTON_H_

#include "libui/Widget.h"

namespace merck {
namespace ui {

class Button;

class ButtonListener {
 public:
  virtual void ButtonPressed(Button* sender, const Event& evt) {
  }

  virtual void ButtonReleased(Button* sender, const Event& evt) {
  }

 protected:
  virtual ~ButtonListener() {}
};

class Button : public Widget {
 public:
  static constexpr const char* VIEW_NAME = "Button";

  enum State {
    STATE_NORMAL = 0,
    STATE_HOVERED,
    STATE_PRESSED,
    STATE_DISABLED,
    STATE_COUNT,
  };
  enum Style {
    STYLE_STANDARD = 0,
    STYLE_TEXT,
  };

 public:
  explicit Button(ButtonListener* listener, std::string& text);
  explicit Button(ButtonListener* listener, std::string& text, int32_t w, int32_t h);
  virtual ~Button();

  virtual void NotifyClick(const ui::Event& evt);
  virtual void NotifyUnclick(const Event& evt);

  virtual State GetState() const;
  virtual Style GetStyle() const;

  virtual void OnDraw(SkCanvas* canvas) override;

  virtual bool OnMousePress(Event& evt) override;
  virtual bool OnMouseRelease(Event& evt) override;
  virtual bool OnMouseMotion(Event& evt) override;

  virtual const SkSize& GetPreferSize() override;

 protected:
  State state_;
  Style style_;
  ButtonListener* listener_;

 private:
  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Button);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_BUTTON_H_

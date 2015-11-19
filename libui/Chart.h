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

#ifndef UI_LIBUI_CHART_H_
#define UI_LIBUI_CHART_H_

#include "libui/Widget.h"
#include "libbase/Mutex.h"

#include <list>

namespace merck {
namespace ui {

static constexpr size_t MAX_COLUMNS = 20;

class ChartData {
 public:
  ChartData(uint64_t timestamp, float value)
    : timestamp_(timestamp),
      value_(value) {}
  ~ChartData() {}

  void Set(uint64_t timestamp, float value) {
    timestamp_ = timestamp;
    value_ = value;
  }
  uint64_t GetTimestamp() const {
    return timestamp_;
  }
  float GetValue() const {
    return value_;
  }

 private:
  uint64_t timestamp_;
  float value_;
};

class Chart : public Widget {
 public:
  static constexpr const char* VIEW_NAME = "Chart";

 public:
  explicit Chart(int32_t width, int32_t height);
  virtual ~Chart();

  virtual void OnDraw(SkCanvas* canvas) override;
  virtual bool OnMousePress(Event& evt) override;
  virtual bool OnMouseRelease(Event& evt) override;
  virtual bool OnMouseMotion(Event& evt) override;

  virtual const SkSize& GetPreferSize() override;

  void AddValue(ChartData& val);
  void Clear();

 private:
  std::list<ChartData> data_;
  std::unique_ptr<Mutex> data_lock_;

  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Chart);
};

}  // namespace ui
}  // namespace merck

#endif  // UI_LIBUI_CHART_H_

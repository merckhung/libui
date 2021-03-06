/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "Benchmark.h"
#include "SkCanvas.h"
#include "SkPath.h"
#include "SkString.h"

static void make_path(SkPath& path) {
    #include "BigPathBench.inc"
}

enum Align {
    kLeft_Align,
    kMiddle_Align,
    kRight_Align
};

const char* gAlignName[] = { "left", "middle", "right" };

// Inspired by crbug.com/455429
class BigPathBench : public Benchmark {
    SkPath      fPath;
    SkString    fName;
    Align       fAlign;
    bool        fRound;

public:
    BigPathBench(Align align, bool round) : fAlign(align), fRound(round) {
        fName.printf("bigpath_%s", gAlignName[fAlign]);
        if (round) {
            fName.append("_round");
        }
    }

protected:
    const char* onGetName() SK_OVERRIDE {
        return fName.c_str();
    }

    SkIPoint onGetSize() SK_OVERRIDE {
        return SkIPoint::Make(640, 100);
    }

    void onPreDraw() SK_OVERRIDE {
        make_path(fPath);
    }

    void onDraw(const int loops, SkCanvas* canvas) SK_OVERRIDE {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2);
        if (fRound) {
            paint.setStrokeJoin(SkPaint::kRound_Join);
        }
        this->setupPaint(&paint);

        const SkRect r = fPath.getBounds();
        switch (fAlign) {
            case kLeft_Align:
                canvas->translate(-r.left(), 0);
                break;
            case kMiddle_Align:
                break;
            case kRight_Align:
                canvas->translate(640 - r.right(), 0);
                break;
        }

        for (int i = 0; i < loops; i++) {
            canvas->drawPath(fPath, paint);
        }
    }

private:
    typedef Benchmark INHERITED;
};

DEF_BENCH( return new BigPathBench(kLeft_Align,     false); )
DEF_BENCH( return new BigPathBench(kMiddle_Align,   false); )
DEF_BENCH( return new BigPathBench(kRight_Align,    false); )

DEF_BENCH( return new BigPathBench(kLeft_Align,     true); )
DEF_BENCH( return new BigPathBench(kMiddle_Align,   true); )
DEF_BENCH( return new BigPathBench(kRight_Align,    true); )


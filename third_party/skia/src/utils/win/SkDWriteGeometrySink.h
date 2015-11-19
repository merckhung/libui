/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkDWriteToPath_DEFINED
#define SkDWriteToPath_DEFINED

#include "SkTypes.h"

class SkPath;

#include <dwrite.h>
#include <d2d1.h>

class SkDWriteGeometrySink : public IDWriteGeometrySink {
private:
    LONG fRefCount;
    SkPath* fPath;

    SkDWriteGeometrySink(const SkDWriteGeometrySink&);
    SkDWriteGeometrySink& operator=(const SkDWriteGeometrySink&);

protected:
    explicit SkDWriteGeometrySink(SkPath* path);
    virtual ~SkDWriteGeometrySink();

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **object) SK_OVERRIDE;
    ULONG STDMETHODCALLTYPE AddRef(void) SK_OVERRIDE;
    ULONG STDMETHODCALLTYPE Release(void) SK_OVERRIDE;

    void STDMETHODCALLTYPE SetFillMode(D2D1_FILL_MODE fillMode) SK_OVERRIDE;
    void STDMETHODCALLTYPE SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags) SK_OVERRIDE;
    void STDMETHODCALLTYPE BeginFigure(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin) SK_OVERRIDE;
    void STDMETHODCALLTYPE AddLines(const D2D1_POINT_2F *points, UINT pointsCount) SK_OVERRIDE;
    void STDMETHODCALLTYPE AddBeziers(const D2D1_BEZIER_SEGMENT *beziers, UINT beziersCount) SK_OVERRIDE;
    void STDMETHODCALLTYPE EndFigure(D2D1_FIGURE_END figureEnd) SK_OVERRIDE;
    HRESULT STDMETHODCALLTYPE Close() SK_OVERRIDE;

    static HRESULT Create(SkPath* path, IDWriteGeometrySink** geometryToPath);
};

#endif

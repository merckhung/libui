/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSwizzler_DEFINED
#define SkSwizzler_DEFINED

#include "SkCodec.h"
#include "SkColor.h"
#include "SkImageInfo.h"

class SkSwizzler : public SkNoncopyable {
public:
    /**
     *  Enum describing the config of the source data.
     */
    enum SrcConfig {
        kGray,
        kIndex1,
        kIndex2,
        kIndex4,
        kIndex,
        kRGB,
        kBGR,
        kRGBX,
        kBGRX,
        kRGBA,
        kBGRA,
        kRGB_565,
    };

    /*
     *
     * Result code for the alpha components of a row.
     *
     */
    typedef uint16_t ResultAlpha;
    static const ResultAlpha kOpaque_ResultAlpha = 0xFFFF;
    static const ResultAlpha kTransparent_ResultAlpha = 0x0000;

    /*
     *
     * Checks if the result of decoding a row indicates that the row was
     * transparent.
     *
     */
    static bool IsTransparent(ResultAlpha r) {
        return kTransparent_ResultAlpha == r;
    }

    /*
     *
     * Checks if the result of decoding a row indicates that the row was
     * opaque.
     *
     */
    static bool IsOpaque(ResultAlpha r) {
        return kOpaque_ResultAlpha == r;
    }
    
    /*
     *
     * Constructs the proper result code based on accumulated alpha masks
     *
     */
    static ResultAlpha GetResult(uint8_t zeroAlpha, uint8_t maxAlpha);

    /*
     *
     * Returns bits per pixel for source config
     *
     */
    static int BitsPerPixel(SrcConfig sc) {
        switch (sc) {
            case kIndex1:
                return 1;
            case kIndex2:
                return 2;
            case kIndex4:
                return 4;
            case kGray:
            case kIndex:
                return 8;
            case kRGB_565:
                return 16;
            case kRGB:
            case kBGR:
                return 24;
            case kRGBX:
            case kRGBA:
            case kBGRX:
            case kBGRA:
                return 32;
            default:
                SkASSERT(false);
                return 0;
        }
    }

    /*
     *
     * Returns bytes per pixel for source config
     * Raises an error if each pixel is not stored in an even number of bytes
     *
     */
    static int BytesPerPixel(SrcConfig sc) {
        SkASSERT(SkIsAlign8(BitsPerPixel(sc)));
        return BitsPerPixel(sc) >> 3;
    }

    /**
     *  Create a new SkSwizzler.
     *  @param SrcConfig Description of the format of the source.
     *  @param SkImageInfo dimensions() describe both the src and the dst.
     *              Other fields describe the dst.
     *  @param dst Destination to write pixels. Must match info and dstRowBytes
     *  @param dstRowBytes rowBytes for dst.
     *  @param ZeroInitialized Whether dst is zero-initialized. The
                               implementation may choose to skip writing zeroes
     *                         if set to kYes_ZeroInitialized.
     *  @return A new SkSwizzler or NULL on failure.
     */
    static SkSwizzler* CreateSwizzler(SrcConfig, const SkPMColor* ctable,
                                      const SkImageInfo&, void* dst,
                                      size_t dstRowBytes,
                                      SkImageGenerator::ZeroInitialized);
    /**
     *  Swizzle the next line. Call height times, once for each row of source.
     *  @param src The next row of the source data.
     *  @return A result code describing if the row was fully opaque, fully
     *          transparent, or neither
     */
    ResultAlpha next(const uint8_t* SK_RESTRICT src);

    /**
     *
     * Alternate version of next that allows the caller to specify the row.
     * It is very important to only use one version of next.  Since the other
     * version modifies the dst pointer, it will change the behavior of this
     * function.  We will check this in Debug mode.
     *
     */
    ResultAlpha next(const uint8_t* SK_RESTRICT src, int y);
private:

#ifdef SK_DEBUG
    /*
     *
     * Keep track of which version of next the caller is using
     *
     */
    enum NextMode {
        kUninitialized_NextMode,
        kConsecutive_NextMode,
        kDesignateRow_NextMode,
    };

    NextMode fNextMode;
#endif

    /**
     *  Method for converting raw data to Skia pixels.
     *  @param dstRow Row in which to write the resulting pixels.
     *  @param src Row of src data, in format specified by SrcConfig
     *  @param width Width in pixels
     *  @param deltaSrc if bitsPerPixel % 8 == 0, deltaSrc is bytesPerPixel
     *                  else, deltaSrc is bitsPerPixel
     *  @param y Line of source.
     *  @param ctable Colors (used for kIndex source).
     */
    typedef ResultAlpha (*RowProc)(void* SK_RESTRICT dstRow,
                                   const uint8_t* SK_RESTRICT src,
                                   int width, int deltaSrc, int y,
                                   const SkPMColor ctable[]);

    const RowProc       fRowProc;
    const SkPMColor*    fColorTable;      // Unowned pointer
    const int           fDeltaSrc;        // if bitsPerPixel % 8 == 0
                                          //     deltaSrc is bytesPerPixel
                                          // else
                                          //     deltaSrc is bitsPerPixel
    const SkImageInfo   fDstInfo;
    void*               fDstRow;
    const size_t        fDstRowBytes;
    int                 fCurrY;

    SkSwizzler(RowProc proc, const SkPMColor* ctable, int deltaSrc,
               const SkImageInfo& info, void* dst, size_t rowBytes);

};
#endif // SkSwizzler_DEFINED

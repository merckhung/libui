# SKIA
set(SKIASRC ${PROJECT_SOURCE_DIR}/third_party/skia)

set(SKIA_CXX_FLAGS "-DSK_INTERNAL -DSK_GAMMA_SRGB -DSK_GAMMA_APPLY_TO_A8 -DSK_SCALAR_TO_FLOAT_EXCLUDED -DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1 -DSK_SUPPORT_GPU=1 -DSK_SUPPORT_OPENCL=0 -DSK_FORCE_DISTANCEFIELD_FONTS=0 -DSK_PICTURE_USE_SK_RECORD -DSK_SCALAR_IS_FLOAT -DSK_SAMPLES_FOR_X -DSK_USE_POSIX_THREADS -DSK_SYSTEM_ZLIB=1 -DSK_DEVELOPER=1 -msse2 -mfpmath=sse -g -fno-exceptions -fstrict-aliasing -Wall -Wextra -Winit-self -Wpointer-arith -Wno-unused-parameter -fno-rtti -Wnon-virtual-dtor -Wno-invalid-offsetof -I${SKIASRC}/src/gpu -I${SKIASRC}/tools/flags -I${SKIASRC}/include/config -I${SKIASRC}/include/core -I${SKIASRC}/include/pathops -I${SKIASRC}/include/pipe -I${SKIASRC}/include/effects -I${SKIASRC}/include/images -I${SKIASRC}/include/ports -I${SKIASRC}/src/sfnt -I${SKIASRC}/include/utils -I${SKIASRC}/src/utils -I${SKIASRC}/include/gpu -I${SKIASRC}/include/views -I${SKIASRC}/include/xml -I${SKIASRC}/include/utils/mac -DSK_BUILD_FOR_UNIX")



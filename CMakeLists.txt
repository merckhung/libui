cmake_minimum_required(VERSION 2.8.8)

project(UI)
include(FindThreads)

set(CMAKE_BUILD_TYPE "Debug")

# Revision
set(UI_VERSION_MAJOR 1)
set(UI_VERSION_MINOR 0)
set(UI_VERSION_PATCH 0)

# SKIA
include(tools/FindSKIA.cmake)

include_directories(${PROJECT_SOURCE_DIR})

# CXX FLAGS
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} ${LLVM_COMPILE_FLAGS} -g3 -D__STDC_INT64__ -D__STDC_CONSTANT_MACROS=1 -D__STDC_FORMAT_MACROS")

macro(TARGET_LINK_SKIA TARGET)
  target_link_libraries(${TARGET} ui base)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKIA_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "../libui/libui.a -Wl,--start-group ${SKIASRC}/out/Debug/libskia_xml.a ${SKIASRC}/out/Debug/libskia_core.a ${SKIASRC}/out/Debug/libskia_codec.a ${SKIASRC}/out/Debug/obj/gyp/libpng.a ${SKIASRC}/out/Debug/libskia_effects.a -Wl,--whole-archive ${SKIASRC}/out/Debug/libskia_images.a -Wl,--no-whole-archive ${SKIASRC}/out/Debug/libskia_utils.a ${SKIASRC}/out/Debug/libskia_opts.a ${SKIASRC}/out/Debug/libskia_opts_ssse3.a ${SKIASRC}/out/Debug/libskia_opts_sse41.a ${SKIASRC}/out/Debug/libskia_ports.a ${SKIASRC}/out/Debug/libskia_sfnt.a ${SKIASRC}/out/Debug/libskia_skgpu.a ${SKIASRC}/out/Debug/obj/gyp/libflags.a ${SKIASRC}/out/Debug/obj/gyp/libjpeg.a ${SKIASRC}/out/Debug/obj/gyp/libetc1.a ${SKIASRC}/out/Debug/obj/gyp/libSkKTX.a ${SKIASRC}/out/Debug/obj/gyp/libwebp_dec.a ${SKIASRC}/out/Debug/obj/gyp/libwebp_demux.a ${SKIASRC}/out/Debug/obj/gyp/libwebp_dsp.a ${SKIASRC}/out/Debug/obj/gyp/libwebp_enc.a ${SKIASRC}/out/Debug/obj/gyp/libwebp_utils.a -Wl,--end-group -lz -lgif -lpthread -lfontconfig -ldl -lfreetype -lGL -lGLU -lX11")
endmacro(TARGET_LINK_SKIA)

add_subdirectory(libbase)
add_subdirectory(libui)
add_subdirectory(VisualDemo)

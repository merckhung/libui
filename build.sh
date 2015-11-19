#!/bin/bash

./third_party/skia/gyp_skia
ninja -C ./third_party/skia/out/Debug SampleApp
make

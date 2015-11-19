project(VisualDemo)

add_executable(VisualDemo
  VisualDemo.cc)

target_link_libraries(VisualDemo base)
TARGET_LINK_SKIA(VisualDemo)

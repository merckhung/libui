project(libui)

add_library(ui STATIC
  Debugger.cc
  AnimateView.cc
  SelectBox.cc
  TextField.cc
  ImageBox.cc
  Chart.cc
  DirectButton.cc
  Button.cc
  Event.cc
  Label.cc
  Layout.cc
  LinearLayout.cc
  StackLayout.cc
  TransparentView.cc
  View.cc
  Widget.cc
  Window.cc
  WindowManager.cc
  os/OSWindow.cc
  os/linux/LinuxOSWindow.cc
  os/linux/keysym2ucs.cc
  os/linux/main.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKIA_CXX_FLAGS}")

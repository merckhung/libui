project(libbase)

add_library(base STATIC
  Mutex.cc
  Thread.cc
  ThreadList.cc
  stringprintf.cc)

cmake_minimum_required(VERSION 3.2)

project(cfastcdr VERSION 1.0.0)

find_package(fastcdr)

add_library(cfastcdr SHARED cfastcdr.cxx)
target_link_libraries(cfastcdr fastcdr)

set_target_properties(cfastcdr PROPERTIES
  PUBLIC_HEADER cfastcdr.h
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS cfastcdr
  PUBLIC_HEADER)

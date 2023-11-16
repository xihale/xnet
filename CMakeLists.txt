cmake_minimum_required(VERSION 3.5.0)
project(xnet VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

# add include
include_directories(lib)

# 直接检测系统头文件目录下是否有 头文件 xihale/json.hpp  
find_path(XJSON NAMES xihale/json.hpp)

# 循环添加 tests 文件夹下的所有文件, 编译
file(GLOB TEST_SOURCES tests/*.cpp)
foreach(TEST_SOURCE ${TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  if(${TEST_NAME} STREQUAL "music163")
    if(NOT XJSON)
      message(STATUS "xjson.hpp not found")
      continue()
    endif()
  endif()
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  # target_link_libraries(${TEST_NAME} xsocket)
  if(${TEST_NAME} STREQUAL "echoServer" OR ${TEST_NAME} STREQUAL "echoClient")
    continue()
  endif()
  add_test(${TEST_NAME} ${TEST_NAME})
endforeach()

add_test(NAME echo COMMAND sh -c "./echoServer & ./echoClient")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

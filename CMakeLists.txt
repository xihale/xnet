cmake_minimum_required(VERSION 3.0.0)
project(xsocket VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

# add_library(xsocket xsocket.cpp)

# test
# add_executable(xsocket_test test.cpp)

# 循环添加 tests 文件夹下的所有文件, 编译执行
file(GLOB TEST_SOURCES tests/*.cpp)
foreach(TEST_SOURCE ${TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  # target_link_libraries(${TEST_NAME} xsocket)
  # add_test(${TEST_NAME} ${TEST_NAME})
endforeach()

add_test(httpPost httpPost)
# add_test(echo ./echoServer & ./echoClient)

# target_link_libraries(xsocket_test xsocket)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

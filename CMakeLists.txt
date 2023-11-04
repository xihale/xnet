cmake_minimum_required(VERSION 3.0.0)
project(xsocket VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

# add_library(xsocket xsocket.cpp)

# test
add_executable(xsocket_test test.cpp)
# target_link_libraries(xsocket_test xsocket)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

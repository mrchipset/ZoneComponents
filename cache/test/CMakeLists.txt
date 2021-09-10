cmake_minimum_required(VERSION 3.0.0)
project(ZoneComponents_Pool VERSION 0.1.0)

include(CTest)
enable_testing()


include_directories(../)

add_executable(testLRUCache testLRUCache.cc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cmake_minimum_required(VERSION 3.6)
project(src)
add_definitions(-DNOT_PEBBLE_BUILD)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("~/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/chalk/include" ../build/chalk/src)
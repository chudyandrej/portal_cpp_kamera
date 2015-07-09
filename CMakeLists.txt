cmake_minimum_required(VERSION 3.2)
project(portals)
find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(portals ${SOURCE_FILES} kalmanCont.cpp kalmanCont.h cJSON.h cJSON.c )
target_link_libraries(portals  ${OpenCV_LIBS})
target_link_libraries(portals  curl curlpp)
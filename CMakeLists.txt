project(portals)
find_package( OpenCV REQUIRED )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -d -g")

set(SOURCE_FILES main.cpp)
add_executable(portals ${SOURCE_FILES} kalmanCont.cpp kalmanCont.h cJSON.h cJSON.c opencv.h opencv.cpp communication.h communication.cpp loaded_data.h loaded_data.cpp declarations.h declarations.cpp easywsclient.h easywsclient.cpp)
target_link_libraries(portals  ${OpenCV_LIBS})
target_link_libraries(portals  curl curlpp)

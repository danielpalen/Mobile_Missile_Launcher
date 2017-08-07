cmake_minimum_required(VERSION 3.0.2)
project(Launcher)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -I/usr/local/include")

set(SDL2_INCLUDE_DIR /usr/include/SDL2)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# find SDL library
find_package(SDL2 REQUIRED)
find_package(OpenCV REQUIRED )
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} usb-1.0 ${OpenCV_LIBS} ${SDL2_LIBRARY})

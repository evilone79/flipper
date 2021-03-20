cmake_minimum_required(VERSION 3.16)
project(flipper)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories(${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_executable(flipper main.cpp)

target_link_libraries(flipper paho-mqttpp3 paho-mqtt3c paho-mqtt3a)

install(TARGETS flipper DESTINATION bin)

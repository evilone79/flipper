cmake_minimum_required(VERSION 3.16)
project(client)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories(../ext_lib/include)
link_directories(../ext_lib/lib)

add_executable(client main.cpp)

target_link_libraries(client paho-mqttpp3 paho-mqtt3c paho-mqtt3a)

install(TARGETS client DESTINATION bin)

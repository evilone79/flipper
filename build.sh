#!/bin/sh

base_path=$(pwd)
install_path="$base_path"/install
lib_path="$install_path"/lib
include_path="$install_path"/include

RUN_CMAKE="cmake -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_PREFIX_PATH=$install_path"
git clone https://github.com/eclipse/paho.mqtt.c
git clone https://github.com/eclipse/paho.mqtt.cpp

init_build() {
	cd $1
	mkdir build && cd build
}

init_build paho.mqtt.c
$RUN_CMAKE -DPAHO_WITH_SSL=FALSE ../ && make -j4 && make install

init_build $base_path/paho.mqtt.cpp
$RUN_CMAKE -DPAHO_WITH_SSL=FALSE -DPAHO_MQTT_C_LIBRARIES="$lib_path"/libpaho-mqtt3a.so -DPAHO_MQTT_C_INCLUDE_DIRS="$include_path" -DCMAKE_INSTALL_RPATH="$lib_path" ../ \
&& make -j4 && make install

init_build $base_path
$RUN_CMAKE -DCMAKE_INSTALL_RPATH="$lib_path" ../ \
&& make && make install

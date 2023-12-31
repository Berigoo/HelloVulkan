cmake_minimum_required(VERSION 3.25)
project(hakureiEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(hakureiEngine main.cpp src/HkDevice.cpp src/HkDevice.h src/HkUtility.h src/HkUtility.cpp)

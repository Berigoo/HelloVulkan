cmake_minimum_required(VERSION 3.25)
project(hakureiEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)

add_library(Core src/HkDevice.cpp src/HkDevice.h src/HkSwapchain.cpp src/HkSwapchain.h src/HkSurface.cpp src/HkSurface.h src/HkGraphicPipeline.cpp src/HkCommandPool.cpp)
add_library(HkUtility src/HkUtility.cpp src/HkUtility.h)
add_library(XcbSurface src/XcbSurface.cpp src/XcbSurface.h)

add_executable(hakureiEngine main.cpp)

target_link_libraries(hakureiEngine ${Vulkan_LIBRARIES})
target_link_libraries(hakureiEngine ${X11_xcb_LIB})

target_link_libraries(hakureiEngine Core)
target_link_libraries(hakureiEngine HkUtility)
target_link_libraries(hakureiEngine XcbSurface)
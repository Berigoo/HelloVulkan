cmake_minimum_required(VERSION 3.25)
project(hakureiEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(IMGUI_DIR /usr/include/imgui-1.90.1)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

add_library(Core src/HkDevice.cpp src/HkDevice.h src/HkSwapchain.cpp src/HkSwapchain.h src/HkSurface.cpp src/HkSurface.h src/HkGraphicPipeline.cpp src/HkCommandPool.cpp src/util.cpp src/util.h src/Vert.h src/Vert.cpp src/HkSyncObject.h src/HkSyncObject.cpp src/XcbSurface.h src/XcbSurface.cpp src/GlfwSurface.h src/GlfwSurface.cpp)
add_library(imgui ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp src/imgui/ImguiBase.h src/imgui/ImguiBase.cpp src/imgui/ImguiVulkanGlfw.h src/imgui/ImguiVulkanGlfw.cpp)

add_executable(hakureiEngine main.cpp)

target_link_libraries(hakureiEngine ${Vulkan_LIBRARIES})
target_link_libraries(hakureiEngine ${X11_xcb_LIB})
target_link_libraries(hakureiEngine glm)
target_link_libraries(hakureiEngine glfw)

target_link_libraries(hakureiEngine Core)
target_link_libraries(hakureiEngine imgui)
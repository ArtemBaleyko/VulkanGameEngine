# env file for Window
if (WIN32)
set(GLFW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/glfw-3.3.8.bin.WIN64)
set(GLM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)
set(VULKAN_SDK_PATH  "$ENV{VULKAN_SDK}")
endif()
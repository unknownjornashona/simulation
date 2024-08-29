cmake_minimum_required(VERSION 3.10)
project(SupernovaSimulation)

set(CMAKE_CXX_STANDARD 11)

# 查找 GLFW 库
find_package(glfw3 REQUIRED)
# 查找 GLEW 库
find_package(GLEW REQUIRED)
# 查找 GLUI 库
find_package(GLUI REQUIRED)

# 包含头文件目录
include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLUI_INCLUDE_DIRS})

# 源文件
set(SOURCES
    Supernova.cpp
)

# 创建可执行文件
add_executable(SupernovaSimulation ${SOURCES})

# 链接库
target_link_libraries(SupernovaSimulation ${GLEW_LIBRARIES} glfw ${GLUI_LIBRARIES})

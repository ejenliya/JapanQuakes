cmake_minimum_required(VERSION 3.5)

project(RGR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RGR main.cpp helper.h classes.h japanMap.txt quakesMap.txt)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
target_link_libraries(RGR ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} glfw)

install(TARGETS RGR
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

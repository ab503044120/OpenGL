cmake_minimum_required(VERSION 3.24)
project(OpenGL)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(glad)
add_executable(fbo main.cpp)
target_link_libraries(fbo PUBLIC -lglfw3 -ldl glad  -lX11 -lpthread )# -lGL
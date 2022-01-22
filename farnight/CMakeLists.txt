cmake_minimum_required(VERSION 3.10)

# set the project name
project(Farnight VERSION 0.1)

# https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Libraries/lib)

# add the executable
add_executable(Farnight 
Main.cpp
EBO.cpp
shaderClass.cpp
stb.cpp
Texture.cpp
VAO.cpp
VBO.cpp
)


# Tell C++ compiler to use C++20 features.
target_compile_features(main PUBLIC cxx_std_20)
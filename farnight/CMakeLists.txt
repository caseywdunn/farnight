cmake_minimum_required(VERSION 3.10)

# set the project name
project(Farnight VERSION 0.001)




# add_executable(foo ${FOO_SRCS})
# target_link_libraries(foo bar) # libbar.so is found in ${CMAKE_SOURCE_DIR}/lib


# add the executable
add_executable(Farnight 
Main.cpp
glad.c
EBO.cpp
shaderClass.cpp
stb.cpp
Texture.cpp
VAO.cpp
VBO.cpp
)

# https://stackoverflow.com/questions/28597351/how-do-i-add-a-library-path-in-cmake
#include_directories(${CMAKE_SOURCE_DIR}/Libraries/include/GLFW)
#link_directories(${CMAKE_SOURCE_DIR}/Libraries/lib)
#target_link_libraries(Farnight glfw)


# https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Libraries/include/glfw-3.3.6)
target_link_libraries(Farnight glfw)
#find_package(OpenGL REQUIRED)
#target_link_libraries(Farnight OpenGL::GL)


include_directories(${CMAKE_SOURCE_DIR}/Libraries/include)

# Tell C++ compiler to use C++20 features.
target_compile_features(Farnight PUBLIC cxx_std_17)
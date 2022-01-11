cmake_minimum_required(VERSION 3.10)

# set the project name
project(
    Farnight
    VERSION 0.1)

# add the executable
add_executable(Farnight Main.cpp)


# Tell C++ compiler to use C++20 features.
target_compile_features(main PUBLIC cxx_std_20)
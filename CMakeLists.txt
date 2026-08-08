cmake_minimum_required(VERSION 3.20)

project(hardware_test_platform VERSION 0.1.0 LANGUAGES CXX)

add_executable(hwtest src/main.cpp)

target_compile_features(hwtest PRIVATE cxx_std_20)

cmake_minimum_required(VERSION 3.24)
project(TrinityCoreModulesExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
find_package(TrinitySDK CONFIG REQUIRED)

add_subdirectory(mod-greeting)
add_subdirectory(mod-example)

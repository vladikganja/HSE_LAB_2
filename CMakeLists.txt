cmake_minimum_required(VERSION 3.15)
project(dsalab2 CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/benchmark")
include_directories("${CMAKE_SOURCE_DIR}/benchmark/include")

add_executable(${PROJECT_NAME} src/main.cpp src/generators.cpp)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
endif()

target_link_libraries(${PROJECT_NAME} benchmark::benchmark)

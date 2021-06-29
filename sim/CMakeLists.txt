cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
project(sim)

find_package(raylib CONFIG REQUIRED)

set(CMAKE_C_STANDARD 11) # Requires C11 standard

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARIES})

cmake_minimum_required(VERSION 3.20)
project(sim)
set(CMAKE_C_STANDARD 11) # Requires C11 standard
add_executable(${PROJECT_NAME} main.c termin.c system.c)
#ALLEGRO
target_include_directories(${PROJECT_NAME} PRIVATE "../libs/allegro/allegro-5.2.7.0/include/")
target_link_directories(${PROJECT_NAME} PRIVATE "../libs/allegro/allegro-5.2.7.0/build-dynamic/lib/")
target_link_directories(${PROJECT_NAME} PRIVATE "../libs/allegro/allegro-5.2.7.0/build-static/lib/")
#SDL
target_include_directories(${PROJECT_NAME} PRIVATE "../libs/sdl/SDL-release-2.0.14/include/")
target_link_directories(${PROJECT_NAME} PRIVATE "../libs/sdl/SDL-release-2.0.14/build/")

target_link_libraries(${PROJECT_NAME} allegro allegro_image allegro_primitives allegro_color allegro_font allegro_ttf allegro_audio allegro_acodec allegro_video allegro_memfile allegro_physfs allegro_dialog)
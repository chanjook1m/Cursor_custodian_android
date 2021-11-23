cmake_minimum_required(VERSION 3.10.2)

project(ndk)

find_library( log-lib

		log )

include_directories(C:/Users/1z3r0/Desktop/ndk/NDK/app/src/main/SDL/include)
include_directories(C:/Users/1z3r0/Desktop/ndk/NDK/app/src/main/SDL/include/SDL2)
include_directories(C:/Users/1z3r0/Desktop/ndk/NDK/app/src/main/include)

add_library(main SHARED main.cpp renderwindow.cpp entity.cpp groundtile.cpp ground.cpp player.cpp )

add_library(sdl SHARED IMPORTED)
set_target_properties(sdl PROPERTIES IMPORTED_LOCATION
		${PROJECT_SOURCE_DIR}/${ANDROID_ABI}/libSDL2.so)
add_library(image SHARED IMPORTED)
set_target_properties(image PROPERTIES IMPORTED_LOCATION
		${PROJECT_SOURCE_DIR}/${ANDROID_ABI}/libSDL2_image.so)
add_library(mixer SHARED IMPORTED)
set_target_properties(mixer PROPERTIES IMPORTED_LOCATION
		${PROJECT_SOURCE_DIR}/${ANDROID_ABI}/libSDL2_mixer.so)
add_library(ttf SHARED IMPORTED)
set_target_properties(ttf PROPERTIES IMPORTED_LOCATION
		${PROJECT_SOURCE_DIR}/${ANDROID_ABI}/libSDL2_ttf.so)
add_library(mpg SHARED IMPORTED)
set_target_properties(mpg PROPERTIES IMPORTED_LOCATION
		${PROJECT_SOURCE_DIR}/${ANDROID_ABI}/libmpg123.so)


target_link_libraries(main sdl image mixer ttf mpg ${log-lib})
						

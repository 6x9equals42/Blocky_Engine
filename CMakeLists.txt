cmake_minimum_required (VERSION 3.9)
project (Blocky_Engine)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/)

find_package(SFML 2.4 COMPONENTS audio graphics window system)

if(NOT SFML_FOUND)
	message(FATAL_ERROR "SFML couldn't be located!")
endif()

include_directories(${SFML_INCLUDE_DIR})

file(GLOB SOURCES
    src/*.hpp
    src/*.cpp
)

add_executable(Blocky_Engine ${SOURCES})

target_link_libraries(Blocky_Engine ${SFML_LIBRARIES})


install(TARGETS Blocky_Engine
		RUNTIME DESTINATION .)

install(DIRECTORY media/
		DESTINATION media/)

install(DIRECTORY levels/
		DESTINATION levels/)
#Set some basic parameters
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(fractal-visualizer-samarbarrett LANGUAGES CXX)

#Set cmake standard
#set(CMAKE_CXX_STANDARD 20)

#Want verbose makefiles
set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

option(ENABLE_ASAN "Enable Address Sanitizer" false)
option(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer" false)
option(ENABLE_COVERAGE "Enable Coverage" false)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()


#Find GL Packages
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lGL -lGLU -lglut")
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(PkgConfig REQUIRED)

#Package for unit testing
find_package(Catch2 REQUIRED)

#Libraries
add_library(henon INTERFACE)
target_compile_options(henon PRIVATE INTERFACE -Werror -Wall -Wextra -O3 -g)
target_include_directories(henon PUBLIC INTERFACE include)

#Libraries
add_library(shaders INTERFACE)
target_include_directories(shaders PUBLIC INTERFACE include)

#################################
###########Create executables####
#################################

#Main
add_executable(main src/main.cpp)
target_include_directories(main PUBLIC ${GLUT_INCLUDE_DIRS})
target_link_libraries(main ${GLUT_LIBRARIES} GLEW::GLEW OpenGL::GL)
target_link_libraries(main henon shaders)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -O3 -g)


add_executable(test_henon test/test_henon.cpp)
target_link_libraries(test_henon henon Catch2::Catch2)
target_compile_options(test_henon PRIVATE -Werror -Wall -Wextra -O3 -g)


install(TARGETS main test_henon DESTINATION bin)

# Install the demo script.
install(PROGRAMS demo DESTINATION bin)


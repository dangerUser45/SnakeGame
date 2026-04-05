cmake_minimum_required(VERSION 3.20)
project(SnakeGame LANGUAGES CXX)

option(SANITIZE "Enable sanitizers" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

#-------------------------------------------------------------------------------
# Setup snake target 
#-------------------------------------------------------------------------------
add_executable(snake
    src/controller.cpp
    src/coord.cpp
    src/event.cpp
    src/main.cpp
    src/model.cpp
    src/model_builder.cpp
    src/options.cpp
    src/rabbit.cpp
    src/snake.cpp
    src/terminal_view.cpp
)

target_include_directories(snake PUBLIC
    include
)

target_link_libraries(snake PRIVATE
      SFML::Graphics
      SFML::Window
      SFML::System
)

target_compile_options(snake PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Weffc++
)

#-------------------------------------------------------------------------------
# Optionally enabling sanitazers for debug 
#-------------------------------------------------------------------------------
if(SANITIZE)
    target_compile_options(snake PRIVATE
        -fsanitize=address,undefined
    )

    target_link_options(snake PRIVATE
        -fsanitize=address,undefined
    )
endif()


#-------------------------------------------------------------------------------
# Enabling testing
#-------------------------------------------------------------------------------

# enable_testing()
# add_subdirectory(tests)

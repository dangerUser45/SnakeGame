cmake_minimum_required(VERSION 3.20)
project(SnakeGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
  $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>
)

add_executable(snake
    src/controller.cpp
    src/coord.cpp
    src/event.cpp
    src/main.cpp
    src/model.cpp
    src/rabbit.cpp
    src/snake.cpp
    src/text_view.cpp
)

target_include_directories(snake PUBLIC
    include
)

target_link_libraries(snake PRIVATE project_warnings)

# enable_testing()
# add_subdirectory(tests)

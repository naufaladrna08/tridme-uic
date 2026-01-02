
cmake_minimum_required(VERSION 3.10.0)
project(example-ui VERSION 0.1.0 LANGUAGES C)

add_executable(
  calc
  calc.c 
)

target_link_libraries(
  calc
  tridme-ui
  -lGL -lGLEW -lglfw
)
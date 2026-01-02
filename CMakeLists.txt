cmake_minimum_required(VERSION 3.10.0)
project(tridme-ui VERSION 0.1.0 LANGUAGES C)

# Find FreeType
find_package(Freetype REQUIRED)

include_directories(include ${FREETYPE_INCLUDE_DIRS})

add_library(tridme-ui 
  src/ui_core.c
  src/ui_widgets.c
  src/ui_shaders.c
)

target_link_libraries(tridme-ui ${FREETYPE_LIBRARIES})

add_subdirectory(examples)
cmake_minimum_required (VERSION 3.1)
project("rainbower")

include_directories(
  include/
  "${PROJECT_BINARY_DIR}")


file(GLOB rainbower_SRC
    "./rc/rainbower.cpp"
)

# Executables
add_executable(rainbower ${rainbower_SRC})

################
# Installation #
################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/rainbower
    DESTINATION bin)




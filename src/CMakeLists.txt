cmake_minimum_required (VERSION 3.5)
project (iathena)

file(GLOB iathena_SRC
    "*.hpp"
    "*.cpp"
)

add_executable(iathena ${iathena_SRC})
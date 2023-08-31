cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project("iathena" LANGUAGES CXX)

# Define the path to custom cmake modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Define variables
include(variables)

# Retrieve project version
include(version)

# Use CTest for unitary testing
include(CTest)

# Execute subdirectories
add_subdirectory(${DOCS})
add_subdirectory(${SRC})


#cmake_minimum_required(VERSION 3.5)
#project(iathena VERSION 0.0)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

#if (MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#endif()

#file(GLOB iathena_SRC
#    "*.hpp"
#    "*.cpp"
#)

#add_executable(iathena ${iathena_SRC})

# Test exe
#add_executable(iathenaPERFT ${iathena_SRC})
#add_executable(iathenaMIRROR ${iathena_SRC})


# main exe target options
#target_compile_definitions(iathena PUBLIC "NPERFT" "NMIRROR" "NDEBUG")
#target_compile_options(iathena PUBLIC "-O3")

# perft target options
#target_compile_definitions(iathenaPERFT PUBLIC "NMIRROR" "PERFT_DEPTH=3")
#target_compile_definitions(iathenaMIRROR PUBLIC "NPERFT")

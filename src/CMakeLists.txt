
file(GLOB files "${SRC}/*.cpp")
list(REMOVE_ITEM files ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(libiathena STATIC ${files})

add_executable(iathena main.cpp)
target_link_libraries(iathena libiathena)

if(MSVC)
  target_compile_options(iathenaPRIVATE /W4 /WX)
else()
  target_compile_options(iathena PRIVATE -Wall -Wextra -Wpedantic)
endif()
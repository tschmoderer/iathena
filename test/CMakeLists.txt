
configure_file(perftsuite.epd perftsuite.epd COPYONLY)
add_executable(position test_position.cpp)
target_link_libraries(position libiathena)
add_test(NAME test_position COMMAND position)

# add memcheck test
if (NOT TARGET memcheck)
add_custom_target(memcheck
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --force-new-ctest-process --test-action memcheck
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
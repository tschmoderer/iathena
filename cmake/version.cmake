# Define version from Git tags
find_package(Git)
if (NOT GIT_FOUND)
    message("Git Package not found. Set a default GUI version 0.0.0")
    set(CMAKE_LIB_VERSION "0.0.0")
else()
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
        RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(NOT GIT_DESCRIBE_ERROR_CODE)
        message(STATUS ${GIT_DESCRIBE_VERSION})

        #parse the version information into pieces.
        string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_DESCRIBE_VERSION}")
        string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_DESCRIBE_VERSION}")
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_DESCRIBE_VERSION}")
        #string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${GIT_DESCRIBE_VERSION}")

        set(CMAKE_LIB_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
        message(STATUS "LIB VERSION : ${CMAKE_LIB_VERSION}")
    else() 
        message("Impossible to get a version tag from git. Set a default lib version 0.0.0")
        set(CMAKE_LIB_VERSION "0.0.0")
    endif()
endif()
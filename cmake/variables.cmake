# cmake script to define variables

## General variables
set(AUTHOR "T. Schmoderer")
set(AUTHOR_MAIL "iathena@mailo.com")
string(TIMESTAMP TODAY "%Y-%m-%d")
string(TIMESTAMP YEAR "%Y")
set(DOCUMENTATION_ADDR "https://tschmoderer.github.io/iathena/html/index.html")
set(LICENSE "GNU GENERAL PUBLIC LICENSE")
set(CMAKE_PROJECT_DESCRIPTION "Free Open Source C++ UCI Chess Engine")
set(CODE_ADDR "https://github.com/tschmoderer/iathena")

## Compilation variables
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 26)

## Directories
set(DOCS ${PROJECT_SOURCE_DIR}/docs)
set(INC ${PROJECT_SOURCE_DIR}/includes)
set(SRC ${PROJECT_SOURCE_DIR}/src)
set(TST ${PROJECT_SOURCE_DIR}/test)
# Configure Doxygen file
set(DOXYGEN_IN  ${DOCS}/doxyfile.in)
set(DOXYGEN_OUT ${DOCS}/doxyfile.conf)

configure_file(${INC}/utils.hpp.in ${INC}/utils.hpp @ONLY)
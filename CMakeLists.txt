cmake_minimum_required(VERSION 3.15..3.26)
project(Project5 CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# You may need to edit these variables
#-----------------------------------------------------------------------

# add source for table modules here
set(LIB_SOURCE
  expression.hpp expression.cpp
  interpreter.hpp interpreter.cpp
  environment.hpp
)

# add source for table (and associated code) unit tests here
set(LIB_TEST_SOURCE
  test_interpreter.cpp 
)

# You should not need to edit below this line
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------

# try to prevent accidental in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(
    FATAL_ERROR
      "In-source builds not allowed. Remove any files created thus far and use a different directory for the build."
)
endif()

# require a C++11 compiler for all targets
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# crank up the warning level on compiler 
if(MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  # lots of warnings and all warnings as errors
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# add cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# build test driver executable
add_executable(unit_tests catch.hpp unit_tests.cpp ${LIB_SOURCE} ${LIB_TEST_SOURCE})
add_executable(interpreter_Line_main Line_interperter.cpp ${LIB_SOURCE} ${LIB_TEST_SOURCE})
add_executable(interpreter_File_main File_interperter.cpp ${LIB_SOURCE} ${LIB_TEST_SOURCE})

# enable testing
include(CTest)
enable_testing()

# register Catch tests with cmake
include(Catch)
catch_discover_tests(unit_tests)

# In the reference environment enable coverage on tests
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX AND COVERAGE)
  message("-- Enabling test coverage")
  set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -fno-elide-constructors -fno-default-inline -fprofile-arcs -ftest-coverage")
  set_target_properties(unit_tests PROPERTIES COMPILE_FLAGS ${GCC_COVERAGE_COMPILE_FLAGS} )
  target_link_libraries(unit_tests gcov)
  add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E env "ROOT=${CMAKE_CURRENT_SOURCE_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/coverage.sh)
endif()

# In the reference environment enable memory checking on tests
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX AND MEMTEST)
  message("-- Enabling memory checks")
  add_custom_target(memtest
    COMMAND valgrind ${CMAKE_BINARY_DIR}/unit_tests)
endif()

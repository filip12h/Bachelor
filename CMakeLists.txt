cmake_minimum_required(VERSION 3.12)
project(Bachelor)

# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(SOURCE_FILES main.cpp redBlackEdges.cpp redBlackEdges.h oneHelpfulSet.cpp cutSizeBisection.cpp pathDecomposition.cpp)

add_library(BAgraph ${SOURCE_FILES})

INCLUDE_DIRECTORIES(C:/Users/Karol/ClionProjects/BAgraph/BAgraph/include)
LINK_DIRECTORIES(C:/Users/Karol/ClionProjects/BAgraph/BAgraph/include)
add_executable(Bachelor main.cpp)
TARGET_LINK_LIBRARIES(BAgraph algorithms.hpp graphs.hpp invariants.hpp io.hpp lib.hpp multipoles.hpp operations.hpp random.hpp sat.hpp snarks.hpp util.hpp)


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fconcepts SUPPORT_CONCEPTS)
set(CMAKE_CXX_FLAGS "-fconcepts ${CMAKE_CXX_FLAGS}")

if(NOT SUPPORT_CONCEPTS)
    message(FATAL_ERROR "Your compiler does not support concepts; try to
use >=gcc-6")
endif()
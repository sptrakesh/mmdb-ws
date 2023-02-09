cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# For INTERPROCEDURAL_OPTIMIZATION
if (POLICY CMP0069)
  cmake_policy(SET CMP0069 NEW)
endif()

project(mmdb-ws)

# Enable C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()


if (APPLE)
  set(CMAKE_MACOSX_RPATH 1)
else()
  include_directories(
    /opt/local/include
  )
  link_directories(
    /opt/local/lib
  )
endif()

SET(CMAKE_INSTALL_PREFIX /opt/spt)

add_subdirectory(src)


if (BUILD_TESTING)
  add_subdirectory(test/integration)
  add_subdirectory(test/performance)
endif()


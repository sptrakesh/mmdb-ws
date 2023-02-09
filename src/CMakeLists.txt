file(GLOB_RECURSE mmdb-ws_SRCS *.cpp *.cc *.hpp *.h)
set(mmdb-ws_SRCS ${mmdb-ws_SRCS})
add_executable(mmdb-ws ${mmdb-ws_SRCS} )

if (CMAKE_BUILD_TYPE MATCHES Debug)
  message(STATUS "Debug build, not enabling IPO/LTO")
else()
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
  if (result)
    message(STATUS "IPO is supported: ${output}")
    set_property(TARGET mmdb-ws PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized -flto=auto")
  else()
    message(STATUS "IPO is not supported: ${output}")
  endif()
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if (APPLE)
  target_link_libraries(mmdb-ws
    PRIVATE Boost::boost
  )
else()
  target_link_libraries(mmdb-ws
    PRIVATE Boost::boost libgeolite2++.a libmaxminddb.a
  )
endif()

install(TARGETS mmdb-ws DESTINATION bin)

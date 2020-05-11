file(GLOB_RECURSE mmdb-ws_SRCS *.cpp *.cc *.hpp *.h)
set(mmdb-ws_SRCS ${mmdb-ws_SRCS})
add_executable(mmdb-ws ${mmdb-ws_SRCS} )

if (APPLE)
  target_link_libraries(mmdb-ws
    PRIVATE Boost::boost
  )
else()
  target_link_libraries(mmdb-ws
    PRIVATE Boost::boost geolite2++ libmaxminddb.a
  )
endif()

install(TARGETS mmdb-ws DESTINATION bin)

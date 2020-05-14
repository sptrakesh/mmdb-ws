find_package(hayai REQUIRED)

add_executable(performance
  websocket.cpp
)

target_link_libraries(performance PRIVATE hayai_main Boost::boost)
add_test(NAME performance COMMAND performance)
install(TARGETS performance DESTINATION bin)

FILE(GLOB_RECURSE integration_SRCS *.cpp *.hpp *.h)
set(integration_SRCS ${integration_SRCS})
add_executable(integration ${integration_SRCS})

target_link_libraries(integration PRIVATE Boost::boost)

add_test(NAME integration COMMAND integration)
install(TARGETS integration DESTINATION bin)

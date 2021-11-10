
add_library(
	server SHARED
	server.h
	server.c
)

target_include_directories(server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
install (TARGETS server DESTINATION /usr/lib)

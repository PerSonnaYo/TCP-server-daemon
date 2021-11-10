add_library(
	daemonize SHARED
	daemonize.h
	daemonize.c
)

target_include_directories(daemonize PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
install (TARGETS daemonize DESTINATION /usr/lib)


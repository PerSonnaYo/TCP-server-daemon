# Make build flags compiler specific
if (CMAKE_COMPILER_IS_GNUCC)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set (CMAKE_C_FLAGS "-D_REETRANT -ggdb -fPIC -Wall -Wextra -pedantic -O0")
	elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
		set (CMAKE_C_FLAGS "-D_REETRANT -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3")
	endif ()
endif (CMAKE_COMPILER_IS_GNUCC)

# Adding executable
add_executable (TCPserver main.c)
target_link_libraries(TCPserver PUBLIC daemonize)
target_link_libraries(TCPserver PUBLIC server)

# Adding thread package
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
	  target_compile_options(TCPserver PUBLIC "-pthread")
endif()
if (CMAKE_THREAD_LIBS_INIT)
	target_link_libraries(TCPserver "${CMAKE_THREAD_LIBS_INIT}")
endif()

install (TARGETS TCPserver
	RUNTIME
	DESTINATION bin)

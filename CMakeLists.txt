cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

project( GZXFANCONTROL LANGUAGES CXX )

find_package (Threads)

add_executable( GZXFANCONTROL 
	gzxfancontrol.cpp
	PID.cpp
)

add_definitions(-DNDEBUG)

target_link_libraries ( GZXFANCONTROL 
    ${CMAKE_THREAD_LIBS_INIT}
)
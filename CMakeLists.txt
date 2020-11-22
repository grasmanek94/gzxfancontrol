cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

project( gzxfancontrol LANGUAGES CXX )

find_package (Threads)

add_executable( gzxfancontrol 
	gzxfancontrol.cpp
	PID.cpp
)

target_link_libraries ( gzxfancontrol 
    ${CMAKE_THREAD_LIBS_INIT}
)
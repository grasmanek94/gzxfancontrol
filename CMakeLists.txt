cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

project( GZXFANCONTROL LANGUAGES CXX )

add_executable( GZXFANCONTROL 
	gzxfancontrol.cpp
	PID.cpp
)

add_definitions(-DNDEBUG)

cmake_minimum_required(VERSION 3.2)
project(arduino-base-tests)


add_executable(
	arduino-base-tests
	main.cpp

	exception.cpp
	map.cpp
)
# target_include_directories(arduino-base-tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

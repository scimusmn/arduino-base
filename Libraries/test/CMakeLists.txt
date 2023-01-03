cmake_minimum_required(VERSION 3.8)
project(arduino-base-tests)


add_executable(
	arduino-base-tests
	main.cpp

	map.cpp
	string.cpp
	SerialController.cpp
)
set_property(TARGET arduino-base-tests PROPERTY CXX_STANDARD 11)
# target_include_directories(arduino-base-tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_include_directories(arduino-base-tests PUBLIC include)
target_compile_definitions(arduino-base-tests PRIVATE SMM_NO_SWITCH)

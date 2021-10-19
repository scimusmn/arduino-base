cmake_minimum_required(VERSION 3.2)
project(arduino-base-tests
  VERSION 1.0.0
  DESCRIPTION "Test suite for SMM's arduino-base")

include_directories(${CMAKE_SOURCE_DIR}/..)
set(TEST_SOURCES
  singletons.cpp
  FakeArduino.cpp
  tests.cpp
  FakeSerialTests.cpp
  FakeEEPROMTests.cpp
  FakeWireTests.cpp
  FixedSizeStringTests.cpp
  LookupTableTests.cpp
  SerialControllerTests.cpp
  RfidTests.cpp
  ButtonTests.cpp
  )

add_executable(tests ${TEST_SOURCES})
# target_link_libraries(tests )


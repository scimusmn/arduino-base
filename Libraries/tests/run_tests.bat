@echo off
if not exist build\ (
   mkdir build
)

cd build
cmake ..
msbuild arduino-base-tests.sln
cd Debug
tests
cd ..\..

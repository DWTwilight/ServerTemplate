@echo off

if %1% == test (
    cd build/test
    cmake --build ../
    ctest
)
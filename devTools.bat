@echo off

if %1% == test (
    cd build/test
    ctest
)
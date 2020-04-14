#!/bin/sh -x

find . -name "*~" -delete

find . -name gen-cpp -type d | xargs rm -rf

find . -name CMakeCache.txt -delete
find . -name CMakeFiles -type d | xargs rm -rf
find . -name cmake_install.cmake -delete
find . -name Makefile -delete
find . -name "*.so" -delete
find . -name "*.tsk" -delete
find . -name __pycache__ -type d -print | xargs rm -rf

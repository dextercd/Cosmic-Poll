#!/bin/sh

find src tests \( -name \*.cpp -o -name \*.hpp \) -exec clang-format -i {} +

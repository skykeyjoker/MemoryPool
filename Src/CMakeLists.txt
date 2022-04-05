cmake_minimum_required(VERSION 3.21)
project(MemoryPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(MemoryPool main.cpp MemoryPool.hpp StackAlloc.hpp)

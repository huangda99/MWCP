cmake_minimum_required(VERSION 3.23)
project(bicliqueIndex1)

set(CMAKE_CXX_STANDARD 11)

add_executable(bicliqueIndex1 main.cpp MemoryManager.h MemoryManager.cpp LinkedList.h LinkedList.cpp misc.h misc.cpp degeneracy_helper.h degeneracy_helper.cpp degeneracy_algorithm_cliques_A.h degeneracy_algorithm_cliques_A.cpp bicliqueIndex.h bicliqueIndex.cpp)

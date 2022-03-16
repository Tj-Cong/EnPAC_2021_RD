cmake_minimum_required(VERSION 3.15)
project(EnPAC_2021)

set(CMAKE_CXX_STANDARD 14)
set(LINK_DIR /usr/local/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -pthread")

link_directories(${LINK_DIR})
link_libraries(tcmalloc)

include_directories(.)
include_directories(BA)

add_executable(EnPAC_2021
        BA/buchi.cpp
        BA/buchi.h
        BA/Syntax_Tree.cpp
        BA/Syntax_Tree.h
        BA/tinystr.cpp
        BA/tinystr.h
        BA/tinyxml.cpp
        BA/tinyxml.h
        BA/tinyxmlerror.cpp
        BA/tinyxmlparser.cpp
        BA/VWAA.cpp
        BA/VWAA.h
        BA/set.h
        BA/Lexer.h
        BA/Lexer.cpp
        BA/atomic.h
        BA/atomic.cpp
        main.cpp
        rref.h
        rref.cpp
        Petri_Net.cpp
        Petri_Net.h
        product.cpp
        product.h
        RG.cpp
        RG.h)

target_link_libraries(EnPAC_2021 tcmalloc)
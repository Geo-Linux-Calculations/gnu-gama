cmake_minimum_required(VERSION 3.5)

project (gnu-gama)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-DGNU_gama_expat_1_1)

file(GLOB_RECURSE SRC_GAMA lib/gnu_gama/*.cpp lib/gnu_gama/*.h)

set(SRC_EXPAT
   lib/expat/xmltok/xmltok.c
   lib/expat/xmltok/xmlrole.c
   lib/expat/xmlwf/codepage.c
   lib/expat/xmlparse/xmlparse.c
   lib/expat/xmlparse/hashtable.c
)

include_directories(lib lib/expat/xmlparse lib/expat/xmltok)

add_library(libgama OBJECT ${SRC_GAMA} ${SRC_EXPAT})

add_executable(gama-local bin/gama-local.cpp $<TARGET_OBJECTS:libgama>)
add_executable(gama-g3 bin/gama-g3.cpp $<TARGET_OBJECTS:libgama>)

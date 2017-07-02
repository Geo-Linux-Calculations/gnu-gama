cmake_minimum_required(VERSION 3.5)

project (gama-local)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-DGNU_gama_expat_1_1)

file(READ configure.ac temp1)
string(REGEX REPLACE ".+AC_INIT.\\[gama\\], \\[" "" temp2 "${temp1}")
string(REGEX REPLACE "\\].+" "" temp3 "${temp2}")
add_definitions(-DVERSION="${temp3}")

file(GLOB_RECURSE Gama_SOURCES lib/gnu_gama/*.cpp)

set(Expat_SOURCES
   lib/expat/xmltok/xmltok.c
   lib/expat/xmltok/xmlrole.c
   lib/expat/xmlwf/codepage.c
   lib/expat/xmlparse/xmlparse.c
   lib/expat/xmlparse/hashtable.c
)

include_directories(lib lib/expat/xmlparse lib/expat/xmltok)

add_executable(gama-local bin/gama-local.cpp
  lib/gnu_gama/outstream.cpp
  ${Gama_SOURCES}
  ${Expat_SOURCES}
)

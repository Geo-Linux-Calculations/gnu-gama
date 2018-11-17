#!/bin/sh
#
# builds dictionaris for GNU Gama
# 

echo
echo Building dictionaries
echo

if pwd | grep /scripts$; then cd ..; fi

SLOVNIKAR=$(pwd)/scripts/slovnikar

# source directory for language.{h|cpp}
cd lib/gnu_gama/local
FILES=$(find ../../../xml/lang -name *.lang)

echo input files: $FILES
echo
echo $FILES | $SLOVNIKAR

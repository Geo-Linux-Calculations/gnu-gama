#!/bin/bash

if pwd | grep /scripts$; then cd ..; fi

find -type f -name "*.h" -or -name "*.cpp" | etags -

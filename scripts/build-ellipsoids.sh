#!/bin/sh
#
# builds ellipsoids.[h|cpp|texi] for GNU GaMa
# 


if pwd | grep /scripts$; then cd ..; fi

echo
echo "Building ellipsoids_xml"
echo

rm -f lib/gnu_gama/ellipsoids.* xml/ellipsoids.html doc/ellipsoids.texi

scripts/ellipsoids_xml xml/ellipsoids.xml lib/gnu_gama

mv lib/gnu_gama/ellipsoids.html xml
mv lib/gnu_gama/ellipsoids.texi doc

# ------------------------------------------------------------------------








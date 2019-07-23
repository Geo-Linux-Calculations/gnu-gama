EXTRA_DIST = build-dictionaries.sh build-ellipsoids.sh

noinst_PROGRAMS   = slovnikar ellipsoids_xml

slovnikar_SOURCES = slovnikar.cpp
slovnikar_LDADD   = $(top_builddir)/lib/libgama.a
slovnikar_CPPFLAGS = -I $(top_srcdir)/lib

ellipsoids_xml_SOURCES = ellipsoids_xml.cpp
ellipsoids_xml_LDADD   = $(top_builddir)/lib/libgama.a
ellipsoids_xml_CPPFLAGS = -I $(top_srcdir)/lib

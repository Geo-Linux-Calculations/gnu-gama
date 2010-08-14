bin_PROGRAMS       = gama-local gama-local-xml2txt \
                     gama-g3

gama_local_SOURCES = gama-local.cpp gama-local-main.h
gama_local_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_xml2txt_SOURCES = gama-local-xml2txt.cpp
gama_local_xml2txt_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_xml2txt_CPPFLAGS = -I $(top_srcdir)/lib

gama_g3_SOURCES = gama-g3.cpp
gama_g3_LDADD   = $(top_builddir)/lib/libgama.a
gama_g3_CPPFLAGS = -I $(top_srcdir)/lib

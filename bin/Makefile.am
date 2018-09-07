GAMA_LOCAL = gama-local \
             gama-local-xml2txt \
             gama-local-xml2sql

if GNU_GAMA_G3_ENABLED
   GAMA_G3 = gama-g3
endif


bin_PROGRAMS = $(GAMA_LOCAL) $(GAMA_G3)

gama_local_SOURCES = gama-local.cpp
gama_local_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_xml2txt_SOURCES = gama-local-xml2txt.cpp
gama_local_xml2txt_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_xml2txt_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_xml2sql_SOURCES = gama-local-xml2sql.cpp
gama_local_xml2sql_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_xml2sql_CPPFLAGS = -I $(top_srcdir)/lib

if GNU_GAMA_G3_ENABLED
   gama_g3_SOURCES  = gama-g3.cpp
   gama_g3_LDADD    = $(top_builddir)/lib/libgama.a
   gama_g3_CPPFLAGS = -I $(top_srcdir)/lib
endif

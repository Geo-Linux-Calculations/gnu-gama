GAMA_LOCAL = gama-local \
             gama-local-xml2txt \
             gama-local-xml2sql \
             gama-local-gkf2yaml \
             krumm2gama-local

if GNU_GAMA_G3_ENABLED
   GAMA_G3 = gama-g3
endif

if GNU_GAMA_LOCAL_TEST_YAML_CPP
   GAMA_YAML = gama-local-yaml2gkf
endif

bin_PROGRAMS = $(GAMA_LOCAL) $(GAMA_G3) $(GAMA_YAML)

gama_local_SOURCES = gama-local.cpp
gama_local_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_xml2txt_SOURCES = gama-local-xml2txt.cpp
gama_local_xml2txt_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_xml2txt_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_xml2sql_SOURCES = gama-local-xml2sql.cpp
gama_local_xml2sql_LDADD   = $(top_builddir)/lib/libgama.a
gama_local_xml2sql_CPPFLAGS = -I $(top_srcdir)/lib

gama_local_gkf2yaml_SOURCES  = gama-local-gkf2yaml.cpp
gama_local_gkf2yaml_LDADD    = $(top_builddir)/lib/libgama.a
gama_local_gkf2yaml_CPPFLAGS = -I $(top_srcdir)/lib

if GNU_GAMA_G3_ENABLED
   gama_g3_SOURCES  = gama-g3.cpp
   gama_g3_LDADD    = $(top_builddir)/lib/libgama.a
   gama_g3_CPPFLAGS = -I $(top_srcdir)/lib
endif

if GNU_GAMA_LOCAL_TEST_YAML_CPP
   gama_local_yaml2gkf_SOURCES = gama-local-yaml2gkf.cpp
   gama_local_yaml2gkf_LDADD   = $(top_builddir)/lib/libgama.a -lyaml-cpp
   gama_local_yaml2gkf_CPPFLAGS = -I $(top_srcdir)/lib
endif

krumm2gama_local_SOURCES  = \
   $(top_srcdir)/lib/krumm/krumm2gama-local.cpp \
   $(top_srcdir)/lib/krumm/common.cpp \
   $(top_srcdir)/lib/krumm/input.cpp \
   $(top_srcdir)/lib/krumm/k2gkf.cpp \
   $(top_srcdir)/lib/krumm/output.cpp \
   $(top_srcdir)/lib/krumm/string_matrix.cpp \
   $(top_srcdir)/lib/gnu_gama/xml/str2xml.cpp
krumm2gama_local_CPPFLAGS = -I $(top_srcdir)/lib/krumm -I $(top_srcdir)/lib

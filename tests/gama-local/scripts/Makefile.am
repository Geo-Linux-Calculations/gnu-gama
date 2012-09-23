EXTRA_DIST = gama-local-adjustment.in  \
             gama-local-algorithms.in  \
             gama-local-equivalents.in \
             gama-local-html.in \
             gama-local-xml-results.in \
             gama-local-version.in

check_PROGRAMS = check_algorithms check_equivalents check_html check_xml_results

check_algorithms_SOURCES  = check_algorithms.cpp \
                            check_xyz.h check_xyz.cpp
check_algorithms_LDADD    = $(top_builddir)/lib/libgama.a
check_algorithms_CPPFLAGS = -I $(top_srcdir)/lib

check_equivalents_SOURCES  = check_equivalents.cpp \
                             check_xyz.h check_xyz.cpp
check_equivalents_LDADD    = $(top_builddir)/lib/libgama.a
check_equivalents_CPPFLAGS = -I $(top_srcdir)/lib

check_html_SOURCES  = check_html.cpp compare_xml_adjustment.h \
                      compare_xml_adjustment.cpp
check_html_LDADD    = $(top_builddir)/lib/libgama.a
check_html_CPPFLAGS = -I $(top_srcdir)/lib

check_xml_results_SOURCES  = check_xml_results.cpp \
                             check_xyz.h check_xyz.cpp
check_xml_results_LDADD    = $(top_builddir)/lib/libgama.a
check_xml_results_CPPFLAGS = -I $(top_srcdir)/lib

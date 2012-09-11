EXTRA_DIST = gama-local-adjustment.in  \
             gama-local-algorithms.in  \
             gama-local-version.in

check_PROGRAMS = check_algorithms

check_algorithms_SOURCES  = check_algorithms.cpp
check_algorithms_LDADD    = $(top_builddir)/lib/libgama.a
check_algorithms_CPPFLAGS = -I $(top_srcdir)/lib

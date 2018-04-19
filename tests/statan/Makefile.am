AM_CPPFLAGS = -I$(top_srcdir)/lib
AM_DEFAULT_SOURCE_EXT = .cpp
LDADD = $(top_builddir)/lib/libgama.a

check_PROGRAMS = normal_critical_values \
		 normal_distribution \
		 chi2_critical_values \
		 student_critical_values \
		 kolmogorov_smirnov_cdf

TESTS = $(check_PROGRAMS)

@VALGRIND_CHECK_RULES@

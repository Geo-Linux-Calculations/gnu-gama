AM_CPPFLAGS = -I$(top_srcdir)/lib

check_PROGRAMS = matvec-basic simple-inversion \
	matvec_demo_001 matvec_demo_002 matvec_demo_003 \
	matvec_demo_004 matvec_demo_005 matvec_demo_006 \
	matvec_test_001 matvec_test_002 matvec_test_003 \
	matvec_test_004 matvec_test_005

#simple_inversion_SOURCES = simple-inversion.cpp
AM_DEFAULT_SOURCE_EXT = .cpp

TESTS = $(check_PROGRAMS)

@VALGRIND_CHECK_RULES@

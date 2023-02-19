EXTRA_DIST = gama-g3-adjustment.sh_ gama-g3-xmllint-xsd.sh_ \
             gama-g3-ellipsoid-xyz2blh.sh_

check_PROGRAMS = check_adjustment check_ellipsoid_xyz2blh

check_adjustment_SOURCES  = check_adjustment.cpp
check_adjustment_LDADD    = $(top_builddir)/lib/libgama.a
check_adjustment_CPPFLAGS = -I $(top_srcdir)/lib

check_ellipsoid_xyz2blh_SOURCES  = check_ellipsoid_xyz2blh.cpp
check_ellipsoid_xyz2blh_LDADD    = $(top_builddir)/lib/libgama.a
check_ellipsoid_xyz2blh_CPPFLAGS = -I $(top_srcdir)/lib

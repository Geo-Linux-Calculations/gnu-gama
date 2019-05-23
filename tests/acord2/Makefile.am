EXTRA_DIST = CMakeLists.txt acord2-a2g.in acord2-a2diff.in

AM_CPPFLAGS = -I$(top_srcdir)/lib
AM_DEFAULT_SOURCE_EXT = .cpp
LDADD = $(top_builddir)/lib/libgama.a

check_PROGRAMS = a2g a2diff

a2g_SOURCES  = a2g-main.cpp a2g.h a2g.cpp
#a2g_LDADD    = $(top_builddir)/lib/libgama.a
#a2g_CPPFLAGS = -I $(top_srcdir)/lib

a2diff_SOURCES  = a2diff-main.cpp
#a2diff_LDADD    = $(top_builddir)/lib/libgama.a
#a2diff_CPPFLAGS = -I $(top_srcdir)/lib

TESTS = acord2-a2g.sh acord2-a2diff.sh

SUBDIRS = input/a2g input/a2diff

@VALGRIND_CHECK_RULES@

ACORD2_A2G_FILES = \
  angle-01.a2g    \
  angle-01.0.a2g  \
  azimuth-01.a2g  \
  polar-01.a2g    \
  polar-02.a2g    \
  traverse-01.a2g \
  traverse-02.a2g \
  traverse-03.a2g \
  trivial-01.a2g  \
  trivial-02.a2g

#   azimuth-01.a2g

ACORD2_A2DIFF_FILES = \
  traverse-01-en-right.gkf \
  traverse-01-ne-left.gkf  \
  traverse-01-es-right.gkf \
  traverse-01-es-left.gkf  \
  traverse-01-ne-right.gkf \
  traverse-01-nw-left.gkf  \
  traverse-01-nw-right.gkf \
  traverse-01-se-left.gkf  \
  traverse-01-se-right.gkf \
  traverse-01-sw-left.gkf  \
  traverse-01-sw-right.gkf \
  traverse-01-wn-left.gkf  \
  traverse-01-wn-right.gkf \
  traverse-01-ws-left.gkf  \
  traverse-01-ws-right.gkf \
  gama-local.gkf

# $(srcdir)/scripts
ACORD2_SCRIPT  = $(srcdir)
ACORD2_INPUT   = $(srcdir)/input
ACORD2_RESULTS = results/$(PACKAGE_VERSION)
ACORD2_OTHERS  = Makefile $(ACORD2_RESULTS)

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
               -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
               -e 's,[@]ACORD2_A2G_FILES[@],$(ACORD2_A2G_FILES),g' \
               -e 's,[@]ACORD2_A2DIFF_FILES[@],$(ACORD2_A2DIFF_FILES),g' \
               -e 's,[@]ACORD2_INPUT[@],$(ACORD2_INPUT),g' \
               -e 's,[@]ACORD2_RESULTS[@],$(ACORD2_RESULTS),g' \
               -e 's,[@]ACORD2_SCRIPT[@],$(ACORD2_SCRIPT),g' \
               -e 's,[@]ACORD2_XML[@],$(top_srcdir)/xml,g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(ACORD2_RESULTS) $(TESTS)

$(ACORD2_RESULTS):
	mkdir -p $(ACORD2_RESULTS)

acord2-a2g.sh : $(ACORD2_SCRIPT)/acord2-a2g.in $(ACORD2_OTHERS)
	@$(do_subst) < $(ACORD2_SCRIPT)/acord2-a2g.in > acord2-a2g.sh
	@chmod +x acord2-a2g.sh

acord2-a2diff.sh : $(ACORD2_SCRIPT)/acord2-a2diff.in $(ACORD2_OTHERS)
	@$(do_subst) < $(ACORD2_SCRIPT)/acord2-a2diff.in > acord2-a2diff.sh
	@chmod +x acord2-a2diff.sh

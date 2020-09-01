TESTS = gama-local-extra-tests.sh

SUBDIRS = scripts input

GAMA_SCRIPT  = $(srcdir)/scripts
GAMA_INPUT   = $(srcdir)/input
GAMA_RESULTS = results/$(PACKAGE_VERSION)
GAMA_OTHERS  = Makefile $(GAMA_RESULTS)

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
	       -e 's,[@]top_srcdir[@],$(top_srcdir),g' \
	       -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
	       -e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
	       -e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g' \
	       -e 's,[@]GAMA_RESULTS[@],$(GAMA_RESULTS),g' \
	       -e 's,[@]GAMA_XML[@],$(top_srcdir)/xml,g' \
	       -e 's,[@]BUG_FILES[@],$(BUG_FILES),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(GAMA_RESULTS) $(TESTS)

$(GAMA_RESULTS):
	mkdir -p $(GAMA_RESULTS)

gama-local-extra-tests.sh : $(GAMA_SCRIPT)/gama-local-extra-tests.in \
	      @top_srcdir@/tests/gama-local/gama-local-adjustment.sh
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-extra-tests.in \
	             > gama-local-extra-tests.sh
	@chmod +x gama-local-extra-tests.sh


@VALGRIND_CHECK_RULES@
# VALGRIND_SUPPRESSIONS_FILES = matvec.supp
# EXTRA_DIST = matvec.supp

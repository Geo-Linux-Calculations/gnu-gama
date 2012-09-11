TESTS = gama-local-version gama-local-adjustment

INPUT_FILES = gama-local \
              tst-tetrahedron-2

GAMA_SCRIPT = $(srcdir)/scripts
GAMA_INPUT = $(srcdir)/input

EXTRA_DIST = $(GAMA_SCRIPT)/gama-local-version.in \
             $(GAMA_SCRIPT)/gama-local-adjustment.in \
             $(GAMA_INPUT)/gama-local.gkf \
             $(GAMA_INPUT)/tst-tetrahedron-2.gkf

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
               -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
               -e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
               -e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf results $(TESTS)

gama-local-version: $(GAMA_SCRIPT)/gama-local-version.in ./Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-version.in > gama-local-version
	chmod +x gama-local-version

gama-local-adjustment: $(GAMA_SCRIPT)/gama-local-adjustment.in ./Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-adjustment.in \
				> gama-local-adjustment
	chmod +x gama-local-adjustment

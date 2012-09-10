TESTS = gama-local-version gama-local-adjustment

INPUT_FILES = gama-local \
              tst-tetrahedron-2

scripts = scripts
noinst_SCRIPTS = $(TESTS)
CLEANFILES = $(noinst_SCRIPTS)
EXTRA_DIST = $(scripts)/gama-local-version.in \
             $(scripts)/gama-local-adjustment.in \
             input/gama-local.gkf \
             input/tst-tetrahedron-2

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
               -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
               -e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g'

clean-local:
	-rm -rf results

gama-local-version: $(scripts)/gama-local-version.in ./Makefile
	$(do_subst) < $(scripts)/gama-local-version.in > gama-local-version
	chmod +x gama-local-version

gama-local-adjustment: $(scripts)/gama-local-adjustment.in ./Makefile
	$(do_subst) < $(scripts)/gama-local-adjustment.in \
				> gama-local-adjustment
	chmod +x gama-local-adjustment

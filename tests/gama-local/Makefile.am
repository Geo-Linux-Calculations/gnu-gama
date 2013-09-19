TESTA = gama-local-version \
	gama-local-adjustment \
	gama-local-algorithms \
	gama-local-xml-xml \
	gama-local-html \
	gama-local-equivalents \
	gama-local-xml-results

if GNU_GAMA_LOCAL_TEST_SQLITE_READER
TESTB = gama-local-sqlite-reader
endif

TESTS = $(TESTA) $(TESTB)

SUBDIRS = scripts input

INPUT_FILES = gama-local \
	      zoltan-test_2d_dms \
	      zoltan-test_2d_gon \
	      zoltan-test_3d_dms \
	      zoltan-test_3d_gon \
	      tst-tetrahedron-2 \
	      tst-tetrahedron-3-deg \
	      tst-tetrahedron-3-gon

GAMA_SCRIPT = $(srcdir)/scripts
GAMA_INPUT = $(srcdir)/input

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
	       -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
	       -e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
	       -e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf results $(TESTS)

gama-local-version: $(GAMA_SCRIPT)/gama-local-version.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-version.in > gama-local-version
	chmod +x gama-local-version

gama-local-adjustment: $(GAMA_SCRIPT)/gama-local-adjustment.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-adjustment.in \
				> gama-local-adjustment
	chmod +x gama-local-adjustment

gama-local-algorithms: $(GAMA_SCRIPT)/gama-local-algorithms.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-algorithms.in \
				> gama-local-algorithms
	chmod +x gama-local-algorithms

gama-local-equivalents: $(GAMA_SCRIPT)/gama-local-equivalents.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-equivalents.in \
				> gama-local-equivalents
	chmod +x gama-local-equivalents

gama-local-xml-results: $(GAMA_SCRIPT)/gama-local-xml-results.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-results.in \
				> gama-local-xml-results
	chmod +x gama-local-xml-results

gama-local-html: $(GAMA_SCRIPT)/gama-local-html.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-html.in \
				> gama-local-html
	chmod +x gama-local-html

gama-local-xml-xml: $(GAMA_SCRIPT)/gama-local-xml-xml.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-xml.in \
				> gama-local-xml-xml
	chmod +x gama-local-xml-xml

gama-local-sqlite-reader: $(GAMA_SCRIPT)/gama-local-sqlite-reader.in Makefile
	$(do_subst) < $(GAMA_SCRIPT)/gama-local-sqlite-reader.in \
				> gama-local-sqlite-reader
	chmod +x gama-local-sqlite-reader

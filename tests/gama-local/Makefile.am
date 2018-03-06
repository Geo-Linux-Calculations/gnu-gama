TESTA = gama-local-version.sh \
	gama-local-adjustment.sh \
	gama-local-algorithms.sh \
	gama-local-xml-xml.sh \
	gama-local-html.sh \
	gama-local-equivalents.sh \
	gama-local-xml-results.sh \
	gama-local-parameters.sh \
	gama-local-updated-xml.sh

if GNU_GAMA_LOCAL_TEST_SQLITE_READER
TESTB = gama-local-sqlite-reader.sh
endif

if GNU_GAMA_LOCAL_TEST_XMLLINT
TESTC = xmllint-gama-local-xsd.sh xmllint-gama-local-adjustment-xsd.sh
endif

if GNU_GAMA_LOCAL_TEST_OCTAVE
TESTD = octave-gama-local-read.sh
endif

TESTS = $(TESTA) $(TESTB) $(TESTC) $(TESTD)

SUBDIRS = scripts input

INPUT_FILES = gama-local \
              zoltan-test_2d_dms \
              zoltan-test_2d_gon \
              zoltan-test_3d_dms \
              zoltan-test_3d_gon \
              tst-tetrahedron-1 \
              tst-tetrahedron-2 \
              tst-tetrahedron-3-deg \
              tst-tetrahedron-3-gon \
              fixed-azimuth \
              azimuth-angle \
              azimuth-azimuth \
              azimuth-distance \
              alex-2015-05-17


GAMA_SCRIPT  = $(srcdir)/scripts
GAMA_INPUT   = $(srcdir)/input
GAMA_RESULTS = results/$(PACKAGE_VERSION)
GAMA_OTHERS  = Makefile $(GAMA_RESULTS)

do_subst = sed -e 's,[@]top_builddir[@],$(top_builddir),g' \
	       -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
	       -e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
	       -e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g' \
	       -e 's,[@]GAMA_RESULTS[@],$(GAMA_RESULTS),g' \
	       -e 's,[@]GAMA_XML[@],$(top_srcdir)/xml,g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(GAMA_RESULTS) $(TESTS)

$(GAMA_RESULTS):
	mkdir -p $(GAMA_RESULTS)

gama-local-version.sh: $(GAMA_SCRIPT)/gama-local-version.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-version.in \
		> gama-local-version.sh
	@chmod +x gama-local-version.sh

gama-local-adjustment.sh: $(GAMA_SCRIPT)/gama-local-adjustment.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-adjustment.in \
				> gama-local-adjustment.sh
	@chmod +x gama-local-adjustment.sh

gama-local-algorithms.sh: $(GAMA_SCRIPT)/gama-local-algorithms.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-algorithms.in \
				> gama-local-algorithms.sh
	@chmod +x gama-local-algorithms.sh

gama-local-equivalents.sh: $(GAMA_SCRIPT)/gama-local-equivalents.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-equivalents.in \
				> gama-local-equivalents.sh
	@chmod +x gama-local-equivalents.sh

gama-local-xml-results.sh: $(GAMA_SCRIPT)/gama-local-xml-results.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-results.in \
				> gama-local-xml-results.sh
	@chmod +x gama-local-xml-results.sh

gama-local-html.sh: $(GAMA_SCRIPT)/gama-local-html.in $(GAMA_OTHERS) \
                 gama-local-adjustment.sh
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-html.in \
				> gama-local-html.sh
	@chmod +x gama-local-html.sh

gama-local-xml-xml.sh: $(GAMA_SCRIPT)/gama-local-xml-xml.in $(GAMA_OTHERS) \
                    gama-local-adjustment.sh
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-xml.in \
				> gama-local-xml-xml.sh
	@chmod +x gama-local-xml-xml.sh

gama-local-parameters.sh: $(GAMA_SCRIPT)/gama-local-parameters.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-parameters.in \
				> gama-local-parameters.sh
	@chmod +x gama-local-parameters.sh

gama-local-updated-xml.sh: $(GAMA_SCRIPT)/gama-local-updated-xml.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-updated-xml.in \
				> gama-local-updated-xml.sh
	@chmod +x gama-local-updated-xml.sh



if GNU_GAMA_LOCAL_TEST_SQLITE_READER
gama-local-sqlite-reader.sh: $(GAMA_SCRIPT)/gama-local-sqlite-reader.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-sqlite-reader.in \
				> gama-local-sqlite-reader.sh
	@chmod +x gama-local-sqlite-reader.sh
endif


if GNU_GAMA_LOCAL_TEST_XMLLINT
xmllint-gama-local-xsd.sh: $(GAMA_SCRIPT)/xmllint-gama-local-xsd.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/xmllint-gama-local-xsd.in \
				> xmllint-gama-local-xsd.sh
	@chmod +x xmllint-gama-local-xsd.sh

xmllint-gama-local-adjustment-xsd.sh: \
		$(GAMA_SCRIPT)/xmllint-gama-local-adjustment-xsd.in \
		$(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/xmllint-gama-local-adjustment-xsd.in \
				> xmllint-gama-local-adjustment-xsd.sh
	@chmod +x xmllint-gama-local-adjustment-xsd.sh
endif


if GNU_GAMA_LOCAL_TEST_OCTAVE
octave-gama-local-read.sh : $(GAMA_SCRIPT)/octave-gama-local-read.in \
		$(GAMA_OTHERS) gama-local-adjustment.sh
	@$(do_subst) < $(GAMA_SCRIPT)/octave-gama-local-read.in \
				> octave-gama-local-read.sh
	@chmod +x octave-gama-local-read.sh
endif


@VALGRIND_CHECK_RULES@
# VALGRIND_SUPPRESSIONS_FILES = matvec.supp
# EXTRA_DIST = matvec.supp

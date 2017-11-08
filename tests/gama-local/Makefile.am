TESTA = gama-local-version \
	gama-local-adjustment \
	gama-local-algorithms \
	gama-local-xml-xml \
	gama-local-html \
	gama-local-equivalents \
	gama-local-xml-results \
	gama-local-parameters \
	gama-local-updated-xml

if GNU_GAMA_LOCAL_TEST_SQLITE_READER
TESTB = gama-local-sqlite-reader
endif

if GNU_GAMA_LOCAL_TEST_XMLLINT
TESTC = xmllint-gama-local-xsd xmllint-gama-local-adjustment-xsd
endif

TESTS = $(TESTA) $(TESTB) $(TESTC)

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

gama-local-version: $(GAMA_SCRIPT)/gama-local-version.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-version.in > gama-local-version
	@chmod +x gama-local-version

gama-local-adjustment: $(GAMA_SCRIPT)/gama-local-adjustment.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-adjustment.in \
				> gama-local-adjustment
	@chmod +x gama-local-adjustment

gama-local-algorithms: $(GAMA_SCRIPT)/gama-local-algorithms.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-algorithms.in \
				> gama-local-algorithms
	@chmod +x gama-local-algorithms

gama-local-equivalents: $(GAMA_SCRIPT)/gama-local-equivalents.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-equivalents.in \
				> gama-local-equivalents
	@chmod +x gama-local-equivalents

gama-local-xml-results: $(GAMA_SCRIPT)/gama-local-xml-results.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-results.in \
				> gama-local-xml-results
	@chmod +x gama-local-xml-results

gama-local-html: $(GAMA_SCRIPT)/gama-local-html.in $(GAMA_OTHERS) \
                 gama-local-adjustment
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-html.in \
				> gama-local-html
	@chmod +x gama-local-html

gama-local-xml-xml: $(GAMA_SCRIPT)/gama-local-xml-xml.in $(GAMA_OTHERS) \
                    gama-local-adjustment
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-xml-xml.in \
				> gama-local-xml-xml
	@chmod +x gama-local-xml-xml

gama-local-parameters: $(GAMA_SCRIPT)/gama-local-parameters.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-parameters.in \
				> gama-local-parameters
	@chmod +x gama-local-parameters

gama-local-updated-xml: $(GAMA_SCRIPT)/gama-local-updated-xml.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-updated-xml.in \
				> gama-local-updated-xml
	@chmod +x gama-local-updated-xml



if GNU_GAMA_LOCAL_TEST_SQLITE_READER
gama-local-sqlite-reader: $(GAMA_SCRIPT)/gama-local-sqlite-reader.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/gama-local-sqlite-reader.in \
				> gama-local-sqlite-reader
	@chmod +x gama-local-sqlite-reader
endif


if GNU_GAMA_LOCAL_TEST_XMLLINT
xmllint-gama-local-xsd: $(GAMA_SCRIPT)/xmllint-gama-local-xsd.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/xmllint-gama-local-xsd.in \
				> xmllint-gama-local-xsd
	@chmod +x xmllint-gama-local-xsd

xmllint-gama-local-adjustment-xsd: $(GAMA_SCRIPT)/xmllint-gama-local-adjustment-xsd.in $(GAMA_OTHERS)
	@$(do_subst) < $(GAMA_SCRIPT)/xmllint-gama-local-adjustment-xsd.in \
				> xmllint-gama-local-adjustment-xsd
	@chmod +x xmllint-gama-local-adjustment-xsd
endif

@VALGRIND_CHECK_RULES@
# VALGRIND_SUPPRESSIONS_FILES = matvec.supp
# EXTRA_DIST = matvec.supp

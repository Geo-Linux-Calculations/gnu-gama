EXTRA_DIST = CMakeLists.txt \
             gama-local-adjustment.sh_  \
             gama-local-algorithms.sh_  \
             gama-local-equivalents.sh_ \
             gama-local-html.sh_ \
             gama-local-xml-results.sh_ \
             gama-local-xml-xml.sh_ \
             gama-local-sqlite-reader.sh_ \
             gama-local-version.sh_ \
             gama-local-parameters.sh_ \
             gama-local-export.sh_ \
             gama-local-externs.sh_ \
             gama-local-yaml2gkf.sh_ \
             gama-local-gkf2yaml.sh_ \
             xmllint-gama-local-yaml2gkf.sh_ \
             xmllint-gama-local-xsd.sh_ \
             xmllint-gama-local-adjustment-xsd.sh_ \
             octave-gama-local.sh_


TESTA = gama-local-version.sh \
        gama-local-adjustment.sh \
        gama-local-algorithms.sh \
        gama-local-xml-xml.sh \
        gama-local-html.sh \
        gama-local-equivalents.sh \
        gama-local-xml-results.sh \
        gama-local-parameters.sh \
        gama-local-export.sh \
        gama-local-externs.sh

if GNU_GAMA_LOCAL_TEST_SQLITE_READER
TESTB = gama-local-sqlite-reader.sh
endif

if GNU_GAMA_LOCAL_TEST_OCTAVE
TESTC = octave-gama-local.sh
endif

if GNU_GAMA_LOCAL_TEST_YAML_CPP
TESTD = gama-local-yaml2gkf.sh gama-local-gkf2yaml.sh
endif

if GNU_GAMA_LOCAL_TEST_XMLLINT
if GNU_GAMA_LOCAL_TEST_YAML_CPP
TESTE = xmllint-gama-local-yaml2gkf.sh
endif
endif

if GNU_GAMA_LOCAL_TEST_XMLLINT
TESTF = xmllint-gama-local-xsd.sh xmllint-gama-local-adjustment-xsd.sh
endif


TESTS = $(TESTA) $(TESTB) $(TESTC) $(TESTD) $(TESTE) $(TESTF)

SUBDIRS = src input

# Removed in 2.16 zoltan-test_3d_dms
#		  zoltan-test_3d_gon
#
INPUT_FILES = gama-local \
              zoltan-test_2d_dms \
              zoltan-test_2d_gon \
              fixed-azimuth \
              azimuth-angle \
              azimuth-azimuth \
              azimuth-distance \
              jezerka-ang \
              jezerka-dir \
              triangle-1 \
              triangle-2 \
              stroner-levelling-a \
              stroner-levelling-b \
              extern-azimuth-distance \
              extern-seq-dsuloha-d

BUG_FILES =   tst-tetrahedron-1 \
              tst-tetrahedron-2 \
              tst-tetrahedron-3-deg \
              tst-tetrahedron-3-gon \
              krasovsky-1926 \
              extern-tst-tetrahedron-3-gon \
              bug-2019-08-06-integer-coordinates \
              bug-2019-08-13-traverse-knin_male \
              bug-2019-08-20-knin_test

# Removed in 2.16 zoltan-test_3d_dms
#
YAML_FILES =  geodet-pc-123 geodet-pc-218 \
              jezerka-ang   jezerka-dir \
              zoltan-test_2d_gon \
              seq-dsuloha-d \
              mikhail-7.4 \
              mikhail-7.4-cov \
              tetrahedron \
              cube-1 \
              minimal

GAMA_SRC     = $(srcdir)/src
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
	               -e 's,[@]BUG_FILES[@],$(BUG_FILES),g' \
	               -e 's,[@]YAML_FILES[@],$(YAML_FILES),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(GAMA_RESULTS) $(TESTS)

$(GAMA_RESULTS):
	mkdir -p $(GAMA_RESULTS)

gama-local-version.sh: $(srcdir)/gama-local-version.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-version.sh_ \
	             > gama-local-version.sh
	@chmod +x gama-local-version.sh

gama-local-adjustment.sh: $(srcdir)/gama-local-adjustment.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-adjustment.sh_ \
	             > gama-local-adjustment.sh
	@chmod +x gama-local-adjustment.sh

gama-local-algorithms.sh: $(srcdir)/gama-local-algorithms.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-algorithms.sh_ \
	             > gama-local-algorithms.sh
	@chmod +x gama-local-algorithms.sh

gama-local-equivalents.sh: $(srcdir)/gama-local-equivalents.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-equivalents.sh_ \
	             > gama-local-equivalents.sh
	@chmod +x gama-local-equivalents.sh

gama-local-xml-results.sh: $(srcdir)/gama-local-xml-results.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-xml-results.sh_ \
	             > gama-local-xml-results.sh
	@chmod +x gama-local-xml-results.sh

gama-local-html.sh: $(srcdir)/gama-local-html.sh_ $(GAMA_OTHERS) \
                       gama-local-adjustment.sh
	@$(do_subst) < $(srcdir)/gama-local-html.sh_ \
	             > gama-local-html.sh
	@chmod +x gama-local-html.sh

gama-local-xml-xml.sh: $(srcdir)/gama-local-xml-xml.sh_ $(GAMA_OTHERS) \
                       gama-local-adjustment.sh
	@$(do_subst) < $(srcdir)/gama-local-xml-xml.sh_ \
	             > gama-local-xml-xml.sh
	@chmod +x gama-local-xml-xml.sh

gama-local-parameters.sh: $(srcdir)/gama-local-parameters.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-parameters.sh_ \
	             > gama-local-parameters.sh
	@chmod +x gama-local-parameters.sh

gama-local-export.sh: $(srcdir)/gama-local-export.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-export.sh_ \
	             > gama-local-export.sh
	@chmod +x gama-local-export.sh

gama-local-externs.sh: $(srcdir)/gama-local-externs.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-externs.sh_ \
	             > gama-local-externs.sh
	@chmod +x gama-local-externs.sh



if GNU_GAMA_LOCAL_TEST_SQLITE_READER
gama-local-sqlite-reader.sh: $(srcdir)/gama-local-sqlite-reader.sh_ \
	                     $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/gama-local-sqlite-reader.sh_ \
	             > gama-local-sqlite-reader.sh
	@chmod +x gama-local-sqlite-reader.sh
endif


if GNU_GAMA_LOCAL_TEST_XMLLINT
xmllint-gama-local-xsd.sh: $(srcdir)/xmllint-gama-local-xsd.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/xmllint-gama-local-xsd.sh_ \
	             > xmllint-gama-local-xsd.sh
	@chmod +x xmllint-gama-local-xsd.sh

xmllint-gama-local-adjustment-xsd.sh: \
	      $(srcdir)/xmllint-gama-local-adjustment-xsd.sh_ $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/xmllint-gama-local-adjustment-xsd.sh_ \
	             > xmllint-gama-local-adjustment-xsd.sh
	@chmod +x xmllint-gama-local-adjustment-xsd.sh
endif


if GNU_GAMA_LOCAL_TEST_OCTAVE
octave-gama-local.sh : $(srcdir)/octave-gama-local.sh_ \
	               $(GAMA_OTHERS) gama-local-adjustment.sh
	@$(do_subst) < $(srcdir)/octave-gama-local.sh_ \
	             > octave-gama-local.sh
	@chmod +x octave-gama-local.sh
endif


if GNU_GAMA_LOCAL_TEST_YAML_CPP
gama-local-yaml2gkf.sh : $(srcdir)/gama-local-yaml2gkf.sh_ \
	               $(GAMA_OTHERS) gama-local-adjustment.sh
	@$(do_subst) < $(srcdir)/gama-local-yaml2gkf.sh_ \
	             > gama-local-yaml2gkf.sh
	@chmod +x gama-local-yaml2gkf.sh

gama-local-gkf2yaml.sh : $(srcdir)/gama-local-gkf2yaml.sh_ \
	               $(GAMA_OTHERS) gama-local-adjustment.sh
	@$(do_subst) < $(srcdir)/gama-local-gkf2yaml.sh_ \
	             > gama-local-gkf2yaml.sh
	@chmod +x gama-local-gkf2yaml.sh
endif


if GNU_GAMA_LOCAL_TEST_XMLLINT
if GNU_GAMA_LOCAL_TEST_YAML_CPP
xmllint-gama-local-yaml2gkf.sh: $(srcdir)/xmllint-gama-local-yaml2gkf.sh_ \
                              $(GAMA_OTHERS)
	@$(do_subst) < $(srcdir)/xmllint-gama-local-yaml2gkf.sh_ \
	             > xmllint-gama-local-yaml2gkf.sh
	@chmod +x xmllint-gama-local-yaml2gkf.sh
endif
endif


@VALGRIND_CHECK_RULES@
# VALGRIND_SUPPRESSIONS_FILES = matvec.supp
# EXTRA_DIST = matvec.supp

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
              local_3d \
              extern-azimuth-distance \
              extern-seq-dsuloha-d


BUG_FILES =   bug/tst-tetrahedron-1 \
              bug/tst-tetrahedron-2 \
              bug/tst-tetrahedron-3-deg \
              bug/tst-tetrahedron-3-gon \
              bug/krasovsky-1926 \
              bug/extern-tst-tetrahedron-3-gon \
              bug/2019-08-06-integer-coordinates \
              bug/2019-08-13-traverse-knin_male \
              bug/2019-08-20-knin_test \
              bug/test-linearization-angle


CTU_FILES =   ctu/2020-barta-phase_0-1TK ctu/2020-barta-phase_0-2TK \
              ctu/2020-barta-phase_1-1TK ctu/2020-barta-phase_1-2TK

FROMTODH_FILES = sim/from-to-dh

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


KRUMM_FILES = krumm/3D/Ghilani_GNSS_Baselines


KRUMM_FILES_EXTRA = krumm/1D/Baumann_Height_fix \
  krumm/1D/Krumm_Height_fix \
  krumm/1D/Ghilani12_6_Height_fix \
  krumm/1D/Niemeier_Height_fix1 \
  krumm/1D/Krumm_Height_dyn \
  krumm/1D/Niemeier_Height_free \
  krumm/2D/Benning82_Distance_fix \
  krumm/2D/Hoepke_Distance_free \
  krumm/2D/Benning83_DistanceDirection_fix \
  krumm/2D/LotherStrehle_Direction1 \
  krumm/2D/Benning85 \
  krumm/2D/LotherStrehle_Direction2 \
  krumm/2D/Benning88_Distance_fix \
  krumm/2D/LotherStrehle_Direction3 \
  krumm/2D/Carosio_DistanceDirection_fix \
  krumm/2D/LotherStrehle_Direction4 \
  krumm/2D/Ghilani14_5_Distance_fix \
  krumm/2D/LotherStrehle_Direction5 \
  krumm/2D/Ghilani15_4_Angle_fix \
  krumm/2D/LotherStrehle_Direction7 \
  krumm/2D/Ghilani15_5_Angle_fix \
  krumm/2D/Niemeier_DistanceDirection_fix \
  krumm/2D/Ghilani16_1_Traverse \
  krumm/2D/StrangBorre_Distance_fix \
  krumm/2D/Ghilani16_2_DistanceAngleAzimuth_fix \
  krumm/2D/StrangBorre_Distance_free \
  krumm/2D/Ghilani21_10_DistanceAngle_fix \
  krumm/2D/WeissEtAl_Distance_fix \
  krumm/2D/Ghilani_Wolf_Distance_Angle \
  krumm/2D/Wolf_DistanceDirectionAngle_free \
  krumm/2D/Grossmann_Direction_fix \
  krumm/3D/Baumann23_3_4_fix \
  krumm/3D/Wolf_3D_Distance_fix \
  krumm/3D/Caspary \
  krumm/3D/Wolf_3D_DistanceVerticalAngle_fix \
  krumm/3D/Wolf_SpatialPolygonTraverse_fix


GAMA_SRC     = $(srcdir)/src
GAMA_INPUT   = $(srcdir)/input
GAMA_RESULTS = results/$(PACKAGE_VERSION)
GAMA_OTHERS  = Makefile $(GAMA_RESULTS)

do_subst = sed \
-e 's,[@]top_builddir[@],$(top_builddir),g' \
-e 's,[@]top_srcdir[@],$(top_srcdir),g' \
-e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
-e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
-e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g' \
-e 's,[@]GAMA_RESULTS[@],$(GAMA_RESULTS),g' \
-e 's,[@]GAMA_XML[@],$(top_srcdir)/xml,g' \
-e 's,[@]BUG_FILES[@],$(BUG_FILES),g' \
-e 's,[@]CTU_FILES[@],$(CTU_FILES),g' \
-e 's,[@]KRUMM_FILES[@],$(KRUMM_FILES),g' \
-e 's,[@]FROMTODH_FILES[@],$(FROMTODH_FILES),g' \
-e 's,[@]YAML_FILES[@],$(YAML_FILES),g' \
-e 's,[@]GNU_GAMA_LOCAL_TEST_XMLLINT[@],$(GNU_GAMA_LOCAL_TEST_XMLLINT),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(GAMA_RESULTS) $(TESTS)

$(GAMA_RESULTS):
	$(MKDIR_P) $(GAMA_RESULTS)

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

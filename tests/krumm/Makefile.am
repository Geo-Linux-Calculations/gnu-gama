EXTRA_DIST = krumm2gama-local.sh_

bin_PROGRAMS = cmp_xml_file

cmp_xml_file_SOURCES  = cmp_xml_file_main.cpp \
                        cmp_xml_file.h cmp_xml_file.cpp
cmp_xml_file_LDADD    = $(top_builddir)/lib/libgama.a
cmp_xml_file_CPPFLAGS = -I $(top_srcdir)/lib

TESTS = krumm2gama-local.sh

KRUMM_SRC     = $(srcdir)/src
KRUMM_INPUT   = $(srcdir)/input
KRUMM_RESULTS = results/$(PACKAGE_VERSION)
KRUMM_OTHERS  = Makefile $(KRUMM_RESULTS)

INPUT_FILES = \
  1D/Baumann_Height_fix \
  1D/Ghilani12_6_Height_fix \
  1D/Krumm_Height_dyn \
  1D/Krumm_Height_fix \
  1D/Niemeier_Height_fix1 \
  1D/Niemeier_Height_free \
  2D/Benning82_Distance_fix \
  2D/Benning83_DistanceDirection_fix \
  2D/Benning85 \
  2D/Benning88_Distance_fix \
  2D/Carosio_DistanceDirection_fix \
  2D/Ghilani14_5_Distance_fix \
  2D/Ghilani15_4_Angle_fix \
  2D/Ghilani15_5_Angle_fix \
  2D/Ghilani16_1_Traverse \
  2D/Ghilani16_2_DistanceAngleAzimuth_fix \
  2D/Ghilani21_10_DistanceAngle_fix \
  2D/Ghilani_Wolf_Distance_Angle \
  2D/Grossmann_Direction_fix \
  2D/Hoepke_Distance_free \
  2D/LotherStrehle_Direction1 \
  2D/LotherStrehle_Direction2 \
  2D/LotherStrehle_Direction3 \
  2D/LotherStrehle_Direction4 \
  2D/LotherStrehle_Direction5 \
  2D/LotherStrehle_Direction7 \
  2D/Niemeier_DistanceDirection_fix \
  2D/StrangBorre_Distance_fix \
  2D/StrangBorre_Distance_free \
  2D/WeissEtAl_Distance_fix \
  2D/Wolf_DistanceDirectionAngle_free \
  3D/Baumann23_3_4_fix \
  3D/Wolf_3D_Distance_fix \
  3D/Wolf_3D_DistanceVerticalAngle_fix \
  3D/Wolf_SpatialPolygonTraverse_fix \
  3D/Ghilani_GNSS_Baselines \
  3D/Caspary

  # ******  unused example files  ******

  # 1D/LotherStrehle_Height_1
  # 1D/LotherStrehle_Height_2
  # 1D/LotherStrehle_Height_3
  # 1D/LotherStrehle_Height_4
  # 1D/LotherStrehle_Height_5
  # 1D/LotherStrehle_Height_6
  # 1D/Mittermayer_Height_fix
  # 1D/Mittermayer_Height_free
  # 2D/Benning83_DistanceDirection_fix_Mb #scale factor is not implemented
  # 2D/Ghilani21_1_DistanceAngle_fix # gama-local can detect only one blunder
  # 2D/Hoepke_Distance_fix  # gama-local does not enable fixing only x coord.
  # 2D/JaegerEtAl_DistanceDirection_fix # not in Examples
  # 2D/Krumm_Traverse1 # gama-local cannot deduce that the azimuth to
  # 2D/Krumm_Traverse2 #     an unknown point P can be combined with
  # 2D/Krumm_Traverse3 #     an angle pointing to P to create a new azimuth
  # 2D/Krumm_Traverse4 # Nonlinear conditions are not implemented in gama-local
  # 2D/Leick53  # .................................
  # 2D/Leick54  # BLH coordinates are not supported
  # 2D/Leick55  #
  # 2D/Leick56  # .................................
  # 2D/LotherStrehle_Direction6  # dyn points with zero variances of xy
  # 2D/Wolf_Direction_fix_with_cond # explicit conditions are not implemented
  # 3D/BlankenbachWillert3D_Distance_fix  # two blunders, not used


do_subst = sed \
-e 's,[@]top_builddir[@],$(top_builddir),g' \
-e 's,[@]top_srcdir[@],$(top_srcdir),g' \
-e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
-e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
-e 's,[@]KRUMM_INPUT[@],$(KRUMM_INPUT),g' \
-e 's,[@]KRUMM_RESULTS[@],$(KRUMM_RESULTS),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(KRUMM_RESULTS) $(TESTS)

$(KRUMM_RESULTS):
	$(MKDIR_P) $(KRUMM_RESULTS)
	$(MKDIR_P) $(KRUMM_RESULTS)/1D
	$(MKDIR_P) $(KRUMM_RESULTS)/2D
	$(MKDIR_P) $(KRUMM_RESULTS)/3D

krumm2gama-local.sh: $(srcdir)/krumm2gama-local.sh_ $(KRUMM_OTHERS)
	@$(do_subst) < $(srcdir)/krumm2gama-local.sh_ \
	             > krumm2gama-local.sh
	@chmod +x krumm2gama-local.sh

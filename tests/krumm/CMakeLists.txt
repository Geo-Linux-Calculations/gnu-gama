set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/krumm)
set(KRUMM2GAMA    ${CMAKE_BINARY_DIR}/krumm2gama-local)
set(GAMA_LOCAL    ${CMAKE_BINARY_DIR}/gama-local)
set(CMP_XML_FILE  ${CMAKE_BINARY_DIR}/tests/krumm/cmp_xml_file)
set(INPUT_DIR     ${TEST_BASE_DIR}/input)
set(RESULT_DIR    ${CMAKE_BINARY_DIR}/tests/krumm/results/${PROJECT_VERSION})

file(MAKE_DIRECTORY ${RESULT_DIR})
file(MAKE_DIRECTORY ${RESULT_DIR}/1D)
file(MAKE_DIRECTORY ${RESULT_DIR}/2D)
file(MAKE_DIRECTORY ${RESULT_DIR}/3D)

#file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-adjustment/bug)

set(INPUT_FILES
  1D/Baumann_Height_fix
  1D/Ghilani12_6_Height_fix
  1D/Krumm_Height_dyn
  1D/Krumm_Height_fix
  1D/Niemeier_Height_fix1
  1D/Niemeier_Height_free
  2D/Benning82_Distance_fix
  2D/Benning83_DistanceDirection_fix
  2D/Benning85
  2D/Benning88_Distance_fix
  2D/Carosio_DistanceDirection_fix
  2D/Ghilani14_5_Distance_fix
  2D/Ghilani15_4_Angle_fix
  2D/Ghilani15_5_Angle_fix
  2D/Ghilani16_1_Traverse
  2D/Ghilani16_2_DistanceAngleAzimuth_fix
  2D/Ghilani21_10_DistanceAngle_fix
  2D/Ghilani_Wolf_Distance_Angle
  2D/Grossmann_Direction_fix
  2D/Hoepke_Distance_free
  2D/LotherStrehle_Direction1
  2D/LotherStrehle_Direction2
  2D/LotherStrehle_Direction3
  2D/LotherStrehle_Direction4
  2D/LotherStrehle_Direction5
  2D/LotherStrehle_Direction7
  2D/Niemeier_DistanceDirection_fix
  2D/StrangBorre_Distance_fix
  2D/StrangBorre_Distance_free
  2D/WeissEtAl_Distance_fix
  2D/Wolf_DistanceDirectionAngle_free
  3D/Baumann23_3_4_fix
  3D/Wolf_3D_Distance_fix
  3D/Wolf_3D_DistanceVerticalAngle_fix
  3D/Wolf_SpatialPolygonTraverse_fix
  3D/Ghilani_GNSS_Baselines
  3D/Caspary
)

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

add_executable(cmp_xml_file cmp_xml_file_main.cpp
               cmp_xml_file.h cmp_xml_file.cpp $<TARGET_OBJECTS:libgama>)


foreach(test ${INPUT_FILES})

  add_test(NAME krumm_${test}_gkf
        COMMAND ${KRUMM2GAMA} -e  ${INPUT_DIR}/${test}.dat
                                  ${RESULT_DIR}/${test}.gkf)
  add_test(NAME krumm_${test}_adj
        COMMAND ${GAMA_LOCAL} ${RESULT_DIR}/${test}.gkf
                       --text ${RESULT_DIR}/${test}.txt
                       --xml  ${RESULT_DIR}/${test}.xml)
  add_test (NAME krumm_${test}_cmp
         COMMAND ${CMP_XML_FILE} ${RESULT_DIR}/${test}.xml
         ${INPUT_DIR}/${test}.adj)

endforeach(test)

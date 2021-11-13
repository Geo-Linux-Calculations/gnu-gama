set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/gama-local)
set(GAMA_LOCAL ${CMAKE_BINARY_DIR}/gama-local )
set(INPUT_DIR ${TEST_BASE_DIR}/input)
set(INPUT_FILES
    gama-local
    zoltan-test_2d_dms
    zoltan-test_2d_gon
    tetrahedron
    fixed-azimuth
    azimuth-angle
    azimuth-azimuth
    azimuth-distance
    jezerka-ang
    jezerka-dir
    triangle-1
    triangle-2
    stroner-levelling-a
    stroner-levelling-b
    extern-azimuth-distance
    extern-seq-dsuloha-d

    bug/tst-tetrahedron-1
    bug/tst-tetrahedron-2
    bug/tst-tetrahedron-3-deg
    bug/tst-tetrahedron-3-gon
    bug/krasovsky-1926
    bug/extern-tst-tetrahedron-3-gon
    bug/2019-08-06-integer-coordinates
    bug/2019-08-13-traverse-knin_male
    bug/2019-08-20-knin_test
  )

set(RESULT_DIR ${CMAKE_BINARY_DIR}/tests/gama-local/results/${PROJECT_VERSION})
file(MAKE_DIRECTORY ${RESULT_DIR})
file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-adjustment/bug)


# ------------------------------------------------------------------------
#
# check version
#
add_executable(check_version src/check_version.cpp
  $<TARGET_OBJECTS:libgama>)
add_test(NAME check_version COMMAND check_version
  ${PROJECT_SOURCE_DIR}/configure.ac ${PROJECT_SOURCE_DIR}/CMakeLists.txt)



# -------------------------------------------------------------------------
#
# check_adjustment
#

file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-adjustment)

foreach(test ${INPUT_FILES})
  foreach(algo svd gso cholesky envelope)
    add_test(NAME gama_local_adjustement_${test}_${algo}
      COMMAND ${GAMA_LOCAL} ${INPUT_DIR}/${test}.gkf --algorithm ${algo}
        --text   ${RESULT_DIR}/gama-local-adjustment/${test}-${algo}.txt
        --html   ${RESULT_DIR}/gama-local-adjustment/${test}-${algo}.html
        --xml    ${RESULT_DIR}/gama-local-adjustment/${test}-${algo}.xml
        --octave ${RESULT_DIR}/gama-local-adjustment/${test}-${algo}.m
        --svg    ${RESULT_DIR}/gama-local-adjustment/${test}-${algo}.svg
      )
  endforeach(algo)
endforeach(test)



# ------------------------------------------------------------------------
#
# check_algorithms
#
add_executable(check_algorithms src/check_xyz.h src/check_xyz.cpp
  src/check_algorithms.cpp $<TARGET_OBJECTS:libgama>)

foreach(test ${INPUT_FILES})
  add_test(NAME check_algorithms_${test}
    COMMAND check_algorithms ${PROJECT_VERSION} ${test}
    ${INPUT_DIR}/${test}.gkf )
endforeach(test)



# ------------------------------------------------------------------------
#
# check_xml_xml
#

add_executable(check_xml_xml src/check_xml_xml.cpp
  src/compare_xml_adjustment.h src/compare_xml_adjustment.cpp
  $<TARGET_OBJECTS:libgama>)

set(RES ${RESULT_DIR}/gama-local-adjustment)

foreach(z ${INPUT_FILES})
  foreach(algorithms gso:svd gso:cholesky gso:envelope
                     svd:cholesky svd:envelope cholesky:envelope)
    string(REPLACE ":" ";" test_list ${algorithms})
    list(GET test_list 0 a)
    list(GET test_list 1 b)
    add_test(NAME check_xml_xml_${z}_${a}_${b} COMMAND
      check_xml_xml "${a} ${b} ${z}" ${RES}/${z}-${a}.xml ${RES}/${z}-${b}.xml
    )
  endforeach(algorithms)
endforeach(z)



# -------------------------------------------------------------------------
#
# check_equivalents
#
add_executable(check_equivalents src/check_xyz.h
  src/check_xyz.cpp src/check_equivalents.cpp
  $<TARGET_OBJECTS:libgama>)

set(EQUIVALENTS_TESTS
    # CMake is weird with 2D arrays, so we apply this hack
    # format: <test_name>;<input_a>;<input_b>
    zoltan_2d:zoltan-test_2d_dms:zoltan-test_2d_gon
    scale:scale-cov-gon:scale-cov-dms
    fixed:fixed-constrained:fixed-azimuth
    gama-local:gama-local:gama-local-deprecated
    stroner:stroner-levelling-a:stroner-levelling-b
    tetrahedron:bug/tst-tetrahedron-3-gon:bug/extern-tst-tetrahedron-3-gon
    azimuth:azimuth-distance:extern-azimuth-distance
)

foreach(test ${EQUIVALENTS_TESTS})
    string(REPLACE ":" ";" TEST_LIST ${test})
    list(GET TEST_LIST 0 testname)
    list(GET TEST_LIST 1 input_a)
    list(GET TEST_LIST 2 input_b)
    add_test(
        NAME check_equivalents_${testname}
        COMMAND check_equivalents ${input_a}
        ${INPUT_DIR}/${input_a}.gkf ${input_b} ${INPUT_DIR}/${input_b}.gkf
    )
endforeach(test)

# -------------------------------------------------------------------------
#
# check_xml_results
#
add_executable(check_xml_results  src/check_xml_results.cpp
  src/check_xyz.h src/check_xyz.cpp $<TARGET_OBJECTS:libgama>)
foreach(test ${INPUT_FILES})
  add_test(
    NAME check_xml_results_${test}
    COMMAND check_xml_results ${test}
      ${INPUT_DIR}/${test}.gkf ${INPUT_DIR}/${test}.xml
  )
endforeach(test)



# ------------------------------------------------------------------------
#
# check externs
#
add_executable(check_externs src/check_externs.cpp
  $<TARGET_OBJECTS:libgama>)
add_test(NAME check_externs COMMAND check_externs
  ${INPUT_DIR} ${RESULT_DIR}/gama-local-adjustment ${INPUT_FILES})



# ------------------------------------------------------------------------
#
# check gama-local-yaml2gkf
#
# Testing of gama-local-yaml2gkf is implemented in three independent
# steps
#
if (EXISTS ${CMAKE_SOURCE_DIR}/lib/yaml-cpp)

  file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-yaml2gkf)
  set(YAML_FILES geodet-pc-123 geodet-pc-218
                 jezerka-ang   jezerka-dir
                 zoltan-test_2d_gon
                 zoltan-test_3d_dms
                 seq-dsuloha-d
                 mikhail-7.4
                 mikhail-7.4-cov
                 tetrahedron
                 cube-1
                 minimal)

  # A) We run all yaml files through gama-local-yaml2gkf to generate
  # input gkf files for gama-local adjustment in the next step
  foreach(test ${YAML_FILES})
    add_test(NAME A_gama_local_yaml2gkf_${test}
      COMMAND
      ${CMAKE_BINARY_DIR}/gama-local-yaml2gkf
      ${INPUT_DIR}/${test}.yaml
      ${RESULT_DIR}/gama-local-yaml2gkf/${test}.gkf
      )
  endforeach(test)

  # B) Now we process all files prepared in the previous step by
  # gama-local and store the adjustment results (.xml files)
  foreach(test ${YAML_FILES})
    add_test(NAME B_gama_local_yaml2gkf_${test}
      COMMAND
      ${CMAKE_BINARY_DIR}/gama-local
      ${RESULT_DIR}/gama-local-yaml2gkf/${test}.gkf
      --xml ${RESULT_DIR}/gama-local-yaml2gkf/${test}.xml
      )
  endforeach(test)

  # C) Finally we compare adjustment results (.xml) from the step B
  # with adjustment results from check_xml_results
  foreach(test ${YAML_FILES})
    add_test(NAME C_gama_local_yaml2gkf_${test}
      COMMAND
      check_xml_results ${test}
      ${INPUT_DIR}/${test}.gkf ${RESULT_DIR}/gama-local-yaml2gkf/${test}.xml
      )
  endforeach(test)
endif()

# ------------------------------------------------------------------------
#
# check gama-local-gkf2yaml
#
# Testing of gama-local-gk2yaml is implemented in following four steps
#
if (EXISTS ${CMAKE_SOURCE_DIR}/lib/yaml-cpp)

  file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-gkf2yaml)
  set(GKF2YAML_FILES
     #${INPUT_FILES}
     zoltan-test_2d_gon
     zoltan-test_2d_dms
     krasovsky-1926
     gama-local jezerka-ang mikhail-7.4 cube-1 minimal seq-dsuloha-d
     tst-tetrahedron-1 tst-tetrahedron-2
     tst-tetrahedron-3-gon tst-tetrahedron-3-deg
     zoltan-test_3d_gon
     zoltan-test_3d_dms
     fixed-azimuth
     azimuth-angle
     azimuth-azimuth
     azimuth-distance
     extern-azimuth-distance
     )

  # 1) compute the .yaml file from the given .gkf
  foreach(test ${GKF2YAML_FILES})
    add_test(NAME 1_gama_local_gkf2yaml_${test}
      COMMAND
      ${CMAKE_BINARY_DIR}/gama-local-gkf2yaml
      ${INPUT_DIR}/${test}.gkf
      ${RESULT_DIR}/gama-local-gkf2yaml/${test}.yaml
      )
  endforeach(test)

  # 2) compute another .gkf from the generated .yaml (in the results
  # directory)
  foreach(test ${GKF2YAML_FILES})
    add_test(NAME 2_gama_local_gkf2yaml_${test}
      COMMAND
      ${CMAKE_BINARY_DIR}/gama-local-yaml2gkf
      ${RESULT_DIR}/gama-local-gkf2yaml/${test}.yaml
      ${RESULT_DIR}/gama-local-gkf2yaml/${test}.gkf
      )
  endforeach(test)

  # 3) from the generated .gkf from the previous step compute .xml
  # adjustment results (using gama-local)
  foreach(test ${GKF2YAML_FILES})
    add_test(NAME 3_gama_local_gkf2yaml_${test}
      COMMAND
      ${CMAKE_BINARY_DIR}/gama-local
            ${RESULT_DIR}/gama-local-gkf2yaml/${test}.gkf
      --xml ${RESULT_DIR}/gama-local-gkf2yaml/${test}.xml
      )
  endforeach(test)

  # 4) Finally compare adjustment results (.xml) from the step 3)
  # with archived adjustment results by check_xml_xml
  foreach(test ${GKF2YAML_FILES})
    add_test(NAME 4_gama_local_gkf2yaml_${test}
      COMMAND
      check_xml_xml ${test}
      ${INPUT_DIR}/${test}.xml
      ${RESULT_DIR}/gama-local-gkf2yaml/${test}.xml
      )
  endforeach(test)

endif()

# check_export
#

file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-export)
file(MAKE_DIRECTORY ${RESULT_DIR}/gama-local-export/bug)

foreach(test ${INPUT_FILES})
  add_test(NAME 1st_gama_local_export_${test}
    COMMAND  ${GAMA_LOCAL} ${INPUT_DIR}/${test}.gkf
      --xml    ${RESULT_DIR}/gama-local-export/${test}-1.xml
      --export ${RESULT_DIR}/gama-local-export/${test}-1.gkf
    )
endforeach(test)

foreach(test ${INPUT_FILES})
  add_test(NAME 2nd_gama_local_export_${test}
    COMMAND  ${GAMA_LOCAL}
       ${RESULT_DIR}/gama-local-export/${test}-1.gkf
       --xml ${RESULT_DIR}/gama-local-export/${test}-2.xml
    )
endforeach(test)

foreach(test ${INPUT_FILES})
  add_test(NAME gama_local_export_${test}
    COMMAND check_xml_xml  xml_xml_${test}
    ${RESULT_DIR}/gama-local-export/${test}-1.xml
    ${RESULT_DIR}/gama-local-export/${test}-2.xml
    )
endforeach(test)

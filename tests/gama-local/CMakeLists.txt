set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/gama-local)
set(GAMA_LOCAL ${CMAKE_BINARY_DIR}/gama-local )
set(INPUT_DIR ${TEST_BASE_DIR}/input)
set(INPUT_FILES
    gama-local
    zoltan-test_2d_dms
    zoltan-test_2d_gon
    zoltan-test_3d_dms
    zoltan-test_3d_gon
    tst-tetrahedron-1
    tst-tetrahedron-2
    tst-tetrahedron-3-deg
    tst-tetrahedron-3-gon
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
    extern-tst-tetrahedron-3-gon
    extern-seq-dsuloha-d
    bug-2019-04-23-oleg-goussev
    bug-2019-08-06-integer-coordinates
    bug-2019-08-13-traverse-knin_male
    bug-2019-08-20-knin_test
  )

set(RESULT_DIR ${CMAKE_BINARY_DIR}/tests/gama-local/results/${PROJECT_VERSION})
file(MAKE_DIRECTORY ${RESULT_DIR})


# ------------------------------------------------------------------------
#
# check version
#
add_executable(check_version scripts/check_version.cpp
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
add_executable(check_algorithms scripts/check_xyz.h scripts/check_xyz.cpp
  scripts/check_algorithms.cpp $<TARGET_OBJECTS:libgama>)

foreach(test ${INPUT_FILES})
  add_test(NAME check_algorithms_${test}
    COMMAND check_algorithms ${PROJECT_VERSION} ${test}
    ${INPUT_DIR}/${test}.gkf )
endforeach(test)



# ------------------------------------------------------------------------
#
# check_xml_xml
#
add_executable(check_xml_xml scripts/check_xml_xml.cpp
  scripts/compare_xml_adjustment.h scripts/compare_xml_adjustment.cpp
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
add_executable(check_equivalents scripts/check_xyz.h
  scripts/check_xyz.cpp scripts/check_equivalents.cpp
  $<TARGET_OBJECTS:libgama>)

set(EQUIVALENTS_TESTS
    # CMake is weird with 2D arrays, so we apply this hack
    # format: <test_name>;<input_a>;<input_b>
    zoltan_2d:zoltan-test_2d_dms:zoltan-test_2d_gon
    zoltan_3d:zoltan-test_3d_dms:zoltan-test_3d_gon
    scale:scale-cov-gon:scale-cov-dms
    fixed:fixed-constrained:fixed-azimuth
    gama-local:gama-local:gama-local-deprecated
    stroner:stroner-levelling-a:stroner-levelling-b
    tetrahedron:tst-tetrahedron-3-gon:extern-tst-tetrahedron-3-gon
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
add_executable(check_xml_results  scripts/check_xml_results.cpp
  scripts/check_xyz.h scripts/check_xyz.cpp $<TARGET_OBJECTS:libgama>)
foreach(test ${INPUT_FILES})
  add_test(
    NAME check_xml_results_${test}
    COMMAND check_xml_results ${test}
      ${INPUT_DIR}/${test}.gkf ${INPUT_DIR}/${test}.xml
  )
endforeach(test)

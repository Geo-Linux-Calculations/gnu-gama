set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/gama-local)
set(RESULT_DIR ${CMAKE_BINARY_DIR}/tests/gama-local/results/${GAMA_VERSION}/gama-local-adjustment)
set(INPUT_DIR ${TEST_BASE_DIR}/input)

file(MAKE_DIRECTORY ${RESULT_DIR})

# check_algorithms
add_executable(check_algorithms scripts/check_xyz.h scripts/check_xyz.cpp scripts/check_algorithms.cpp $<TARGET_OBJECTS:libgama>)

set(ALGORITHMS_TESTS
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
    alex-2015-05-17
    triangle-1
    triangle-2
    stroner-levelling-a
    stroner-levelling-b
    extern-azimuth-distance
    extern-tst-tetrahedron-3-gon
)

foreach(test ${ALGORITHMS_TESTS})
    add_test(NAME check_algorithms_${test} COMMAND check_algorithms ${PROJECT_VERSION} ${test} ${INPUT_DIR}/${test}.gkf )
endforeach(test)

add_executable(check_equivalents scripts/check_xyz.h scripts/check_xyz.cpp scripts/check_equivalents.cpp $<TARGET_OBJECTS:libgama>)

# check_equivalents
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
        COMMAND check_equivalents ${input_a} ${INPUT_DIR}/${input_a}.gkf ${input_b} ${INPUT_DIR}/${input_b}.gkf
    )
endforeach(test)

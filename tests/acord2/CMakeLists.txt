set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/acord2)
set(RESULT_DIR ${CMAKE_BINARY_DIR}/tests/acord2/results/${VERSION}/acord2-a2g)
set(INPUT_DIR ${TEST_BASE_DIR}/input)

file(MAKE_DIRECTORY ${RESULT_DIR})

add_executable(a2g a2g-main.cpp a2g.cpp a2g.h $<TARGET_OBJECTS:libgama>)

set(A2G_FILES
  angle-01.a2g
  angle-01.0.a2g
  polar-01.a2g
  polar-02.a2g
  traverse-01.a2g
  traverse-02.a2g
  traverse-03.a2g
  trivial-01.a2g
  trivial-02.a2g
  )
#   azimuth-01.a2g

add_test(NAME acord2-a2g COMMAND a2g ${RESULT_DIR}/a2g
         ${INPUT_DIR}/a2g ${A2G_FILES})


add_executable(a2diff a2diff-main.cpp $<TARGET_OBJECTS:libgama>)

set(A2DIFF_FILES
  traverse-01-en-right.gkf
  traverse-01-ne-left.gkf
  traverse-01-es-left.gkf
  traverse-01-es-right.gkf
  traverse-01-ne-right.gkf
  traverse-01-nw-left.gkf
  traverse-01-nw-right.gkf
  traverse-01-se-left.gkf
  traverse-01-se-right.gkf
  traverse-01-sw-left.gkf
  traverse-01-sw-right.gkf
  traverse-01-wn-left.gkf
  traverse-01-wn-right.gkf
  traverse-01-ws-left.gkf
  traverse-01-ws-right.gkf
  gama-local.gkf
  )

add_test(NAME acord2-a2diff COMMAND a2diff ${RESULT_DIR}/a2diff
        ${INPUT_DIR}/a2diff ${A2DIFF_FILES})

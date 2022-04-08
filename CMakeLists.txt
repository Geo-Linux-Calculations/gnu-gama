cmake_minimum_required(VERSION 3.5)

project (gnu-gama VERSION 2.17.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DGNU_gama_expat_1_1)
#add_definitions(-DDEBUG_ACORD2)
#add_definitions(-DA2G_DEBUG)
#add_definitions(-DDEBUG_REDUCED_OBS)

# Library
#
set(SRC_GAMA
   lib/gnu_gama/e3.cpp
   lib/gnu_gama/e3.h
   lib/gnu_gama/ellipsoid.cpp
   lib/gnu_gama/ellipsoid.h
   lib/gnu_gama/ellipsoids.cpp
   lib/gnu_gama/ellipsoids.h
   lib/gnu_gama/exception.h
   lib/gnu_gama/gon2deg.cpp
   lib/gnu_gama/gon2deg.h
   lib/gnu_gama/intfloat.h
   lib/gnu_gama/latlong.cpp
   lib/gnu_gama/latlong.h
   lib/gnu_gama/model.h
   lib/gnu_gama/movetofront.h
   lib/gnu_gama/obsdata.h
   lib/gnu_gama/outstream.cpp
   lib/gnu_gama/outstream.h
   lib/gnu_gama/pointbase.h
   lib/gnu_gama/radian.h
   lib/gnu_gama/comb.cpp
   lib/gnu_gama/comb.h
   lib/gnu_gama/simplified.h
   lib/gnu_gama/simplified.cpp
   lib/gnu_gama/statan.cpp
   lib/gnu_gama/statan.h
   lib/gnu_gama/utf8.cpp
   lib/gnu_gama/utf8.h
   lib/gnu_gama/version.cpp
   lib/gnu_gama/version.h
   lib/gnu_gama/visitor.h
   lib/gnu_gama/xml_expat.h
   lib/gnu_gama/xsd.h

   lib/gnu_gama/adj/adj_base.h
   lib/gnu_gama/adj/adj_basefull.h
   lib/gnu_gama/adj/adj_basesparse.h
   lib/gnu_gama/adj/adj_chol.h
   lib/gnu_gama/adj/adj_envelope.h
   lib/gnu_gama/adj/adj.cpp
   lib/gnu_gama/adj/adj.h
   lib/gnu_gama/adj/adj_input_data.cpp
   lib/gnu_gama/adj/adj_input_data.h
   lib/gnu_gama/adj/adj_gso.h
   lib/gnu_gama/adj/adj_svd.h
   lib/gnu_gama/adj/envelope.h
   lib/gnu_gama/adj/homogenization.h

   lib/gnu_gama/sparse/intlist.h
   lib/gnu_gama/sparse/sbdiagonal.h
   lib/gnu_gama/sparse/smatrix_graph_connected.h
   lib/gnu_gama/sparse/smatrix_graph.h
   lib/gnu_gama/sparse/smatrix_ordering.h
   lib/gnu_gama/sparse/smatrix.h
   lib/gnu_gama/sparse/svector.h

   lib/gnu_gama/xml/baseparser.cpp
   lib/gnu_gama/xml/baseparser.h
   lib/gnu_gama/xml/encoding_cp1251.cpp
   lib/gnu_gama/xml/encoding.cpp
   lib/gnu_gama/xml/encoding.h
   lib/gnu_gama/xml/encoding_unknown_handler.cpp
   lib/gnu_gama/xml/htmlparser.cpp
   lib/gnu_gama/xml/htmlparser.h
   lib/gnu_gama/xml/localnetworkoctave.cpp
   lib/gnu_gama/xml/localnetworkoctave.h
   lib/gnu_gama/xml/localnetworksql.cpp
   lib/gnu_gama/xml/localnetworksql.h
   lib/gnu_gama/xml/localnetworkxml.cpp
   lib/gnu_gama/xml/localnetworkxml.h
   lib/gnu_gama/xml/localnetwork_adjustment_results.cpp
   lib/gnu_gama/xml/localnetwork_adjustment_results.h
   lib/gnu_gama/xml/localnetwork_adjustment_results_data.cpp
   lib/gnu_gama/xml/localnetwork_adjustment_results_data.h
   lib/gnu_gama/xml/gkfparser.cpp
   lib/gnu_gama/xml/gkfparser.h
   lib/gnu_gama/xml/str2xml.cpp
   lib/gnu_gama/xml/str2xml.h

   lib/gnu_gama/local/angobs.h
   lib/gnu_gama/local/bearing.cpp
   lib/gnu_gama/local/bearing.h
   lib/gnu_gama/local/html.cpp
   lib/gnu_gama/local/html.h
   lib/gnu_gama/local/cluster.h
   lib/gnu_gama/local/exception.h
   lib/gnu_gama/local/float.h
   lib/gnu_gama/local/itstream.h
   lib/gnu_gama/local/language.cpp
   lib/gnu_gama/local/language.h
   lib/gnu_gama/local/svg.cpp
   lib/gnu_gama/local/svg.h
   lib/gnu_gama/local/acord/approx_heights.cpp
   lib/gnu_gama/local/acord/approx_heights.h
   lib/gnu_gama/local/acord/approx_vectors.cpp
   lib/gnu_gama/local/acord/approx_vectors.h
   lib/gnu_gama/local/acord/acord2.cpp
   lib/gnu_gama/local/acord/acord2.h
   lib/gnu_gama/local/acord/acordalgorithm.cpp
   lib/gnu_gama/local/acord/acordalgorithm.h
   lib/gnu_gama/local/acord/acordazimuth.cpp
   lib/gnu_gama/local/acord/acordazimuth.h
   lib/gnu_gama/local/acord/acordhdiff.cpp
   lib/gnu_gama/local/acord/acordhdiff.h
   lib/gnu_gama/local/acord/acordintersection.cpp
   lib/gnu_gama/local/acord/acordintersection.h
   lib/gnu_gama/local/acord/acordpolar.cpp
   lib/gnu_gama/local/acord/acordpolar.h
   lib/gnu_gama/local/acord/acordstatistics.h
   lib/gnu_gama/local/acord/acordstatistics.cpp
   lib/gnu_gama/local/acord/acordtraverse.cpp
   lib/gnu_gama/local/acord/acordtraverse.h
   lib/gnu_gama/local/acord/acordvector.cpp
   lib/gnu_gama/local/acord/acordvector.h
   lib/gnu_gama/local/acord/acordweakchecks.cpp
   lib/gnu_gama/local/acord/acordweakchecks.h
   lib/gnu_gama/local/acord/acordzderived.cpp
   lib/gnu_gama/local/acord/acordzderived.h
   lib/gnu_gama/local/acord/reduce_to_ellipsoid.cpp
   lib/gnu_gama/local/acord/reduce_to_ellipsoid.h
   lib/gnu_gama/local/format.cpp
   lib/gnu_gama/local/format.h
   lib/gnu_gama/local/gamadata.cpp
   lib/gnu_gama/local/gamadata.h
   lib/gnu_gama/local/lcoords.cpp
   lib/gnu_gama/local/lcoords.h
   lib/gnu_gama/local/local_linearization.cpp
   lib/gnu_gama/local/local_linearization.h
   lib/gnu_gama/local/localnetwork2sql.cpp
   lib/gnu_gama/local/localnetwork2sql.h
   lib/gnu_gama/local/lpoint.h
   lib/gnu_gama/local/median/g2d_cogo.cpp
   lib/gnu_gama/local/median/g2d_cogo.h
   lib/gnu_gama/local/median/g2d_coordinates.cpp
   lib/gnu_gama/local/median/g2d_coordinates.h
   lib/gnu_gama/local/median/g2d_exception.h
   lib/gnu_gama/local/median/g2d_helper.cpp
   lib/gnu_gama/local/median/g2d_helper.h
   lib/gnu_gama/local/median/g2d_point.cpp
   lib/gnu_gama/local/median/g2d_point.h
   lib/gnu_gama/local/network.cpp
   lib/gnu_gama/local/network.h
   lib/gnu_gama/local/orientation.cpp
   lib/gnu_gama/local/orientation.h
   lib/gnu_gama/local/results/text/adjusted_observations.h
   lib/gnu_gama/local/results/text/adjusted_unknowns.h
   lib/gnu_gama/local/results/text/approximate_coordinates.h
   lib/gnu_gama/local/results/text/error_ellipses.h
   lib/gnu_gama/local/results/text/fixed_points.h
   lib/gnu_gama/local/results/text/general_parameters.h
   lib/gnu_gama/local/results/text/network_description.h
   lib/gnu_gama/local/results/text/outlying_abs_terms.h
   lib/gnu_gama/local/results/text/reduced_observations.h
   lib/gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h
   lib/gnu_gama/local/results/text/residuals_observations.h
   lib/gnu_gama/local/results/text/underline.cpp
   lib/gnu_gama/local/results/text/underline.h
   lib/gnu_gama/local/test_linearization_visitor.cpp
   lib/gnu_gama/local/test_linearization_visitor.h
   lib/gnu_gama/local/local_revision.cpp
   lib/gnu_gama/local/local_revision.h
   lib/gnu_gama/local/matvec.h
   lib/gnu_gama/local/medianf.h
   lib/gnu_gama/local/observation.cpp
   lib/gnu_gama/local/observation.h
   lib/gnu_gama/local/pointid.cpp
   lib/gnu_gama/local/pointid.h
   lib/gnu_gama/local/readsabw.h
   lib/gnu_gama/local/skipcomm.cpp
   lib/gnu_gama/local/skipcomm.h
   lib/gnu_gama/local/sqlitereader.h
   lib/gnu_gama/local/sqlitereader.cpp
   lib/gnu_gama/local/writevisitor.h
   lib/gnu_gama/local/xmlerror.h
   lib/gnu_gama/local/xmlerror.cpp

   lib/gnu_gama/g3/g3_adjres.cpp
   lib/gnu_gama/g3/g3_adjres.h
   lib/gnu_gama/g3/g3_cluster.cpp
   lib/gnu_gama/g3/g3_cluster.h
   lib/gnu_gama/g3/g3_model.cpp
   lib/gnu_gama/g3/g3_model.h
   lib/gnu_gama/g3/g3_model_init.cpp
   lib/gnu_gama/g3/g3_model_linearization.cpp
   lib/gnu_gama/g3/g3_model_revision.cpp
   lib/gnu_gama/g3/g3_model_write_xml_adjustment_results.cpp
   lib/gnu_gama/g3/g3_observation.h
   lib/gnu_gama/g3/g3_parameter.h
   lib/gnu_gama/g3/g3_point.cpp
   lib/gnu_gama/g3/g3_point.h
   lib/gnu_gama/g3/g3_write_observation_xml.cpp
   lib/gnu_gama/g3/g3_write_observation_xml.h

   lib/gnu_gama/xml/dataobject.cpp
   lib/gnu_gama/xml/dataobject.h
   lib/gnu_gama/xml/dataparser_adj.cpp
   lib/gnu_gama/xml/dataparser.cpp
   lib/gnu_gama/xml/dataparser_g3.cpp
   lib/gnu_gama/xml/dataparser_g3adj.cpp
   lib/gnu_gama/xml/dataparser.h

   lib/matvec/array.h      lib/matvec/bandmat.h      lib/matvec/choldec.h
   lib/matvec/covmat.h     lib/matvec/gso.h          lib/matvec/hilbert.h
   lib/matvec/inderr.h     lib/matvec/jacobian.h     lib/matvec/matbase.h
   lib/matvec/mat.h        lib/matvec/matvecbase.h   lib/matvec/matvec.h
   lib/matvec/memrep.h     lib/matvec/pinv.h         lib/matvec/sortvec.h
   lib/matvec/svd.h        lib/matvec/symmat.h       lib/matvec/transmat.h
   lib/matvec/transvec.h   lib/matvec/unsigned.h     lib/matvec/vecbase.h
   lib/matvec/vec.h
)

set(SRC_EXPAT
   lib/expat/xmltok/xmltok.c
   lib/expat/xmltok/xmlrole.c
   lib/expat/xmlwf/codepage.c
   lib/expat/xmlparse/xmlparse.c
   lib/expat/xmlparse/hashtable.c
)

set(SRC_YAMLCPP
  lib/yaml-cpp/src/binary.cpp
  lib/yaml-cpp/src/collectionstack.h
  lib/yaml-cpp/src/convert.cpp
  lib/yaml-cpp/src/depthguard.cpp
  lib/yaml-cpp/src/directives.cpp
  lib/yaml-cpp/src/directives.h
  lib/yaml-cpp/src/emit.cpp
  lib/yaml-cpp/src/emitfromevents.cpp
  lib/yaml-cpp/src/emitter.cpp
  lib/yaml-cpp/src/emitterstate.cpp
  lib/yaml-cpp/src/emitterstate.h
  lib/yaml-cpp/src/emitterutils.cpp
  lib/yaml-cpp/src/emitterutils.h
  lib/yaml-cpp/src/exceptions.cpp
  lib/yaml-cpp/src/exp.cpp
  lib/yaml-cpp/src/exp.h
  lib/yaml-cpp/src/indentation.h
  lib/yaml-cpp/src/memory.cpp
  lib/yaml-cpp/src/nodebuilder.cpp
  lib/yaml-cpp/src/nodebuilder.h
  lib/yaml-cpp/src/node.cpp
  lib/yaml-cpp/src/node_data.cpp
  lib/yaml-cpp/src/nodeevents.cpp
  lib/yaml-cpp/src/nodeevents.h
  lib/yaml-cpp/src/null.cpp
  lib/yaml-cpp/src/ostream_wrapper.cpp
  lib/yaml-cpp/src/parse.cpp
  lib/yaml-cpp/src/parser.cpp
  lib/yaml-cpp/src/ptr_vector.h
  lib/yaml-cpp/src/regeximpl.h
  lib/yaml-cpp/src/regex_yaml.cpp
  lib/yaml-cpp/src/regex_yaml.h
  lib/yaml-cpp/src/scanner.cpp
  lib/yaml-cpp/src/scanner.h
  lib/yaml-cpp/src/scanscalar.cpp
  lib/yaml-cpp/src/scanscalar.h
  lib/yaml-cpp/src/scantag.cpp
  lib/yaml-cpp/src/scantag.h
  lib/yaml-cpp/src/scantoken.cpp
  lib/yaml-cpp/src/setting.h
  lib/yaml-cpp/src/simplekey.cpp
  lib/yaml-cpp/src/singledocparser.cpp
  lib/yaml-cpp/src/singledocparser.h
  lib/yaml-cpp/src/streamcharsource.h
  lib/yaml-cpp/src/stream.cpp
  lib/yaml-cpp/src/stream.h
  lib/yaml-cpp/src/stringsource.h
  lib/yaml-cpp/src/tag.cpp
  lib/yaml-cpp/src/tag.h
  lib/yaml-cpp/src/token.h
  )

include_directories(lib lib/expat/xmlparse lib/expat/xmltok)

add_library(libgama OBJECT ${SRC_GAMA} ${SRC_EXPAT})

# Gama install directory
#
if (NOT DEFINED GAMA_INSTDIR)
  set (GAMA_INSTDIR bin)
endif()


# Binaries
#
add_executable(gama-local src/gama-local.cpp)
target_link_libraries(gama-local PUBLIC libgama)

add_executable(gama-g3 src/gama-g3.cpp)
target_link_libraries(gama-g3 PUBLIC libgama)

add_executable(gama-local-gkf2yaml src/gama-local-gkf2yaml.cpp
               lib/gnu_gama/local/gkf2yaml.cpp)
               target_link_libraries(gama-local-gkf2yaml PUBLIC libgama)

add_executable(krumm2gama-local src/krumm2gama-local.cpp
               lib/krumm/common.h          lib/krumm/common.cpp
               lib/krumm/input.h           lib/krumm/input.cpp
               lib/krumm/k2gkf.h           lib/krumm/k2gkf.cpp
               lib/krumm/output.h          lib/krumm/output.cpp
               lib/krumm/string_matrix.h   lib/krumm/string_matrix.cpp
               lib/gnu_gama/xml/str2xml.h  lib/gnu_gama/xml/str2xml.cpp)

if (EXISTS ${CMAKE_SOURCE_DIR}/lib/yaml-cpp)
   include_directories(lib/yaml-cpp/include)
   add_library(libyaml STATIC ${SRC_YAMLCPP})

   add_executable(gama-local-yaml2gkf src/gama-local-yaml2gkf.cpp
                  lib/gnu_gama/local/yaml2gkf.cpp)
   target_link_libraries(gama-local-yaml2gkf PUBLIC libgama libyaml)

   install(TARGETS gama-local-yaml2gkf DESTINATION ${GAMA_INSTDIR})
endif()


# Installation
#
install(TARGETS gama-local gama-g3 gama-local-gkf2yaml krumm2gama-local
        DESTINATION ${GAMA_INSTDIR})


# Tests
#
if (NOT DEFINED DISABLE_GNU_GAMA_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

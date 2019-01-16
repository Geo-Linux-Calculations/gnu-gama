AUTOMAKE_OPTIONS = subdir-objects

libgama_src = \
   gnu_gama/e3.cpp \
   gnu_gama/e3.h \
   gnu_gama/ellipsoid.cpp \
   gnu_gama/ellipsoid.h \
   gnu_gama/ellipsoids.cpp \
   gnu_gama/ellipsoids.h \
   gnu_gama/exception.h \
   gnu_gama/gon2deg.cpp \
   gnu_gama/gon2deg.h \
   gnu_gama/intfloat.h \
   gnu_gama/latlong.cpp \
   gnu_gama/latlong.h \
   gnu_gama/model.h \
   gnu_gama/movetofront.h \
   gnu_gama/obsdata.h \
   gnu_gama/outstream.cpp \
   gnu_gama/outstream.h \
   gnu_gama/pointbase.h \
   gnu_gama/radian.h \
   gnu_gama/comb.cpp \
   gnu_gama/comb.h \
   gnu_gama/size_to.h \
   gnu_gama/size_to.cpp \
   gnu_gama/simplified.h \
   gnu_gama/simplified.cpp \
   gnu_gama/statan.cpp \
   gnu_gama/statan.h \
   gnu_gama/utf8.cpp \
   gnu_gama/utf8.h \
   gnu_gama/version.cpp \
   gnu_gama/version.h \
   gnu_gama/visitor.h \
   gnu_gama/xml_expat.h \
   gnu_gama/xsd.h \
   gnu_gama/adj/adj_base.h \
   gnu_gama/adj/adj_basefull.h \
   gnu_gama/adj/adj_basesparse.h \
   gnu_gama/adj/adj_chol.h \
   gnu_gama/adj/adj_chol_implementation.h \
   gnu_gama/adj/adj_envelope.h \
   gnu_gama/adj/adj_envelope_implementation.h \
   gnu_gama/adj/adj.cpp \
   gnu_gama/adj/adj.h \
   gnu_gama/adj/adj_input_data.cpp \
   gnu_gama/adj/adj_input_data.h \
   gnu_gama/adj/adj_gso.h \
   gnu_gama/adj/adj_svd.h \
   gnu_gama/adj/envelope.h \
   gnu_gama/adj/homogenization.h \
   gnu_gama/sparse/intlist.h \
   gnu_gama/sparse/sbdiagonal.h \
   gnu_gama/sparse/smatrix_graph_connected.h \
   gnu_gama/sparse/smatrix_graph.h \
   gnu_gama/sparse/smatrix_ordering.h \
   gnu_gama/sparse/smatrix.h \
   gnu_gama/sparse/svector.h \
   gnu_gama/xml/baseparser.cpp \
   gnu_gama/xml/baseparser.h \
   gnu_gama/xml/encoding_cp1251.cpp \
   gnu_gama/xml/encoding.cpp \
   gnu_gama/xml/encoding.h \
   gnu_gama/xml/encoding_unknown_handler.cpp \
   gnu_gama/xml/htmlparser.cpp \
   gnu_gama/xml/htmlparser.h \
   gnu_gama/xml/localnetworkoctave.cpp \
   gnu_gama/xml/localnetworkoctave.h \
   gnu_gama/xml/localnetworksql.cpp \
   gnu_gama/xml/localnetworksql.h \
   gnu_gama/xml/localnetworkxml.cpp \
   gnu_gama/xml/localnetworkxml.h \
   gnu_gama/xml/localnetwork_adjustment_results.cpp \
   gnu_gama/xml/localnetwork_adjustment_results.h \
   gnu_gama/xml/localnetwork_adjustment_results_data.cpp \
   gnu_gama/xml/localnetwork_adjustment_results_data.h \
   gnu_gama/xml/gkfparser.cpp \
   gnu_gama/xml/gkfparser.h \
   gnu_gama/local/angobs.h \
   gnu_gama/local/html.cpp \
   gnu_gama/local/html.h \
   gnu_gama/local/cluster.h \
   gnu_gama/local/exception.h \
   gnu_gama/local/float.h \
   gnu_gama/local/itstream.h \
   gnu_gama/local/language.cpp \
   gnu_gama/local/language.h \
   gnu_gama/local/svg.cpp \
   gnu_gama/local/svg.h \
   gnu_gama/local/acord/approx_heights.cpp \
   gnu_gama/local/acord/approx_heights.h \
   gnu_gama/local/acord/approx_vectors.cpp \
   gnu_gama/local/acord/approx_vectors.h \
   gnu_gama/local/acord.cpp \
   gnu_gama/local/acord.h \
   gnu_gama/local/acord/reduce_observations.cpp \
   gnu_gama/local/acord/reduce_observations.h \
   gnu_gama/local/acord/reduce_to_ellipsoid.cpp \
   gnu_gama/local/acord/reduce_to_ellipsoid.h \
   gnu_gama/local/gamadata.cpp \
   gnu_gama/local/gamadata.h \
   gnu_gama/local/lcoords.h \
   gnu_gama/local/local_linearization.h \
   gnu_gama/local/localnetwork2sql.cpp \
   gnu_gama/local/localnetwork2sql.h \
   gnu_gama/local/lpoint.h \
   gnu_gama/local/median/g2d_cogo.cpp \
   gnu_gama/local/median/g2d_cogo.h \
   gnu_gama/local/median/g2d_coordinates.cpp \
   gnu_gama/local/median/g2d_coordinates.h \
   gnu_gama/local/median/g2d_exception.h \
   gnu_gama/local/median/g2d_helper.cpp \
   gnu_gama/local/median/g2d_helper.h \
   gnu_gama/local/median/g2d_point.cpp \
   gnu_gama/local/median/g2d_point.h \
   gnu_gama/local/network.cpp \
   gnu_gama/local/network.h \
   gnu_gama/local/observation_local.cpp \
   gnu_gama/local/orientation.cpp \
   gnu_gama/local/orientation.h \
   gnu_gama/local/pobs/bearing.h \
   gnu_gama/local/pobs/format.cpp \
   gnu_gama/local/pobs/format.h \
   gnu_gama/local/pobs/write.cpp \
   gnu_gama/local/results/text/adjusted_observations.h \
   gnu_gama/local/results/text/adjusted_unknowns.h \
   gnu_gama/local/results/text/approximate_coordinates.h \
   gnu_gama/local/results/text/error_ellipses.h \
   gnu_gama/local/results/text/fixed_points.h \
   gnu_gama/local/results/text/general_parameters.h \
   gnu_gama/local/results/text/network_description.h \
   gnu_gama/local/results/text/outlying_abs_terms.h \
   gnu_gama/local/results/text/reduced_observations.h \
   gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h \
   gnu_gama/local/results/text/residuals_observations.h \
   gnu_gama/local/results/text/underline.cpp \
   gnu_gama/local/results/text/underline.h \
   gnu_gama/local/test_linearization_visitor.cpp \
   gnu_gama/local/test_linearization_visitor.h \
   gnu_gama/local/local_revision.h \
   gnu_gama/local/matvec.h \
   gnu_gama/local/medianf.h \
   gnu_gama/local/observation.cpp \
   gnu_gama/local/observation.h \
   gnu_gama/local/pointid.cpp \
   gnu_gama/local/pointid.h \
   gnu_gama/local/readsabw.h \
   gnu_gama/local/skipcomm.cpp \
   gnu_gama/local/skipcomm.h \
   gnu_gama/local/sqlitereader.h \
   gnu_gama/local/sqlitereader.cpp \
   gnu_gama/local/writevisitor.h \
   gnu_gama/local/xmlerror.h \
   gnu_gama/local/xmlerror.cpp

if GNU_GAMA_G3_ENABLED
libg3_src = \
   gnu_gama/g3/g3_adjres.cpp \
   gnu_gama/g3/g3_adjres.h \
   gnu_gama/g3/g3_cluster.cpp \
   gnu_gama/g3/g3_cluster.h \
   gnu_gama/g3/g3_model.cpp \
   gnu_gama/g3/g3_model.h \
   gnu_gama/g3/g3_model_init.cpp \
   gnu_gama/g3/g3_model_linearization.cpp \
   gnu_gama/g3/g3_model_revision.cpp \
   gnu_gama/g3/g3_model_write_xml_adjustment_results.cpp \
   gnu_gama/g3/g3_observation.h \
   gnu_gama/g3/g3_parameter.h \
   gnu_gama/g3/g3_point.cpp \
   gnu_gama/g3/g3_point.h \
   gnu_gama/g3/g3_write_observation_xml.cpp \
   gnu_gama/g3/g3_write_observation_xml.h \
   gnu_gama/xml/dataobject.cpp \
   gnu_gama/xml/dataobject.h \
   gnu_gama/xml/dataparser_adj.cpp \
   gnu_gama/xml/dataparser.cpp \
   gnu_gama/xml/dataparser_g3.cpp \
   gnu_gama/xml/dataparser_g3adj.cpp \
   gnu_gama/xml/dataparser.h
endif

matvec_src = \
   matvec/array.h \
   matvec/bandmat.h \
   matvec/choldec.h \
   matvec/covmat.h \
   matvec/gso.h \
   matvec/hilbert.h \
   matvec/inderr.h \
   matvec/jacobian.h \
   matvec/matbase.h \
   matvec/mat.h \
   matvec/matvecbase.h \
   matvec/matvec.h \
   matvec/memrep.h \
   matvec/pinv.h \
   matvec/sortvec.h \
   matvec/svd.h \
   matvec/symmat.h \
   matvec/transmat.h \
   matvec/transvec.h \
   matvec/unsigned.h \
   matvec/vecbase.h \
   matvec/vec.h

matvec_extra = \
   matvec/COPYING \
   matvec/README \
   matvec/imtql1.for.gz \
   matvec/svd.for.gz

expat_src = \
   expat/xmltok/xmltok.c \
   expat/xmltok/xmlrole.c \
   expat/xmlwf/codepage.c \
   expat/xmlparse/xmlparse.c \
   expat/xmlparse/hashtable.c

expat_extra = \
   expat/Makefile \
   expat/xmlwf/filemap.h \
   expat/xmlwf/xmlwf.c \
   expat/xmlwf/xmltchar.h \
   expat/xmlwf/codepage.h \
   expat/xmlwf/win32filemap.c \
   expat/xmlwf/unixfilemap.c \
   expat/xmlwf/xmlfile.c \
   expat/xmlwf/xmlfile.h \
   expat/xmlwf/readfilemap.c \
   expat/README \
   expat/gennmtab/gennmtab.c \
   expat/sample/elements.c \
   expat/xmltok/latin1tab.h \
   expat/xmltok/asciitab.h \
   expat/xmltok/xmltok_ns.c \
   expat/xmltok/iasciitab.h \
   expat/xmltok/dllmain.c \
   expat/xmltok/xmldef.h \
   expat/xmltok/xmltok.h \
   expat/xmltok/utf8tab.h \
   expat/xmltok/xmltok_impl.c \
   expat/xmltok/xmltok_impl.h \
   expat/xmltok/xmlrole.h \
   expat/xmltok/nametab.h \
   expat/expat.html \
   expat/xmlparse/hashtable.h \
   expat/xmlparse/xmlparse.h

noinst_LIBRARIES  = libgama.a

if GNU_GAMA_LOCAL_TEST_EXPAT_1_1
AM_CFLAGS= -I@top_srcdir@/lib/expat/xmltok
AM_CPPFLAGS= -I@top_srcdir@/lib/expat/xmltok
libgama_a_SOURCES = $(libgama_src) $(libg3_src) $(matvec_src) $(expat_src)
else
libgama_a_SOURCES = $(libgama_src) $(libg3_src) $(matvec_src)
endif

EXTRA_DIST = $(expat_src) $(expat_extra) $(matvec_extra)

/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2005  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef GNU_Gama_GaMa_XML_DataParser__data_parser__dataparser___h_
#define GNU_Gama_GaMa_XML_DataParser__data_parser__dataparser___h_

#include <gnu_gama/xml/baseparser.h>
#include <gnu_gama/xml/dataobject.h>
#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/g3/g3_cluster.h>
#include <gnu_gama/list.h>
#include <gnu_gama/exception.h>
#include <cstddef>
#include <string>
#include <list>

namespace GNU_gama {

  struct DataParser_adj;
  struct DataParser_g3;
  struct DataParser_g3adj;

  /** \brief General Gama XML data parser

       DataParser class reads XML input and creates a list of pointers
       DataObjects
   */

  class DataParser : public BaseParser<Exception::parser>
    {
    public:

      DataParser(List<DataObject::Base*>&);
      ~DataParser();
      int startElement(const char *name, const char **atts)
        {
          return (this->*stag[state][tag(name)])(name, atts);
        }
      int characterDataHandler(const char *s, int len)
        {
          return (this->*data[state])(s, len);
        }
      int endElement(const char *name)
        {
          return (this->*etag[state])(name);
        }

      static const char* const xml_start;
      static const char* const xml_end;

    private:

      List<DataObject::Base*>& objects;

      enum parser_state
        {
          s_error,
          s_start,
          s_gama_data,
          s_g3_model,

          // ..................................................

          s_g3_const,
          s_g3_const_apriori_sd,
          s_g3_const_conf_level,
          s_g3_const_tol_abs,
          s_g3_const_ellipsoid,
          s_g3_const_ellipsoid2,
          s_g3_const_ellipsoid_id,
          s_g3_const_ellipsoid_a,
          s_g3_const_ellipsoid_b,
          s_g3_const_ellipsoid_inv_f,
          s_g3_const_ang,

          // ..................................................

          s_g3_param,
          s_g3_param_n,
          s_g3_param_e,
          s_g3_param_u,

          // ..................................................

          s_g3_point_1,
          s_g3_point_id,
          s_g3_point_2,
          s_g3_point_b,
          s_g3_point_after_b,
          s_g3_point_l,
          s_g3_point_after_l,
          s_g3_point_h,
          s_g3_point_x,
          s_g3_point_after_x,
          s_g3_point_y,
          s_g3_point_after_y,
          s_g3_point_z,
          s_g3_point_height,
          s_g3_point_geoid,
          s_g3_point_param,
          s_g3_point_param_n,
          s_g3_point_param_e,
          s_g3_point_param_u,
          s_g3_point_db,
          s_g3_point_after_db,
          s_g3_point_dl,

          // ..................................................

          s_g3_obs,
          s_g3_obs_covmat,
          s_g3_obs_covmat_dim,
          s_g3_obs_covmat_after_dim,
          s_g3_obs_covmat_band,
          s_g3_obs_covmat_after_band,
          s_g3_obs_covmat_flt,

          s_g3_obs_dist,
          s_g3_obs_dist_from,
          s_g3_obs_dist_after_from,
          s_g3_obs_dist_to,
          s_g3_obs_dist_after_to,
          s_g3_obs_dist_val,
          s_g3_obs_dist_opt,
          s_g3_obs_dist_opt_stdev,
          s_g3_obs_dist_opt_variance,
          s_g3_obs_dist_opt_from_dh,
          s_g3_obs_dist_opt_to_dh,

          s_g3_obs_hdiff,
          s_g3_obs_hdiff_from,
          s_g3_obs_hdiff_after_from,
          s_g3_obs_hdiff_to,
          s_g3_obs_hdiff_after_to,
          s_g3_obs_hdiff_val,
          s_g3_obs_hdiff_opt,
          s_g3_obs_hdiff_opt_stdev,
          s_g3_obs_hdiff_opt_variance,
          s_g3_obs_hdiff_opt_from_dh,
          s_g3_obs_hdiff_opt_to_dh,

          s_g3_obs_zenith,
          s_g3_obs_zenith_from,
          s_g3_obs_zenith_after_from,
          s_g3_obs_zenith_to,
          s_g3_obs_zenith_after_to,
          s_g3_obs_zenith_val,
          s_g3_obs_zenith_opt,
          s_g3_obs_zenith_opt_stdev,
          s_g3_obs_zenith_opt_variance,
          s_g3_obs_zenith_opt_from_dh,
          s_g3_obs_zenith_opt_to_dh,

          s_g3_obs_azimuth,
          s_g3_obs_azimuth_from,
          s_g3_obs_azimuth_after_from,
          s_g3_obs_azimuth_to,
          s_g3_obs_azimuth_after_to,
          s_g3_obs_azimuth_val,
          s_g3_obs_azimuth_opt,
          s_g3_obs_azimuth_opt_stdev,
          s_g3_obs_azimuth_opt_variance,
          s_g3_obs_azimuth_opt_from_dh,
          s_g3_obs_azimuth_opt_to_dh,

          s_g3_obs_vector,
          s_g3_obs_vector_from,
          s_g3_obs_vector_after_from,
          s_g3_obs_vector_to,
          s_g3_obs_vector_after_to,
          s_g3_obs_vector_dx,
          s_g3_obs_vector_after_dx,
          s_g3_obs_vector_dy,
          s_g3_obs_vector_after_dy,
          s_g3_obs_vector_dz,
          s_g3_obs_vector_opt,
          s_g3_obs_vector_opt_from_dh,
          s_g3_obs_vector_opt_to_dh,

          s_g3_obs_xyz,
          s_g3_obs_xyz_id,
          s_g3_obs_xyz_after_id,
          s_g3_obs_xyz_x,
          s_g3_obs_xyz_after_x,
          s_g3_obs_xyz_y,
          s_g3_obs_xyz_after_y,
          s_g3_obs_xyz_z,
          s_g3_obs_xyz_after_z,

          s_g3_obs_height,
          s_g3_obs_height_id,
          s_g3_obs_height_after_id,
          s_g3_obs_height_val,
          s_g3_obs_height_opt,
          s_g3_obs_height_opt_stdev,
          s_g3_obs_height_opt_variance,

          s_g3_obs_angle,
          s_g3_obs_angle_from,
          s_g3_obs_angle_after_from,
          s_g3_obs_angle_left,
          s_g3_obs_angle_after_left,
          s_g3_obs_angle_right,
          s_g3_obs_angle_after_right,
          s_g3_obs_angle_val,
          s_g3_obs_angle_opt,
          s_g3_obs_angle_opt_stdev,
          s_g3_obs_angle_opt_variance,
          s_g3_obs_angle_opt_from_dh,
          s_g3_obs_angle_opt_left_dh,
          s_g3_obs_angle_opt_right_dh,

          // ..................................................

          s_text,

          // ..................................................

          s_adj_input_data_1,
          s_adj_input_data_2,
          s_adj_input_data_3,
          s_adj_input_data_4,
          s_adj_input_data_5,

          s_sparse_mat_1,
          s_sparse_mat_rows,
          s_sparse_mat_2,
          s_sparse_mat_cols,
          s_sparse_mat_3,
          s_sparse_mat_nonz,
          s_sparse_mat_4,
          s_sparse_mat_row_1,
          s_sparse_mat_row_nonz,
          s_sparse_mat_row_2,
          s_sparse_mat_row_int,
          s_sparse_mat_row_3,
          s_sparse_mat_row_flt,

          s_block_diagonal_1,
          s_block_diagonal_blocks,
          s_block_diagonal_2,
          s_block_diagonal_nonz,
          s_block_diagonal_3,
          s_block_diagonal_block_1,
          s_block_diagonal_block_d,
          s_block_diagonal_block_2,
          s_block_diagonal_block_w,
          s_block_diagonal_block_3,
          s_block_diagonal_block_f,

          s_vector_1,
          s_vector_dim,
          s_vector_2,
          s_vector_flt,

          s_array_1,
          s_array_dim,
          s_array_2,
          s_array_int,

          // ..................................................

          s_g3a_adj_results,
          s_g3a_adj_results_end,

          s_g3a_rejected_obs,
          s_g3a_rejected_obs_end,
          s_g3a_x_rejected,
          s_g3a_x_reason,
          s_g3a_x_observation,
          s_g3a_x_id1,
          s_g3a_x_id2,
          s_g3a_x_id3,
          s_g3a_x_obs1,
          s_g3a_x_obs2,
          s_g3a_x_obs3,
          s_g3a_x_flt,

          s_g3a_s_statistics,
          s_g3a_s_statistics_end,
          s_g3a_s_s_algorithm,
          s_g3a_s_ellipsoid,
          s_g3a_s_ellipsoid_cap,
          s_g3a_s_ellipsoid_id,
          s_g3a_s_ellipsoid_a,
          s_g3a_s_ellipsoid_b,
          s_g3a_s_parameters,
          s_g3a_s_equations,
          s_g3a_s_defect,
          s_g3a_s_redundancy,
          s_g3a_s_sum_of_squares,
          s_g3a_s_apriori_var,
          s_g3a_s_aposteriori_var,
          s_g3a_s_variance_factor,
          s_g3a_s_design_m_graph,

          s_g3a_r_adj_results,
          s_g3a_r_adj_results_end,
          s_g3a_r_point,
          s_g3a_r_point_end,
          s_g3a_r_point_id,
          s_g3a_r_point_after_id,
          s_g3a_r_point_n_fixed,
          s_g3a_r_point_n_free,
          s_g3a_r_point_n_constr,
          s_g3a_r_point_n_unused,
          s_g3a_r_point_e_fixed,
          s_g3a_r_point_e_free,
          s_g3a_r_point_e_constr,
          s_g3a_r_point_e_unused,
          s_g3a_r_point_u_fixed,
          s_g3a_r_point_u_free,
          s_g3a_r_point_u_constr,
          s_g3a_r_point_u_unused,

          s_g3a_r_point_after_n,
          s_g3a_r_point_after_e,
          s_g3a_r_point_after_u,

          s_g3a_r_point_n_dn,
          s_g3a_r_point_e_de,
          s_g3a_r_point_u_du,
          s_g3a_r_point_n_ind,
          s_g3a_r_point_e_ind,
          s_g3a_r_point_u_ind,

          s_g3a_r_point_cnn,
          s_g3a_r_point_cne,
          s_g3a_r_point_cnu,
          s_g3a_r_point_cee,
          s_g3a_r_point_ceu,
          s_g3a_r_point_cuu,

          s_g3a_r_point_x_given,
          s_g3a_r_point_x_correction,
          s_g3a_r_point_x_adjusted,
          s_g3a_r_point_y_given,
          s_g3a_r_point_y_correction,
          s_g3a_r_point_y_adjusted,
          s_g3a_r_point_z_given,
          s_g3a_r_point_z_correction,
          s_g3a_r_point_z_adjusted,

          s_g3a_r_point_cxx,
          s_g3a_r_point_cxy,
          s_g3a_r_point_cxz,
          s_g3a_r_point_cyy,
          s_g3a_r_point_cyz,
          s_g3a_r_point_czz,

          s_g3a_r_point_b_given,
          s_g3a_r_point_b_correction,
          s_g3a_r_point_b_adjusted,
          s_g3a_r_point_l_given,
          s_g3a_r_point_l_correction,
          s_g3a_r_point_l_adjusted,
          s_g3a_r_point_h_given,
          s_g3a_r_point_h_correction,
          s_g3a_r_point_h_adjusted,

          s_g3a_o_observations,
          s_g3a_o_observations_end,

          s_g3a_o_vector,
          s_g3a_o_vector_from,
          s_g3a_o_vector_to,
          s_g3a_o_vector_ind,
          s_g3a_o_vector_dx_observed,
          s_g3a_o_vector_dx_residual,
          s_g3a_o_vector_dx_adjusted,
          s_g3a_o_vector_dy_observed,
          s_g3a_o_vector_dy_residual,
          s_g3a_o_vector_dy_adjusted,
          s_g3a_o_vector_dz_observed,
          s_g3a_o_vector_dz_residual,
          s_g3a_o_vector_dz_adjusted,

          s_g3a_o_vector_dx_stdev_obs,
          s_g3a_o_vector_dx_stdev_adj,
          s_g3a_o_vector_dy_stdev_obs,
          s_g3a_o_vector_dy_stdev_adj,
          s_g3a_o_vector_dz_stdev_obs,
          s_g3a_o_vector_dz_stdev_adj,

          s_g3a_o_vector_cxx,
          s_g3a_o_vector_cxy,
          s_g3a_o_vector_cxz,
          s_g3a_o_vector_cyy,
          s_g3a_o_vector_cyz,
          s_g3a_o_vector_czz,

          s_g3a_o_xyz,
          s_g3a_o_xyz_id,
          s_g3a_o_xyz_ind,
          s_g3a_o_xyz_x_observed,
          s_g3a_o_xyz_x_residual,
          s_g3a_o_xyz_x_adjusted,
          s_g3a_o_xyz_y_observed,
          s_g3a_o_xyz_y_residual,
          s_g3a_o_xyz_y_adjusted,
          s_g3a_o_xyz_z_observed,
          s_g3a_o_xyz_z_residual,
          s_g3a_o_xyz_z_adjusted,

          s_g3a_o_xyz_x_stdev_obs,
          s_g3a_o_xyz_x_stdev_adj,
          s_g3a_o_xyz_y_stdev_obs,
          s_g3a_o_xyz_y_stdev_adj,
          s_g3a_o_xyz_z_stdev_obs,
          s_g3a_o_xyz_z_stdev_adj,

          s_g3a_o_xyz_cxx,
          s_g3a_o_xyz_cxy,
          s_g3a_o_xyz_cxz,
          s_g3a_o_xyz_cyy,
          s_g3a_o_xyz_cyz,
          s_g3a_o_xyz_czz,

          s_g3a_o_distance,
          s_g3a_o_distance_from,
          s_g3a_o_distance_to,
          s_g3a_o_distance_ind,
          s_g3a_o_distance_observed,
          s_g3a_o_distance_residual,
          s_g3a_o_distance_adjusted,
          s_g3a_o_distance_stdev_obs,
          s_g3a_o_distance_stdev_adj,

          // ..................................................

          s_stop
        };

      enum data_tag
        {
          t_a,
          t_adj_input_data,
          t_adj_results,
          t_adj_results_end,
          t_adj_observations,
          t_adj_observations_end,
          t_adj_statistics,
          t_adj_statistics_end,
          t_adjusted,
          t_apriori_var,
          t_aposteriori_var,
          t_algorithm,
          t_ang_degrees,
          t_ang_gons,
          t_angle,
          t_apriori_sd,
          t_array,
          t_azimuth,
          t_b,
          t_b_given,
          t_b_correction,
          t_b_adjusted,
          t_band,
          t_block,
          t_block_diagonal,
          t_blocks,
          t_caption,
          t_cee,
          t_ceu,
          t_cols,
          t_conf_level,
          t_constants,
          t_constr,
          t_covmat,
          t_cnn,
          t_cne,
          t_cnu,
          t_cuu,
          t_cxx,
          t_cxy,
          t_cxz,
          t_cyy,
          t_cyz,
          t_czz,
          t_db,
          t_de,
          t_defect,
          t_design_m_graph,
          t_dl,
          t_dn,
          t_dim,
          t_dist,
          t_du,
          t_dx,
          t_dx_observed,
          t_dx_residual,
          t_dx_adjusted,
          t_dx_stdev_obs,
          t_dx_stdev_adj,
          t_dy,
          t_dy_observed,
          t_dy_residual,
          t_dy_adjusted,
          t_dy_stdev_obs,
          t_dy_stdev_adj,
          t_dz,
          t_dz_observed,
          t_dz_residual,
          t_dz_adjusted,
          t_dz_stdev_obs,
          t_dz_stdev_adj,
          t_e,
          t_e_fixed,
          t_e_free,
          t_e_constr,
          t_e_unused,
          t_ellipsoid,
          t_equations,
          t_flt,
          t_fixed,
          t_free,
          t_from,
          t_from_dh,
          t_g3_adj_results,
          t_g3_model,
          t_gama_data,
          t_geoid,
          t_h,
          t_h_given,
          t_h_correction,
          t_h_adjusted,
          t_hdiff,
          t_height,
          t_hobs,
          t_id,
          t_ind,
          t_int,
          t_inv_f,
          t_l,
          t_l_given,
          t_l_correction,
          t_l_adjusted,
          t_left,
          t_left_dh,
          t_n,
          t_n_fixed,
          t_n_free,
          t_n_constr,
          t_n_unused,
          t_nonz,
          t_obs,
          t_observed,
          t_parameters,
          t_point,
          t_reason,
          t_redundancy,
          t_rejected,
          t_rejected_obs,
          t_residual,
          t_right,
          t_right_dh,
          t_rows,
          t_row,
          t_sparse_mat,
          t_stdev,
          t_stdev_obs,
          t_stdev_adj,
          t_sum_of_squares,
          t_text,
          t_to,
          t_to_dh,
          t_tol_abs,
          t_u,
          t_u_fixed,
          t_u_free,
          t_u_constr,
          t_u_unused,
          t_unknown,
          t_val,
          t_variance,
          t_variance_factor,
          t_vector,
          t_width,
          t_x,
          t_x_stdev_obs,
          t_x_stdev_adj,
          t_x_given,
          t_x_correction,
          t_x_adjusted,
          t_x_observed,
          t_x_residual,
          t_xyz,
          t_y,
          t_y_stdev_obs,
          t_y_stdev_adj,
          t_y_given,
          t_y_correction,
          t_y_adjusted,
          t_y_observed,
          t_y_residual,
          t_z,
          t_z_stdev_obs,
          t_z_stdev_adj,
          t_z_given,
          t_z_correction,
          t_z_adjusted,
          t_z_observed,
          t_z_residual,
          t_zenith,
          t_unused
        };

      data_tag tag(const char *name);

      typedef int (DataParser::*Stag)(const char *name, const char **atts);
      typedef int (DataParser::*Data)(const char *name, int len);
      typedef int (DataParser::*Etag)(const char *name);

      Stag stag[s_stop+1][t_unused+1];
      Data data[s_stop+1];
      Etag etag[s_stop+1];

      int next [s_stop+1][t_unused+1];
      int after[s_stop+1];

      int gama_data               (const char *name, const char **atts);
      int g3_model                (const char *name, const char **atts);
      int g3_model                (const char *name);

      int g3_const_apriori_sd     (const char *name);
      int g3_const_conf_level     (const char *name);
      int g3_const_tol_abs        (const char *name);
      int g3_const_ellipsoid_id   (const char *name);
      int g3_const_ellipsoid_b    (const char *name);
      int g3_const_ellipsoid_inv_f(const char *name);
      int g3_const_ang_degrees    (const char *name);
      int g3_const_ang_gons       (const char *name);

      int g3_param_unused         (const char *name, const char **atts);
      int g3_param_fixed          (const char *name, const char **atts);
      int g3_param_free           (const char *name, const char **atts);
      int g3_param_constr         (const char *name, const char **atts);

      int g3_param_n              (const char *name);
      int g3_param_e              (const char *name);
      int g3_param_u              (const char *name);

      int g3_point                (const char *name);
      int g3_point_id             (const char *name);
      int g3_point_b              (const char *name);
      int g3_point_l              (const char *name);
      int g3_point_h              (const char *name);
      int g3_point_z              (const char *name);
      int g3_point_height         (const char *name);
      int g3_point_geoid          (const char *name);
      int g3_point_param_n        (const char *name);
      int g3_point_param_e        (const char *name);
      int g3_point_param_u        (const char *name);
      int g3_point_dl             (const char *name);

      int g3_obs                  (const char *name, const char **atts);
      int g3_obs                  (const char *name);
      int g3_obs_cov              (const char *name);
      int g3_obs_dist             (const char *name);
      int g3_obs_zenith           (const char *name);
      int g3_obs_azimuth          (const char *name);
      int g3_obs_vector           (const char *name);
      int g3_obs_xyz              (const char *name);
      int g3_obs_hdiff            (const char *name);
      int g3_obs_height           (const char *name);
      int g3_obs_angle            (const char *name);

      int text                    (const char *name);

      int adj_input_data          (const char *name, const char **atts);
      int adj_input_data          (const char *name);
      int sparse_mat              (const char *name);
      int sparse_mat_nonz         (const char *name);
      int sparse_mat_row          (const char *name, const char **atts);
      int sparse_mat_row          (const char *name);
      int sparse_mat_row_n        (const char *name);
      int sparse_mat_row_f        (const char *name);
      int block_diagonal          (const char *name);
      int block_diagonal_nonz     (const char *name);
      int block_diagonal_block_w  (const char *name);
      int block_diagonal_vec_flt  (const char *name);
      int block_diagonal_block    (const char *name);
      int vector                  (const char *name);
      int vector_dim              (const char *name);
      int vector_flt              (const char *name);
      int array                   (const char *name);
      int array_dim               (const char *name);
      int array_int               (const char *name);

      int g3a_x_rejected          (const char *name);
      int g3a_x_reason            (const char *name);
      int g3a_x_observation       (const char *name);
      int g3a_x_id1               (const char *name);
      int g3a_x_id2               (const char *name);
      int g3a_x_id3               (const char *name);
      int g3a_x_obs1              (const char *name);
      int g3a_x_obs2              (const char *name);
      int g3a_x_obs3              (const char *name);
      int g3a_x_flt               (const char *name);

      int g3a_s_adj_results       (const char *name, const char **atts);
      int g3a_s_adj_results       (const char *name);
      int g3a_s_algorithm         (const char *name);
      int g3a_s_ell_caption       (const char *name);
      int g3a_s_ell_id            (const char *name);
      int g3a_s_ell_a             (const char *name);
      int g3a_s_ell_b             (const char *name);
      int g3a_s_parameters        (const char *name);
      int g3a_s_equations         (const char* name);
      int g3a_s_defect            (const char* name);
      int g3a_s_redundancy        (const char* name);
      int g3a_s_sum_of_squares    (const char* name);
      int g3a_s_apriori_var       (const char* name);
      int g3a_s_aposteriori_var   (const char* name);
      int g3a_s_variance_factor   (const char* name);
      int g3a_s_design_m_graph    (const char* name);

      int g3a_r_point             (const char *name, const char **atts);
      int g3a_r_point             (const char *name);
      int g3a_r_point_id          (const char *name);

      int g3a_r_point_n_fixed     (const char *name);
      int g3a_r_point_n_free      (const char *name);
      int g3a_r_point_n_constr    (const char *name);
      int g3a_r_point_n_unused    (const char *name);
      int g3a_r_point_e_fixed     (const char *name);
      int g3a_r_point_e_free      (const char *name);
      int g3a_r_point_e_constr    (const char *name);
      int g3a_r_point_e_unused    (const char *name);
      int g3a_r_point_u_fixed     (const char *name);
      int g3a_r_point_u_free      (const char *name);
      int g3a_r_point_u_constr    (const char *name);
      int g3a_r_point_u_unused    (const char *name);

      int g3a_r_point_n_dn        (const char *name);
      int g3a_r_point_e_de        (const char *name);
      int g3a_r_point_u_du        (const char *name);
      int g3a_r_point_n_ind       (const char *name);
      int g3a_r_point_e_ind       (const char *name);
      int g3a_r_point_u_ind       (const char *name);

      int g3a_r_point_cnn         (const char *name);
      int g3a_r_point_cne         (const char *name);
      int g3a_r_point_cnu         (const char *name);
      int g3a_r_point_cee         (const char *name);
      int g3a_r_point_ceu         (const char *name);
      int g3a_r_point_cuu         (const char *name);

      int g3a_r_point_x_given     (const char *name);
      int g3a_r_point_x_correction(const char *name);
      int g3a_r_point_x_adjusted  (const char *name);
      int g3a_r_point_y_given     (const char *name);
      int g3a_r_point_y_correction(const char *name);
      int g3a_r_point_y_adjusted  (const char *name);
      int g3a_r_point_z_given     (const char *name);
      int g3a_r_point_z_correction(const char *name);
      int g3a_r_point_z_adjusted  (const char *name);

      int g3a_r_point_cxx         (const char *name);
      int g3a_r_point_cxy         (const char *name);
      int g3a_r_point_cxz         (const char *name);
      int g3a_r_point_cyy         (const char *name);
      int g3a_r_point_cyz         (const char *name);
      int g3a_r_point_czz         (const char *name);

      int g3a_r_point_b_given     (const char *name);
      int g3a_r_point_b_correction(const char *name);
      int g3a_r_point_b_adjusted  (const char *name);
      int g3a_r_point_l_given     (const char *name);
      int g3a_r_point_l_correction(const char *name);
      int g3a_r_point_l_adjusted  (const char *name);
      int g3a_r_point_h_given     (const char *name);
      int g3a_r_point_h_correction(const char *name);
      int g3a_r_point_h_adjusted  (const char *name);

      int g3a_o_observation       (const char *name, const char **atts);
      int g3a_o_observation       (const char *name);

      int g3a_o_from              (const char *name);
      int g3a_o_to                (const char *name);
      int g3a_o_ind               (const char *name);
      int g3a_o_obs1              (const char *name);
      int g3a_o_res1              (const char *name);
      int g3a_o_adj1              (const char *name);
      int g3a_o_obs2              (const char *name);
      int g3a_o_res2              (const char *name);
      int g3a_o_adj2              (const char *name);
      int g3a_o_obs3              (const char *name);
      int g3a_o_res3              (const char *name);
      int g3a_o_adj3              (const char *name);
      int g3a_o_stdev_obs1        (const char *name);
      int g3a_o_stdev_adj1        (const char *name);
      int g3a_o_stdev_obs2        (const char *name);
      int g3a_o_stdev_adj2        (const char *name);
      int g3a_o_stdev_obs3        (const char *name);
      int g3a_o_stdev_adj3        (const char *name);
      int g3a_o_c11               (const char *name);
      int g3a_o_c12               (const char *name);
      int g3a_o_c13               (const char *name);
      int g3a_o_c22               (const char *name);
      int g3a_o_c23               (const char *name);
      int g3a_o_c33               (const char *name);


      int add_text     (const char *name, int len);
      int end_tag      (const char *name);
      int no_attributes(const char *name, const char **atts);
      int parser_error (const char *name, const char **atts);
      int start_tag    (const char *name, const char **atts);
      int white_spaces (const char *name, int len);

      int optional_stdev   (const char *name, int len);
      int optional_variance(const char *name, int len);
      int optional_from_dh (const char *name, int len);
      int optional_to_dh   (const char *name, int len);
      int optional_left_dh (const char *name, int len);
      int optional_right_dh(const char *name, int len);

      void init(int state, int tag,
                int next_state, int end_state, int after_state,
                Stag, Data, Etag,
                int end_state2=0);
      int  g3_get_float (const char *name, double&);
      bool pure_data(std::istream&);   // test for trailing junk in input data


      // ***  DataObject::g3_model ***

      DataParser_g3* g3;

      void        init_g3();
      void        close_g3();
      std::string g3_get_id(std::string err);
      double      optional(double& attr)
      {
        double tmp = attr;  attr=0; return tmp;
      }


      // ***  DataObject::Text  ***

      std::string      text_buffer;


      // ***  DataObject::AdjInput  ***

      DataParser_adj* adj;

      void         init_adj();
      void         close_adj();

      GNU_gama::SparseMatrix <> *adj_sparse_mat;
      GNU_gama::BlockDiagonal<> *adj_block_diagonal;
      Vec<>            adj_vector;
      Vec<>::iterator  adj_vector_iterator;
      std::size_t      adj_vector_dim;
      GNU_gama::IntegerList<>   *adj_array;
      GNU_gama::IntegerList<>::iterator adj_array_iterator;
      std::size_t      adj_array_dim;
      std::size_t      adj_sparse_mat_nonz;
      std::size_t      adj_sparse_mat_row_nonz;
      std::size_t      block_diagonal_blocks_;
      std::size_t      block_diagonal_nonz_;
      std::size_t      block_diagonal_dim;
      std::size_t      block_diagonal_width;
      Vec<>            bd_vector;
      Vec<>::iterator  bd_vector_iterator;
      std::size_t      bd_vector_dim;
      g3::Point        *point;

      g3::Parameter    local_state;
      g3::Parameter    global_state_N;
      g3::Parameter    global_state_E;
      g3::Parameter    global_state_U;

      struct {
        double b, l, h;
      } blh;


      // ***  DataObject::g3_adjustment_results ***

      DataParser_g3adj* g3adj;

      void        init_g3adj();
      void        close_g3adj();

      void        g3a_text_string (std::string& str);
      void        g3a_text_float  (std::string& str);
      void        g3a_text_integer(std::string& str);
    };
}

#endif

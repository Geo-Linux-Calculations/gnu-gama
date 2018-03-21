/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2005, 2018  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library

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

#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/radian.h>
#include <cstring>

using namespace std;
using namespace GNU_gama;

namespace GNU_gama {

  struct DataParser_g3 {

    DataParser_g3() : model(0)
    {
    }
    ~DataParser_g3()
    {
      delete model;
    }

    typedef std::list<double> Scale;
    Scale              scale;


    typedef g3::Model::ObservationType::CovarianceMatrix Cov;

    g3::Model*         model;
    g3::ObsCluster*    obs_cluster;
    std::list<Cov>     cov_list;
    double             from_dh;
    double             to_dh;
    double             left_dh;
    double             right_dh;
  };

}


void DataParser::close_g3()
{
  delete g3;
}

void DataParser::init_g3()
{
  g3 = new DataParser_g3;

  optional(g3->from_dh);
  optional(g3->to_dh);


  // .....  <g3-model>  ..............................................

  init(s_gama_data, t_g3_model,
       s_g3_model, 0, 0,
       &DataParser::g3_model, 0, &DataParser::g3_model);

  // .....  <g3-model>  <constants>  .................................

  init(s_g3_model, t_constants,
       s_g3_const, 0, s_g3_model,
       0, 0, 0);

  init(s_g3_const, t_apriori_sd,
       s_g3_const_apriori_sd, 0, s_g3_const,
       0, &DataParser::add_text, &DataParser::g3_const_apriori_sd);

  init(s_g3_const, t_conf_level,
       s_g3_const_conf_level, 0, s_g3_const,
       0, &DataParser::add_text, &DataParser::g3_const_conf_level);

  init(s_g3_const, t_tol_abs,
       s_g3_const_tol_abs, 0, s_g3_const,
       0, &DataParser::add_text, &DataParser::g3_const_tol_abs);

  init(s_g3_const, t_ang_degrees,
       s_g3_const_ang, 0, s_g3_const,
       0, 0, &DataParser::g3_const_ang_degrees);

  init(s_g3_const, t_ang_gons,
       s_g3_const_ang, 0, s_g3_const,
       0, 0, &DataParser::g3_const_ang_gons);

  // .....  <g3-model>  <constants>  <ellipsoid>  ....................

  init(s_g3_const, t_ellipsoid,
       s_g3_const_ellipsoid, s_g3_const_ellipsoid2, s_g3_const,
       0, 0, 0);

  init(s_g3_const_ellipsoid, t_id,
       s_g3_const_ellipsoid_id, 0, s_g3_const_ellipsoid2,
       0, &DataParser::add_text, &DataParser::g3_const_ellipsoid_id);

  init(s_g3_const_ellipsoid, t_a,
       0, 0, s_g3_const_ellipsoid_a,
       0, &DataParser::add_text, 0);

  init(s_g3_const_ellipsoid_a, t_b,
       s_g3_const_ellipsoid_b, 0, s_g3_const_ellipsoid2,
       0, &DataParser::add_text, &DataParser::g3_const_ellipsoid_b);

  init(s_g3_const_ellipsoid_a, t_inv_f,
       s_g3_const_ellipsoid_inv_f, 0, s_g3_const_ellipsoid2,
       0, &DataParser::add_text, &DataParser::g3_const_ellipsoid_inv_f);

  // .....  <g3-model> <unused | fixed | free | constr>  .............

  init(s_g3_model, t_unused,
       s_g3_param, 0, s_g3_model,
       &DataParser::g3_param_unused, 0, 0);

  init(s_g3_model, t_fixed,
       s_g3_param, 0, s_g3_model,
       &DataParser::g3_param_fixed, 0, 0);

  init(s_g3_model, t_free,
       s_g3_param, 0, s_g3_model,
       &DataParser::g3_param_free, 0, 0);

  init(s_g3_model, t_constr,
       s_g3_param, 0, s_g3_model,
       &DataParser::g3_param_constr, 0, 0);

  init(s_g3_param, t_n,
       s_g3_param_n, 0, 0,
       0, 0, &DataParser::g3_param_n);

  init(s_g3_param, t_e,
       s_g3_param_e, 0, 0,
       0, 0, &DataParser::g3_param_e);

  init(s_g3_param, t_u,
       s_g3_param_u, 0, 0,
       0, 0, &DataParser::g3_param_u);

  // .....  <g3-model> <point>  .....................................

  init(s_g3_model, t_point,
       s_g3_point_1, s_g3_point_2, s_g3_model,
       0, 0, &DataParser::g3_point);

  init(s_g3_point_1, t_id,
       s_g3_point_id, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_id);


  init(s_g3_point_2, t_b,
       s_g3_point_b, 0, s_g3_point_after_b,
       0, &DataParser::add_text, &DataParser::g3_point_b);

  init(s_g3_point_after_b, t_l,
       s_g3_point_l, 0, s_g3_point_after_l,
       0, &DataParser::add_text, &DataParser::g3_point_l);

  init(s_g3_point_after_l, t_h,
       s_g3_point_h, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_h);

  init(s_g3_point_2, t_x,
       s_g3_point_x, 0, s_g3_point_after_x,
       0, &DataParser::add_text, 0);

  init(s_g3_point_after_x, t_y,
       s_g3_point_y, 0, s_g3_point_after_y,
       0, &DataParser::add_text, 0);

  init(s_g3_point_after_y, t_z,
       s_g3_point_z, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_z);

  init(s_g3_point_2, t_height,
       s_g3_point_height, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_height);

  init(s_g3_point_2, t_geoid,
       s_g3_point_geoid, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_geoid);

  init(s_g3_point_2, t_unused,
       s_g3_point_param, 0, s_g3_point_2,
       &DataParser::g3_param_unused, 0, 0);

  init(s_g3_point_2, t_fixed,
       s_g3_point_param, 0, s_g3_point_2,
       &DataParser::g3_param_fixed, 0, 0);

  init(s_g3_point_2, t_free,
       s_g3_point_param, 0, s_g3_point_2,
       &DataParser::g3_param_free, 0, 0);

  init(s_g3_point_2, t_constr,
       s_g3_point_param, 0, s_g3_point_2,
       &DataParser::g3_param_constr, 0, 0);

  init(s_g3_point_param, t_n,
       s_g3_point_param_n, 0, 0,
       0, 0, &DataParser::g3_point_param_n);

  init(s_g3_point_param, t_e,
       s_g3_point_param_e, 0, 0,
       0, 0, &DataParser::g3_point_param_e);

  init(s_g3_point_param, t_u,
       s_g3_point_param_u, 0, 0,
       0, 0, &DataParser::g3_point_param_u);

  init(s_g3_point_2, t_db,
       s_g3_point_db, 0, s_g3_point_after_db,
       0, &DataParser::add_text, 0);

  init(s_g3_point_after_db, t_dl,
       s_g3_point_dl, 0, s_g3_point_2,
       0, &DataParser::add_text, &DataParser::g3_point_dl);

  // .....  <g3-model> <obs>  ........................................

  init(s_g3_model, t_obs,
       s_g3_obs, 0, 0,
       &DataParser::g3_obs, 0, &DataParser::g3_obs);

  // .....  <g3-model> <obs> <cov-mat>  ..............................

  init(s_g3_obs, t_covmat,
       s_g3_obs_covmat, s_g3_obs_covmat_after_band, 0,
       0, 0, &DataParser::g3_obs_cov);

  init(s_g3_obs_covmat, t_dim,
       s_g3_obs_covmat_dim, 0, s_g3_obs_covmat_after_dim,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_covmat_after_dim, t_band,
       s_g3_obs_covmat_band, 0, s_g3_obs_covmat_after_band,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_covmat_after_band, t_flt,
       s_g3_obs_covmat_flt, 0, s_g3_obs_covmat_after_band,
       0, &DataParser::add_text, 0);

  // .....  <g3-model> <obs> <distance>  .............................

  init(s_g3_obs, t_dist,
       s_g3_obs_dist, s_g3_obs_dist_opt, 0,
       0, 0, &DataParser::g3_obs_dist);

  init(s_g3_obs_dist, t_from,
       s_g3_obs_dist_from, 0, s_g3_obs_dist_after_from,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_dist_after_from, t_to,
       s_g3_obs_dist_to, 0, s_g3_obs_dist_after_to,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_dist_after_to, t_val,
       s_g3_obs_dist_val, 0, s_g3_obs_dist_opt,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_dist_opt, t_stdev,
       s_g3_obs_dist_opt_stdev, 0, 0,
       0, &DataParser::optional_stdev, 0);

  init(s_g3_obs_dist_opt, t_variance,
       s_g3_obs_dist_opt_variance, 0, 0,
       0, &DataParser::optional_variance, 0);

  init(s_g3_obs_dist_opt, t_from_dh,
       s_g3_obs_dist_opt_from_dh, 0, 0,
       0, &DataParser::optional_from_dh, 0);

  init(s_g3_obs_dist_opt, t_to_dh,
       s_g3_obs_dist_opt_to_dh, 0, 0,
       0, &DataParser::optional_to_dh, 0);

  // .....  <g3-model> <obs> <zenith>  ...............................

  init(s_g3_obs, t_zenith,
       s_g3_obs_zenith, s_g3_obs_zenith_opt, 0,
       0, 0, &DataParser::g3_obs_zenith);

  init(s_g3_obs_zenith, t_from,
       s_g3_obs_zenith_from, 0, s_g3_obs_zenith_after_from,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_zenith_after_from, t_to,
       s_g3_obs_zenith_to, 0, s_g3_obs_zenith_after_to,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_zenith_after_to, t_val,
       s_g3_obs_zenith_val, 0, s_g3_obs_zenith_opt,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_zenith_opt, t_stdev,
       s_g3_obs_zenith_opt_stdev, 0, 0,
       0, &DataParser::optional_stdev, 0);

  init(s_g3_obs_zenith_opt, t_variance,
       s_g3_obs_zenith_opt_variance, 0, 0,
       0, &DataParser::optional_variance, 0);

  init(s_g3_obs_zenith_opt, t_from_dh,
       s_g3_obs_zenith_opt_from_dh, 0, 0,
       0, &DataParser::optional_from_dh, 0);

  init(s_g3_obs_zenith_opt, t_to_dh,
       s_g3_obs_zenith_opt_to_dh, 0, 0,
       0, &DataParser::optional_to_dh, 0);

 // .....  <g3-model> <obs> <azimuth>  ...............................

  init(s_g3_obs, t_azimuth,
       s_g3_obs_azimuth, s_g3_obs_azimuth_opt, 0,
       0, 0, &DataParser::g3_obs_azimuth);

  init(s_g3_obs_azimuth, t_from,
       s_g3_obs_azimuth_from, 0, s_g3_obs_azimuth_after_from,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_azimuth_after_from, t_to,
       s_g3_obs_azimuth_to, 0, s_g3_obs_azimuth_after_to,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_azimuth_after_to, t_val,
       s_g3_obs_azimuth_val, 0, s_g3_obs_azimuth_opt,
       0, &DataParser::add_text, 0);

  init(s_g3_obs_azimuth_opt, t_stdev,
       s_g3_obs_azimuth_opt_stdev, 0, 0,
       0, &DataParser::optional_stdev, 0);

  init(s_g3_obs_azimuth_opt, t_variance,
       s_g3_obs_azimuth_opt_variance, 0, 0,
       0, &DataParser::optional_variance, 0);

  init(s_g3_obs_azimuth_opt, t_from_dh,
       s_g3_obs_azimuth_opt_from_dh, 0, 0,
       0, &DataParser::optional_from_dh, 0);

  init(s_g3_obs_azimuth_opt, t_to_dh,
       s_g3_obs_azimuth_opt_to_dh, 0, 0,
       0, &DataParser::optional_to_dh, 0);

  // .....  <g3-model> <obs> <vector>  ...............................

  init(s_g3_obs, t_vector,
       s_g3_obs_vector, s_g3_obs_vector_opt, 0,
       0, 0, &DataParser::g3_obs_vector);

   init(s_g3_obs_vector, t_from,
        s_g3_obs_vector_from, 0, s_g3_obs_vector_after_from,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_vector_after_from, t_to,
        s_g3_obs_vector_to, 0, s_g3_obs_vector_after_to,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_vector_after_to, t_dx,
        s_g3_obs_vector_dx, 0, s_g3_obs_vector_after_dx,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_vector_after_dx, t_dy,
        s_g3_obs_vector_dy, 0, s_g3_obs_vector_after_dy,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_vector_after_dy, t_dz,
        s_g3_obs_vector_dz, 0, s_g3_obs_vector_opt,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_vector_opt, t_from_dh,
        s_g3_obs_vector_opt_from_dh, 0, 0,
        0, &DataParser::optional_from_dh, 0);

   init(s_g3_obs_vector_opt, t_to_dh,
        s_g3_obs_vector_opt_to_dh, 0, 0,
        0, &DataParser::optional_to_dh, 0);

   // .....  <g3-model> <obs> <xyz>  ..................................

   init(s_g3_obs, t_xyz,
        s_g3_obs_xyz, s_g3_obs_xyz_after_z, 0,
        0, 0, &DataParser::g3_obs_xyz);

   init(s_g3_obs_xyz, t_id,
        s_g3_obs_xyz_id, 0, s_g3_obs_xyz_after_id,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_xyz_after_id, t_x,
        s_g3_obs_xyz_x, 0, s_g3_obs_xyz_after_x,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_xyz_after_x, t_y,
        s_g3_obs_xyz_y, 0, s_g3_obs_xyz_after_y,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_xyz_after_y, t_z,
        s_g3_obs_xyz_z, 0, s_g3_obs_xyz_after_z,
        0, &DataParser::add_text, 0);

   // .....  <g3-model> <obs> <hdiff>  ................................

   init(s_g3_obs, t_hdiff,
        s_g3_obs_hdiff, s_g3_obs_hdiff_opt, 0,
        0, 0, &DataParser::g3_obs_hdiff);

   init(s_g3_obs_hdiff, t_from,
        s_g3_obs_hdiff_from, 0, s_g3_obs_hdiff_after_from,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_hdiff_after_from, t_to,
        s_g3_obs_hdiff_to, 0, s_g3_obs_hdiff_after_to,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_hdiff_after_to, t_val,
        s_g3_obs_hdiff_val, 0, s_g3_obs_hdiff_opt,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_hdiff_opt, t_stdev,
        s_g3_obs_hdiff_opt_stdev, 0, 0,
        0, &DataParser::optional_stdev, 0);

   init(s_g3_obs_hdiff_opt, t_variance,
        s_g3_obs_hdiff_opt_variance, 0, 0,
        0, &DataParser::optional_variance, 0);

//  init(s_g3_obs_hdiff_opt, t_from_dh,
//       s_g3_obs_hdiff_opt_from_dh, 0, 0,
//       0, &DataParser::optional_from_dh, 0);
//
//  init(s_g3_obs_hdiff_opt, t_to_dh,
//       s_g3_obs_hdiff_opt_to_dh, 0, 0,
//       0, &DataParser::optional_to_dh, 0);

   // ..... <g3-model> <obs> <height> ...................................

   init(s_g3_obs, t_height,
        s_g3_obs_height, s_g3_obs_height_opt, 0,
        0, 0, &DataParser::g3_obs_height);

   init(s_g3_obs_height, t_id,
        s_g3_obs_height_id, 0, s_g3_obs_height_after_id,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_height_after_id, t_val,
        s_g3_obs_height_val, 0, s_g3_obs_height_opt,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_height_opt, t_stdev,
        s_g3_obs_height_opt_stdev, 0, 0,
        0, &DataParser::optional_stdev, 0);

   init(s_g3_obs_height_opt, t_variance,
        s_g3_obs_height_opt_variance, 0, 0,
        0, &DataParser::optional_variance, 0);

   // ..... <g3-model> <obs> <angle> ...................................

   init(s_g3_obs, t_angle,
        s_g3_obs_angle, s_g3_obs_angle_opt, 0,
        0, 0, &DataParser::g3_obs_angle);

   init(s_g3_obs_angle, t_from,
        s_g3_obs_angle_from, 0, s_g3_obs_angle_after_from,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_angle_after_from, t_left,
        s_g3_obs_angle_left, 0, s_g3_obs_angle_after_left,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_angle_after_left, t_right,
        s_g3_obs_angle_right, 0, s_g3_obs_angle_after_right,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_angle_after_right, t_val,
        s_g3_obs_angle_val, 0, s_g3_obs_angle_opt,
        0, &DataParser::add_text, 0);

   init(s_g3_obs_angle_opt, t_stdev,
        s_g3_obs_angle_opt_stdev, 0, 0,
        0, &DataParser::optional_stdev, 0);

   init(s_g3_obs_angle_opt, t_variance,
        s_g3_obs_angle_opt_variance, 0, 0,
        0, &DataParser::optional_variance, 0);

   init(s_g3_obs_angle_opt, t_from_dh,
        s_g3_obs_angle_opt_from_dh, 0, 0,
        0, &DataParser::optional_from_dh, 0);

   init(s_g3_obs_angle_opt, t_left_dh,
        s_g3_obs_angle_opt_left_dh, 0, 0,
        0, &DataParser::optional_left_dh, 0);

   init(s_g3_obs_angle_opt, t_right_dh,
        s_g3_obs_angle_opt_right_dh, 0, 0,
        0, &DataParser::optional_right_dh, 0);
}

int DataParser::g3_model(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  g3->model = new g3::Model;

  return 0;
}

int DataParser::g3_model(const char *name)
{
  objects.push_back( new DataObject::g3_model(g3->model) );
  g3->model = 0;

  return  end_tag(name);
}

int DataParser::g3_param_unused(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  local_state.set_unused();

  return  0;
}

int DataParser::g3_param_fixed(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  local_state.set_fixed();

  return  0;
}

int DataParser::g3_param_free(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  local_state.set_free();

  return  0;
}

int DataParser::g3_param_constr(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  local_state.set_constr();

  return  0;
}

int DataParser::g3_param_n(const char *name)
{
  global_state_N.set_state(local_state);

  return  end_tag(name);
}

int DataParser::g3_param_e(const char *name)
{
  global_state_E.set_state(local_state);

  return  end_tag(name);
}

int DataParser::g3_param_u(const char *name)
{
  global_state_U.set_state(local_state);

  return  end_tag(name);
}

std::string DataParser::g3_get_id(std::string err)
{
  char    c;
  string id;
  string::const_iterator i=text_buffer.begin();
  string::const_iterator e=text_buffer.end();

  while (i!=e &&  isspace((c = *i))) { ++i;          }
  while (i!=e && !isspace((c = *i))) { ++i; id += c; }
  while (i!=e &&  isspace((c = *i))) { ++i;          }

  if (i!=e || id.empty()) error(err);

  text_buffer.erase();
  return id;
}

int DataParser::g3_point(const char *name)
{
  // checks consistency of the current point

  if (!point->N.cmp_state(point->E))
    return error("### parameters N and E do not have common status");

  return  end_tag(name);
}

int DataParser::g3_point_id(const char *name)
{
  string id = g3_get_id("### bad point name in <id> tag");

  point = g3->model->get_point(id);

  point->N.set_state(global_state_N);
  point->E.set_state(global_state_E);
  point->U.set_state(global_state_U);

  return  end_tag(name);
}

int DataParser::g3_point_param_n(const char *name)
{
  point->N.set_state(local_state);

  return  end_tag(name);
}

int DataParser::g3_point_param_e(const char *name)
{
  point->E.set_state(local_state);

  return  end_tag(name);
}

int DataParser::g3_point_param_u(const char *name)
{
  point->U.set_state(local_state);

  return  end_tag(name);
}

int DataParser::g3_point_b(const char *name)
{
  if (!deg2gon(text_buffer, blh.b))
    {
      return error("### bad format of numerical data in <point> <b> ");
    }
  text_buffer.erase();

  return  end_tag(name);
}

int DataParser::g3_point_l(const char *name)
{
  if (!deg2gon(text_buffer, blh.l))
    {
      return error("### bad format of numerical data in <point> <l> ");
    }
  text_buffer.erase();

  return  end_tag(name);
}

int DataParser::g3_point_h(const char *name)
{
  stringstream istr(text_buffer);
  if (!(istr >> blh.h))
    {
      return error("### bad format of numerical data in <point> <h> ");
    }
  text_buffer.erase();

  blh.b *= GON_TO_RAD;
  blh.l *= GON_TO_RAD;
  point->set_blh(blh.b, blh.l, blh.h);

  return  end_tag(name);
}

int DataParser::g3_point_z(const char *name)
{
  stringstream istr(text_buffer);
  double x, y, z;

  if (!(istr >> x >> y >> z))
    {
      return error("### bad format of numerical data in <point> xyz ");
    }

  text_buffer.erase();

  point->set_xyz(x, y, z);

  return  end_tag(name);
}

int DataParser::g3_point_height(const char *name)
{
  stringstream istr(text_buffer);
  double h;

  if (!(istr >> h))
    {
      return error("### bad format of numerical data in <point> height ");
    }

  text_buffer.erase();

  point->set_height(h);

  return  end_tag(name);
}

int DataParser::g3_point_geoid(const char *name)
{
  stringstream istr(text_buffer);
  double g;

  if (!(istr >> g))
    {
      return error("### bad format of numerical data in <point> geoid ");
    }

  text_buffer.erase();

  point->set_geoid(g);

  return  end_tag(name);
}

int DataParser::g3_point_dl(const char *name)
{
  stringstream istr(text_buffer);
  double db, dl;

  if (!(istr >> db >> dl))
    {
      return error("### bad format of numerical data in <point> db dl ");
    }
  text_buffer.erase();

  point->dB.set_init_value(db*SS_TO_RAD);
  point->dL.set_init_value(dl*SS_TO_RAD);

  return  end_tag(name);
}

int DataParser::g3_obs(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  g3->obs_cluster = new g3::ObsCluster(&g3->model->obsdata);
  g3->scale.clear();

  return 0;
}

int DataParser::g3_obs(const char *name)
{
  using namespace g3;

  int obs_dim = 0;
  for (std::list<g3::Observation*>::const_iterator
         i=g3->obs_cluster->observation_list.begin(),
         e=g3->obs_cluster->observation_list.end();  i!=e;  ++i)
    {
      obs_dim += (*i)->dimension();
    }

  if (obs_dim != int(g3->scale.size()))
    return error("### INTERNAL ERROR IN "
                 "int DataParser::g3_obs(const char *name)");


  size_t cov_dim  = 0;
  size_t cov_band = 0;
  typedef std::list<CovMat<> >::const_iterator Iterator;
  for (Iterator i=g3->cov_list.begin(), e=g3->cov_list.end(); i!=e; ++i)
    {
      const CovMat<>& cov = *i;

      cov_dim += cov.dim();
      if (int(cov.bandWidth()) > cov_band) cov_band = cov.bandWidth();
    }

  if (obs_dim == 0)       return error("### no observations in <obs>");
  if (obs_dim != cov_dim) return error("### bad covariance matrix dimension");

  g3->obs_cluster->covariance_matrix.reset(cov_dim, cov_band);
  g3->obs_cluster->covariance_matrix.set_zero();

  size_t offset = 0;
  for (Iterator i=g3->cov_list.begin(), e=g3->cov_list.end(); i!=e; ++i)
    {
      const CovMat<>& cov = *i;

      for (size_t i=1; i<=cov.dim(); i++)
        for (size_t j=0; j<=cov.bandWidth() && i+j<=cov.dim(); j++)
          g3->obs_cluster->covariance_matrix(offset+i, offset+i+j) = cov(i, i+j);

      offset += cov.dim();
    }

  /* TODO: !!! here we should better test Cholesky decomposition !!! */
  for (size_t N=g3->obs_cluster->covariance_matrix.dim(), i=1; i<=N; i++)
    if(g3->obs_cluster->covariance_matrix(i,i) <= 0)
      return error("### zero or negative variance");


  // here we scale covariance matrix so that all its elements have the
  // same units as their corresponding internal representation of
  // observables (linear data are stored in meters, angular values in
  // radians)

  DataParser_g3::Scale::const_iterator s = g3->scale.begin();
  for (int i=1; i<=obs_dim; i++, s++)
    {
      if (*s != 1.0) g3->obs_cluster->scaleCov(i, *s);
    }
  g3->scale.clear();

  g3->obs_cluster->update();
  g3->model->obsdata.clusters.push_back(g3->obs_cluster);
  g3->cov_list.clear();

  return  end_tag(name);
}

int DataParser::g3_obs_cov(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  int     d, b;
  double  f;

  if (!(istr >> d >> b))  return error("### bad cov-mat");

  DataParser_g3::Cov cov(d, b);
  cov.set_zero();
  for (int i=1; i<=d; i++)          // upper triangular matrix by rows
    for (int j=i; j<=i+b && j<=d; j++)
      if (istr >> f)
        cov(i,j) = f;
      else        return error("### bad cov-mat / some data missing");

  if (!pure_data(istr)) return error("### bad cov-mat / redundant data");

  text_buffer.clear();
  g3->cov_list.push_back(cov);

  return  end_tag(name);
}

int DataParser::optional_stdev(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s, len));
  DataParser_g3::Cov   cov(1,0);
  double  f;

  if (!pure_data(istr >> f)) return error("### bad <stdev>");

  cov(1,1) = f*f;
  g3->cov_list.push_back(cov);

  return 0;
}

int DataParser::optional_variance(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s, len));
  DataParser_g3::Cov   cov(1,0);
  double  f;

  if (!pure_data(istr >> f)) return error("### bad <variance>");

  cov(1,1) = f;
  g3->cov_list.push_back(cov);

  return 0;
}

int DataParser::optional_from_dh(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s,len));

  if (pure_data(istr >> g3->from_dh)) return 0;

  return error("### bad data in <from-dh>");
}

int DataParser::optional_to_dh(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s,len));

  if (pure_data(istr >> g3->to_dh)) return 0;

  return error("### bad data in <to-dh>");
}

int DataParser::optional_left_dh(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s,len));

  if (pure_data(istr >> g3->left_dh)) return 0;

  return error("### bad data in <left-dh>");
}

int DataParser::optional_right_dh(const char *s, int len)
{
  using namespace g3;
  stringstream istr(string(s,len));

  if (pure_data(istr >> g3->right_dh)) return 0;

  return error("### bad data in <right-dh>");
}

int DataParser::g3_obs_dist(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       from, to;
  double       val;

  if (pure_data(istr >> from >> to >> val))
    {
      text_buffer.clear();

      Distance* distance = new Distance;
      distance->from = from;
      distance->to   = to;
      distance->set(val);
      distance->from_dh = optional(g3->from_dh);
      distance->to_dh   = optional(g3->to_dh);
      g3->obs_cluster->observation_list.push_back(distance);
      g3->scale.push_back(1.0);

      return  end_tag(name);
    }

  return error("### bad <distance>");
}

int DataParser::g3_obs_zenith(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       from, to;
  string       sval;

  if (pure_data(istr >> from >> to >> sval))
    {
      text_buffer.clear();

      double val;
      if (deg2gon(sval, val))
        {
          g3->scale.push_back(3.0864); // ss --> cc
        }
      else
        {
          istringstream istr(sval);
          istr >> val;

          g3->scale.push_back(1.0);
        }

      ZenithAngle* zenith = new ZenithAngle;
      zenith->from = from;
      zenith->to   = to;
      zenith->set(val*GON_TO_RAD);
      zenith->from_dh = optional(g3->from_dh);
      zenith->to_dh   = optional(g3->to_dh);
      g3->obs_cluster->observation_list.push_back(zenith);

      return  end_tag(name);
    }

  return error("### bad <zenith>");
}

int DataParser::g3_obs_azimuth(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       from, to;
  string       sval;

  if (pure_data(istr >> from >> to >> sval))
    {
      text_buffer.clear();

      double val;
      if (!deg2gon(sval, val))
        {
          istringstream istr(sval);
          istr >> val;
        }

      Azimuth* azimuth = new Azimuth;
      azimuth->from = from;
      azimuth->to   = to;
      azimuth->set(val);
      azimuth->from_dh = optional(g3->from_dh);
      azimuth->to_dh   = optional(g3->to_dh);
      g3->obs_cluster->observation_list.push_back(azimuth);

      return  end_tag(name);
    }

  return error("### bad <azimuth>");
}

int DataParser::g3_obs_vector(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string from, to;
  double dx, dy, dz;

  if (pure_data(istr >> from >> to >> dx >> dy >> dz))
    {
      text_buffer.clear();

      Vector* vector = new Vector;
      vector->from = from;
      vector->to   = to;
      vector->set_dxyz(dx, dy, dz);
      vector->from_dh = optional(g3->from_dh);
      vector->to_dh   = optional(g3->to_dh);

      g3->obs_cluster->observation_list.push_back(vector);
      g3->scale.push_back(1.0);
      g3->scale.push_back(1.0);
      g3->scale.push_back(1.0);

      return end_tag(name);
    }

  return error("### bad <vector> from to dx dy dz : " + text_buffer);
}

int DataParser::g3_obs_xyz(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string id;
  double x, y, z;

  if (pure_data(istr >> id >> x >> y >> z))
    {
      text_buffer.clear();

      XYZ* xyz = new XYZ;
      xyz->id  = id;
      xyz->set_xyz(x, y, z);

      g3->obs_cluster->observation_list.push_back(xyz);
      g3->scale.push_back(1.0);
      g3->scale.push_back(1.0);
      g3->scale.push_back(1.0);

      return end_tag(name);
    }

  return error("### bad <xyz>");
}

int DataParser::g3_obs_hdiff(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       from, to;
  double       val;

  if (pure_data(istr >> from >> to >> val))
   {
     text_buffer.clear();

     HeightDiff* hdiff = new HeightDiff;
     hdiff->from = from;
     hdiff->to   = to;
     hdiff->set(val);
     hdiff->from_dh = optional(g3->from_dh);
     hdiff->to_dh   = optional(g3->to_dh);
     g3->obs_cluster->observation_list.push_back(hdiff);
     g3->scale.push_back(1.0);

     return  end_tag(name);
   }

  return error("### bad <hdiff>");
}

int DataParser::g3_obs_height(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       id;
  double       val;

  if (pure_data(istr >> id >> val))
   {
     text_buffer.clear();

     Height* height = new Height;
     height->id = id;
     height->set(val);
     g3->obs_cluster->observation_list.push_back(height);
     g3->scale.push_back(1.0);

     return  end_tag(name);
   }

  return error("### bad <height>");
}

int DataParser::g3_const_apriori_sd(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  double       sd;

  if (pure_data(istr >> sd))
   {
     text_buffer.clear();

     g3->model->set_apriori_sd(sd);

     return  end_tag(name);
   }

  return error("### bad <height>");
}

int DataParser::g3_const_conf_level(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  double       cl;

  if (pure_data(istr >> cl))
   {
     text_buffer.clear();

     g3->model->set_conf_level(cl);

     return  end_tag(name);
   }

  return error("### bad <confidence-level>");
}

int DataParser::g3_const_tol_abs(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  double       ta;

  if (pure_data(istr >> ta))
   {
     text_buffer.clear();

     g3->model->set_tol_abs(ta);

     return  end_tag(name);
   }

  return error("### bad <tol-abs>");
}

int DataParser::g3_const_ellipsoid_id(const char * /*name*/)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       s;

  if (pure_data(istr >> s))
   {
     text_buffer.clear();
     const char* const name = s.c_str();
     gama_ellipsoid id = ellipsoid(name);

     set(&g3->model->ellipsoid, id);

     if (id != ellipsoid_unknown) return end_tag(name);
   }

  return error("### bad <ellipsoid> <id>");
}

int DataParser::g3_const_ellipsoid_b(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  double       a, b;

  if (pure_data(istr >> a >> b))
   {
     text_buffer.clear();

     set(&g3->model->ellipsoid, ellipsoid_unknown);
     g3->model->ellipsoid.set_ab(a, b);

     return end_tag(name);
   }

  return error("### bad <ellipsoid> <a> <b>");
}

int DataParser::g3_const_ellipsoid_inv_f(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  double       a, inv_f;

  if (pure_data(istr >> a >> inv_f))
   {
     text_buffer.clear();

     set(&g3->model->ellipsoid, ellipsoid_unknown);
     g3->model->ellipsoid.set_af1(a, inv_f);

     return end_tag(name);
   }

  return error("### bad <ellipsoid> <a> <inv-f>");
}

int DataParser::g3_const_ang_degrees(const char *name)
{
  text_buffer.clear();
  g3->model->set_angular_units_degrees();

  return end_tag(name);
}

int DataParser::g3_const_ang_gons(const char *name)
{
  text_buffer.clear();
  g3->model->set_angular_units_gons();

  return end_tag(name);
}

int DataParser::g3_obs_angle(const char *name)
{
  using namespace g3;
  stringstream istr(text_buffer);
  string       from, left, right;
  string       sval;

  if (pure_data(istr >> from >> left >> right >> sval))
   {
     text_buffer.clear();

     double val;
     if (deg2gon(sval, val))
       {
         g3->scale.push_back(3.0864);   // ss --> cc
       }
     else
       {
          istringstream istr(sval);
          istr >> val;

          g3->scale.push_back(1.0);
       }

     Angle* angle = new Angle;
     angle->from  = from;
     angle->left  = left;
     angle->right = right;
     angle->set(val*GON_TO_RAD);
     angle->from_dh  = optional(g3->from_dh);
     angle->left_dh  = optional(g3->to_dh);
     angle->right_dh = optional(g3->to_dh);
     g3->obs_cluster->observation_list.push_back(angle);

     return  end_tag(name);
   }

  return error("### bad <angle>");
}

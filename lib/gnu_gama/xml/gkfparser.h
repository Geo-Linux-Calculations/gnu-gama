/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000, 2002, 2013, 2014, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local_GKF_XML_parser_h_
#define gama_local_GKF_XML_parser_h_


#include <gnu_gama/xml/baseparser.h>
#include <gnu_gama/xml/dataobject.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>


namespace GNU_gama { namespace local {

    /** A removed class ParserException has been replaced by a typedef
	to GNU_gama::Exception::parser;
     */

    typedef GNU_gama::Exception::parser ParserException;

  // class ParserException : public GNU_gama::local::Exception
  // {
  // public:
  //
  //   int line, error_code;
  //
  //   ParserException(std::string s, int r, int c)
  //     : GNU_gama::local::Exception(s), line(r), error_code(c)
  //     {
  //     }
  //
  // };


  class GKFparser : public GNU_gama::BaseParser<ParserException>
    {
    public:

      GKFparser(GNU_gama::local::LocalNetwork& locnet);
      ~GKFparser();

      int characterDataHandler(const char* s, int len);
      int startElement(const char *cname, const char **atts);
      int endElement(const char * name);

      double implicit_stdev_direction() const { return direction_stdev_; }
      double implicit_stdev_angle()     const { return angle_stdev_;     }
      double implicit_stdev_zangle()    const { return zenith_stdev_;    }
      double implicit_stdev_azimuth()   const { return azimuth_stdev_;   }
      double implicit_stdev_distance(double d) const
        {
          using namespace std;
          return distance_stdev_ + distance_stdev_km_ * pow(d/1000, distance_stdev_exp_);
        }
      double implicit_stdev_distance_a() const { return distance_stdev_;     }
      double implicit_stdev_distance_b() const { return distance_stdev_km_;  }
      double implicit_stdev_distance_c() const { return distance_stdev_exp_; }

      /* check if covariance matrices are positive-definite */
      void   check_covariances(bool ch=true)   { check_cov_mat = ch;   }

    private:

      GNU_gama::local::LocalNetwork&    lnet;
      GNU_gama::local::PointData&       SB;        // point list
      GNU_gama::local::ObservationData& OD;        // observation list

      std::string description;                     // network description

      enum gkf_tag {
        tag_unknown,
        tag_gama_xml,
        tag_network,
        tag_description,
        tag_parameters,
        tag_points_observations,
        tag_point,
        tag_obs,
        tag_cov_mat,
        tag_direction,
        tag_distance,
        tag_angle,
        tag_s_distance,
        tag_z_angle,
        tag_height_differences,
        tag_dh,
        tag_coordinates,
        tag_vectors,
        tag_vec,
        tag_azimuth
      };

      gkf_tag tag(const char* cname);

      enum gkf_state {
        state_error,
        state_start,
        state_gama_xml,
        state_network,
        state_description,
        state_parameters,
        state_point_obs,
        state_point,
        state_obs,
        state_obs_direction,
        state_obs_distance,
        state_obs_angle,
        state_obs_sdistance,
        state_obs_zangle,
        // xsd 0.91 state_obs_dh,
        state_obs_azimuth,
        state_obs_cov,
        state_obs_after_cov,
        state_coords,
        state_coords_point,
        state_coords_cov,
        state_coords_after_cov,
        state_hdiffs,
        state_hdiffs_dh,
        state_hdiffs_cov,
        state_hdiffs_after_cov,
        state_vectors,
        state_vectors_vec,
        state_vectors_cov,
        state_vectors_after_cov,
        state_stop
      };

      // 1.7.09 std::pair<"standard deviation", "angular value in degrees">
      std::vector<std::pair<Double, bool> > sigma;
      Index        idim, iband;            // covariance matrix dim. / band
      bool         pp_xydef, pp_zdef;      // process_point();
      Double       pp_x, pp_y, pp_z;
      PointID      pp_id;
      std::string  cov_mat_data;

      // Implicit value of stanpoint ID is set for sets of
      // directions/distances and/or angles.

      std::string         standpoint_id;
      Double              obs_from_dh;

      StandPoint        * standpoint;
      Coordinates       * coordinates;
      HeightDifferences * heightdifferences;
      Vectors           * vectors;

      bool                check_cov_mat;

      int process_gama_xml   (const char** atts);
      int process_network    (const char** atts);
      int process_parameters (const char** atts);
      int process_point_obs  (const char** atts);
      int process_point      (const char** atts);
      int process_distance   (const char** atts);
      int process_angle      (const char** atts);
      int process_direction  (const char** atts);
      int process_sdistance  (const char** atts);
      int process_zangle     (const char** atts);
      // xsd 0.91 int process_obs_dh     (const char** atts);
      int process_azimuth    (const char** atts);

      int process_obs(const char** atts);
      int finish_obs();

      int process_coords(const char** atts);
      int finish_coords();
      int process_coords_point(const char** atts);

      int process_hdiffs(const char** atts);
      int finish_hdiffs();
      int process_dh(const char** atts);

      int process_cov(const char** atts);
      int finish_cov(CovMat&);
      int process_obs_cov(const char** atts)
        {
          state = state_obs_cov;
          return process_cov(atts);
        }
      int process_coords_cov(const char** atts)
        {
          state = state_coords_cov;
          return process_cov(atts);
        }
      int process_hdiffs_cov(const char** atts)
        {
          state = state_hdiffs_cov;
          return process_cov(atts);
        }
      int process_vectors_cov(const char** atts)
        {
          state = state_vectors_cov;
          return process_cov(atts);
        }

      int process_vectors(const char** atts);
      int finish_vectors();
      int process_vec(const char** atts);

      // implicit values of standard deviations are initialized in
      // GKFparser::process_point_obs(const char** atts)

      double distance_stdev_;
      double distance_stdev_km_;
      double distance_stdev_exp_;
      double direction_stdev_;
      double angle_stdev_;
      double zenith_stdev_;
      double azimuth_stdev_;
    };  // class GKFparser
}}       // namespace GNU_gama::local


#endif

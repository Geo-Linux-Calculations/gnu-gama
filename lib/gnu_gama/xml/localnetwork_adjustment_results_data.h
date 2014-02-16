/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2013, 2014  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/** \file localnetwork_adjustmet_results_data.h
 * \brief #GNU_gama::local::LocalNetworkAdjustmentResultsData class implementation
 *
 * \author Ales Cepek
 */

#ifndef GNU_gama_localnetwork_adjustment_results_data__DATA_h
#define GNU_gama_localnetwork_adjustment_results_data__DATA_h

#include <string>
#include <vector>
#include <matvec/covmat.h>
#include <gnu_gama/local/xmlerror.h>


namespace GNU_gama
{
  class LocalNetworkAdjustmentResultsData
  {
  public:

    LocalNetworkAdjustmentResultsData();

    GNU_gama::local::XMLerror xmlerror;

    bool gons;
    std::string description;

    struct
    {
      std::string gama_local_version;
      std::string gama_local_algorithm;
      std::string gama_local_compiler;
      std::string axes_xy;
      std::string angles;
      std::string epoch;
      std::string latitude;
      std::string ellipsoid;

    } network_general_parameters;

    struct count
    {
      int xyz, xy, z;
    };

    struct
    {
      count adjusted;
      count constrained;
      count fixed;

    } coordinates_summary;

    struct
    {
      int distances;
      int directions;
      int angles;
      int xyz_coords;
      int h_diffs;
      int z_angles;
      int s_dists;
      int vectors;
      int azimuths;

    } observations_summary;

    struct
    {
      int    equations;
      int    unknowns;
      int    degrees_of_freedom;
      int    defect;
      double sum_of_squares;
      bool   connected_network;

    } project_equations;

    struct
    {
      double apriori;
      double aposteriori;
      bool   using_aposteriori;
      double probability;
      double ratio;
      double lower;
      double upper;
      bool   passed;
      double confidence_scale;

    } standard_deviation;

    struct Point
    {
      std::string id;
      double x, y, z;

      bool   hxy, hz;             // point has   x, y, z
      bool   cxy, cz;             // constrained x, y, z
      int    indx, indy, indz;    // adjustment indexes

      void clear()
      {
        x=y=z=0;
        hxy=hz=cxy=cz=false;
        indx=indy=indz=0;
      }
    };

    typedef std::vector<Point> PointList;
    PointList  fixed_points, approximate_points, adjusted_points;

    struct Orientation
    {
      std::string id;
      double      approx;
      double      adj;
      int         index;          // adjustment index
    };

    typedef std::vector<Orientation> OrientationList;
    OrientationList orientations;

    CovMat<> cov;

    std::vector<int> original_index; // original indexes from the adjustment

    struct Observation
    {
      std::string xml_tag;
      std::string from;
      std::string to;
      std::string left;         // used in angle observation
      std::string right;        //  ....   angle   ....

      double obs;               // observed value
      double adj;               // adjusted
      double stdev;             // standard deviation of adj. value
      double qrr;               // weight coefficient of the residual
      double f;

      double      std_residual; // standardized residual
      std::string err_obs;      // estimate of observed value error
      std::string err_adj;      //  ....       adjusted  ....

      void clear()
      {
        obs = adj = stdev = qrr = f = std_residual = 0;
        xml_tag     .clear();
        from        .clear();
        to          .clear();
        left        .clear();
        right       .clear();
        err_obs     .clear();
        err_adj     .clear();
      }

      double residual() const throw();
    };

    typedef std::vector<Observation> ObservationList;
    ObservationList  obslist;

  private:

    void init();

  };
}

#endif

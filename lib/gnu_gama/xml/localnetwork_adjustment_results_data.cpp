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

/** \file localnetwork_adjustmet_results_data.cpp
 * \brief #GNU_gama::local::LocalNetworkAdjustmentResultsData class implementation
 *
 * \author Ales Cepek
 */

#include <iostream>
#include <cstring>
#include <string>
#include <stack>
#include <sstream>
#include <gnu_gama/xml/localnetwork_adjustment_results_data.h>
#include <gnu_gama/intfloat.h>
#include <gnu_gama/xml/htmlparser.h>

using namespace std;
using namespace GNU_gama;

LocalNetworkAdjustmentResultsData::LocalNetworkAdjustmentResultsData()
{
  init();
}

void LocalNetworkAdjustmentResultsData::init()
{
  gons = true;

  xmlerror.clear();
  description.clear();

  network_general_parameters.gama_local_version.  clear();
  network_general_parameters.gama_local_algorithm.clear();
  network_general_parameters.gama_local_compiler. clear();
  network_general_parameters.axes_xy.  clear();
  network_general_parameters.angles.   clear();
  network_general_parameters.epoch.    clear();
  network_general_parameters.latitude. clear();
  network_general_parameters.ellipsoid.clear();

  coordinates_summary.adjusted.xyz = 0;
  coordinates_summary.adjusted.xy  = 0;
  coordinates_summary.adjusted.z   = 0;
  coordinates_summary.constrained.xyz = 0;
  coordinates_summary.constrained.xy  = 0;
  coordinates_summary.constrained.z   = 0;
  coordinates_summary.fixed.xyz = 0;
  coordinates_summary.fixed.xy  = 0;
  coordinates_summary.fixed.z   = 0;

  observations_summary.distances = 0;
  observations_summary.directions = 0;
  observations_summary.angles = 0;
  observations_summary.xyz_coords = 0;
  observations_summary.h_diffs = 0;
  observations_summary.z_angles = 0;
  observations_summary.s_dists = 0;
  observations_summary.vectors = 0;
  observations_summary.azimuths = 0;

  project_equations.equations = 0;
  project_equations.unknowns = 0;
  project_equations.degrees_of_freedom = 0;
  project_equations.defect = 0;
  project_equations.sum_of_squares = 0;
  project_equations.connected_network = true;

  standard_deviation.apriori = 0;
  standard_deviation.aposteriori = 0;
  standard_deviation.using_aposteriori = true;
  standard_deviation.probability = 0;
  standard_deviation.ratio = 0;
  standard_deviation.lower = 0;
  standard_deviation.upper = 0;
  standard_deviation.passed = false;
  standard_deviation.confidence_scale = 0;

  fixed_points      .clear();
  approximate_points.clear();
  adjusted_points   .clear();
}

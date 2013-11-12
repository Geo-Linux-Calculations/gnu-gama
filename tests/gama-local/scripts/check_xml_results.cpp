/* GNU Gama -- testing adjustment results from different algorithms
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "check_xyz.h"
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

using GNU_gama::local::LocalNetwork;
using GNU_gama::LocalNetworkAdjustmentResults;

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      std::cout << "   #### " << argv[0] << " wrong number of arguments\n";
      return 1;
    }

  std::string conf = argv[1];

  LocalNetwork* lnet = getNet(alg_gso, argv[2]);
  LocalNetworkAdjustmentResults* adjres = new LocalNetworkAdjustmentResults;

  {
    std::ifstream inp_xml(argv[3]);
    if (!inp_xml)
      {
        std::cout << "   ####  ERROR ON OPENING FILE " << argv[3] << "\n";
        return 1;
      }

    adjres->read_xml(inp_xml);
  }

  int failed = 0;
  std::cout << "max.diff to available adjustment results for "
            << argv[1] << "\n";

  { // adjusted coordinates xyz
    double maxdiffxyz = 0;

    const GNU_gama::local::Vec& x = lnet->solve();
    const int y_sign = GaMaConsistent(lnet->PD) ? +1 : -1;

    for (int i=0; i<adjres->adjusted_points.size(); i++)
      {
         LocalNetworkAdjustmentResults::Point A
           = adjres->adjusted_points[i];
        GNU_gama::local::LocalPoint P = lnet->PD[A.id];

        if (A.indx && A.indy && P.index_x() && P.index_y())
          {
            double adj_x = P.x()+x(P.index_x())/1000;
            double adj_y = y_sign*(P.y()+x(P.index_y())/1000);

            double dx = adj_x - A.x;
            if (std::abs(dx) > std::abs(maxdiffxyz)) maxdiffxyz = dx;

            double dy = adj_y - A.y;
            if (std::abs(dy) > std::abs(maxdiffxyz)) maxdiffxyz = dy;
          }
        if (A.indz && P.index_z())
          {
            double adj_z = P.z()+x(P.index_z())/1000;

            double dz = adj_z - A.z;
            if (std::abs(dz) > std::abs(maxdiffxyz)) maxdiffxyz = dz;
          }
      }

    std::cout << "         adjusted coordinates "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << maxdiffxyz << " [m] ";
    if (std::abs(maxdiffxyz) >= 1e-5)
      {
        std::cout << " FAILED";
        failed = 1;
      }
    std::cout << "\n";
  } // adjusted coordinates xyz


  { // coordinate standard deviations
    double maxdiffstd = 0;

    for (int i=0; i<adjres->adjusted_points.size(); i++)
      {
         LocalNetworkAdjustmentResults::Point A
           = adjres->adjusted_points[i];
        GNU_gama::local::LocalPoint P = lnet->PD[A.id];

        if (A.indx && A.indy && P.index_x() && P.index_y())
          {
            double mx = std::sqrt(adjres->cov(A.indx, A.indx));
            double my = std::sqrt(adjres->cov(A.indy, A.indy));

            double dx = lnet->unknown_stdev(P.index_x()) - mx;
            double dy = lnet->unknown_stdev(P.index_y()) - my;

            if (std::abs(dx) > std::abs(maxdiffstd)) maxdiffstd = dx;
            if (std::abs(dy) > std::abs(maxdiffstd)) maxdiffstd = dy;
          }

        if (A.indz && P.index_z())
          {
            double mz = std::sqrt(adjres->cov(A.indz, A.indz));
            double dz = lnet->unknown_stdev(P.index_z()) - mz;

            if (std::abs(dz) > std::abs(maxdiffstd)) maxdiffstd = dz;
          }
      }

    maxdiffstd *= 1e-3;  // millimeters to meters
    std::cout << "         xy/z std. deviations "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << maxdiffstd << " [m]";
    if (std::abs(maxdiffstd) >= 1e-5)
      {
        std::cout << " FAILED";
        failed = 1;
      }
    std::cout << "\n";
  } // coordinate standard deviations


  { // adjusted observations
    double maxdiffobs = 0;
    double mdfobstd   = 0;

    const GNU_gama::local::Vec& r = lnet->residuals();

    for (int i=0; i<adjres->obslist.size(); i++)
      {
        LocalNetworkAdjustmentResults::Observation A
          = adjres->obslist[i];
        GNU_gama::local::Observation* obs
          = lnet->ptr_obs(i+1);

        std::cout.flush();
        double dfobs = 0;
        double dfstd = 0;
        if (A.xml_tag == "angle"        || A.xml_tag == "direction" ||
            A.xml_tag == "zenith-angle" || A.xml_tag == "azimuth")
          {
            GNU_gama::local::LocalPoint F = lnet->PD[A.from];
            GNU_gama::local::LocalPoint T;
            if (A.xml_tag == "angle")
              T = lnet->PD[A.left];
            else
              T = lnet->PD[A.to];
            double dx = F.x() - T.x();
            double dy = F.y() - T.y();
            double D  = std::sqrt(dx*dx + dy*dy);

            double p = obs->value()*R2G + r(i+1)/10000;
            while (p >= 400) p -= 400;
            while (p  <  0 ) p += 400;
            double q = A.adj;
            while (q >= 400) q -= 400;
            while (q  <  0 ) q += 400;

            dfobs = D*std::sin((p-q)*G2R);
            dfstd = D*std::sin((lnet->stdev_obs(i+1) - A.stdev)*CC2R);
         }
        else
          {
            dfobs = obs->value() + r(i+1)/1000 - A.adj;
            dfstd = lnet->stdev_obs(i+1) - A.stdev;
          }
        if (std::abs(dfobs) > std::abs(maxdiffobs)) maxdiffobs = dfobs;
        if (std::abs(dfstd) > std::abs(mdfobstd)) mdfobstd = dfstd;
     }

    std::cout << "         adjusted observations"
              << std::scientific << std::setprecision(3) << std::setw(11)
              << maxdiffobs << " [m]";
    if (std::abs(maxdiffobs) >= 1e-5)
      {
        std::cout << " FAILED";
        failed = 1;
      }
    std::cout << "\n";

    mdfobstd *= 1e-3;  // millimeters to meters
    std::cout << "         adj obs standard devs"
              << std::scientific << std::setprecision(3) << std::setw(11)
              << mdfobstd << " [m]";
    if (std::abs(mdfobstd) >= 1e-5)
      {
        std::cout << " FAILED";
        failed = 1;
      }
    std::cout << "\n";

  } // adjusted observations

  return failed;
}

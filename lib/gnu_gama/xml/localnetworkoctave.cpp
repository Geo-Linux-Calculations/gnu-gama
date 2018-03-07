/*
  GNU Gama C++ library
  Copyright (C) 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <vector>
#include <iostream>
#include <iomanip>
#include <gnu_gama/xml/localnetworkoctave.h>

using namespace std;
using GNU_gama::LocalNetworkOctave;
using GNU_gama::local::LocalPoint;


void LocalNetworkOctave::write(std::ostream& out) const
{
  out <<
    "% gama-local : simplified adjustement results for GNU Octave\n"
    "% version 1.00\n"
    "%\n\n";


  /* Adjusted points, coordinates, indexes and covariances */

  out <<
    "%  Addjusted points are stored in four matrix objects\n"
    "%\n"
    "%  * Points   points ids\n"
    "%  * Indexes  indexes of adjusted covatiances\n"
    "%  * XZY      ajusted coordinates (zero if not available)\n"
    "%  * Cov      covariance matrix of adjusted coordinates\n"
    "%\n\n";

  out << "Points = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          out << "   '" << (*i).first << "'\n";
        }
    }
  out << "];\n\n";

  std::vector<Index> ind(netinfo->sum_unknowns() + 1);
  Index n = 1;
  out << "Indexes = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          out << "   " << setw(4);
          if (Index i = p.index_x()) {
            ind[n] = i;
            out << n++ << " ";
          }
          else {
            out << 0 << " ";
          }
	  out << setw(4);
          if (Index i = p.index_y()) {
            ind[n] = i;
            out << n++ << " ";
          }
          else {
            out << 0 << " ";
          }
	  out << setw(4);
          if (Index i = p.index_z()) {
            ind[n] = i;
            out << n++ << "\n";
          }
          else {
            out << 0 << "\n";
          }
        }
    }
  out << "];\n\n";


  out << "XYZ = [\n";
  const GNU_gama::local::Vec& X = netinfo->solve();
  const int y_sign = netinfo->y_sign();
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          out << "   " << setprecision(6) << fixed << setw(17);
          if (p.index_x()) {
            out << (p.x() + X(p.index_x())/1000) << " ";
          }
          else {
            out << 0 << " ";
          }
          out << " " << setprecision(6) << fixed << setw(17);
          if (p.index_y()) {
            out << (p.y() + X(p.index_y())/1000)*y_sign << " ";
          }
          else {
            out << 0 << " ";
          }
          out << " " << setprecision(6) << fixed << setw(12);
          if (p.index_z()) {
            out << (p.z() + X(p.index_z())/1000) << "\n";
          }
          else {
            out << 0 << "\n";
          }
        }

    }
  out << "];\n\n";


  const double m2 = netinfo->m_0() * netinfo->m_0();
  out << "Cov = [\n";
  for (Index k=0, i=1; i<n; i++, k=0)
    {
      for (Index j=1; j<n; j++)
        {
          out << " " << setprecision(7) << scientific << setw(14);
          out << m2*netinfo->qxx(ind[i], ind[j]);
          if (++k%5 == 0) out << " ...\n";
        };
      out << ";\n";
    }
  out << "];\n\n";


  /* Fixed Points */

  out <<
    "% Fixed points are store in a cell array of the size n x 2\n"
    "%\n"
    "% The first column contains points ids, the second row vectors of\n"
    "% coordinates [x y z], [x y] or [z]\n"
    "%\n"
    "% Example: [id xyz] = FixedPoints{1,:}\n"
    "%\n\n";

  out << "FixedPoints = {\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.fixed_xy() || p.fixed_z())
        {
          out << "   '" << (*i).first << "'  [";
          if (p.fixed_xy())
            {
              out << setprecision(6) << fixed << setw(17)
                  << p.x() << "  " << p.y();
            }
          if (p.fixed_z())
            {
              out << setprecision(6) << fixed << setw(12)
                  << p.z();
            }
          out << " ]\n";
        }
    }
  out << "};\n\n";
}

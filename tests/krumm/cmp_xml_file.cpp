/* Krumm2gama-local -- conversion from F. Krumm format to XML gama-local
   Copyright (C) 2022 Ales Cepek <cepek@gnu.org>

   This file is part of Krumm2gama-local.

   Krumm2gama-local is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Krumm2gama-local is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Krumm2gama-local. If not, see <https://www.gnu.org/licenses/>.
*/

#include <gnu_gama/xml/localnetwork_adjustment_results.h>
#include <gnu_gama/xml/localnetwork_adjustment_results_data.h>
#include "cmp_xml_file.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <array>


int cmp_xml_file(std::istream& xml, std::istream& adj)
{
  auto adjres = std::make_unique<GNU_gama::LocalNetworkAdjustmentResults>();
  adjres->read_xml(xml);

  const auto& adjlist = adjres->adjusted_points;
  std::string::size_type max_id = 0;
  for (const auto& p : adjlist) max_id = std::max(max_id, p.id.length());

  std::cerr
      << std::string(max_id, ' ')
      << "  " << std::setw(8) << std::string("X_adj")
      << "  " << std::setw(8) << std::string("X_corr")
      << "  " << std::setw(8) << std::string("X_stdev")
      << "  " << std::setw(8) << std::string("Y_adj")
      << "  " << std::setw(8) << std::string("Y_corr")
      << "  " << std::setw(8) << std::string("Y_stdev")
      << "  " << std::setw(8) << std::string("Z_adj")
      << "  " << std::setw(8) << std::string("Z_corr")
      << "  " << std::setw(8) << std::string("Z_stdev")
      << "  " << std::setw(8) << std::string("3D_stdev")
      << "\n";

  const auto& covmat  = adjres->cov;
  std::unordered_map<std::string, int> adjmap;
  for (int i=0; i<adjlist.size(); i++) adjmap[adjlist[i].id] = i;

  // symbolic names:
  // xyz, a - adjustment, c - correction, s - standard deviation
  // z3 = sqrt(xs^2 + ys^2 + zs^2), only for 3D networks
  enum { xa, xc, xs, ya, yc, ys, za, zc, zs, z3 };
  // maximal differences
  std::array<double, 10> max = {0,0,0,0,0,0,0,0,0,0};

  std::string line, token;
  while (std::getline(adj, line)) {
      auto comment = line.find('#');
      if (comment != std::string::npos) line.erase(comment);
      std::istringstream istr(line);
      std::vector<std::string> tokens;
      while (istr >> token) tokens.push_back(token);

      switch (tokens.size()) {
        case  0:    // empty line
          continue;
        case  4:    // z (heights only)
        case  8:    // x y
        case 11:    // x y z
          break;
        default:
          return 1; // error
        }

      // point differences
      std::array<double, 10> pnt = {0,0,0,0,0,0,0,0,0,0};

      std::string id = tokens[0];
      auto n = adjmap[id];
      auto point = adjlist[n];

      // const auto& apr = adjres->approximate_points;
      const auto& adj = adjres->adjusted_points;

      // Differences in corrections cannot be compared with
      // values given by F. Krumm  because if gama-local repeats
      // adjustment with improved approximate xyz values.
      // Original xyz_0 are not stored in AdjustmentResults object,
      // this can be considered to be a bug.
      int indz0 {0};
      if (point.hxy)
        {
          int indx = adj[n].indx;
          int indy = adj[n].indy;
          double ptxs = std::sqrt(adjres->cov(indx,indx))/1000;
          double ptys = std::sqrt(adjres->cov(indy,indy))/1000;

          pnt[xa] = std::stod(tokens[1])       - point.x;
          // pnt[xc] = std::stod(tokens[2])/1000  - adj[n].x + apr[n].x;
          pnt[xs] = std::stod(tokens[3])/100   - ptxs;
          pnt[ya] = std::stod(tokens[4])       - point.y;
          // pnt[yc] = std::stod(tokens[5])/1000  - adj[n].y + apr[n].y;
          pnt[ys] = std::stod(tokens[6])/100   - ptys;

          indz0 = 6;
        }
      if (point.hz)
        {
          int indz = adj[n].indz;
          double ptzs = std::sqrt(adjres->cov(indz,indz))/1000;

          pnt[za] = std::stod(tokens[indz0 + 1])       - point.z;
          // pnt[zc] = std::stod(tokens[indz0 + 2])/1000  - adj[n].z + apr[n].z;
          /* sigma for coordinates are given in cm, for heights in mm */
          pnt[zs] = std::stod(tokens[indz0 + 3])/(indz0 ? 100 : 1000) - ptzs;
          // pnt[z3] = std::stod(tokens[indz0 + 4])/100
          //     - std::sqrt(ptxs*ptxs + ptys*ptys + ptzs*ptzs);
        }

      std::cerr.precision(3);
      std::cerr << std::setw(max_id) << id;
      for (int i=0; i<10; i++)
        {
          std::cerr << " " << std::setw(9) << pnt[i];
        }
      std::cerr << std::endl;

      for (int i=0; i<10; i++)
        {
          if (std::abs(pnt[i]) > std::abs(max[i])) max[i] = pnt[i];
        }
    }

    double maxdif = 0;
    auto cerr_prec = std::cerr.precision();
    std::cerr.precision(3);
    std::cerr << "\n" << std::string(max_id, ' ');
    for (int i=0; i<10; i++)
      {
        std::cerr << " " << std::setw(9) << max[i];
        //maxdif = std::max(maxdif, std::abs(max[i]))*std::si;
        if (std::abs(max[i]) > std::abs(maxdif)) maxdif = max[i];
      }
    std::cerr << "\n"
     << std::string(max_id, ' ') << " "
     << std::setw(9) << maxdif << " max\n";
    std::cerr.precision(cerr_prec);

    return std::abs(maxdif) > 1e-4;
}

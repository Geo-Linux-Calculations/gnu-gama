/*
  GNU Gama C++ library
  Copyright (C) 2018, 2020  Ales Cepek <cepek@gnu.org>

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
#include <gnu_gama/version.h>

using namespace std;
using GNU_gama::LocalNetworkOctave;
using GNU_gama::local::LocalPoint;
using GNU_gama::local::PointData;

void LocalNetworkOctave::write(std::ostream& out) const
{
  auto exit = [this]() { return (netinfo->getAdjInputData() == nullptr) ||
                                (netinfo->algorithm() != "envelope"); };

  out << "% gama-local adjustment results for GNU Octave (.m script)\n"
      << "%\n"
      << "% version    1.02\n"
      << "% gama-local " << GNU_gama_version() << "\n"
      << "%\n"
      << "% https://www.gnu.org/software/octave/\n"
      << "% https://www.gnu.org/software/gama/\n"
      << "%\n\n\n";


  /* General parameters */
  /**********************/

  out <<
    "% General adjustment parameters\n"
    "%\n\n";

  {
    int a_xyz = 0, a_xy = 0, a_z = 0;      // adjusted
    int c_xyz = 0, c_xy = 0, c_z = 0;      // constrained
    int f_xyz = 0, f_xy = 0, f_z = 0;      // fixed

    for (PointData::const_iterator i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
      {
        const LocalPoint& p = (*i).second;
        if (p.active())
          {
            if (p.free_xy() && p.free_z()) a_xyz++;
            else if (p.free_xy()) a_xy++;
            else if (p.free_z())  a_z++;

            if (p.constrained_xy() && p.constrained_z()) c_xyz++;
            else if (p.constrained_xy()) c_xy++;
            else if (p.constrained_z())  c_z++;

            if (p.fixed_xy() && p.fixed_z()) f_xyz++;
            else if (p.fixed_xy()) f_xy++;
            else if (p.fixed_z())  f_z++;
          }
      }

  out << "unknowns        = " << netinfo->sum_unknowns() << ";\n"
      << "observations    = " << netinfo->sum_observations() << ";\n\n"
      << "adjusted_xyz    = " << a_xyz << ";\n"
      << "adjusted_xy     = " << a_xy  << ";\n"
      << "adjusted_z      = " << a_z   << ";\n"
      << "constrained_xyz = " << c_xyz << ";\n"
      << "constrained_xy  = " << c_xy  << ";\n"
      << "constrained_z   = " << c_z   << ";\n"
      << "fixed_xyz       = " << f_xyz << ";\n"
      << "fixed_xy        = " << f_xy  << ";\n"
      << "fixed_z         = " << f_z   << ";\n\n"
      << "network_defect  = " << netinfo->null_space() << ";\n\n"
      << "m_0_apriori     = " << netinfo->apriori_m_0() << ";\n"
      << "m_0_aposteriori = " << netinfo->m_0() << ";\n"
      << "sum_of_squares  = " << netinfo->trans_VWV() << ";\n"
      << "\n\n";
  }

  /* Fixed Points */
  /****************/

  out <<
    "% Fixed points' ids are stored in a cell array of the size n x 1\n"
    "%\n"
    "% Corresponding coordinates are stored in the matrix FixedXYZ, where\n"
    "% first two columns indicate if xy and/or z are available (1 or 0),\n"
    "% the following three columns contain x, y and z (or zeros if not\n"
    "% available)\n"
    "%\n\n";

  out << "FixedPoints = {\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.fixed_xy() || p.fixed_z())
        {
          out << "   '" << (*i).first << "'\n";
        }
    }
  out << "};\n\n";

  out << "FixedXYZ = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (!p.fixed_xy() && !p.fixed_z()) continue;

      double x{0}, y{0}, z{0};

      if (p.fixed_xy())
        {
          x = p.x();
          y = p.y();
        }

      if (p.fixed_z())
        {
          z = p.z();
        }

      out << "   " << (p.fixed_xy() ? 1:0) << " " << (p.fixed_z() ? 1:0)
          << "   " << setprecision(6) << fixed << setw(14)
          << x << "  " << setw(14) << y << " " << setw(12) << z << endl;
    }
  out << "];\n\n";

  /* Adjusted points, coordinates, indexes and covariances */
  /*********************************************************/

  out <<
    "%  Adjustment information is stored in the following matrix objects\n"
    "%\n"
    "%  Points       points' ids (cell array of the size n x 1)\n"
    "%  Indexes      indexes of adjusted coordinates\n"
    "%  Constrained  indexes of constrained coordinates (a subset of Indexes)\n"
    "%  XYZ_0        approximate coordinates (zero if not available)\n"
    "%  XYZ          ajusted coordinates (zero if not available)\n"
    "%  C_xx         covariance matrix of adjusted coordinates\n"
    "%  A            project equation matrix, Ax = b\n"
    "%  b            rhs of ptoject equations\n"
    "%  C_ll         covatiance matrix of observations\n"
    "%  P            weight matrix, P = inv(C_ll)\n"
    "%  H            matrix of constraints equations (Octave solution)\n"
    "%  xyzdiff_mm   size of vector of differences between coordinates\n"
    "%               from Gama and Octave adjustments (if available)\n"
    "%\n\n";

  out << "Points = {\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          out << "   '" << (*i).first << "'\n";
        }
    }
  out << "};\n\n";


  std::vector<int> ind(netinfo->sum_unknowns() + 1, 0);
  int indcov = 0;  // used to index covariances of adjusted coordinates Cov = [...]

  out << "Indexes = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          int i = p.index_x();
          int j = p.index_y();
          int k = p.index_z();

          out << "   ";
          out << setw(4) << i << " ";
          out << setw(4) << j << " ";
          out << setw(4) << k << "\n";

          if (i) ind[++indcov] = i;
          if (j) ind[++indcov] = j;
          if (k) ind[++indcov] = k;
        }
    }
  out << "];\n\n";


  bool xy_constr {false}, z_constr {false};

  out << "Constrained = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          int i = p.index_x();
          int j = p.index_y();
          int k = p.index_z();

          if (p.constrained_xy()) xy_constr = true;
          if (p.constrained_z() ) z_constr = true;

          if (!p.constrained_xy()) i = j = 0;
          if (!p.constrained_z() ) k = 0;

          out << "   ";
          out << setw(4) << i << " ";
          out << setw(4) << j << " ";
          out << setw(4) << k << "\n";
        }
    }
  out << "];\n\n";


  out << "XYZ_0 = [\n";
  for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.free_xy() || p.free_z() || p.constrained_xy() || p.constrained_z())
        {
          out << "   " << setprecision(6) << fixed << setw(17);
          if (p.index_x()) {
            out << p.x_0() << " ";
          }
          else {
            out << 0 << " ";
          }
          out << " " << setprecision(6) << fixed << setw(17);
          if (p.index_y()) {
            out << p.y_0() << " ";
          }
          else {
            out << 0 << " ";
          }
          out << " " << setprecision(6) << fixed << setw(12);
          if (p.index_z()) {
            out << p.z_0() << "\n";
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
  out << "C_xx = [\n";
  for (int k=0, i=1; i<=indcov; i++, k=0)
    {
      for (int j=1; j<=indcov; j++)
        {
          out << " " << setprecision(7) << scientific << setw(14);
          out << m2*netinfo->qxx(ind[i], ind[j]);
          if (++k%5 == 0) out << " ...\n";
        };
      out << ";\n";
    }
  out << "];\n\n";




  /* Adjustment input data (if available) */
  /****************************************/

  auto input = netinfo->getAdjInputData();
  if (input != nullptr)
    {
      out.precision(16);
      auto A = input->mat();
      if (A != nullptr && A->check())
        {
          out << "tmp = [   % project matrix nonzero elements (row, col, val)\n";
          for (int row=1; row<=A->rows(); row++)
            {
              auto* value  = A->begin(row);
              auto* endval = A->end(row);
              for (auto* ind=A->ibegin(row); value!=endval; value++, ind++)
                {
                  out << "  " << row << " " << *ind << " " << *value << "\n";
                }
            }
          out << "];\n\n";

          out << "A = sparse(tmp(:,1), tmp(:,2), tmp(:,3));\nclear tmp\n\n";
        }
    }


  if (exit())
    {
      out << "error('full adjustment output is available only "
             "for ''envelope'' algorithm')\n";

      return;
    }


  const BlockDiagonal<>* bd = input->cov();
  if (bd != nullptr)
    {
      out << "tmp = [\n";
      for (int n=0, i=1; i<=bd->blocks(); i++)
        {
          int dim   = bd->dim(i);
          int width = bd->width(i);
          const double* b = bd->begin(i);
          //const double* e = bd->end(i);

          for (int i=1; i<=dim; i++)
            {
              for (int j=i; j<=dim && (j-i)<=width; j++)
                {
                  /*******/ out << n+i << " " << n+j << " " << *b << endl;
                  if (i!=j) out << n+j << " " << n+i << " " << *b << endl;
                  b++;
                }
            }

          n += dim;
        }
      out << "];\n\n";

      out << "C_ll = sparse(tmp(:,1), tmp(:,2), tmp(:,3));\nclear tmp\n\n";
    }



    if (input->rhs().dim() != 0)
      {
        out << "b = [\n";
        const auto* b = input->rhs().begin();
        const auto* e = input->rhs().end();
        while (b != e) out << *b++ << "\n";
        out << "];\n\n";
      }

    out << "P = inv(C_ll);           % weight matrix\n";

    // coordinates xyz can be defined as constrained even in fixed network
    if (netinfo->null_space() && (xy_constr || z_constr))
      {
        int row_x {0}, row_y {0}, row_z {0};
        if (xy_constr)
          {
            row_x = 1;
            row_y = 2;
          }
        if (z_constr)
          {
            row_z = row_y + 1;
          }

        int row_xyz = 0;      // coordinates' row in matrix H
        if ( xy_constr && !z_constr) row_xyz = 3;
        if ( xy_constr &&  z_constr) row_xyz = 4;
        if (!xy_constr &&  z_constr) row_xyz = 1;

        out << "tmp = [\n";
        for (auto i=netinfo->PD.cbegin(); i!=netinfo->PD.cend(); ++i)
          {
            const LocalPoint& p = (*i).second;
            if (p.constrained_xy())
              {
                out << row_x   << " " << p.index_x() << " 1\n";
                out << row_xyz << " " << p.index_x() << " " << -p.y_0() << "\n";

                out << row_y   << " " << p.index_y() << " 1\n";
                out << row_xyz << " " << p.index_y() << " " <<  p.x_0() << "\n";
              }

            if (p.constrained_z())
              {
                out << row_z   << " " << p.index_z() << " 1\n";
                //out << row_xyz << " " << p.index_z() << " " << 0 << "\n";
              }
          }
        out << "];\n\n";

        out << "H_rows = " << row_xyz << ";\n\n";

        out << "H = sparse(tmp(:,1), tmp(:,2), tmp(:,3), H_rows, unknowns);\n";
        out <<"clear tmp;\n\n";

        { /* Special case of fixed point to constrained point fixed bearing
           * --------------------------------------------------------------
           */

          GNU_gama::local::LocalPoint fixed, constrained;
          int fixed_points = 0, constraint_points = 0;
          int index_x = 0, index_y = 0;
          for (auto point : netinfo->PD)
          {
            if (point.second.fixed_xy())
            {
              fixed_points++;
              fixed = point.second;
            }
            if (point.second.constrained_xy())
            {
              constraint_points++;
              constrained = point.second;

              index_x = point.second.index_x();
              index_y = point.second.index_y();
            }
          }

          if (fixed_points == 1 && constraint_points == 1)
          {
            out << "\n% We have to handle the special case of a free"
                << " network with one fixed\n"
                << "% bearing, i.e. the bearing from a fixed point to a"
                << " constrained point\n\n";

            out << "H_rows = H_rows - 2;     % reduce 3 xy equations to 1\n";
            out << "H(1:2,:) = [];\n";
            out << "H(1,:) = zeros(1,unknowns);\n\n";

            double dx = constrained.x() - fixed.x();
            double dy = constrained.y() - fixed.y();
            double d2 = dx*dx + dy*dy;
            out << "H(1," << index_x << ") = " << -dy/d2 << ";\n";
            out << "H(1," << index_y << ") = " <<  dx/d2 << ";\n\n";
          }

        }   /* end of the fixed bearing section */


        out << "if (rank(H) < H_rows)\n"
            << " error('rank(H) is less than H_rows, cannot solve normal equations')\n"
            << " return;\n"
            << "end   % should be endif in Octave\n\n";

        out << "Z = zeros(H_rows, H_rows);\n";
        out << "z = zeros(H_rows, 1);\n";
        out << "N = [A'*P*A H'; H Z];    % normal equations (free network)\n";
        out << "n = [A'*P*b ; z];\n";
        out << "x = inv(N)*n;            % unknowns and Lagrange multipliers\n";
        out << "x = x(1:unknowns);       % reduction to vector of unknowns\n\n";
      }
    else
      {
        out << "N = A'*P*A;              % normal equations (fixed network)\n";
        out << "n = A'*P*b;              % absolute terms\n";
        out << "x = inv(N)*n;            % vector of adjusted unknowns\n\n";
      }


    /* Check adjustment results from netinfo vs. octave */
    /****************************************************/

    {
      out << "% Check adjustment results from netinfo vs. octave\n\n";

      out << "tmp = [\n";
      const GNU_gama::local::Vec& X = netinfo->solve();
      for (int i=1; i<=X.dim(); i++)
        {
          out << "  " << X(i)   /* unknown type 'R' is orientation shift */
              << "  " << (netinfo->unknown_type(i) == 'R' ? 0 : 1)
              << "\n";
        }
      out << "];\n\n";

      out << "tmp = (tmp(:,1) - x).*tmp(:,2)*1000;\n";  // differences in mm
      out << "xyzdiff_mm = norm(tmp); clear tmp;\n\n";
    }

    out << "if (xyzdiff_mm > 1e-3) \n error('xyzdiff_mm > 1e-3')\nend\n\n";


    out << "% Example: Reliability matrix R\n";
    out << "% R = eye(observations) - A*inv(A'*P*A)*A'*P;\n";
    out << "% surf(R);\n\n";
}

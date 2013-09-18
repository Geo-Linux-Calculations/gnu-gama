/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>
                  2013  Ales Cepek <cepek@gnu.org>

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

/** \file outlying_abs_terms.h
 * \brief Function and visitor class for writing outlying absolute terms table
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef GaMa_GaMaProg_Vybocujici_Absolutni_Cleny_h_
#define GaMa_GaMaProg_Vybocujici_Absolutni_Cleny_h_

#include <iomanip>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/pobs/format.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/utf8.h>

namespace GNU_gama { namespace local {

/** \brief Visitor class for writing 'value' column in 'Outlying absolute terms' table.
 *
 * \tparam OutStream output stream type
 */
template <typename OutStream>
class OutlyingAbsoluteTermsVisitor : public AllObservationsVisitor
{
private:
    OutStream& out;
    GNU_gama::local::LocalNetwork* IS;
    static const int width = 12;
    static const int distPrecision = 5;
    static const int angularPrecision = 6;
    static const int coordPrecision = 5;

public:
    /**
     * \param localNetwork pointer to local network
     * \param outStream reference to output stream
     */
    OutlyingAbsoluteTermsVisitor(GNU_gama::local::LocalNetwork* localNetwork, OutStream& outStream)
        : IS(localNetwork), out(outStream)
    {}

    void  visit(Distance* obs)
    {
        out << T_GaMa_distance;
        out.precision(distPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Direction* obs)
    {
        out << T_GaMa_direction;
        out.precision(angularPrecision);
        out.width(width);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }

    void  visit(Angle* obs)
    {
        out << '\n';
        out.width(IS->maxw_obs() + 2 + 2*(IS->maxw_id()));
        out << Utf8::leftPad(obs->fs().str(), IS->maxw_id());
        out << T_GaMa_angle;
        out.precision(angularPrecision);
        out.width(width);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }

    void  visit(H_Diff* obs)
    {
        out << T_GaMa_levell;
        out.precision(distPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(S_Distance* obs)
    {
        out << T_GaMa_s_distance;
        out.precision(distPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Z_Angle* obs)
    {
        out << T_GaMa_z_angle;
        out.precision(angularPrecision);
        out.width(width);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }

    void  visit(X* obs)
    {
        out << T_GaMa_x;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Y* obs)
    {
        out << T_GaMa_y;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Z* obs)
    {
        out << T_GaMa_z;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Xdiff* obs)
    {
        out << T_GaMa_xdiff;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Ydiff* obs)
    {
        out << T_GaMa_ydiff;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Zdiff* obs)
    {
        out << T_GaMa_zdiff;
        out.precision(coordPrecision);
        out.width(width);
        Double m = obs->value();
        out << m << " ";
    }

    void  visit(Azimuth* obs)
    {
        out << T_GaMa_azimuth;
        out.precision(angularPrecision);
        out.width(width);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }
};

/** \brief Writes 'Outlying absolute terms' table.
 *
 */
template <typename OutStream>
void OutlyingAbsoluteTerms(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GNU_gama::local;

  if (!IS->huge_abs_terms()) return;

  out << T_GaMa_abstrm_Review_of_outlying_abs_terms << "\n"
      << underline(T_GaMa_abstrm_Review_of_outlying_abs_terms, '*') << "\n\n";

  out.width(IS->maxw_obs());
  out << "i" << " ";
  out.width(IS->maxw_id());
  out << T_GaMa_standpoint << " ";
  out.width(IS->maxw_id());
  out << T_GaMa_target << T_GaMa_abstrm_header1;
  {  // for ...
    for (int i=0; i < (IS->maxw_obs() + 2*(IS->maxw_id()) + 13); i++)
      out << "=";
  }  // for ...
  out << T_GaMa_abstrm_header2;
  out.flush();

  PointID predcs = "";   // previous standpoint ID

  OutlyingAbsoluteTermsVisitor<OutStream> visitor(IS, out);

  for (int i=1; i<=IS->sum_observations(); i++)
    {
      Observation* pm = IS->ptr_obs(i);
      if (IS->test_abs_term(i))
        {
          out.width(IS->maxw_obs());
          out << i << " ";

          PointID cs = pm->from();
          out.width(IS->maxw_id());
          if (cs != predcs)
            out << Utf8::leftPad(cs.str(), IS->maxw_id());
          else
            out << " ";
          out << " ";

          PointID cc = pm->to();
          out << Utf8::leftPad(cc.str(), IS->maxw_id());
          out.setf(ios_base::fixed, ios_base::floatfield);

          pm->accept(&visitor);

          out << setiosflags(ios_base::scientific) << setprecision(5);
          out << setw(13) << IS->rhs(i);       // 1.1.56 << pm->rhs();
          out << '\n';
          out.flush();
        }
    }

  out << "\n\n";
}

}}

#endif


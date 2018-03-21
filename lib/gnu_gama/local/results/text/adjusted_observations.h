/*
    Geodesy and Mapping C++ library (GNU GaMa)
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

/** \file adjusted_observations.h
 * \brief Function for writing adjusted observations
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef GaMa_GaMaProg_Vyrovnana_Pozorovani_h_
#define GaMa_GaMaProg_Vyrovnana_Pozorovani_h_

#include <gnu_gama/local/network.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/utf8.h>

namespace GNU_gama { namespace local {


/** \brief Writes part of row in table 'Adjusted observations'.
 *
 * \warning Index of observation has to be specified before each visit.
 * \sa setObservationIndex()
 */
template <typename OutStream>
class AdjustedObservationsTextVisitor : public AllObservationsVisitor
{
private:
    OutStream& out;
    GNU_gama::local::LocalNetwork* IS;
    const int maxval;
    const GNU_gama::local::Vec& v; ///< residuals
    GNU_gama::Index i;
    const int y_sign;

    static const int distPrecision = 5;
    static const int angularPrecision = 6;
    static const int coordPrecision = 5;

public:
    /**
     * \param localNetwork pointer to local network
     * \param outStream reference to output stream
     * \param residuals vector of residuals
     * \param ySign sing of y coordinates
     * \param columnWidth width of column with values
     */
    AdjustedObservationsTextVisitor(GNU_gama::local::LocalNetwork* localNetwork,
                                    OutStream& outStream,
                                    const GNU_gama::local::Vec& residuals,
                                    int ySign,
                                    int columnWidth)
      : out(outStream), IS(localNetwork), maxval(columnWidth),
        v(residuals), y_sign(ySign)
    {}

    /** \brief Sets index of observation which will be used in the next visit. */
    void setObservationIndex(GNU_gama::Index index) { i = index; }

    void visit(Distance* obs)
    {
        out << T_GaMa_distance;
        out.precision(distPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }

    void visit(Direction* obs)
    {
        out << T_GaMa_direction;
        out.precision(angularPrecision);
        out.width(maxval);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
        out.width(maxval);
        m += v(i)/10000;
        if (m < 0) m += 400;
        if (m >= 400) m -= 400;
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }

    void visit(Angle* obs)
    {
        out << '\n';
        const int w = IS->maxw_obs() + 2 + 2*(IS->maxw_id());
        out << Utf8::leftPad(obs->fs().str(), w);
        out << T_GaMa_angle;
        out.precision(angularPrecision);
        out.width(maxval);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
        out.width(maxval);
        m += v(i)/10000;
        if (m < 0) m += 400;
        if (m >= 400) m -= 400;
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }

    void visit(H_Diff* obs)
    {
        out << T_GaMa_levell;
        out.precision(distPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }

    void visit(S_Distance* obs)
    {
        out << T_GaMa_s_distance;
        out.precision(distPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }

    void visit(Z_Angle* obs)
    {
        out << T_GaMa_z_angle;
        out.precision(angularPrecision);
        out.width(maxval);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
        out.width(maxval);
        m += v(i)/10000;
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }
    void visit(X* obs)
    {
        out << T_GaMa_x;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }
    void visit(Y* obs)
    {
        out << T_GaMa_y;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << y_sign*m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << y_sign*m << " ";
    }

    void visit(Z* obs)
    {
        out << T_GaMa_z;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }
    void visit(Xdiff* obs)
    {
        out << T_GaMa_xdiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }

    void visit(Ydiff* obs)
    {
        out << T_GaMa_ydiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << y_sign*m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << y_sign*m << " ";
    }

    void visit(Zdiff* obs)
    {
        out << T_GaMa_zdiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
    }

    void visit(Azimuth* obs)
    {
        out << T_GaMa_azimuth;
        out.precision(angularPrecision);
        out.width(maxval);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
        out.width(maxval);
        m += v(i)/10000;
        if (m < 0) m += 400;
        if (m >= 400) m -= 400;
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
    }
};


template <typename OutStream>
void AdjustedObservations(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;
   // using GNU_gama::local::Double;

   const double y_sign = IS->y_sign();
   const Vec&   v      = IS->residuals();
   const int    pocmer = IS->sum_observations();
   const double scale  = IS->gons() ? 1.0 : 0.324;

   out << T_GaMa_adjobs_Adjusted_observations << "\n"
       << underline(T_GaMa_adjobs_Adjusted_observations, '*') << "\n\n";

   int minval = 12;
   int maxval = minval;   // maximal value field width (coordinates!)
   {
     for (int i=1; i<=pocmer; i++)
       {
         const Observation* pm = IS->ptr_obs(i);
         int z = 0;
         double d = pm->value();
         if (d < 0)
           {
             z = 1;
             d = -d;
           }
         if (d < 1e5) continue;
         z += 6;   // ... decimal point plus 5 digits
         do {
           z++;
           d /= 10;
         } while (d >= 1);
         if (z > maxval) maxval = z;
       }
   }

   Double kki = IS->conf_int_coef();
   out.width(IS->maxw_obs());
   out << "i" << " ";
   out.width(IS->maxw_id());
   out << T_GaMa_standpoint << " ";
   out.width(IS->maxw_id());
   out << T_GaMa_target << "       ";
   out.width(maxval);
   out << T_GaMa_adjobs_observed << " ";
   out.width(maxval);
   out << T_GaMa_adjobs_adjusted << T_GaMa_adjobs_header1;
   {   // for ...
     int kk = 13 + maxval-minval;
     for (int i=0; i < (IS->maxw_obs()+2*(IS->maxw_id())+kk); i++) out << "=";
   }   // for ...
   out << T_GaMa_adjobs_value;
   {
     for (int i=minval; i<maxval; i++) out << "=";
   }
   if (IS->gons())
     out << "==== [m|g] ====== [mm|cc] ==\n\n";
   else
     out << "==== [m|d] ====== [mm|ss] ==\n\n";
   out.flush();

   AdjustedObservationsTextVisitor<OutStream> textVisitor(IS, out, v, y_sign, maxval);

   PointID predcs = "";   // provious standpoint ID
   for (int i=1; i<=pocmer; i++)
   {
      Observation* pm = IS->ptr_obs(i);
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

      textVisitor.setObservationIndex(i);
      pm->accept(&textVisitor);

      out.precision(1);
      out.width(7);
      Double ml = IS->stdev_obs(i);
      if (dynamic_cast<Direction*>(pm))
        ml *= scale;
      else if (dynamic_cast<Angle*>(pm))
        ml *= scale;
      else if (dynamic_cast<Z_Angle*>(pm))
        ml *= scale;

      out << ml << " ";
      out.width(7);
      out << ml*kki;

      out << '\n';
      out.flush();

      predcs = cs;  // previous standpoint ID
   }
   out << "\n\n";
   out.flush();
}

}}

#endif

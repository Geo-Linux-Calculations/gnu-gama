/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: test_linearization.h,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */

#ifndef GaMa_GaMaProg_Prehled_Test_Chyby_z_Linearizace_h_
#define GaMa_GaMaProg_Prehled_Test_Chyby_z_Linearizace_h_

#include <gnu_gama/gon2deg.h>
#include <gamalib/local/gamadata.h>
#include <gamalib/local/network.h>
#include <gamalib/local/pobs/bearing.h>
#include <gnu_gama/statan.h>
#include <cmath>
#include <algorithm>
#include <typeinfo>


namespace GaMaLib {

template <typename OutStream> 
bool 
TestLinearization(GaMaLib::LocalNetwork* IS, OutStream& out,
                  double max_pyx = 0.1500, // suspicious coorection in meters
                  double max_dif = 0.0005  // max. positional difference in mm
                  )
{

  using namespace std;
  using namespace GaMaLib;
  using GaMaLib::Double;
  
  bool test  = false;     // result of bad linearization test
  
  // const char hlavicka[] = "Bad linearization test\n"
  //                        "***********************\n";
  
  /****************************  test removed  **************************
  * // repeat adjustment if suspiciously large corrections of coordinates
  * // ==================================================================
  * {
  *   const int N = IS->sum_unknowns();
  * 
  *   const Vec& x = IS->solve();
  *   Double pyx = 0;
  *   for (int i=1; i<=N; i++)
  *     if (IS->typ_neznama(i) == 'X')
  *       {
  *         const PointID cb = IS->bod_neznama(i);
  *         const LocalPoint&  b = IS->PD[cb];
  *         if (!b.constrained_xy())
  *           {
  *             Double dx = x(i++)/1000;
  *             Double dy = x(i  )/1000;
  *             pyx = max(pyx, dy*dy + dx*dx);
  *           }
  *       }
  *   pyx = sqrt(pyx);
  *   if (pyx >= max_pyx)
  *     {
  *       if (!test) out << hlavicka;
  *       test  = true;
  *       out.setf(ios_base::fixed, ios_base::floatfield);
  *       out.precision(3);
  *       out << "\nMaximal correction of point " << pyx
  *           << " is greater then " << max_pyx << " [m]\n";
  *     }
  * }
  ******************************************************************/
  
  // difference in adjusted observations computed from residuals and
  // from adjusted coordinates
  // ===============================================================
  {
    const int M = IS->sum_observations();

    Vec dif_m(M);   // difference in computation of adjusted observation
    Vec dif_p(M);   //               corresponds to positional shift
    
    const Vec& v = IS->residuals();
    const Vec& x = IS->solve();

    for (int i=1; i<=M; i++) 
      {
        dif_m(i) = 0;
        dif_p(i) = 0;
        // if (IS->obs_control(i) < 0.1) continue;  // uncontrolled observation
        // if (IS->obs_control(i) < 5.0) continue;  // weakly controlled obs.

        Double  mer, pol;

        Observation* pm = IS->ptr_obs(i);
        if (dynamic_cast<const H_Diff*>(pm)) 
          {
            dif_m(i) = dif_p(i) = 0;
            continue;
          }
        if (dynamic_cast<const Coordinates*>(pm->ptr_cluster())) 
          {
            dif_m(i) = dif_p(i) = 0;
            continue;
          }

        const LocalPoint& stan = IS->PD[pm->from()];
        const LocalPoint& cil  = IS->PD[pm->to() ];
        Double sy = stan.y();
        Double sx = stan.x();
        if (stan.free_xy())
          {
            sy += x(stan.index_y())/1000;
            sx += x(stan.index_x())/1000;
          }
        Double cy = cil .y();
        Double cx = cil .x();
        if (cil.free_xy())
          {
            cy += x(cil .index_y())/1000;
            cx += x(cil .index_x())/1000; 
          }
        Double ds, dd;
        GaMaLib::bearing_distance(sy, sx, cy, cx, ds, dd);
        
        if (Distance* d = dynamic_cast<Distance*>(pm))
          {
            mer  = d->value() + v(i)/1000;
            mer -= dd;
            mer *= 1000;
            pol  = mer;
          }
        else if (Direction* s = dynamic_cast<Direction*>(pm))
          {
            Double orp = s->orientation();
            mer = s->value() + v(i)*CC2R + orp + x(s->index_orientation())*CC2R;
            mer -= ds;
            while (mer >  M_PI) mer -= 2*M_PI;
            while (mer < -M_PI) mer += 2*M_PI;
            pol  = mer*dd*1000;
            mer *= R2CC;
          }
        else if (Angle* u = dynamic_cast<Angle*>(pm))
          {
            const LocalPoint& cil2 = IS->PD[u->fs() ];
            Double cy2 = cil2.y() + x(cil2.index_y())/1000;
            Double cx2 = cil2.x() + x(cil2.index_x())/1000;
            Double ds2, dd2;
            GaMaLib::bearing_distance(sy, sx, cy2, cx2, ds2, dd2);
            mer = u->value() + v(i)*CC2R - ds2 + ds;         
            while (mer >  M_PI) mer -= 2*M_PI;
            while (mer < -M_PI) mer += 2*M_PI;
            pol  = mer*max(dd,dd2)*1000;
            mer *= R2CC;
          }
        else
          {
            mer = pol = 0; 
          }
        
        dif_m(i) = mer;
        dif_p(i) = pol;                                    
      }
    
    
    Double max_pol = 0;
    {
      for (Vec::iterator i=dif_p.begin(); i != dif_p.end(); ++i)
        if (fabs(*i) > max_pol)
          max_pol = fabs(*i);
    }
    if (max_pol >= max_dif)
      {
        // print header
        // ------------
        {
          // if (!test) out << hlavicka;
          if (!test)
            out << T_GaMa_tstlin_Test_of_linearization_error << "\n"
                << underline(T_GaMa_tstlin_Test_of_linearization_error, '*') 
                << "\n";
          test = true;
          
          out << "\n" 
               << T_GaMa_tstlin_Differences
              << "\n"
              << underline(T_GaMa_tstlin_Differences, '*')
              << "\n\n"; 
          
          out.width(IS->maxw_obs());
          out << "i" << " ";
          out.width(IS->maxw_id());
          out << T_GaMa_standpoint << " ";
          out.width(IS->maxw_id());
          out << T_GaMa_target << "      ";
          out << T_GaMa_tstlin_obs_r_diff << "\n";
          
          for (int i=0; i<(IS->maxw_obs()+2*IS->maxw_id()+8); i++) out << '=';
          out << T_GaMa_tstlin_header_value; 
          if (IS->gons())
            out << "= [mm|cc] == [cc] == [mm] =\n\n";
          else
            out << "= [mm|ss] == [ss] == [mm] =\n\n";
          out.flush();
        }
        
        // print table
        // -----------
        
        PointID predcs;   // previous standpoint ID
        
        for (int i=1; i<=M; i++)
          {
            if (fabs(dif_p(i)) < max_dif) continue;

            Observation* pm = IS->ptr_obs(i);
            out.width(IS->maxw_obs());
            out << i << " ";
            PointID cs = pm->from();
            out.width(IS->maxw_id());
            if (cs != predcs)
              out << cs.c_str();
            else
              out << " ";
            out << " ";
            PointID cc = pm->to();
            out.width(IS->maxw_id());
            out << cc.c_str();
            out.setf(ios_base::fixed, ios_base::floatfield);
            
            bool dms = false;
            Double mer;
            if (Distance* d = dynamic_cast<Distance*>(pm))
              {
                out << T_GaMa_distance;
                mer = d->value();
                out.precision(5);
              }
            else if (Direction* s = dynamic_cast<Direction*>(pm))
              {
                dms = IS->degrees();
                out << T_GaMa_direction;
                mer = (s->value())*R2G;
                out.precision(6);
              }
            else if (Angle* u = dynamic_cast<Angle*>(pm))
              {
                dms = IS->degrees();
                out << '\n';
                out.width(IS->maxw_obs() + 2 + 2*(IS->maxw_id()));
                out << (u->fs()).c_str();
                out << T_GaMa_angle;
                mer = (u->value())*R2G;
                out.precision(6);
              }
            
            if (!dms)
              {
                out.width(12);
                out << mer << ' ';
              }
            else
              {
                out << GNU_gama::gon2deg(mer, 1, 1) << ' ';
              }
            
            out.precision(3);
            out.width(9);
            if (!dms)
              out << v(i) << ' ';
            else
              out << v(i)*0.324 << ' ';
              
            if (typeid(*pm) == typeid(Distance))
              {
                out << "         ";
              }
            else
              {
                out.precision(3);
                out.width(8);
                if (!dms)
                  out << dif_m(i) << ' ';
                else
                  out << dif_m(i)*0.324 << ' ';
              }            
            
            out.precision(3);
            out.width(7);
            out << dif_p(i);

            out << '\n';
            out.flush();
            
            predcs = cs;  // previous standpoint ID
          }
      }     
  }
  
  if (test && !(IS->update_constrained_coordinates()))
    { 
      out << "\n\n";
      out.flush();
      

      // if all adjusted points are constrained, adjustment is never
      // repeated (unless explicitly asked for)
      // ------------------------------------------------------
      
      test = false;
      for (PointData::const_iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
        {
          const LocalPoint& b = (*i).second;
          if (b.free_xy() && !b.constrained_xy())
            {
              test = true;
              break;
            }
        }
    }
  
  return test;
}

}

#endif





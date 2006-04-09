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
 *  $Id: residuals_observations.h,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */


#ifndef GaMa_GaMaProg_Opravy_Pozorovani__h_
#define GaMa_GaMaProg_Opravy_Pozorovani__h_

#include <gamalib/local/network.h>
#include <gnu_gama/statan.h>
#include <algorithm>
#include <typeinfo>

static float ResidualsObservations_N01(float x)   // local helper function
{
   double D, f;
   GNU_gama::NormalDistribution(double(x), D, f);
   return D;
}


/* *******************************************************************
 * local helper class StOpSort for sorting outlying observations (sort
 * by "studentized" residuals)
 * ******************************************************************* */

class StOpSort {

  GaMaLib::LocalNetwork* IS;

public:

  StOpSort(GaMaLib::LocalNetwork* is) : IS(is) {}
  bool operator()(int a, int b) 
    {
      using namespace std;
      GaMaLib::Double sa = fabs(IS->studentized_residual(a));
      GaMaLib::Double sb = fabs(IS->studentized_residual(b));
      return sa > sb; 
    } 
};


namespace GaMaLib {

template <typename OutStream>
void ResidualsObservations(GaMaLib::LocalNetwork* IS, OutStream& out)
{
  if(IS->degrees_of_freedom() <= 1) return; 

  using namespace std;
  using namespace GaMaLib;
  using GaMaLib::Double;

  const Vec&    v      = IS->residuals();
  const int     pocmer = IS->sum_observations();
  const double  scale  = IS->gons() ? 1.0 : 0.324;

  vector<int> odlehla;
  
  Double kki = IS->conf_int_coef();
  int imax = 1;         // index of maximal studentized residual
  {
    using namespace std;
    Double maxno = 0;
    for (int i=1; i<=pocmer; i++)
      {
        if (IS->obs_control(i) < 0.1) continue;

        Double no = fabs(IS->studentized_residual(i));
        if (no > maxno) {
          maxno = no;
          imax = i;
        }
        if (no > kki) odlehla.push_back(i);
      }
    if (odlehla.size() > 0)
      sort(odlehla.begin(), odlehla.end(), StOpSort(IS));
  }
  
  /* *****************************************************************
   * Review of residuals is printed twice. Firstly all observations
   * and then only outlying (if any are apresent)
   * ***************************************************************** */
  int max_pruchod = odlehla.size()==0 ? 1 : 2;
  for (int pruchod=1; pruchod<=max_pruchod; pruchod++)
    {
      
      if (pruchod == 1)
        out << T_GaMa_resobs_Review_of_residuals_analysis_obs << "\n"
            << underline(T_GaMa_resobs_Review_of_residuals_analysis_obs, '*') 
            << "\n\n";
      else
        out << "\n\n"
            << T_GaMa_resobs_Outlying_observations << "\n"
            << underline(T_GaMa_resobs_Outlying_observations, '*') << "\n\n";
      
      out.width(IS->maxw_obs());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_standpoint << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_target 
          << T_GaMa_resobs_header1;
      {   // for ...
        for (int i=0; i < (IS->maxw_obs() + 2*(IS->maxw_id()) + 10); i++) 
          out << "=";
      }   // for ...
      // removed in 1.7.09 : out << T_GaMa_resobs_header2;
      if (IS->gons())
        out << "======== [mm|cc] =========== [mm|cc] ===\n\n";
      else
        out << "======== [mm|ss] =========== [mm|ss] ===\n\n";
      out.flush();
      
      
      PointID predcs = "";   // previous standpoint ID
      int max_ii = pruchod==1 ? pocmer : odlehla.size();
      for (int ii=1; ii<=max_ii; ii++)
        {
          int i = pruchod==1 ? ii : odlehla[ii-1];
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
          
          if (typeid(*pm) == typeid(Distance))
            {
              out << T_GaMa_distance;
            }
          else if (typeid(*pm) == typeid(Direction))
            {
              out << T_GaMa_direction;
            }
          else if (Angle* u = dynamic_cast<Angle*>(pm))
            {
              out << '\n';
              out.width(IS->maxw_obs() + 2 + 2*(IS->maxw_id()));
              out << (u->fs()).c_str();
              out << T_GaMa_angle;
            }
          else if (typeid(*pm) == typeid(S_Distance))
            {
              out << T_GaMa_s_distance;
            }
          else if (typeid(*pm) == typeid(Z_Angle))
            {
              out << T_GaMa_z_angle;
            }
          else if (typeid(*pm) == typeid(X))
            {
              out << T_GaMa_x;
            }
          else if (typeid(*pm) == typeid(Y))
            {
              out << T_GaMa_y;
            }
          else if (typeid(*pm) == typeid(Z))
            {
              out << T_GaMa_z;
            }
          else if (typeid(*pm) == typeid(H_Diff))
            {
              out << T_GaMa_levell;
            }
          else if (typeid(*pm) == typeid(Xdiff))
            {
              out << T_GaMa_xdiff;
            }
          else if (typeid(*pm) == typeid(Ydiff))
            {
              out << T_GaMa_ydiff;
            }
          else if (typeid(*pm) == typeid(Zdiff))
            {
              out << T_GaMa_zdiff;
            }
          else
            {
            throw GaMaLib::Exception("review/residuals_observations.h - "
                                     "unknown observation type");              
            }
          
          Double f  = IS->obs_control(i); 
          out.precision(1);
          out.width(5);
          out << f;
          if (f < 0.1)    out << T_GaMa_resobs_no_control;   // uncontrolled
          else if (f < 5) out << T_GaMa_resobs_weak_control; // weak control
          else            out << "  ";
          out << ' ';
          
          double sc = 1.0;
          if (dynamic_cast<Direction*>(pm))
            sc = scale;
          else if (dynamic_cast<Angle*>(pm))
            sc = scale;
          else if (dynamic_cast<Z_Angle*>(pm))
            sc = scale;         
    
          out.precision(3);
          out.width(9);
          out << v(i)*sc << ' ';
          out.precision(1);
          out.width(4);

          if (f >= 0.1)
            {
              using namespace std;
              Double no = fabs(IS->studentized_residual(i));
              out << no;
              
              if (i == imax)
                {
                  if (no > kki)  out << T_GaMa_resobs_mc_max_critical;
                  else           out << T_GaMa_resobs_mc_max;
                }
              else if (no > kki) out << T_GaMa_resobs_mc_critical;
              else               out << "   ";
            
          
              if ( (pm->ptr_cluster())->covariance_matrix.bandWidth() == 0 && 
                  (f >=5 || (f >= 0.1 && no > kki))) 
                {
                  Double em = v(i) / (IS->wcoef_res(i)*IS->weight_obs(i));
                  out.width(7);
                  out << em*sc;
                  
                  Double ev = em - v(i);
                  out.width(7);
                  out << ev*sc;
                }
            }
          
          out << '\n';
          out.flush();
          
          predcs = cs;  // previous standpoint ID
        }
    }
  
  if (pocmer >= 30)
    {
      using namespace GaMaLib;

      out << "\n\n"
          << T_GaMa_resobs_normality_test << "\n"
          << underline(T_GaMa_resobs_normality_test, '=') << "\n\n";

      { // ****** Kolmogorov-Smirnov

        Vec   pv(pocmer);
        Float pvvar = 0, pvstr = 0, p;
        {
          for (int i=1; i<=pocmer; i++) 
            {
              p = sqrt(IS->weight_obs(i))*v(i);
              pv(i)  = p;
              pvvar += p*p;
              pvstr += p;
            }
        }
        pvstr /= pocmer;        
        pvvar  = pvvar/pocmer - pvstr*pvstr;
        if (pvvar > 0)
          pvvar = sqrt(pvvar);
        else
          pvvar = 0;   // random noise

        if (pvvar) 
          for (int i=1; i<=pocmer; i++) pv(i) = (pv(i) - pvstr) / pvvar;
        
        float  ks, prob;
        GNU_gama::KStest(pv.begin(), 
                         pocmer, ResidualsObservations_N01, ks, prob);
        
        
        out.setf(ios_base::fixed, ios_base::floatfield);
        out.precision(1);
        out.width(5);
        out << "Test Kolmogorov-Smirnov : " << 100*prob << " %\n";

      }
    }

  if (Double cond = IS->cond())
    {
      out.setf(ios_base::scientific, ios_base::floatfield);
      out.precision(1);
      out << "\n"
          << T_GaMa_resobs_condition_number << cond << "\n";
    }
  
  out << "\n\n";
  out.flush();
}

}

#endif
















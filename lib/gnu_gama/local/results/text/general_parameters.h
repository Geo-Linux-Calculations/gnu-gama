/*
  GNU Gama C++ library
  Copyright (C) 1999, 2010  Ales Cepek <cepek@fsv.cvut.cz>
                2011  Vaclav Petras <wenzeslaus@gmail.com>

  This file is part of the GNU Gama C++ library

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

/** \file general_parameters.h
 * \brief Function for writing general network parameters
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef GaMa_GaMaProg_Zakladni_Parametry_h_
#define GaMa_GaMaProg_Zakladni_Parametry_h_

#include <iomanip>
#include <gnu_gama/local/writevisitor.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/pobs/format.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/utf8.h>
#include <gnu_gama/local/results/text/underline.h>
#include <cstring>

namespace GNU_gama { namespace local {

/** \brief
 *
 * \todo Use visitor for counting.
 * \todo Replace \c dynamic_cast by visitor and consider why other observation types are ignored.
 */
template <typename OutStream>
bool GeneralParameters(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GNU_gama::local;

  IS->null_space();   // triggers adjusment; needed for printing removed points

  {
    if (!IS->removed_points.empty())
      {
        out << T_LN_rm_removed_points << "\n"
            << underline(T_LN_rm_removed_points, '*') << "\n\n";

        list<LocalNetwork::rm_points>::iterator c = IS->removed_code.begin();

        for (PointIDList::const_iterator i =IS->removed_points.begin();
             i!=IS->removed_points.end(); ++i, ++c)
          {
            // out << setw(IS->maxw_id()) << (*i).c_str() << "   ";
            out << Utf8::leftPad((*i).str(),IS->maxw_id()) << "   ";
            switch ( *c )
              {
              case LocalNetwork::rm_missing_xyz :
                out << T_LN_rm_missing_xyz;     break;
              case LocalNetwork::rm_missing_xy  :
                out << T_LN_rm_missing_xy;      break;
              case LocalNetwork::rm_missing_z   :
                out << T_LN_rm_missing_z;       break;
              case LocalNetwork::rm_singular_xy :
                out << T_LN_rm_singular_xy;     break;
              case LocalNetwork::rm_singular_z  :
                out << T_LN_rm_singular_z;      break;
              case LocalNetwork::rm_huge_cov_xyz:
                out << T_LN_rm_huge_cov_xyz;    break;
              case LocalNetwork::rm_huge_cov_xy :
                out << T_LN_rm_huge_cov_xy;     break;
              case LocalNetwork::rm_huge_cov_z  :
                out << T_LN_rm_huge_cov_z;      break;
              default:
                ;
              }
            out << '\n';
          }
        out << "\n\n";
      }

  }

  out << T_GaMa_General_solution_parameters << "\n"
      << underline(T_GaMa_General_solution_parameters, '*') << "\n\n";

  {
    // summary of coordinates in adjustment

    int a_xyz = 0, a_xy = 0, a_z = 0;      // adjusted
    int c_xyz = 0, c_xy = 0, c_z = 0;      // constrained
    int f_xyz = 0, f_xy = 0, f_z = 0;      // fixed

    for (PointData::const_iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
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

    int w1 = 0, w_ = 8;
    {
      int n;
      n = Utf8::length(T_GaMa_gpar1_coordinates);            if (n > w1) w1 = n;
      n = Utf8::length(T_GaMa_gpar1_adjusted_coordinates);   if (n > w1) w1 = n;
      n = Utf8::length(T_GaMa_gpar1_constrained_coordinates);if (n > w1) w1 = n;
      n = Utf8::length(T_GaMa_gpar1_fixed_coordinates);      if (n > w1) w1 = n;
      n = Utf8::length(T_GaMa_gpar1_total);                  if (n > w1) w1 = n;
    }

    out.setf(ios_base::left,  ios_base::adjustfield);
    out << setw(w1) << T_GaMa_gpar1_coordinates << " ";
    out.setf(ios_base::right, ios_base::adjustfield);
    out << setw(w_+1) << "xyz"
        << setw(w_-1) << "xy"
        << setw(w_)   << "z"  << "\n\n";

    out.setf(ios_base::left,  ios_base::adjustfield);
    out << Utf8::rightPad(T_GaMa_gpar1_adjusted_coordinates, w1) << ":";
    out.setf(ios_base::right, ios_base::adjustfield);
    out << setw(w_)  << a_xyz
        << setw(w_)  << a_xy
        << setw(w_)  << a_z
        << '\n';
    out.setf(ios_base::left,  ios_base::adjustfield);
    out << Utf8::rightPad(T_GaMa_gpar1_constrained_coordinates, w1) << ":";
    out.setf(ios_base::right, ios_base::adjustfield);
    out << setw(w_)  << c_xyz
        << setw(w_)  << c_xy
        << setw(w_)  << c_z
        << '\n';
    out.setf(ios_base::left,  ios_base::adjustfield);
    out << Utf8::rightPad(T_GaMa_gpar1_fixed_coordinates, w1) << ":";
    out.setf(ios_base::right, ios_base::adjustfield);
    out << setw(w_)  << f_xyz
        << setw(w_)  << f_xy
        << setw(w_)  << f_z
        << '\n';

    for (int ii=0; ii<w1+1+3*w_+1; ii++) out << '-'; out << "\n";

    out.setf(ios_base::left,  ios_base::adjustfield);
    out << Utf8::rightPad(T_GaMa_gpar1_total, w1) << ":";
    out.setf(ios_base::right, ios_base::adjustfield);
    out << setw(w_)  << (a_xyz + f_xyz)
        << setw(w_)  << (a_xy  + f_xy )
        << setw(w_)  << (a_z   + f_z  )
        << "\n\n";
  }

  int w1 = 0;
  {
    int n;
    n = Utf8::length(T_GaMa_gpar1_directions);       if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_angles);           if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_distances);        if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_observed_coords);  if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_leveling_diffs);   if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_z_angles);         if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_s_dists);          if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_obs_total);        if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_equations);        if (n > w1) w1 = n;
    n = Utf8::length(T_GaMa_gpar1_redundancy);       if (n > w1) w1 = n;
  }

  int pocosn = 0;
  for (int i=1; i<=IS->sum_unknowns(); i++)
    if (IS->unknown_type(i) == 'R')
      pocosn++;

  int w2 = 0;
  {
    int n;
    if (pocosn)
      {
        n = Utf8::length(T_GaMa_gpar2_bearings);        if (n > w2) w2 = n;
      }
    n = Utf8::length(T_GaMa_gpar2_number_of_unknowns);  if (n > w2) w2 = n;
    n = Utf8::length(T_GaMa_gpar2_network_defect);      if (n > w2) w2 = n;
  }
  const char* tab_sep = "            ";

  int pocsmer=0, pocuhl=0, pocdel=0, pocsour=0, pocnivp = 0,
      poczeni=0, pocsikm=0;
  {   // for ...
    for (int i=1; i<=IS->sum_observations(); i++)
      // *****************************************************
      if      (dynamic_cast<Distance*  >(IS->ptr_obs(i))) pocdel++;
      else if (dynamic_cast<Direction* >(IS->ptr_obs(i))) pocsmer++;
      else if (dynamic_cast<Angle*     >(IS->ptr_obs(i))) pocuhl++;
      else if (dynamic_cast<X*         >(IS->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<Y*         >(IS->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<Z*         >(IS->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<H_Diff*    >(IS->ptr_obs(i))) pocnivp++;
      else if (dynamic_cast<Z_Angle*   >(IS->ptr_obs(i))) poczeni++;
      else if (dynamic_cast<S_Distance*>(IS->ptr_obs(i))) pocsikm++;
    // *****************************************************
  }   // for ...

  if (pocosn)
    {
      out << set_width(T_GaMa_gpar1_directions, w1) << ":"
          << setw(6) << pocsmer << tab_sep
          << set_width(T_GaMa_gpar2_bearings, w2) << ":"
          << setw(6) << pocosn << '\n';
    }
  if (pocuhl)
    {
      out << set_width(T_GaMa_gpar1_angles, w1) << ":"
          << setw(6) << pocuhl << '\n';
    }
  if (pocdel)
    {
      out << set_width(T_GaMa_gpar1_distances, w1) << ":"
          << setw(6) << pocdel << '\n';
    }
  if (pocsour)
    {
      out << set_width(T_GaMa_gpar1_observed_coords, w1) << ":"
          << setw(6) << pocsour << '\n';
    }
  if (pocnivp && (pocnivp != IS->sum_observations()))
    {
      out << set_width(T_GaMa_gpar1_leveling_diffs, w1) << ":"
          << setw(6) << pocnivp << '\n';
    }
  if (poczeni)
    {
      out << set_width(T_GaMa_gpar1_z_angles, w1) << ":"
          << setw(6) << poczeni << '\n';
    }
  if (pocsikm)
    {
      out << set_width(T_GaMa_gpar1_s_dists, w1) << ":"
          << setw(6) << pocsikm << '\n';
    }
  int types = 0;
  if (pocsmer) types++;
  if (pocdel)  types++;
  if (pocuhl)  types++;
  if (pocsour) types++;
  if (pocnivp) types++;
  if (poczeni) types++;
  if (pocsikm) types++;
  if (types != 1)
    out << set_width(T_GaMa_gpar1_obs_total, w1) << ":"
        << setw(6) << IS->sum_observations() << "\n";
  out << '\n';
  out.flush();

  // *********  here we handle singular free networks  *********

  {
    int d = IS->null_space();
    try {
      if (IS->min_n() < d)
        throw MatVecException(GNU_gama::Exception::BadRegularization,
                              T_GaMa_not_enough_constrained_points);
      IS->trans_VWV();  // now I try to adjust the nework
    }
    catch (const MatVecException& vs)
      {
        if (vs.error != GNU_gama::Exception::BadRegularization) throw;

        out << T_GaMa_Free_network << "\n"
            << underline(T_GaMa_Free_network, '*') << "\n\n";

        out << T_GaMa_Free_network_defect_is << d << ". ";
        out << T_GaMa_Given_network_configuration_can_not_be_adjusted << ".\n";
        if (IS->min_n() < d)
          out <<T_GaMa_not_enough_constrained_points << ".\n";
        out << "\n";

        out << T_GaMa_detected_singular_variables << "\n\n";
        out << T_GaMa_index_type_point << "\n"
            << "-----------------------------\n\n";

        for (int i=1; i<=IS->sum_unknowns(); i++)
          if (IS->lindep(i))
            {
              out << setw(6) << i << "   " << IS->unknown_type(i)
                  <<  "   "  << IS->unknown_pointid(i) << '\n';
            }
        out << "\n";

        return false;  // *********  network can't be adjusted  *********
      }
  }

  out << set_width(T_GaMa_gpar1_equations, w1) << ":"
      << setw(6) << IS->sum_observations()      << tab_sep
      << set_width(T_GaMa_gpar2_number_of_unknowns, w2) << ":"
      << setw(6) << IS->sum_unknowns()
      << '\n'
      << set_width(T_GaMa_gpar1_redundancy, w1) << ":"
      << setw(6) << IS->degrees_of_freedom() << tab_sep
      << set_width(T_GaMa_gpar2_network_defect, w2) << ":"
      << setw(6) << IS->null_space()
      << '\n';
  out.setf(ios_base::fixed, ios_base::floatfield);

  out << "\n"
      << T_GaMa_m0_apriori << ":"
      << setprecision(2) << setw(9) << IS->apriori_m_0() << '\n';
  out << T_GaMa_m0_empirical << ":"
      << setprecision(2) << setw(9)
      << (IS->degrees_of_freedom() > 0 ?
          sqrt(IS->trans_VWV()/IS->degrees_of_freedom()) : 0);
  out.setf(ios_base::scientific, ios_base::floatfield);
  out << "         "
      << "[pvv] : "
      << setprecision(5)<< IS->trans_VWV()
      << '\n';
  out.flush();

  out.setf(ios_base::fixed, ios_base::floatfield);
  out << "\n";
  out << T_GaMa_During_statistical_analysis_we_work << "\n\n"
      << (IS->m_0_aposteriori() ?
          T_GaMa_statan_with_empirical_standard_deviation :
          T_GaMa_statan_with_apriori_standard_deviation);
  out << setprecision(2) << IS->m_0() << "\n"
      <<  T_GaMa_statan_with_confidence_level
      << setprecision(0) << IS->conf_pr()*100 << " %\n\n";
  out.flush();

  const int nadb = IS->degrees_of_freedom();
  if (nadb)
    {
      const double alfa_pul = (1 - IS->conf_pr())/2;
      //if (IS->m_0_aposteriori())
        {
          float testm0 = IS->m_0_aposteriori_value() / IS->apriori_m_0();
          float dolni = sqrt(GNU_gama::Chi_square(1-alfa_pul,nadb)/nadb);
          float horni = sqrt(GNU_gama::Chi_square(  alfa_pul,nadb)/nadb);

          out << T_GaMa_Ratio_empirical_to_apriori << setprecision(3)
              << testm0 << '\n'
              << setprecision(0) << IS->conf_pr()*100
              << " % " << T_GaMa_interval << " ("
              << setprecision(3) << dolni
              << ", " << horni
              << ") "
              << (dolni<testm0 && horni>testm0 ?
                  T_GaMa_interval_contains :
                  T_GaMa_interval_doesnt_contain)
              << "\n";
          out.flush();

          float m0d=0, m0s=0, m0u=0;   // m0' from dists. / dirs. / angles
          float sqd=0, sqs=0, squ=0;   // sum of weight coefficients
          int   itd=0, its=0, itu=0;
          for (int i=1; i<=IS->sum_observations(); i++)
            {
              float v = IS->residuals()(i);
              float q = IS->wcoef_res(i);
              if (dynamic_cast<Distance*>(IS->ptr_obs(i)))
                {
                  itd = 1;
                  m0d += v*v;
                  sqd += q;
                }
              else if (dynamic_cast<Direction*>(IS->ptr_obs(i)))
                {
                  its = 1;
                  m0s += v*v;
                  sqs += q;
                }
              else if (dynamic_cast<Angle*>(IS->ptr_obs(i)))
                {
                  itu = 2;
                  m0u += v*v;
                  squ += q;
                }
            }
          if (itd) m0d = sqrt(m0d/sqd);
          if (its + itu) m0s = sqrt((m0s+m0u)/(sqs+squ));
          if (itd+its+itu > 1)
            {
              float ma = IS->apriori_m_0();
              if (itd)
                out << T_GaMa_m0_distances << m0d/ma << "   ";
              if (its+itu)
                {
                  switch (its+itu) {
                  case 1: out << T_GaMa_m0_directions; break;
                  case 2: out << T_GaMa_m0_angles; break;
                  case 3: out << T_GaMa_m0_dirs_angs; break;
                  }
                  out << m0s/ma;
                }
              out << '\n';
            }

          out << '\n';
        }

      Observation* ptr;
      double stud_opr;
      double max_stud = 0;
      int imax = 0;
      {   // for ...
        for (int i=1; i<=IS->sum_observations(); i++)
          //if (IS->obs_control(i) > 0.1)
          if (IS->wcoef_res(i) > 1e-4)     // *** test after getu03
            {
              stud_opr = fabs(IS->studentized_residual(i));
              if (stud_opr > max_stud)
                {
                  max_stud = stud_opr;
                  ptr = IS->ptr_obs(i);
                  imax = i;
                }
            }
      }   // for ...
      bool aprm0 = IS->m_0_apriori();
      float krit_opr;
      if (aprm0)
        krit_opr = GNU_gama::Normal(alfa_pul);
      else
        {
          float s = GNU_gama::Student(alfa_pul, nadb-1);
          float t = s*s;
          krit_opr = sqrt(nadb*t/(nadb-1+t));
        }

      if (nadb > 1 && imax > 0 && IS->m_0_aposteriori())
        {
          double v = IS->residuals()(imax);
          double q = IS->wcoef_res(imax);
          double m_0_red = sqrt(fabs(IS->trans_VWV()-v*v/q)/(nadb-1));
          out << T_GaMa_Maximal_decrease_of_m0
              << setprecision(3) << m_0_red/IS->apriori_m_0()
              << "\n\n";
        }

      if (imax > 0)
        {
          out.setf(ios_base::fixed, ios_base::floatfield);
          if (aprm0)
            out << T_GaMa_Maximal_normalized_residual;
          else
            out << T_GaMa_genpar_Maximal_studentized_residual;
          out << setprecision(2) << max_stud;
          if (max_stud > krit_opr)
            out << T_GaMa_genpar_exceeds;
          else
            out << T_GaMa_genpar_doesnt_exceed;
          out << T_GaMa_genpar_critical_value <<  krit_opr << "\n"
              << T_GaMa_genpar_on_significance_level
              << setprecision(0) << (1 - IS->conf_pr())*100
              << T_GaMa_genpar_for_observation_ind
              << imax << "\n";
          WriteVisitor<OutStream> write_visitor(out, true);
          ptr->accept(&write_visitor);
          out << "\n";
        }

    }  // end of redundant observation processing

  out << "\n\n";
  out.flush();

  return true;
}

}}

#endif

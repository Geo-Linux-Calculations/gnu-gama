/* GNU Gama C++ librarys
   Copyright (C) 2012, 2013, 2014, 2016, 2018  Ales Cepek <cepek@gnu.org>

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
   along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file svg.h
 * \brief #GNU_gama::local::GamaLocalHTML class implementation
 *
 * \author Ales Cepek
 */

#include <gnu_gama/local/html.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/writevisitor.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/local/test_linearization_visitor.h>

#include <sstream>
#include <cmath>

namespace {

using namespace GNU_gama::local;

class HtmlStringStream {
public:
  HtmlStringStream(std::string& t) : str(t) {}

  HtmlStringStream& operator<< (const std::string& p)
  {
    str.append(p); return *this;
  }
  HtmlStringStream& operator<< (const char* p)
  {
    str.append(p); return *this;
  }

private:
    std::string& str;
};

std::string str2html  (const std::string& str);
std::string tdSpace   (int n=1);
std::string int2str   (int n);
std::string double2str(double d, char format, int precision);

std::string tdLeft (std::string s, int l=0, int r=0);
std::string tdRight(std::string s, int l=0, int r=0);
// std::string tdLeft (int n,         int l=0, int r=0);  ... unused function
std::string tdRight(int n,         int l=0, int r=0);
// std::string tdLeft (double d, char format, int precision, int l=0, int r=0); ... unused function
std::string tdRight(double d, char format, int precision, int l=0, int r=0);

std::string double2str(double d, char format, int precision)
{
  std::ostringstream out;
  switch (format)
    {
    case 'f':
    case 'F':
      out.setf(std::ios_base::fixed, std::ios_base::floatfield);
      break;
    case 'e':
    case 'E':
      out.setf(std::ios_base::scientific, std::ios_base::floatfield);
      break;
    default:
      break;
    }
  out.precision(precision);
  out << d;
  return out.str();
}
std::string tdLeft(std::string s, int l, int r)
{
  return
    "<td align='left'>" + tdSpace(l) + str2html(s) + tdSpace(r) + "</td>";
}
std::string tdRight(std::string s, int l, int r)
{
  return
    "<td align='right'>" + tdSpace(l) + str2html(s) + tdSpace(r) + "</td>";
}
// std::string tdLeft(int n, int l, int r)   ... unused function
// {
//   return tdLeft(int2str(n), l, r);
// }
std::string tdRight(int n, int l, int r)
{
  return tdRight(int2str(n), l, r);
}
// std::string tdLeft (double d, char format, int precision, int l, int r)
// {
//   return tdLeft(double2str(d,format,precision),l,r);  ... unused function
// }
std::string tdRight(double d, char format, int precision, int l, int r)
{
  return tdRight(double2str(d,format,precision),l,r);
}
std::string str2html(const std::string &str)
{
  std::string t;
  for (std::string::const_iterator i=str.begin(), e=str.end(); i!=e; ++i)
    {
      char c = *i;
      if      (c == '<') t += "&lt;";
      else if (c == '>') t += "&gt;";
      else if (c == '&') t += "&amp;";
      else if (c =='\'') t += "&quot;";
      else               t += c;
    }
  return t;
}
std::string tdSpace(int n)
{
  std::string t;
  for (int i=0; i<n; i++) t.append("&nbsp;");
  return t;
}
std::string int2str(int n)
{
  std::ostringstream out;
  out << n;
  return out.str();
}


class HtmlAdjustedObservationsBaseVisitor
  : public GNU_gama::local::AllObservationsVisitor
{
public:
  HtmlAdjustedObservationsBaseVisitor(HtmlStringStream& output,
                                      LocalNetwork*     locnet)
    : out(output), lnet(locnet)
  {
    kki   = lnet->conf_int_coef();
  }
  virtual ~HtmlAdjustedObservationsBaseVisitor() {}

  void visit(GNU_gama::local::Direction* /*element*/)
  {
    out << tdLeft(T_GaMa_direction);
    angular();
  }
  void visit(GNU_gama::local::Distance* /*element*/)
  {
    out << tdLeft(T_GaMa_distance);
    linear();
  }
  void visit(GNU_gama::local::Angle* element)
  {
    out << "</tr>\n<tr><td></td><td></td>"
        << tdRight(element->fs().str(), 2,2)
        << tdLeft(T_GaMa_angle);
    angular();
  }
  void visit(GNU_gama::local::H_Diff* /*element*/)
  {
    out << tdLeft(T_GaMa_levell);
    linear();
  }
  void visit(GNU_gama::local::S_Distance* /*element*/)
  {
    out << tdLeft(T_GaMa_s_distance);
    linear();
  }
  void visit(GNU_gama::local::Z_Angle* /*element*/)
  {
    out << tdLeft(T_GaMa_z_angle);
    angular();
  }
  void visit(GNU_gama::local::X* /*element*/)
  {
    out << tdLeft(T_GaMa_x);
    linear();
  }
  void visit(GNU_gama::local::Y* /*element*/)
  {
    out << tdLeft(T_GaMa_y);
    linear();
  }
  void visit(GNU_gama::local::Z* /*element*/)
  {
    out << tdLeft(T_GaMa_z);
    linear();
  }
  void visit(GNU_gama::local::Xdiff* /*element*/)
  {
    out << tdLeft(T_GaMa_xdiff);
    linear();
  }
  void visit(GNU_gama::local::Ydiff* /*element*/)
  {
    out << tdLeft(T_GaMa_ydiff);
    linear();
  }
  void visit(GNU_gama::local::Zdiff* /*element*/)
  {
    out << tdLeft(T_GaMa_zdiff);
    linear();
  }
  void visit(GNU_gama::local::Azimuth* /*element*/)
  {
    out << tdLeft(T_GaMa_azimuth);
    angular();
  }

  void adjustedObservation(int i)
  {
    obs   = lnet->ptr_obs(i);
    index = i;

    out << "<tr>" << tdRight(i);

    PointID id = obs->from();
    if (prev_id != id)
      out << tdRight(id.str(), 2, 2);
    else
      out << "<td></td>";
    prev_id = id;
    out << tdRight(obs->to().str(), 2, 2);

    obs->accept(this);
  }

  virtual void angular() = 0;
  virtual void linear () = 0;

protected:
  HtmlStringStream& out;
  LocalNetwork*     lnet;
  Observation*      obs;
  PointID           prev_id;
  int               index;
  double            kki;
};


class HtmlAdjustedObservationsVisitor
  : public HtmlAdjustedObservationsBaseVisitor
{
public:
  HtmlAdjustedObservationsVisitor
  (
   HtmlStringStream& stream,
   GNU_gama::local::LocalNetwork* local_network
   )
    : HtmlAdjustedObservationsBaseVisitor(stream, local_network)
  {
    out << "<tr>"
        << "<th>i</th>"
        << "<th>" << T_GaMa_standpoint << tdSpace(2) << "</th>"
        << "<th>" << T_GaMa_target << tdSpace(2) << "</th>"
        << "<th></th>"
        << "<th>" << T_HTML_observed << "</th>"
        << "<th>" << T_HTML_adjusted << "</th>"
        << "<th colspan='2'>" << T_HTML_stddev_confi << "</th>"
        << "</tr>\n";
    out << "<tr><th colspan='4'></th>"
        << "<th>" << T_HTML_value << "</th>";
    if (lnet->gons())
      out << "<th>" << T_HTML_mg << "</th>"
          << "<th colspan='2'>" << T_HTML_mmcc << "</th>";
    else
      out << "<th>" << T_HTML_md << "</th>"
          << "<th colspan='2'>" << T_HTML_mmss << "</th>";
    out << "</tr>";
  }

private:

  void linear()
  {
    double val = obs->value();
    double adj = val + lnet->residuals()(index)/1000;

    out << tdRight(val, 'F', 5, 2,2)
        << tdRight(adj, 'F', 5, 2,2);

    double ml = lnet->stdev_obs(index);

    out << tdRight(ml,     'F', 1, 2,2)
        << tdRight(ml*kki, 'F', 1, 2,2);

    out << "</tr>\n";
  }

  void angular()
  {
    double val = R2G*obs->value();
    double adj = val + lnet->residuals()(index)/10000;
    if (adj < 0) adj += 400;
    if (adj >= 400) adj -= 400;
    if (lnet->gons())
      out << tdRight(val, 'F', 6, 2,2)
          << tdRight(adj, 'F', 6, 2,2);
    else
      out << tdRight(GNU_gama::gon2deg(val, 0,2), 2,2)
          << tdRight(GNU_gama::gon2deg(adj, 0,2), 2,2);

    double ml = lnet->stdev_obs(index);

    out << tdRight(ml,     'F', 1, 2,2)
        << tdRight(ml*kki, 'F', 1, 2,2);

    out << "</tr>\n";
  }
};

class HtmlAdjustedResidualsVisitor
  : public HtmlAdjustedObservationsBaseVisitor
{
public:
  HtmlAdjustedResidualsVisitor
  (
   HtmlStringStream& stream,
   GNU_gama::local::LocalNetwork* local_network,
   std::vector<int>* outl = 0
   )
    : HtmlAdjustedObservationsBaseVisitor(stream, local_network),
      odlehla(outl)
  {
    out << "<tr>"
        << "<th>i</th>"
        << "<th>" << T_GaMa_standpoint << tdSpace(2) << "</th>"
        << "<th>" << T_GaMa_target << tdSpace(2) << "</th>"
        << "<th></th>"
        << "<th>" << T_HTML_f_proc << "</th><th></th>"
        << "<th>" << T_HTML_r      << "</th>"
        << "<th>" << T_HTML_rstud  << "</th><th></th>"
        << "<th colspan='2'>"
        << T_HTML_e_obs_adj << "</th>"
        << "</tr>\n";
    out << "<tr><th colspan='6'></th>";
    if (lnet->gons())
      out << "<th>" << T_HTML_mmcc << "</th><th colspan='2'></th>"
          << "<th colspan='2'>"<< T_HTML_mmcc << "</th>";
    else
      out << "<th>" << T_HTML_mmss << "</th><th colspan='2'></th>"
          << "<th colspan='2'>"<< T_HTML_mmss << "</th>";
    out << "</tr>";

    init_studres();
  }

private:
  double scale;
  std::vector<double> studres;
  int imax;
  std::vector<int>*   odlehla;

  void init_studres()
  {
    if (!odlehla) return;

    odlehla->clear();

    using namespace std;
    imax = 1;              // index of maximal studentized residual
    double maxno = 0;
    for (int i=1; i<=lnet->sum_observations(); i++)
      {
        if (lnet->obs_control(i) < 0.1) continue;

        double no = fabs(lnet->studentized_residual(i));

        if (no > maxno) {
          maxno = no;
          imax = i;
        }
        if (odlehla && no > kki) odlehla->push_back(i);
      }
    if (odlehla && odlehla->size() > 0)
      sort(odlehla->begin(), odlehla->end(), StOpSort(lnet));
  }

  void linear()
  {
    scale = 1.0;
    // double val = obs->value();                        ... unused
    // double adj = val + lnet->residuals()(index)/1000; ... unused

    double f  = lnet->obs_control(index);
    out << tdRight(f, 'F', 1, 2,0);
    if (f < 0.1)
      out << "<td>" << T_GaMa_resobs_no_control   << "</td>"; // uncontrolled
    else if (f < 5)
      out << "<td>" << T_GaMa_resobs_weak_control << "</td>"; // weak control
    else
      out << "<td></td>";

    out << tdRight(lnet->residuals()(index)*scale, 'F', 3, 2,2);

    if (f >= 0.1)
      {
        using namespace std;
        double no = fabs(lnet->studentized_residual(index));
        out << tdRight(no, 'F', 1, 2,0);

        if (index == imax)
          {
            if (no > kki)
              out << "<td>" << T_GaMa_resobs_mc_max_critical << "</td>";
            else
              out << "<td>" << T_GaMa_resobs_mc_max << "</td>";
          }
        else if (no > kki)
          {
            out << "<td>" << T_GaMa_resobs_mc_critical << "</td>";
          }
        else
          {
            out << "<td></td>";
          }

        if ( (obs->ptr_cluster())->covariance_matrix.bandWidth() == 0 &&
             (f >=5 || (f >= 0.1 && no > kki)))
          {
            double em = lnet->residuals()(index) /
              (lnet->wcoef_res(index)*lnet->weight_obs(index));
            out << tdRight(em*scale, 'F', 1, 2,1);

            double ev = em - lnet->residuals()(index);
            out << tdRight(ev*scale, 'F', 1, 1,2);
          }
        else
          {
            out << "<td></td>"
                << "<td></td>";
          }
      }

    out << "</tr>\n";
  }

  void angular()
  {
    scale = 0.324;
    linear();
  }

  /* *******************************************************************
   * local helper class StOpSort for sorting outlying observations (sort
   * by "studentized" residuals)
   * ******************************************************************* */

  class StOpSort {

    GNU_gama::local::LocalNetwork* lnet;

  public:

    StOpSort(GNU_gama::local::LocalNetwork* is) : lnet(is) {}
    bool operator()(int a, int b)
    {
      using namespace std;
      double sa = fabs(lnet->studentized_residual(a));
      double sb = fabs(lnet->studentized_residual(b));
      return sa > sb;
    }
  };
};

} // anonymous namespace

using namespace GNU_gama::local;
using std::string;

GamaLocalHTML::GamaLocalHTML(GNU_gama::local::LocalNetwork *local_network)
  : lnet(local_network)
{
  set_all_parts_active();
  set_title("GNU Gama");

  set_style("<style type='text/css'>\n"
            "  table { border:0; margin:0.2em 0; }\n"
            "  th    { white-space:nowrap; }\n"
            "</style>\n");
}

void GamaLocalHTML::set_local_network(LocalNetwork *local_network)
{
  lnet = local_network;

  terms       .str.clear();
  info        .str.clear();
  unknowns    .str.clear();
  observations.str.clear();
  residuals   .str.clear();
  rejected    .str.clear();

  set_all_parts_active();
}

void GamaLocalHTML::html(std::ostream & ostr) const
{
  ostr << html_begin()
       << html_terms()
       << html_info()
       << html_unknowns()
       << html_observations()
       << html_residuals()
       << html_rejected()
       << html_end();
}

std::string GamaLocalHTML::str() const
{
  std::ostringstream ostr;
  html(ostr);
  return ostr.str();
}

void GamaLocalHTML::exec()
{
  htmlBegin();
  htmlTerms();
  htmlInfo();
  htmlUnknowns();
  htmlObservations();
  htmlResiduals();
  htmlRejected();
  htmlEnd();
}

void GamaLocalHTML::htmlBegin()
{
  std::string& str = begin.str;
  str.clear();

  str  =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
    "  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
    "<html xmlns='http://www.w3.org/1999/xhtml'"
    " xml:lang='en' lang='en'>\n"
    "<head>\n"
    + html_style +
    "<meta content=\"text/html; charset=UTF-8\""
    " http-equiv=\"Content-Type\" />\n"
    "<title>" + title + "</title>"
    "</head>\n"
    "<body>\n"
    "<h1>" + (!h1.empty() ? h1 : title) + "</h1>\n"
    ;
}

void GamaLocalHTML::htmlInfo()
{
  std::string& str = info.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  HtmlStringStream out(str);

  if (!lnet->description.empty())
    {
      out << "<h2>" << T_GaMa_network_description << "</h2>\n";

      if (lnet->description[0] == '<') // description in HTML
        out << lnet->description;
      else
        {
          out << "<p id='description'>";
          int N = lnet->description.length();
          while (N > 0 && lnet->description[N-1] == '\n') N--;
          int br=2;
          for (int i=0; i<N; i++)
            {
              char c = lnet->description[i];
              if (c == '\n')
                {
                  if (br++ < 2) out << "<br/>\n";
                }
              else
                {
                  std::string t;
                  t += c;
                  out << t;
                  br = 0;
                }
            }
          out << "</p>\n";
        }
    }

  out << "<h2>" << T_GaMa_General_solution_parameters << "</h2>";

  // summary of coordinates in adjustment

  int a_xyz = 0, a_xy = 0, a_z = 0;      // adjusted
  int c_xyz = 0, c_xy = 0, c_z = 0;      // constrained
  int f_xyz = 0, f_xy = 0, f_z = 0;      // fixed

  for (PointData::const_iterator i=lnet->PD.begin(); i!=lnet->PD.end(); ++i)
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

  {
    const int N = 3;
    out << "<table id='coordinates_summary'>\n";
    out << "<tr><th align='left'>" << T_GaMa_gpar1_coordinates << "</th>"
        << "<th>xyz</th>" << "<th>xy</th>" << "<th>z</th>" <<  "</tr>\n";
    out << "<tr>" << tdLeft(T_GaMa_gpar1_adjusted_coordinates,0,N)
        << tdRight(a_xyz,N,N) << tdRight(a_xy,N,N)
        << tdRight(a_z,N,N) << "</tr>\n";
    out << "<tr>" << tdLeft(T_GaMa_gpar1_constrained_coordinates,0,N)
        << tdRight(c_xyz,N,N) << tdRight(c_xy,N,N)
        << tdRight(c_z,N,N) << "</tr>\n";
    out << "<tr>" << tdLeft(T_GaMa_gpar1_fixed_coordinates,0,N)
        << tdRight(f_xyz,N,N) << tdRight(f_xy,N,N)
        << tdRight(f_z,N,N) << "</tr>\n";
    out << "<tr>" << tdLeft(T_GaMa_gpar1_total,0,N)
        << tdRight(f_xyz + a_xyz,N,N)
        << tdRight(f_xy  + a_xy,N,N)
        << tdRight(f_z   + a_z,N,N) + "</tr>\n";
    out << "</table>\n";
  }
  int pocosn = 0;
  for (int i=1; i<=lnet->sum_unknowns(); i++)
    if (lnet->unknown_type(i) == 'R')
      pocosn++;

  int pocsmer=0, pocuhl=0, pocdel=0, pocsour=0, pocnivp = 0,
    poczeni=0, pocsikm=0, pocvec=0, pocazim=0;
  for (int i=1; i<=lnet->sum_observations(); i++)
    {
      if      (dynamic_cast<Distance*  >(lnet->ptr_obs(i))) pocdel++;
      else if (dynamic_cast<Direction* >(lnet->ptr_obs(i))) pocsmer++;
      else if (dynamic_cast<Angle*     >(lnet->ptr_obs(i))) pocuhl++;
      else if (dynamic_cast<X*         >(lnet->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<Y*         >(lnet->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<Z*         >(lnet->ptr_obs(i))) pocsour++;
      else if (dynamic_cast<H_Diff*    >(lnet->ptr_obs(i))) pocnivp++;
      else if (dynamic_cast<Z_Angle*   >(lnet->ptr_obs(i))) poczeni++;
      else if (dynamic_cast<S_Distance*>(lnet->ptr_obs(i))) pocsikm++;
      else if (dynamic_cast<Xdiff*     >(lnet->ptr_obs(i))) pocvec++;
      else if (dynamic_cast<Azimuth*   >(lnet->ptr_obs(i))) pocazim++;
      //else if (dynamic_cast<Ydiff*     >(lnet->ptr_obs(i))) pocvec++;
      ///else if (dynamic_cast<Zdiff*     >(lnet->ptr_obs(i))) pocvec++;
    }

  if (lnet->sum_observations() > 0 && pocnivp != lnet->sum_observations())
  {
    out << "<table id='observations_summary'>\n";
    if (pocsmer)
      {
        out << "<tr id='count_dir'>" << tdLeft(T_GaMa_gpar1_directions,0,2)
            << tdRight(pocsmer, 0,8)
            << tdLeft(T_GaMa_gpar2_bearings,0,2)
            << tdRight(pocosn) + "</tr>\n";
      }
    if (pocuhl)
      {
        out << "<tr id='count_ang'>" << tdLeft(T_GaMa_gpar1_angles,0,2)
            << tdRight(pocuhl,  0,8) << "<td/><td/></tr>\n";
      }
    if (pocdel)
      {
        out << "<tr id='count_dist'>" << tdLeft(T_GaMa_gpar1_distances,0,2)
            << tdRight(pocdel,  0,8) << "<td/><td/></tr>\n";
      }
    if (pocsour)
      {
        out << "<tr id='count_coord'>"
            << tdLeft(T_GaMa_gpar1_observed_coords,0,2)
            << tdRight(pocsour, 0,8) << "<td/><td/></tr>\n";
      }
    if (pocnivp) // && (pocnivp != lnet->sum_observations()))
      {
        out << "<tr id='count_level'>"
            << tdLeft(T_GaMa_gpar1_leveling_diffs,0,2)
            << tdRight(pocnivp, 0,8) << "<td/><td/></tr>\n";
      }
    if (poczeni)
      {
        out << "<tr id='count_zang'>" << tdLeft(T_GaMa_gpar1_z_angles,0,2)
            << tdRight(poczeni, 0,8) << "<td/><td/></tr>\n";
      }
    if (pocsikm)
      {
        out << "<tr id='count_sdist'>" << tdLeft(T_GaMa_gpar1_s_dists,0,2)
            << tdRight(pocsikm, 0,8) << "<td/><td/></tr>\n";
      }
    if (pocvec)
      {
        out << "<tr id='count_vect'>" << tdLeft("Coordinate differences",0,2)
            << tdRight(pocvec, 0,8) << "<td/><td/></tr>\n";
      }
    if (pocazim)
      {
        out << "<tr id='count_azim'>" << tdLeft("Azimuths",0,2)
            << tdRight(pocazim, 0,8) << "<td/><td/></tr>\n";
      }

    int types = 0;
    if (pocsmer) types++;
    if (pocdel)  types++;
    if (pocuhl)  types++;
    if (pocsour) types++;
    if (pocnivp) types++;
    if (poczeni) types++;
    if (pocsikm) types++;
    if (pocvec)  types++;
    if (pocazim) types++;
    if (types != 1)
      {
        out << "<tr id='count_total'>" << tdLeft(T_GaMa_gpar1_obs_total,0,2)
            << tdRight(lnet->sum_observations(),0,8) << "<td/><td/></tr>\n";
      }
    out << "</table>\n";

    if (!lnet->connected_network())
      {
        out << "<table><tr>" << tdLeft(T_GaMa_network_not_connected)
            << "</tr></table>\n";
      }
  }

  // *********  singular free networks  *********
  {
    int d = lnet->null_space();
    try {
      if (lnet->min_n() < d)
        throw MatVecException(GNU_gama::Exception::BadRegularization,
                              T_GaMa_not_enough_constrained_points);
      lnet->trans_VWV();  // now let's try to adjust the nework
    }
    catch (const MatVecException& vs)
      {
        if (vs.error != GNU_gama::Exception::BadRegularization) throw;

        out << "<h3>" << T_GaMa_Free_network << "</h3>\n";

        out << "<p>";
        out << T_GaMa_Free_network_defect_is << int2str(d) << ". ";
        out << T_GaMa_Given_network_configuration_can_not_be_adjusted
            << std::string(".\n");
        if (lnet->min_n() < d)
          out << T_GaMa_not_enough_constrained_points + std::string(".");
        out << "</p>";

        out << "<h4>" << T_GaMa_detected_singular_variables << "</h4>";
        out << "<table><caption id='caption'>";
        out << T_GaMa_index_type_point;
        out << "</caption>\n";

        for (int i=1; i<=lnet->sum_unknowns(); i++)
          if (lnet->lindep(i))
            {
              out << "<tr>" << tdRight(i, 2, 2)
                  << tdRight(lnet->unknown_type(i), 0,2)
                  << tdRight(lnet->unknown_pointid(i).str()) << "</tr>\n";
            }
        out << "</table>\n";

        return; // *********  network can not be adjusted  *********
      }
  }  // free networks section

  out << "<table id='project_equations'>\n";
  out << "<tr"
      << (lnet->connected_network() ?
          " id='connected_network'" :
          " id='disconnected_network'")
      << "><td>" << T_GaMa_gpar1_equations << "</td>"
      << tdRight(lnet->sum_observations(), 2,8)
      << tdLeft(T_GaMa_gpar2_number_of_unknowns)
      << tdRight(lnet->sum_unknowns(), 2,0) << "</tr>\n";
  out << "<tr>" << tdLeft(T_GaMa_gpar1_redundancy)
      << tdRight(lnet->degrees_of_freedom(),2,8)
      << tdLeft(T_GaMa_gpar2_network_defect)
      << tdRight(lnet->null_space(),2,0) + "</tr>\n";
  out << "</table>\n";

  out << "<table id='sum_of_squares'>\n";
  out << "<tr>" + tdLeft(T_GaMa_m0_apriori)
      << tdRight(lnet->apriori_m_0(), 'F',2, 2,8)
      << "<td>&nbsp;</td><td>&nbsp;</td></tr>\n"
      << "<tr>" + tdLeft(T_GaMa_m0_empirical)
      << tdRight((lnet->degrees_of_freedom() > 0 ?
        sqrt(lnet->trans_VWV()/lnet->degrees_of_freedom()) : 0), 'F',2, 2,8)
      << tdLeft("[pvv]",5,2)
      << tdRight(lnet->trans_VWV(), 'E', 5)
      << "</tr>\n";
  out << "</table>\n";

  out << "<table id='standard_deviation'>\n";
  out << "<tr>" + tdLeft(T_GaMa_During_statistical_analysis_we_work) + "</tr>";
  out << "<tr id="
      << std::string(lnet->m_0_aposteriori()
                     ? "'a_posteriori'>" : "'a_priori'>")
      << tdLeft((lnet->m_0_aposteriori() ?
              std::string(T_GaMa_statan_with_empirical_standard_deviation) :
              std::string(T_GaMa_statan_with_apriori_standard_deviation)),3,0)
      <<  tdRight(double2str(lnet->m_0(), 'F', 2))
      << "</tr>\n";

  out << "<tr>"
      << tdLeft(std::string(T_GaMa_statan_with_confidence_level), 3,0)
      << tdRight(double2str(lnet->conf_pr()*100, 'F', 0))
      << "<td>%</td>"
      << "</tr>\n";
  out << "</table>\n";


  const int nadb = lnet->degrees_of_freedom();
  if (nadb)
    {
      const double alfa_pul = (1 - lnet->conf_pr())/2;
      //if (lnet->m_0_aposteriori())
        {
          double testm0 = lnet->m_0_aposteriori_value() / lnet->apriori_m_0();
          double dolni  = sqrt(GNU_gama::Chi_square(1-alfa_pul,nadb)/nadb);
          double horni  = sqrt(GNU_gama::Chi_square(  alfa_pul,nadb)/nadb);
          bool   passed = dolni<testm0 && horni>testm0;

          out << "<table id='standard_deviation_2'><tr><td>"
              << T_GaMa_Ratio_empirical_to_apriori
              << "</td><td></td>"
              << tdLeft(double2str(testm0, 'F',3))
              << "</tr>\n"
              << "<tr id="
              << (passed ? "'test_m0_passed'" : "'test_m0_failed'")
              << "><td>"
              << double2str(lnet->conf_pr()*100, 'F', 0)
              << " % " << T_GaMa_interval << " "
              << (passed ?
                  T_GaMa_interval_contains :
                  T_GaMa_interval_doesnt_contain)
              << "</td>"
              << "<td>(</td><td>" << double2str(dolni, 'F',3)
              << "</td><td>,</td>"
              << "<td>" << double2str(horni, 'F',3)
              << "</td><td>)</td></tr>\n";

          float m0d=0, m0s=0, m0u=0;   // m0' from dists. / dirs. / angles
          float sqd=0, sqs=0, squ=0;   // sum of weight coefficients
          int   itd=0, its=0, itu=0;
          for (int i=1; i<=lnet->sum_observations(); i++)
            {
              float v = lnet->residuals()(i);
              float q = lnet->wcoef_res(i);
              if (dynamic_cast<Distance*>(lnet->ptr_obs(i)))
                {
                  itd = 1;
                  m0d += v*v;
                  sqd += q;
                }
              else if (dynamic_cast<Direction*>(lnet->ptr_obs(i)))
                {
                  its = 1;
                  m0s += v*v;
                  sqs += q;
                }
              else if (dynamic_cast<Angle*>(lnet->ptr_obs(i)))
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
              double ma = lnet->apriori_m_0();
              if (itd)
                out << "<tr>" << tdLeft(T_GaMa_m0_distances)
                    << "<td></td>"
                    << tdLeft(double2str(m0d/ma, 'F', 3))
                    << "</tr>\n";
              if (its+itu)
                {
                  out << "<tr><td>";
                  switch (its+itu) {
                  case 1: out << T_GaMa_m0_directions; break;
                  case 2: out << T_GaMa_m0_angles; break;
                  case 3: out << T_GaMa_m0_dirs_angs; break;
                  }
                  out << "</td><td colspan='1'></td>"
                      << tdLeft(double2str(m0s/ma, 'F', 3))
                      << "</tr>";
                }
            }
          out << "<tr id='confidence_scale'>"
              << tdLeft("Confidence coeficient:")
              << "<td colspan='1'></td>"
              << "<td colspan='5' align='left'>"
              << double2str(lnet->conf_int_coef(), 'F', 5)
              << "</td></tr>\n";

          out << "</table>\n";
        }

      Observation* ptr {nullptr};
      double stud_opr;
      double max_stud = 0;
      int imax = 0;
      for (int i=1; i<=lnet->sum_observations(); i++)
        //if (lnet->obs_control(i) > 0.1)
        if (lnet->wcoef_res(i) > 1e-4)     // *** test after getu03
          {
            stud_opr = fabs(lnet->studentized_residual(i));
            if (stud_opr > max_stud)
              {
                max_stud = stud_opr;
                ptr = lnet->ptr_obs(i);
                imax = i;
              }
          }
      bool aprm0 = lnet->m_0_apriori();
      float krit_opr;
      if (aprm0)
        krit_opr = GNU_gama::Normal(alfa_pul);
      else
        {
          float s = GNU_gama::Student(alfa_pul, nadb-1);
          float t = s*s;
          krit_opr = sqrt(nadb*t/(nadb-1+t));
        }

      if (nadb > 1 && imax > 0 && lnet->m_0_aposteriori())
        {
          double v = lnet->residuals()(imax);
          double q = lnet->wcoef_res(imax);
          double m_0_red = sqrt(fabs(lnet->trans_VWV()-v*v/q)/(nadb-1));
          out << "<p>" << T_GaMa_Maximal_decrease_of_m0
              << double2str(m_0_red/lnet->apriori_m_0(), 'F', 3)
              << "</p>";
        }

      if (imax > 0)
        {
          out << "<p id='max_decrease'>";
          if (aprm0) out << T_GaMa_Maximal_normalized_residual;
          else       out << T_GaMa_genpar_Maximal_studentized_residual;

          out << double2str(max_stud, 'F', 2);

          if (max_stud > krit_opr) out << T_GaMa_genpar_exceeds;
          else                     out << T_GaMa_genpar_doesnt_exceed;

          out << T_GaMa_genpar_critical_value
              <<  double2str(krit_opr, 'F', 2) + "<br/>"
              << T_GaMa_genpar_on_significance_level
              << double2str( (1 - lnet->conf_pr())*100, 'F', 0 )
              << T_GaMa_genpar_for_observation_ind
              << int2str(imax) + "<br/>";

          std::ostringstream outv;
          outv.setf(std::ios_base::fixed, std::ios_base::floatfield);
          outv.precision(4);
          WriteVisitor<std::ostringstream> write_visitor(outv, true);
          ptr->accept(&write_visitor);
          out << str2html(outv.str());

          out << "</p>\n";
        }
    }  // end of redundant observation processing
}

void GamaLocalHTML::htmlUnknowns()
{
  std::string& str = unknowns.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  HtmlStringStream out(str);

  out << "<h2>" << T_HTML_points << "</h2>\n";

  // fixed points
  {
    const int N_fixed = 3;

    using namespace std;
    using namespace GNU_gama::local;

    const int y_sign = lnet->y_sign();

    int pocpevb=0, pocpevv=0;
    for (PointData::iterator i=lnet->PD.begin(); i!=lnet->PD.end(); ++i)
      {
        if ((*i).second.fixed_xy()) pocpevb++;
        if ((*i).second.fixed_z())  pocpevv++;
      }

    if (pocpevb != 0 || pocpevv != 0)
      {
        out << "<h3>" << T_GaMa_Review_of_fixed_points << "</h3>\n";

        out << "<table id='fixed_points'>\n";
        out << "<tr><th>" << T_GaMa_point << tdSpace(N_fixed) << "</th>";
        if (pocpevb) out << "<th>x</th><th>y</th>";
        if (pocpevv) out << "<th>z</th>";
        out << "</tr>\n";
        for (PointData::iterator i=lnet->PD.begin(); i!=lnet->PD.end(); ++i)
          {
            if (!(*i).second.fixed_xy() && !(*i).second.fixed_z()) continue;

            out << "<tr>" << tdRight(i->first.str(), 0, N_fixed);

            if ((*i).second.fixed_xy())
              {
                out << tdRight((*i).second.x(), 'F', 3, 0, N_fixed)
                    << tdRight((*i).second.y()*y_sign, 'F', 3, 0, N_fixed);
              }
            if ((*i).second.fixed_z())
              {
                if (pocpevb && !(*i).second.fixed_xy())
                {
                    out << "<td></td><td></td>";
                }
                out << tdRight((*i).second.z(), 'F', 5);
              }

            out << "</tr>\n";
          }
        out << "</table>\n";
      }
  } // fixed points


    // adjusted unknowns
  {
    using namespace std;
    using namespace GNU_gama::local;

    const int y_sign = lnet->y_sign();

    const Vec& x = lnet->solve();
    double kki = lnet->conf_int_coef();
    const int pocnez = lnet->sum_unknowns();

    std::string coordinates_table_header;
    {
      HtmlStringStream out(coordinates_table_header);
      out << "<tr>"
          << "<th>i</th>"
          << "<th>" << T_GaMa_point        << "</th>"
          << "<th></th>"    // '*' for constrained points
          << "<th>" << T_HTML_approximate << "</th>"
          << "<th>" << T_HTML_correction  << "</th>"
          << "<th>" << T_HTML_adjusted    << "</th>"
          << "<th colspan='2'>" << T_HTML_stddev_confi << "</th>"
          << "</tr>\n"
          << "<tr>"
          << "<th colspan='3'>&nbsp;</th>"
          << "<th>" << T_HTML_value << "</th>"
          << "<th>" << T_HTML_m     << "</th>"
          << "<th>" << T_HTML_value << "</th>"
          << "<th colspan='2'>" << T_HTML_mm << "</th>"
          << "</tr>\n";
    }

    bool coordinates = false;
    for (int i=1; i<=pocnez; i++)
      if (lnet->unknown_type(i) == 'X')
        {
          coordinates = true;
          break;
        }
    if (coordinates)
      {
        out << "<h3>" << T_GaMa_adjunk_Review_of_unknowns_coordidantes
            << "</h3>\n";

        out << "<table id='adjusted_coordinates'>\n";
        out << coordinates_table_header;

        PointID prev_id;
        for (PointData::const_iterator
               ii=lnet->PD.begin(); ii!=lnet->PD.end(); ii++)
          {
            const PointID point_id = (*ii).first;
            const LocalPoint&  b   = (*ii).second;

            if (b.free_xy() && b.index_x())
              {
                out << "<tr><td></td>";
                if (prev_id != point_id)
                  out << tdRight(point_id.str());
                else
                  out << "<td></td>";
                prev_id = point_id;
                double mx = lnet->unknown_stdev(b.index_x());
                double my = lnet->unknown_stdev(b.index_y());
                out << "<td colspan='5'></td></tr>\n";

                out << "<tr>"
                    << tdRight(b.index_x(),0,2);
                if (b.constrained_xy())
                  out << tdRight("X") << tdRight("*");
                else
                  out << tdRight("x") << tdRight("");
                double adj_x = b.x()+x(b.index_x())/1000;
                out << tdRight(b.x_0(), 'F', 5, 2,2);
                out << tdRight(adj_x - b.x_0(), 'F', 5, 2,2);
                out << tdRight(adj_x, 'F', 5, 2,2);
                out << tdRight(mx, 'F', 1,2,2);
                out << tdRight(mx*kki, 'F', 1, 2,2+4);
                out << "</tr>\n";

                out << "<tr>"
                    << tdRight(b.index_y(),0,2);
                if (b.constrained_xy())
                  out << tdRight("Y") << tdRight("*");
                else
                  out << tdRight("y") << tdRight("");
                double adj_y = y_sign*(b.y()+x(b.index_y())/1000);
                out << tdRight(y_sign*b.y_0(), 'F', 5, 2,2);
                out << tdRight(adj_y - y_sign*b.y_0(), 'F',5, 2,2);
                out << tdRight(adj_y, 'F', 5, 2,2);
                out << tdRight(my, 'F', 1, 2,2);
                out << tdRight(my*kki, 'F', 1, 2,2+4);
                out << "</tr>\n";
              }

            if (b.free_z() && b.index_z())
              {
                if (!b.free_xy())
                  {
                    out << "<tr><td></td>";
                    if (prev_id != point_id)
                      out << tdRight(point_id.str());
                    else
                      out << "<td></td>";
                    out << "<td colspan='5'></td></tr>\n";
                  }

                prev_id = point_id;
                out << "<tr>"
                    << tdRight(b.index_z(), 0,2);
                if (b.constrained_z())
                  out << tdRight("Z") << tdRight("*");
                else
                  out << tdRight("z") << tdRight("");
                double adj_z = b.z()+x(b.index_z())/1000;
                out << tdRight(b.z_0(), 'F', 5, 2,2);
                out << tdRight(adj_z - b.z_0(), 'F', 5, 2,2);
                out << tdRight(adj_z, 'F', 5, 2,2);
                double mz = lnet->unknown_stdev(b.index_z());
                out << tdRight(mz, 'F', 1, 2,2);
                out << tdRight(mz*kki, 'F', 1, 2,2+4);
                out << "</tr>\n";
              }

            if ((b.free_xy() && b.index_x()) ||
                (b.free_z()  && b.index_z()) )
              out << "<tr><td colspan='10'></td></tr>";
          }
        out << "</table>\n";

      } // coordinates


    bool orp = false;
    for (int i=1; i<=pocnez; i++)
      if (lnet->unknown_type(i) == 'R')
        {
          orp = true;
          break;
        }
    if (orp)
      {
        const double scale  = lnet->gons() ? 1.0 : 0.324;

        out << "<h3>" << T_GaMa_adjunk_Review_of_unknowns_bearings
            << "</h3>\n";

        out << "<table id='orientation_unknowns'>"
            << "<tr>"
            << "<th>i</th>"
            << "<th>" << T_GaMa_point << "</th>"
            << "<th>" << T_HTML_approximate << "</th>"
            << "<th>" << T_HTML_correction  << "</th>"
            << "<th>" << T_HTML_adjusted    << "</th>"
            << "<th colspan='2'>" << T_HTML_stddev_confi << "</th>"
            << "</tr>\n";
        out << "<tr>"
            << "<th colspan='2'></th>";
        if (lnet->gons())
          out << "<th>" << T_HTML_value_g << "</th>"
              << "<th>" << T_HTML_g       << "</th>"
              << "<th>" << T_HTML_value_g << "</th>"
              << "<th colspan='2'>" << T_HTML_cc << "</th>";
        else
          out << "<th>" << T_HTML_value_d << "</th>"
              << "<th>" << T_HTML_d       << "</th>"
              << "<th>" << T_HTML_value_d << "</th>"
              << "<th colspan='2'>" << T_HTML_ss << "</th>";
        out << "</tr>\n";

        for (int i=1; i<=pocnez; i++)
          if (lnet->unknown_type(i) == 'R')
            {
              const PointID cb = lnet->unknown_pointid(i);
              StandPoint* k = lnet->unknown_standpoint(i);
              double z = y_sign*( k->orientation() )*R2G;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;

              out << "<tr>";
              out << tdRight(i, 0,2)
                  << tdRight(cb.str());
              if (lnet->gons())
                out << tdRight(z, 'F', 6, 4,2);
              else
                out << tdRight(GNU_gama::gon2deg(z, 0, 2),4,2);

              double cor = y_sign*x(i)/10000;
              if (lnet->gons())
                out << tdRight(cor, 'F', 6, 2,2);
              else
                out << tdRight(GNU_gama::gon2deg(cor, 2, 2),2,2);

              z += cor;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              if (lnet->gons())
                out << tdRight(z, 'F', 6, 2,2);
              else
                out << tdRight(GNU_gama::gon2deg(z, 0, 2), 2,2);

              double mz = lnet->unknown_stdev(i)*scale;
              out << tdRight(mz, 'F', 1, 2,2);
              out << tdRight(mz*kki, 'F', 1, 2,2+4);

              out << "</tr>\n";
            }
        out << "</table>\n";
      }


    bool heights = false;
    {
      for (int i=1; i<=pocnez; i++)
        if (lnet->unknown_type(i) == 'Z')
          {
            heights = true;
            break;
          }
    }
    if (heights && !coordinates)
      {
        PointID prev_id;

        out << "<h3>" << T_GaMa_adjunk_Review_of_unknowns_heights
            << "</h3>\n";

        out << "<table id='unknown_heights'>\n";
        out << coordinates_table_header;

        for (int i=1; i<=pocnez; i++)
          if (lnet->unknown_type(i) == 'Z')
            {
              const PointID point_id = lnet->unknown_pointid(i);
              const LocalPoint&    b = lnet->PD[point_id];

              out << "<tr>" << tdRight(i);
              if (prev_id != point_id)
                out << tdRight(point_id.str());
              else
                out << "<td></td>";
              prev_id = point_id;

              if (b.constrained_z())
                out << tdRight("*");
              else
                out << tdRight("");

              double adj_z = b.z()+x(i)/1000;
              out << tdRight(b.z_0(), 'F', 5, 2,2);
              out << tdRight(adj_z - b.z_0(), 'F', 5, 2,2);
              out << tdRight(adj_z, 'F', 5, 2,2);
              double mv = lnet->unknown_stdev(i);
              out << tdRight(mv, 'F', 1, 2,2);
              out << tdRight(mv*kki, 'F', 1, 2,2+4);

              out << "</tr>\n";
            }
        out << "</table>\n";
      }
  } // adjusted unknowns


  // error ellipses
  {
    const int pocnez = lnet->sum_unknowns();
    bool sour = false;
    for (int i=1; i<=pocnez; i++)
      if (lnet->unknown_type(i) == 'X')
        {
          sour = true;
          break;
        }

    if (sour)
      {
        using namespace std;
        using namespace GNU_gama::local;

        const int y_sign = lnet->y_sign();

        const Vec& x = lnet->solve();
        double elp_k = 0;
        {
          double alfa = (1 - lnet->conf_pr());
          if (lnet->m_0_apriori())
            {
              elp_k = sqrt(GNU_gama::Chi_square(float(alfa), 2));
            }
          else
            {
              int n = lnet->degrees_of_freedom();
              if (n > 0)
                elp_k = sqrt( n*(pow(alfa, -2.0/n) - 1));
              else
                elp_k = 0;
            }
        }

        double mp_max = -1, mp_prum = 0;
        PointID mp_max_cb;
        int pocbod = 0;

        out << "<h3>"
            << T_GaMa_errell_review_of_mean_errors_and_error_ellipses
            << "</h3>\n";

        out << "<table id='error_ellipses'>\n";

        out << "<tr>"
            << "<th>" << T_GaMa_point << "</th>"
            << "<th>" << T_HTML_mp    << "</th>"
            << "<th>" << T_HTML_mxy   << "</th>"
            << "<th colspan='3'>" << T_HTML_mean_error_ellipse << "</th>"
            << "<th colspan='2'>" << T_HTML_confidence << "</th>"
            << "<th>g</th>"
            << "</tr>\n";

        out << "<tr>"
            << "<th></th>"
            << "<th>[mm]</th>"
            << "<th>[mm]</th>"
            << "<th colspan='2'>a [mm] b</th>"
            << "<th>alpha " << (lnet->gons() ? "[g]" : "[d]") << "</th>"
            << "<th colspan='2'>a' [mm] b'</th>"
            << "<th></th>"
            << "</tr>\n";

        const int Nell = 3; // table column separator for ellipses
        for (PointData::const_iterator
               point=lnet->PD.begin(); point!=lnet->PD.end(); ++point)
          if ((*point).second.free_xy())
            if ((*point).second.index_x())
              {
                out << "<tr>";
                const PointID point_id  = (*point).first;
                out << tdRight(point_id.str(), Nell, Nell);

                const LocalPoint& p = (*point).second;
                double my = lnet->unknown_stdev(p.index_y());
                double mx = lnet->unknown_stdev(p.index_x());
                double mp = sqrt(my*my+mx*mx);

                out << tdRight(mp, (mp<1000 ? 'F' : 'E'), 1, Nell, Nell);

                mp_prum += mp;
                if (mp > mp_max) {
                  mp_max = mp;
                  mp_max_cb = point_id;
                }
                pocbod++;

                double myx = mp/sqrt(2.0);
                out << tdRight(myx, (myx<1000 ? 'F' : 'E'), 1, Nell, Nell);

                double a, b, alfa;
                lnet->std_error_ellipse(point_id, a, b, alfa);
                out << tdRight(a, (a<1000 ? 'F' : 'E'), 1, Nell, Nell);
                out << tdRight(b, (b<1000 ? 'F' : 'E'), 1, Nell, Nell);
                double ea = alfa*R2G;
                if (lnet->degrees()) ea *= 360.0/400;
                out << tdRight(ea, (ea<1000 ? 'F' : 'E'), 1, Nell, Nell);

                if (mp < 1000 && mp > 1e-3)
                  {           // ********* testing noise (coordinates are OK)
                    double ak = a*elp_k;
                    double bk = b*elp_k;
                    out << tdRight(ak, (ak<1000 ? 'F' : 'E'), 1, Nell, Nell);
                    out << tdRight(bk, (bk<1000 ? 'F' : 'E'), 1, Nell, Nell);

                    double g  = 0;
                    double dx = x( p.index_x() );
                    double dy = y_sign*x( p.index_y() );
                    double p1 = (dx*cos(alfa) + dy*sin(alfa));
                    double p2 = (dy*cos(alfa) - dx*sin(alfa));
                    if (ak > 0 && bk > 0 && bk > ak*1e-4)
                      {           // ***** testing noise (bk is practically 0)
                        p1 /= ak;
                        p2 /= bk;
                        g = sqrt(p1*p1 + p2*p2);
                      }
                    out << tdRight(g, (g<1000 ? 'F' : 'E'), 1);
                  }

                out << "</tr>\n";
              }
        out << "</table>\n";

        if (pocbod >= 5)
          {
            out << "<p>"
                << T_GaMa_adjunk_mean_position_error_maximal
                << double2str(mp_max, 'F', 1)
                << T_GaMa_adjunk_mean_position_error_on_point
                << mp_max_cb.str() << "<br/>"
                << T_GaMa_adjunk_mean_position_error_average
                << double2str(mp_prum/pocbod, 'F', 1)
                << " mm</p>\n";
          }
      }
  }  // error ellipses
}

void GamaLocalHTML::htmlObservations()
{
  std::string& str = observations.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  HtmlStringStream out(str);

  out << "<h2>" << T_HTML_observations << "</h2>\n";

  out << "<h3>" << T_HTML_adjusted_observations << "</h3>\n";
  out << "<table id='adjusted_observations'>\n";

  HtmlAdjustedObservationsVisitor visitor(out, lnet);
  for (int i=1; i<=lnet->sum_observations(); i++)
    {
      visitor.adjustedObservation(i);
    }

  out << "</table>\n";
}

void GamaLocalHTML::htmlResiduals()
{
  std::string& str = residuals.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  HtmlStringStream out(str);

  out << "<h3>"
      << T_GaMa_resobs_Review_of_residuals_analysis_obs << "</h3>\n";
  out << "<table id='residuals'>\n";

  std::vector<int> index;
  HtmlAdjustedResidualsVisitor visitor(out, lnet, &index);
  for (int i=1; i<=lnet->sum_observations(); i++)
    {
      visitor.adjustedObservation(i);
    }
  out << "</table>\n";

  if (const int M = index.size())
    {
      out << "<h3>" << T_GaMa_resobs_Outlying_observations << "</h3>\n";
      out << "<table id='outlying_observations'>\n";

      HtmlAdjustedResidualsVisitor rvis(out, lnet);
      for (int m=0; m<M; m++)
        {
          rvis.adjustedObservation(index[m]);
        }

      out << "</table>";
    }
}

void GamaLocalHTML::htmlRejected()
{
  std::string& str = rejected.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  bool points = !lnet->removed_points.empty();
  bool obs    =  lnet->sum_rejected_observations().size() > 0;
  if (!points && !obs) return;

  str  = "<h2>" + std::string(T_HTML_rejected) + "</h2>\n";

  if (points)
    {
      str += "<h3>" + std::string(T_HTML_points) + "</h3>\n";
      str += "<table id='rejected_points'>\n";

      const char* codes[] = { "missing xyz",
                              "missing xy",
                              "missing z",
                              "singular xy",
                              "singular z",
                              "huge cov xyz",
                              "huge cov xy",
                              "huge cov z" };
      typedef std::list<LocalNetwork::rm_points> PointCodes;
      PointCodes::const_iterator j=lnet->removed_code.begin();
      for (PointIDList::const_iterator i=lnet->removed_points.begin();
           i != lnet->removed_points.end(); ++i, ++j)
        {
          if (j == lnet->removed_code.end()) break;

          PointID id = *i;
          str += "<tr>";
          str += tdRight(id.str(),0,1);
          // str += tdLeft(*j < 8 ? codes[*j] : "error : bad code");
          //
          // warning: comparison of constant 8 with expression of type
          // 'const GNU_gama::local::LocalNetwork::rm_points' is
          // always true  [-Wtautological-constant-out-of-range-compare]
          //
          str += tdLeft(codes[*j]);
          str += "</tr>\n";
        }
      str += "</table>\n";
    }

  using namespace GNU_gama::local;

  if (obs)
    {
      str += "<h3>" + std::string(T_HTML_observations) + "</h3>\n";
      str += "<table id='rejected_observations'>\n";

      for (ObservationList::const_iterator
             i = lnet->sum_rejected_observations().begin(),
             e = lnet->sum_rejected_observations().end();
             i !=e; ++i
           )
        {
          Observation* obs = const_cast<Observation*>(*i);
          std::ostringstream out;
          out.setf(std::ios_base::fixed, std::ios_base::floatfield);
          WriteVisitor<std::ostringstream> write_visitor(out, true);
          obs->accept(&write_visitor);

          str += "<tr>" + tdLeft(out.str()) + "</tr>\n";
        }
      str += "</table>\n";

    }
}

void GamaLocalHTML::htmlEnd()
{
  end.str = "</body>\n</html>\n";
}

void GamaLocalHTML::htmlTerms()
{
  std::string& str = terms.str;
  str.clear();
  if (!lnet->is_adjusted()) return;

  const int  iterations = lnet->linearization_iterations();
  const bool lintest    = TestLinearization(lnet);

  if (iterations > 0 || lintest)
    {
      str += "<h2>" + std::string(T_GaMa_linearization) + "</h2>\n";
      str += "<table id='linearization_iterations'>\n";
      str += "<tr><td>";
      str += T_GaMa_Number_of_linearization_iterations;
      str += "</td><td>" + std::to_string(iterations) + "</td></tr></table>\n";
    }

  if (!lintest) return;

  str += "<table id='linearization_errors'>";
  str += "<tr>";
  str += "<th colspan='4'>"
      + std::string(T_GaMa_tstlin_Test_of_linearization_error) + "</th>";
  str += "<th>observed</th>";
  str += "<th>r</th><th colspan='2'>difference</th>";
  str += "</tr>\n";
  str += "<tr>";
  str += "<th>i</th>";
  str += "<th>" + std::string(T_GaMa_standpoint) + "</th>";
  str += "<th>" + std::string(T_GaMa_target) + "</th>";
  str += "<th></th>";
  str += "<th>value</th>";
  if (lnet->gons())
     str += "<th>[mm|cc]</th><th>[cc]</th><th>[mm]</th>";
  else
     str += "<th>[mm|ss]</th><th>[ss]</th><th>[mm]</th>";
  str += "</tr>\n";


  const double max_dif = 0.0005;
  const int M = lnet->sum_observations();
  Vec dif_m(M);   // difference in computation of adjusted observation
  Vec dif_p(M);   //               corresponds to positional shift
  const Vec& v = lnet->residuals();
  const Vec& x = lnet->solve();
  TestLinearizationVisitor testVisitor(lnet, v, x);

  for (int i=1; i<=M; i++)
    {
      dif_m(i) = dif_p(i) = 0;
      double  mer = 0, pol = 0;
      Observation* pm = lnet->ptr_obs(i);
      // special case for coordinates
      if (dynamic_cast<const Coordinates*>(pm->ptr_cluster())) continue;

        testVisitor.setObservationIndex(i);
        pm->accept(&testVisitor);

        mer = testVisitor.getMer();
        pol = testVisitor.getPol();
        dif_m(i) = mer;
        dif_p(i) = pol;
    }

  std::ostringstream out;
  TestLinearizationWriteVisitor<std::ostringstream>
    writeVisitor(out, lnet);
  std::string predcs {};
  for (int i=1; i<=M; i++)
    {
      if (std::abs(dif_p(i)) < max_dif) continue;

      Observation* pm = lnet->ptr_obs(i);
      str += "<tr>";
      str += tdRight(std::to_string(i));
      std::string from = pm->from().str();
      std::string tmp = from;
      if (from == predcs) tmp.clear();
      // str += "<td>" + (from != predcs) ? from : std::string() + "</td>";
      str += tdRight(tmp);
      predcs = from;
      str += tdRight(pm->to().str());

      out.setf(std::ios_base::fixed, std::ios_base::floatfield);
      pm->accept(&writeVisitor);
      double mer = writeVisitor.getMer();
      bool   dms = writeVisitor.getDms();
      str += tdLeft(out.str());
      out.str(std::string());

      if (!dms) out << mer;
      else      out << GNU_gama::gon2deg(mer, 1, 1);
      str += tdRight(out.str());
      out.str(std::string());

      out.precision(3);
      if (!dms) out << v(i);
      else      out << v(i)*0.324;
      str += tdRight(out.str());
      out.str(std::string());

      if (pm->angular())
        {
          out.precision(3);
          if (!dms)
            out << dif_m(i);
          else
            out << dif_m(i)*0.324;
        }
      str += tdRight(out.str());
      out.str(std::string());

      out.precision(3);
      out << dif_p(i);
      str += tdRight(out.str());
      out.str(std::string());

      str += "</tr>\n";
    }
  str += "</table>\n";

}

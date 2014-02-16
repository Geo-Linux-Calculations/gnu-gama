/*
    GNU Gama C++ library
    Copyright (C) 2006, 2010  Ales Cepek <cepek@gnu.org>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>
                  2012, 2013, 2014  Ales Cepek <cepek@gnu.org>

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

/** \file localnetworkxml.cpp
 * \brief #GNU_gama::LocalNetworkXML implementation
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <gnu_gama/xml/localnetworkxml.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/version.h>
#include <gnu_gama/xsd.h>

using namespace std;
using GNU_gama::LocalNetworkXML;
using GNU_gama::local::PointData;
using GNU_gama::local::LocalPoint;
using GNU_gama::local::StandPoint;
using GNU_gama::local::PointID;
using GNU_gama::local::Observation;
using GNU_gama::local::Distance;
using GNU_gama::local::Direction;
using GNU_gama::local::X;
using GNU_gama::local::Y;
using GNU_gama::local::Z;
using GNU_gama::local::Angle;
using GNU_gama::local::H_Diff;
using GNU_gama::local::Z_Angle;
using GNU_gama::local::S_Distance;
using GNU_gama::local::Xdiff;
using GNU_gama::local::Ydiff;
using GNU_gama::local::Zdiff;
using GNU_gama::local::Azimuth;

namespace
{
  // maximal possible precision needed by 'make check' test rule
  int make_check_precision(int) { return 16; }
}

/** \brief Writes observations XML representation to stream.
 *
 * \todo Remove secondary output stream.
 */
class WriteXMLVisitor : public GNU_gama::local::AllObservationsVisitor
{
private:
    std::ostream& out;
    std::ostream* ostr;
    std::string tag;
    const int linear;
    const int angular;
    const GNU_gama::local::Vec& v;
    GNU_gama::Index i;
    const int y_sign;
public:
    WriteXMLVisitor(std::ostream& outStream, int linearOutputPrecision, int angularOutputPrecision,
                    const GNU_gama::local::Vec& residuals, int ySign)
        : out(outStream), ostr(&outStream),
          linear(linearOutputPrecision), angular(angularOutputPrecision),
          v(residuals), y_sign(ySign)
    {
    }

    std::string lastTag() const { return tag; }

    /** \brief Sets index of observation which will be used in the next visit. */
    void setObservationIndex(GNU_gama::Index index) { i = index; }

    /** If secondary output stream is not specified output stream given in constructor is used. */
    void setSecondaryOutStream(std::ostream& outStream) { ostr = &outStream; }

    void visit(Distance* obs)
    {
      out << "<" << (tag="distance") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Direction* obs)
    {
      out << "<" << (tag="direction") << ">";
      ostr->precision(angular);
      double m = R2G*(obs->value());
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/10000;
      if (m < 0) m += 400;
      if (m >= 400) m -= 400;
      *ostr << " <adj>" <<  m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Angle* obs)
    {
      out << "<" << (tag="angle") << ">";
      ostr->precision(angular);
      double m = R2G*(obs->value());
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/10000;
      if (m < 0) m += 400;
      if (m >= 400) m -= 400;
      *ostr << "<adj>" << m << "</adj>";

      out << " <from>"  << obs->from() << "</from>"
          << " <left>"  << obs->bs()   << "</left>"
          << " <right>" << obs->fs()   << "</right>\n";
    }
    void visit(H_Diff* obs)
    {
      out << "<" << (tag="height-diff") << ">";
      ostr->precision(linear);
      double m =obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(S_Distance* obs)
    {
      out << "<" << (tag="slope-distance") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Z_Angle* obs)
    {
      out << "<" << (tag="zenith-angle") << ">";
      ostr->precision(angular);
      double m = R2G*(obs->value());
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/10000;
      *ostr << "<adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(X* obs)
    {
      out << "<" << (tag="coordinate-x") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << "<adj>" << m << "</adj>";

      tag_id(obs);
    }
    void visit(Y* obs)
    {
      out << "<" << (tag="coordinate-y") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << y_sign*m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << y_sign*m << "</adj>";

      tag_id(obs);
    }
    void visit(Z* obs)
    {
      out << "<" << (tag="coordinate-z") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_id(obs);
    }
    void visit(Xdiff* obs)
    {
      out << "<" << (tag="dx") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Ydiff* obs)
    {
      out << "<" << (tag="dy") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << y_sign*m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << y_sign*m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Zdiff* obs)
    {
      out << "<" << (tag="dz") << ">";
      ostr->precision(linear);
      double m = obs->value();
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/1000;
      *ostr << " <adj>" << m << "</adj>";

      tag_from_to(obs);
    }
    void visit(Azimuth* obs)
    {
      out << "<" << (tag="azimuth") << ">";
      ostr->precision(angular);
      double m = R2G*(obs->value());
      *ostr << " <obs>" << m << "</obs>";
      m += v(i)/10000;
      if (m < 0) m += 400;
      if (m >= 400) m -= 400;
      *ostr << " <adj>" <<  m << "</adj>";

      tag_from_to(obs);
    }

    void tag_id(const GNU_gama::local::Observation* obs)
    {
        out << " <id>" << obs->from() << "</id>\n";
    }

    void tag_from_to(const GNU_gama::local::Observation* obs)
    {
        out << " <from>" << obs->from() << "</from>"
            << " <to>"   << obs->to()   << "</to>\n";
    }

};


void LocalNetworkXML::write(std::ostream& out) const
{
  out << "<?xml version=\"1.0\"?>\n"
      << "<gama-local-adjustment "
      << "xmlns=\"" << XSD_GAMA_LOCAL_ADJUSTMENT << "\">\n";

  out << "\n<description>" << netinfo->description << "</description>\n";

  {
    {
      using GNU_gama::GNU_gama_version;
      using GNU_gama::GNU_gama_compiler;

      out << "\n<network-general-parameters\n";

      out << "   gama-local-version=\""   << GNU_gama_version    << "\"\n";
      out << "   gama-local-algorithm=\"" << netinfo->algorithm()<< "\"\n";
      out << "   gama-local-compiler=\""  << GNU_gama_compiler   << "\"\n";

      out << "   axes-xy=\"";
      switch (netinfo->PD.local_coordinate_system)
        {
        case   1: out << "en"; break;
        case   2: out << "nw"; break;
        case   4: out << "se"; break;
        case   8: out << "ws"; break;
        case  16: out << "ne"; break;
        case  32: out << "sw"; break;
        case  64: out << "es"; break;
        case 128: out << "wn"; break;
        default : break;
        }
      out << "\"\n";

      out << "   angles=\""
          << (netinfo->PD.right_handed_angles() ?
                                "right-handed" : "left-handed")
          << "\"\n";

      out.setf(ios_base::fixed, ios_base::floatfield);
      out.precision(7);

      // nullable data

      if (netinfo->has_epoch())
        out << "   epoch=\""<< netinfo->epoch() << "\"\n";

      if (netinfo->has_latitude())
        out << "   latitude=\"" << netinfo->latitude()/M_PI*200 << "\"\n";

      if (netinfo->has_ellipsoid())
        out << "   ellipsoid=\"" << netinfo->ellipsoid() << "\"\n";

      out << "/>\n";
    }

    out.setf(ios_base::scientific, ios_base::floatfield);
    out.precision(7);

    out << "\n<network-processing-summary>\n";

    coordinates_summary(out);
    observations_summary(out);
    equations_summary(out);
    std_dev_summary(out);

    out << "\n</network-processing-summary>\n";


  }

  coordinates(out);
  observations(out);

  out << "\n</gama-local-adjustment>\n";
}

void LocalNetworkXML::coordinates_summary(std::ostream& out) const
{
  out << "\n<coordinates-summary>\n";

  // summary of coordinates in adjustment

  int a_xyz = 0, a_xy = 0, a_z = 0;      // adjusted
  int c_xyz = 0, c_xy = 0, c_z = 0;      // constrained
  int f_xyz = 0, f_xy = 0, f_z = 0;      // fixed

  for (PointData::const_iterator
         i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
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

  out << "   <coordinates-summary-adjusted>    ";
  tagsp(out, "count-xyz", a_xyz);
  tagsp(out, "count-xy" , a_xy );
  tagsp(out, "count-z"  , a_z  );
  out << "</coordinates-summary-adjusted>\n";

  out << "   <coordinates-summary-constrained> ";
  tagsp(out, "count-xyz", c_xyz);
  tagsp(out, "count-xy" , c_xy );
  tagsp(out, "count-z"  , c_z  );
  out << "</coordinates-summary-constrained>\n";

  out << "   <coordinates-summary-fixed>       ";
  tagsp(out, "count-xyz", f_xyz);
  tagsp(out, "count-xy" , f_xy );
  tagsp(out, "count-z"  , f_z  );
  out << "</coordinates-summary-fixed>\n";

  out << "</coordinates-summary>\n";
}


void LocalNetworkXML::observations_summary(std::ostream& out) const
{
  out << "\n<observations-summary>\n";

  /** Local helper visitor class for counting observations.
    *
    * \todo Consider generalizing
    * (constructor can take parameters which determines observations kinds).
    * It  can be used in function GeneralParameters.
    */
  class ObservationSummaryCounter : public GNU_gama::local::AllObservationsVisitor
  {
  public:
      ObservationSummaryCounter() :
          dirs(0),  angles(0), dists(0), coords(0),
          hdiffs(0), zangles(0), chords(0), vectors(0), azimuth(0)
      {}

      void visit(Direction*)  { dirs++; }
      void visit(Distance*)   { dists++; }
      void visit(Angle*)      { angles++; }
      void visit(H_Diff*)     { hdiffs++; }
      void visit(S_Distance*) { chords++; }
      void visit(Z_Angle*)    { zangles++; }
      void visit(X*)          { coords++; }
      void visit(Y*)          { }
      void visit(Z*)          { }
      void visit(Xdiff*)      { vectors++; }
      void visit(Ydiff*)      { }
      void visit(Zdiff*)      { }
      void visit(Azimuth*)    { azimuth++; }

      int dirs,  angles, dists, coords,
        hdiffs, zangles, chords, vectors,
        azimuth;
  };

  ObservationSummaryCounter counter;

  for (int i=1; i<=netinfo->sum_observations(); i++)
      netinfo->ptr_obs(i)->accept(&counter);

  tagnl(out, "distances",  counter.dists);
  tagnl(out, "directions", counter.dirs);
  tagnl(out, "angles",     counter.angles);
  tagnl(out, "xyz-coords", counter.coords);
  tagnl(out, "h-diffs",    counter.hdiffs);
  tagnl(out, "z-angles",   counter.zangles);
  tagnl(out, "s-dists",    counter.chords);
  tagnl(out, "vectors",    counter.vectors);
  tagnl(out, "azimuths",   counter.azimuth);

  out << "</observations-summary>\n";

  // int bearings = 0;
  // for (int i=1; i<=netinfo->sum_unknowns(); i++)
  //   if (netinfo->unknown_type(i) == 'R')
  //     bearings++;
}


template <typename T>
void LocalNetworkXML::tagnl(std::ostream& out, const char* t, T n) const
{
  out << "   <" << t << ">" << n << "</" << t << ">\n";;
}


template <typename T>
void LocalNetworkXML::tagsp(std::ostream& out, const char* t, T n) const
{
  out << "<" << t << ">" << n << "</" << t << "> ";
}


void LocalNetworkXML::equations_summary(std::ostream& out) const
{
  out << "\n<project-equations>\n";

  const int obs = netinfo->sum_observations();
  const int par = netinfo->sum_unknowns();
  const int dof = netinfo->degrees_of_freedom();
  const int nul = netinfo->null_space();

  tagnl(out, "equations",          obs);
  tagnl(out, "unknowns",           par);
  tagnl(out, "degrees-of-freedom", dof);
  tagnl(out, "defect",             nul);

  out.setf(ios_base::scientific, ios_base::floatfield);
  out.precision(7);
  tagnl(out, "sum-of-squares", netinfo->trans_VWV());

  if (netinfo->connected_network())
    out << "   <connected-network/>\n";
  else
    out << "   <disconnected-network/>\n";

  out << "</project-equations>\n";
}


void LocalNetworkXML::std_dev_summary(std::ostream& out) const
{
    out << "\n<standard-deviation>\n";

    tagnl(out, "apriori", netinfo->apriori_m_0());
    tagnl(out, "aposteriori",
        (netinfo->degrees_of_freedom() > 0 ?
         sqrt(netinfo->trans_VWV()/netinfo->degrees_of_freedom()) : 0));
    tagnl(out, "used",
        (netinfo->m_0_aposteriori() ?
         string("aposteriori") :
         string("apriori") ));

    out << "\n";

    out.setf(ios_base::fixed, ios_base::floatfield);
    out.precision(3);
    tagnl(out, "probability", netinfo->conf_pr());

    const int dof = netinfo->degrees_of_freedom();
    float test=0, lower=0, upper=0;

    test  = netinfo->m_0_aposteriori_value() / netinfo->apriori_m_0();
    if (dof)
      //if (netinfo->m_0_aposteriori())
        {
          const double alfa_pul = (1 - netinfo->conf_pr())/2;
          lower = sqrt(GNU_gama::Chi_square(1-alfa_pul,dof)/dof);
          upper = sqrt(GNU_gama::Chi_square(  alfa_pul,dof)/dof);
        }
      /* else
        {
          out << "   <!-- no test for apriori standard deviation -->\n";
        } */

    tagnl(out, "ratio",  test);
    tagnl(out, "lower",  lower);
    tagnl(out, "upper",  upper);
    //if (lower < test && test < upper || netinfo->m_0_apriori())
    if (lower < test && test < upper)
      out << "   <passed/>\n\n";
    else
      out << "   <failed/>\n\n";

    out.setf(ios_base::scientific, ios_base::floatfield);
    out.precision(7);
    tagnl(out, "confidence-scale",  netinfo->conf_int_coef());

    out << "</standard-deviation>\n";
}


void LocalNetworkXML::coordinates(std::ostream& out) const
{
  const int y_sign = GaMaConsistent(netinfo->PD) ? +1 : -1;

  out << "\n<coordinates>\n";

  out.setf(ios_base::fixed, ios_base::floatfield);
  out.precision(6);

  const GNU_gama::local::Vec& X = netinfo->solve();
  std::vector<Index> ind(netinfo->sum_unknowns() + 1);
  Index dim = 0;


  out << "\n<fixed>\n";

  for (PointData::const_iterator
         i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (!p.active_xy() && !p.active_z()) continue;
      bool bxy = p.active_xy() && p.index_x() == 0;
      bool bz  = p.active_z () && p.index_z() == 0;
      if (!bxy && !bz) continue;
      out << "   <point> ";
      tagsp(out, "id", (*i).first);
      if (bxy)
        {
          const double x = (p.x()+X(p.index_x())/1000);
          const double y = (p.y()+X(p.index_y())/1000)*y_sign;
          tagsp(out, "x", x);
          tagsp(out, "y", y);
        }
      if (bz)
        {
          const double z = (p.z()+X(p.index_z())/1000);
          tagsp(out, "z", z);
        }
      out << "</point>\n";
    }

  out << "</fixed>\n";


  out << "\n<approximate>\n";

  for (PointData::const_iterator
         i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (!p.active_xy() && !p.active_z()) continue;
      bool bxy = p.active_xy() && p.index_x() != 0;
      bool bz  = p.active_z () && p.index_z() != 0;
      if (!bxy && !bz) continue;
      out << "   <point> ";
      tagsp(out, "id", (*i).first);
      if (bxy)
        {
          const char* cx = "x";
          const char* cy = "y";
          if (p.constrained_xy())
            {
              cx = "X";
              cy = "Y";
            }
          const double x = p.x();
          const double y = p.y()*y_sign;
          tagsp(out, cx, x);
          tagsp(out, cy, y);
        }
      if (bz)
        {
          const char* cz = "z";
          if (p.constrained_z())
            {
              cz = "Z";
            }
          const double z = p.z();
          tagsp(out, cz, z);
        }
      out << "</point>\n";
    }
  out << "</approximate>\n";


  out << "\n<!-- capital X,Y,Z denote constrained coordinates -->\n"
      << "<adjusted>\n";

  out.precision(make_check_precision(6));

  for (PointData::const_iterator
         i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (!p.active_xy() && !p.active_z()) continue;
      bool bxy = p.active_xy() && p.index_x() != 0;
      bool bz  = p.active_z () && p.index_z() != 0;
      if (!bxy && !bz) continue;
      out << "   <point> ";
      tagsp(out, "id", (*i).first);
      if (bxy)
        {
          const char* cx = "x";
          const char* cy = "y";
          if (p.constrained_xy())
            {
              cx = "X";
              cy = "Y";
            }
          const double x = (p.x()+X(p.index_x())/1000);
          const double y = (p.y()+X(p.index_y())/1000)*y_sign;
          tagsp(out, cx, x);
          tagsp(out, cy, y);
          ind[++dim] = p.index_x();
          ind[++dim] = p.index_y();
        }
      if (bz)
        {
          const char* cz = "z";
          if (p.constrained_z())
            {
              cz = "Z";
            }
          const double z = (p.z()+X(p.index_z())/1000);
          tagsp(out, cz, z);
          ind[++dim] = p.index_z();
        }
      out << "</point>\n";
    }
  out << "</adjusted>\n";

  orientation_shifts(out, ind, dim);

  int band = 0;   // signed value, must not be declared as Index
  if (dim)
    {
      band = netinfo->xml_covband();
      if (band == -1 || band > dim-1) band = dim - 1;
    }
  out << "\n<!-- upper part of symmetric matrix band by rows -->\n"
      << "<cov-mat>\n"
      << "<dim>"  << dim  << "</dim> "
      << "<band>" << band << "</band>\n";

  out.setf(ios_base::scientific, ios_base::floatfield);
  out.precision(7);
  const double m2 = netinfo->m_0() * netinfo->m_0();
  for (Index k=0, i=1; i<=dim; i++)
    for (Index j=i; j<=std::min(dim, i+band); j++)
      {
        out << "<flt>" << m2*netinfo->qxx(ind[i], ind[j]) << "</flt>";
        if (++k == 3)
          {
            k = 0;
            out << "\n";
          }
        else
          {
            out << " ";
          }
      }

  out << "</cov-mat>\n";


  out << "\n<!-- original indexes from the adjustment -->\n"
      << "<original-index>\n";

  for (PointData::const_iterator
         i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (!p.active_xy() && !p.active_z()) continue;
      const bool bxy = p.active_xy() && p.index_x() != 0;
      const bool bz  = p.active_z () && p.index_z() != 0;
      if (bxy) out << "<ind>" << p.index_x() << "</ind>\n";
      if (bxy) out << "<ind>" << p.index_y() << "</ind>\n";
      if (bz ) out << "<ind>" << p.index_z() << "</ind>\n";
    }

  for (int i=1; i<=netinfo->sum_unknowns(); i++)
    if (netinfo->unknown_type(i) == 'R')
      {
        out << "<ind>" << i << "</ind>\n";
      }

  out << "</original-index>\n";

  out << "\n</coordinates>\n";
}


void  LocalNetworkXML::orientation_shifts(std::ostream& out,
                                          std::vector<GNU_gama::Index>& ind,
                                          GNU_gama::Index& dim) const
{
  out << "\n<orientation-shifts>\n";

  const GNU_gama::local::Vec& X = netinfo->solve();
  //const double scale    = netinfo->gons() ? 1.0 : 0.324;
  const int    y_sign   = GaMaConsistent(netinfo->PD) ? +1 : -1;
  //const double kki      = netinfo->conf_int_coef();
  const int    unknowns = netinfo->sum_unknowns();

  for (int i=1; i<=unknowns; i++)
    if (netinfo->unknown_type(i) == 'R')
      {
        out << "   <orientation> ";
        const PointID cb = netinfo->unknown_pointid(i);
        tagsp(out, "id", cb.str().c_str());

        StandPoint* k = netinfo->unknown_standpoint(i);
        ind[++dim] =  k->index_orientation();

        double z = y_sign*( k->orientation() )*R2G;
        if (z <  0 ) z += 400;
        if (z > 400) z -= 400;
        out.setf(ios_base::fixed, ios_base::floatfield);
        out.precision(6);
        tagsp(out, "approx", z);

        double cor = y_sign*X(i)/10000;
        // out << cor << " ";
        z += cor;
        if (z <  0 ) z += 400;
        if (z > 400) z -= 400;
        tagsp(out, "adj", z);

        // out.precision(3);
        // out.width(8);
        // double mz = netinfo->unknown_stdev(i)*scale;
        // out << " stdev=\"" << mz << "\" ";

        // out << mz*kki;

        out << "</orientation>\n";
      }

  out << "</orientation-shifts>\n";
}


void LocalNetworkXML::observations(std::ostream& out) const
{
  out << "\n<observations>\n\n";

   using namespace std;
   // using namespace GNU_gama::local;

   const int      y_sign = GaMaConsistent(netinfo->PD) ? +1 : -1;
   const GNU_gama::local::Vec& v = netinfo->residuals();
   const int      pocmer = netinfo->sum_observations();
   const double   scale  = netinfo->gons() ? 1.0 : 0.324;
   const double   kki    = netinfo->conf_int_coef();

   const int linear  =  make_check_precision(6);    // output precision
   const int angular =  make_check_precision(7);    // output precision


   WriteXMLVisitor writeVisitor(out, linear, angular, v, y_sign);

   PointID predcs = "";   // provious standpoint ID
   for (int i=1; i<=pocmer; i++)
     {
       Observation* pm = netinfo->ptr_obs(i);
       // bool isangle    = false;

       Angle* u = 0;
       bool xyz = false;
       const char* tag = 0;

       ostringstream ostr;
       ostr.setf(ios_base::fixed, ios_base::floatfield);

       writeVisitor.setSecondaryOutStream(ostr);
       writeVisitor.setObservationIndex(i);

       pm->accept(&writeVisitor);

       out << "  " << ostr.str();
       //out << "\n";

       out.setf(ios_base::fixed, ios_base::floatfield);
       out.precision(3);
       out.width(7);


       double ml = netinfo->stdev_obs(i);

       if (dynamic_cast<Direction*>(pm))
         ml *= scale;
       else if (dynamic_cast<Angle*>(pm))
         ml *= scale;
       else if (dynamic_cast<Z_Angle*>(pm))
         ml *= scale;

       out.precision(make_check_precision(3));
       out << " <stdev>" << ml << "</stdev>\n";
       out.precision(3);

       // weight coefficient of the residual
       double qrr = netinfo->wcoef_res(i);
       out << "   <qrr>" << qrr << "</qrr>";

       double f = netinfo->obs_control(i);
       out << " <f>" << f << "</f>";

       double sc=scale;
       if (f >= 0.1)
         {
           using namespace std;
           double no = fabs(netinfo->studentized_residual(i));
           out << " <std-residual>" << no << "</std-residual>";

           if ( (pm->ptr_cluster())->covariance_matrix.bandWidth() == 0 &&
                (f >=5 || (f >= 0.1 && no > kki)))
             {
               double em = v(i)/(netinfo->wcoef_res(i)*netinfo->weight_obs(i));
               out << "\n   <err-obs>" << em*sc << "</err-obs>";

               double ev = em - v(i);
               out << " <err-adj>" << ev*sc << "</err-adj>";
             }
         }

       out << "\n   </" << writeVisitor.lastTag() << ">\n";

   } // end for

  out << "\n</observations>\n";
}

/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2003, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/g3/g3_point.h>
#include <gnu_gama/g3/g3_observation.h>
#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/latlong.h>
#include <gnu_gama/radian.h>
#include <cmath>
#include <iomanip>


using namespace GNU_gama::g3;

using GNU_gama::latitude;
using GNU_gama::longitude;

using std::setw;

Point::Point()
  : B(B_), L(L_), H(H_), X(X_), Y(Y_), Z(Z_)
{
  set_unused();

  has_xyz_ = has_blh_ = has_height_ = has_geoid_ = false;

  N     .set_owner(this);
  E     .set_owner(this);
  U     .set_owner(this);
  height.set_owner(this);
  geoid .set_owner(this);
  dB    .set_owner(this);
  dL    .set_owner(this);
}

Point::Point(const Point& point)
  : B(B_), L(L_), H(H_), X(X_), Y(Y_), Z(Z_)
{
  point_copy(point);
}

Point& Point::operator=(const Point& point)
{
  if (this != &point)
    {
      point_copy(point);
    }

  return *this;
}

void Point::point_copy(const Point& point)
{
  name   = point.name;
  common = point.common;

  N = point.N;
  E = point.E;
  U = point.U;

  height = point.height;
  geoid  = point.geoid;
  dB     = point.dB;
  dL     = point.dL;

  X_ = point.X_;
  Y_ = point.Y_;
  Z_ = point.Z_;

  has_xyz_    = point.has_xyz_;
  has_blh_    = point.has_blh_;
  has_height_ = point.has_height_;
}

void Point::set_unused()
{
  N.set_unused();
  E.set_unused();
  U.set_unused();
}

void Point::set_fixed_horizontal_position()
{
  N.set_fixed();
  E.set_fixed();
}

void Point::set_fixed_height()
{
  U.set_fixed();
}

void Point::set_fixed_position()
{
  N.set_fixed();
  E.set_fixed();
  U.set_fixed();
}

void Point::set_free_horizontal_position()
{
  N.set_free();
  E.set_free();
}

void Point::set_free_height()
{
  U.set_free();
}

void Point::set_free_position()
{
  N.set_free();
  E.set_free();
  U.set_free();
}

void Point::set_constr_horizontal_position()
{
  N.set_constr();
  E.set_constr();
}

void Point::set_constr_height()
{
  U.set_constr();
}

void Point::set_constr_position()
{
  N.set_constr();
  E.set_constr();
  U.set_constr();
}

bool Point::unused() const
{
  return N.unused() && E.unused() && U.unused();
}

bool Point::fixed_horizontal_position() const
{
  return N.fixed() && E.fixed();
}

bool Point::fixed_height() const
{
  return U.fixed();
}

bool Point::fixed_position() const
{
  return N.fixed() && E.fixed() && U.fixed();
}

bool Point::free_horizontal_position() const
{
  return N.free() && E.free();
}

bool Point::free_height() const
{
  return U.free();
}

bool Point::free_position() const
{
  return N.free() && E.free() && U.free();
}

bool Point::constr_horizontal_position() const
{
  return N.constr() && E.constr();
}

bool Point::constr_height() const
{
  return U.constr();
}

bool Point::constr_position() const
{
  return N.constr() && E.constr() && U.constr();
}

void Point::set_blh(double b, double l, double h)
{
  has_blh_ = true;
  has_xyz_ = false;

  B_.set_init_value(b);
  L_.set_init_value(l);
  H_.set_init_value(h);

  double x, y, z;
  common->ellipsoid.blh2xyz(b, l, h, x, y, z);
  X_.set_init_value(x);
  Y_.set_init_value(y);
  Z_.set_init_value(z);

  transformation_matrix(b, l);
}

void Point::set_xyz(double x, double y, double z)
{
  has_xyz_ = true;
  has_blh_ = false;

  X_.set_init_value(x);
  Y_.set_init_value(y);
  Z_.set_init_value(z);

  double b, l, h;
  common->ellipsoid.xyz2blh(x, y, z, b, l, h);
  B_.set_init_value(b);
  L_.set_init_value(l);
  H_.set_init_value(h);

  transformation_matrix(b, l);
}

void Point::set_height(double h)
{
  has_height_ = true;
  height.set_init_value(h);
}

void Point::set_geoid(double h)
{
  has_geoid_ = true;
  geoid.set_init_value(h);
  geoid.set_fixed();
}

//  R is the transformation matrix  NEU --> XYZ  (XYZ = R * NEU)
//  R is orthogonal ==> inv(R) == trans(R)

void Point::transformation_matrix(double b, double l)
{
  using namespace std;

  r11 = -sin(b)*cos(l);
  r12 = -sin(l);
  r13 =  cos(b)*cos(l);

  r21 = -sin(b)*sin(l);
  r22 =  cos(l);
  r23 =  cos(b)*sin(l);

  r31 = cos(b);
  r32 = 0.0;
  r33 = sin(b);
}

double Point::x_transform(double n, double e, double u)
{
  return r11*n + r12*e + r13*u;
}

double Point::y_transform(double n, double e, double u)
{
  return r21*n + r22*e + r23*u;
}

double Point::z_transform(double n, double e, double u)
{
  return r31*n + r32*e + r33*u;
}

void Point::set_diff_XYZ(double dx, double dy, double dz)
{
  dX = dx;  dY = dy;  dZ = dz;
}

double Point::diff_N() const
{
  return r11*dX + r21*dY + r31*dZ;
}

double Point::diff_E() const
{
  return r12*dX + r22*dY + r32*dZ;
}

double Point::diff_U() const
{
  return r13*dX + r23*dY + r33*dZ;
}

void Point::set_cov_neu()
{
  cnn = cne = cnu = cee = ceu = cuu = 0.0;
  int n = N.index();
  int e = E.index();
  int u = U.index();
  if (n)
    {
      cnn = common->cov_xx(n,n);
      if (e) cne = common->cov_xx(n,e);
      if (u) cnu = common->cov_xx(n,u);
    }
  if (e)
    {
      cee = common->cov_xx(e,e);
      if (u) ceu = common->cov_xx(e,u);
    }
  if (u)
    {
      cuu = common->cov_xx(u,u);
    }
}

void Point::set_cov_xyz()
{
  const double t11 = r11*cnn + r12*cne + r13*cnu;
  const double t12 = r11*cne + r12*cee + r13*ceu;
  const double t13 = r11*cnu + r12*ceu + r13*cuu;

  const double t21 = r21*cnn + r22*cne + r23*cnu;
  const double t22 = r21*cne + r22*cee + r23*ceu;
  const double t23 = r21*cnu + r22*ceu + r23*cuu;

  const double t31 = r31*cnn + r32*cne + r33*cnu;
  const double t32 = r31*cne + r32*cee + r33*ceu;
  const double t33 = r31*cnu + r32*ceu + r33*cuu;

  cxx = t11*r11 + t12*r12 + t13*r13;
  cxy = t11*r21 + t12*r22 + t13*r23;
  cxz = t11*r31 + t12*r32 + t13*r33;

  cyy = t21*r21 + t22*r22 + t23*r23;
  cyz = t21*r31 + t22*r32 + t23*r33;

  czz = t31*r31 + t32*r32 + t33*r33;
}

bool Point::test_model_height() const
{
  if (1)
    {
      if (!has_position()) return false;
      if (!has_geoid()   ) return false;
    }
  else
    {
      if (!has_height()) return false;
    }

  return true;
}

double Point::model_height() const
{
  if (1)
    return H() - geoid();
  else
    return height();
}

void Point::write_xml(std::ostream& ostr)
{
  const double n = N();
  const double e = E();
  const double u = U();

  X_.set_correction(x_transform(n, e, u));
  Y_.set_correction(y_transform(n, e, u));
  Z_.set_correction(z_transform(n, e, u));

  ostr << "\n<point> ";
  ostr << "<id> " << name << " </id>\n\n";

  ostr.setf(std::ios_base::fixed, std::ios_base::floatfield);
  ostr.precision(3);

  ostr << "        ";
  if (N.fixed())
    ostr << "<n-fixed/>  ";
  else if (N.constr())
    ostr << "<n-constr/> ";
  else if (N.free())
    ostr << "<n-free/>   ";
  else
    ostr << "<n-unused/> ";
  if (N.index())
    {
      ostr << "<dn>"  << setw(8) << N()*1000  << " </dn> "
           << "<ind>" << N.index() << "</ind> ";
    }
  ostr << "\n";

  ostr << "        ";
  if (E.fixed())
    ostr << "<e-fixed/>  ";
  else if (E.constr())
    ostr << "<e-constr/> ";
  else if (E.free())
    ostr << "<e-free/>   ";
  else
    ostr << "<e-unused/> ";
  if (E.index())
    {
      ostr << "<de>"  << setw(8) << E()*1000  << " </de> "
           << "<ind>" << E.index() << "</ind> ";
    }
  ostr << "\n";

  ostr << "        ";
  if (U.fixed())
    ostr << "<u-fixed/>  ";
  else if (U.constr())
    ostr << "<u-constr/> ";
  else if (U.free())
    ostr << "<u-free/>   ";
  else
    ostr << "<unused/> ";
  if (U.index())
    {
      ostr << "<du>"  << setw(8) << U()*1000  << " </du> "
           << "<ind>" << U.index() << "</ind> ";
    }
  ostr << "\n";

  if (!fixed_position())
    {
      set_cov_neu();

      ostr.setf(std::ios_base::scientific, std::ios_base::floatfield);
      ostr.precision(7);
      ostr << "\n";
      ostr << "        ";
      ostr << "<cnn> " << cnn << " </cnn> ";
      ostr << "<cne> " << setw(14) << cne << " </cne> ";
      ostr << "\n                                   ";
      ostr << "<cnu> " << setw(14) << cnu << " </cnu>\n";
      ostr << "        ";
      ostr << "<cee> " << cee << " </cee> ";
      ostr << "<ceu> " << setw(14) << ceu << " </ceu>\n";
      ostr << "        ";
      ostr << "<cuu> " << cuu << " </cuu>\n";
    }

  if (has_position())
    {
      ostr.setf(std::ios_base::fixed, std::ios_base::floatfield);
      ostr.precision(5);
      ostr << "\n";
      ostr << "        <x-given     >";
      ostr << setw(19) << X.init_value();
      ostr << " </x-given>\n";
      if (!fixed_position())
        {
          ostr << "        <x-correction>";
          ostr << setw(19) << X.correction();
          ostr << " </x-correction>\n";
          ostr << "        <x-adjusted  >";
          ostr << setw(19) << X();
          ostr << " </x-adjusted>\n";
          ostr << "\n";
        }
      ostr << "        <y-given     >";
      ostr << setw(19) << Y.init_value();
      ostr << " </y-given>\n";
      if (!fixed_position())
        {
          ostr << "        <y-correction>";
          ostr << setw(19) << Y.correction();
          ostr << " </y-correction>\n";
          ostr << "        <y-adjusted  >";
          ostr << setw(19) << Y();
          ostr << " </y-adjusted>\n";
          ostr << "\n";
        }
      ostr << "        <z-given     >";
      ostr << setw(19) << Z.init_value();
      ostr << " </z-given>\n";
      if (!fixed_position())
        {
          ostr << "        <z-correction>";
          ostr << setw(19) << Z.correction();
          ostr << " </z-correction>\n";
          ostr << "        <z-adjusted  >";
          ostr << setw(19) << Z();
          ostr << " </z-adjusted>\n";
        }
    }

  if (!fixed_position())
    {
      set_cov_xyz();

      ostr.setf(std::ios_base::scientific, std::ios_base::floatfield);
      ostr.precision(7);
      ostr << "\n";
      ostr << "        ";
      ostr << "<cxx> " << cxx << " </cxx> ";
      ostr << "<cxy> " << setw(14) << cxy << " </cxy> ";
      ostr << "\n                                   ";
      ostr << "<cxz> " << setw(14) << cxz << " </cxz>\n";
      ostr << "        ";
      ostr << "<cyy> " << cyy << " </cyy> ";
      ostr << "<cyz> " << setw(14) << cyz << " </cyz>\n";
      ostr << "        ";
      ostr << "<czz> " << czz << " </czz>\n";
    }

  if (has_position())
     {
       ostr.setf(std::ios_base::fixed, std::ios_base::floatfield);

       double dB {0}, dL {0}, dH, BB, LL, HH;
       double B0 = B.init_value();
       double L0 = L.init_value();
       double H0 = H.init_value();

       if (!fixed_position())
         {
           common->ellipsoid.xyz2blh(X(), Y(), Z(), BB, LL, HH);
           dB = BB - B0;
           dL = LL - L0;
           dH = HH - H0;
         }

       ostr << "\n";
       ostr << "        <b-given     > ";
       ostr << latitude(B0);
       ostr << " </b-given>\n";
       if (!fixed_position())
         {
           ostr << "        <b-correction> ";
           ostr.precision(7);
           ostr << setw(18) << dB*RAD_TO_SS;
           ostr << " </b-correction>\n";
           ostr << "        <b-adjusted  > ";
           ostr << latitude(BB);
           ostr << " </b-adjusted>\n";
           ostr << "\n";
         }
       ostr << "        <l-given     > ";
       ostr << longitude(L0);
       ostr << " </l-given>\n";
       if (!fixed_position())
         {
           ostr << "        <l-correction> ";
           ostr << setw(18) << dL*RAD_TO_SS;
           ostr << " </l-correction>\n";
           ostr << "        <l-adjusted  > ";
           ostr << longitude(LL);
           ostr << " </l-adjusted>\n";
           ostr << "\n";
         }
       ostr.precision(5);
       ostr << "        <h-given     > ";
       ostr << setw(18) << H0;
       ostr << " </h-given>\n";
       if (!fixed_position())
         {
           ostr << "        <h-correction> ";
           ostr << setw(18) << dH;
           ostr << " </h-correction>\n";
           ostr << "        <h-adjusted  > ";
           ostr << setw(18) << HH;
           ostr << " </h-adjusted>\n";
         }
     }

  if (has_height())
    {
      ostr.setf(std::ios_base::fixed, std::ios_base::floatfield);
      ostr.precision(5);
      ostr << "\n        <height-given>";
      ostr << setw(19) << height.init_value();
      ostr << " </height-given>\n";
      if (free_height())
        {
          ostr << "        <height-correction>";
          ostr << setw(14) << height.correction();
          ostr << " </height-correction>\n";
          ostr << "        <height-adjusted  >";
          ostr << setw(14) << height();
          ostr << " </height-adjusted>\n";
        }
    }

  if (has_geoid())
    {
      ostr.precision(5);
      ostr << "\n        <geoid>       ";
      ostr << setw(19) << geoid.init_value();
      ostr << " </geoid>\n";
    }


  double db = dB(), dl =dL();
  if (db*db + dl*dl)
    {
      db *= RAD_TO_SS;
      dl *= RAD_TO_SS;
      ostr.precision(5);
      ostr << "\n        "
           << "<db> " << db << " </db>   <dl> " << dl << " </dl>\n\n";
    }


  ostr << "\n        </point>\n";

  N.write_xml_done();
  E.write_xml_done();
  U.write_xml_done();
}

// ----------------------------------------------------------------------


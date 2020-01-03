/* GNU Gama -- testing Acord2 approximate coordinates
   Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

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
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "a2g.h"
#include <fstream>
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <gnu_gama/simplified.h>
#include <gnu_gama/local/bearing.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/lcoords.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/xml/gkfparser.h>

using std::cout;
using std::endl;
using std::istringstream;
using std::string;
using std::fixed;
using std::setprecision;
using GNU_gama::local::LocalPoint;
using GNU_gama::local::StandPoint;
using GNU_gama::local::Direction;
using GNU_gama::local::Distance;
using GNU_gama::local::Angle;
using GNU_gama::local::Azimuth;
using GNU_gama::local::bearing_distance;

A2G::A2G()
{
}

A2G::~A2G()
{
}

int A2G::read(std::string file)
try
{
  file_ = file;
  std::ifstream inpf(input_dir_ + file_);
  std::string istr;
  char c;
  while (inpf.get(c))
    {
      if (c == '-')   // negative coordinates?
        {
          int p = inpf.peek();
          if (isdigit(p) || p == '.')
            {
              istr.push_back(c);
              continue;
            }
        }
      switch (c) {
        case '!':
        case '?':
        case '(':
        case ')':
        case '>':
        case '-':
        case '=':
        case '&':
        case '^':
          istr.push_back(' ');
          istr.push_back(c);
          istr.push_back(' ');
          break;
        default:
          istr.push_back(c);
          break;
        }
    }

  std::vector<double> covar;

  std::string angor = ang_right_handed_ ? "right" : "left";
  using namespace GNU_gama::local;
  std::string locos = LocalCoordinateSystem::locos_tags[int(locos_)];

  double sign{ang_right_handed_ ? 1.0 : -1.0}, shift{};

  auto trang = [&sign, &shift](double a)
    {
      a = sign*a + shift;
      if (a >= 400) a -= 400;
      if (a < 0) a += 400;

      return a;
    };

  xy (*trans)(double, double) = nullptr; /* x, y transformations from EN */

  using CS=GNU_gama::local::LocalCoordinateSystem::CS;
  switch (locos_)
    {
    case CS::EN:
      trans = [](double x, double y) { return xy( x, y); };
      shift = ang_right_handed_ ? 0 : 400;
      break;
    case CS::NW:
      trans = [](double x, double y) { return xy( y,-x); };
      shift = ang_right_handed_ ? 300 : 100;
      break;
    case CS::SE:
      trans = [](double x, double y) { return xy(-y, x); };
      shift = ang_right_handed_ ? 100 : 300;
      break;
    case CS::WS:
      trans = [](double x, double y) { return xy(-x,-y); };
      shift = ang_right_handed_ ? 200 : 200;
      break;

    case CS::NE:
      trans = [](double x, double y) { return xy( y, x); };
      shift = ang_right_handed_ ? 300 : 100;
      break;
    case CS::SW:
      trans = [](double x, double y) { return xy(-y,-x); };
      shift = ang_right_handed_ ? 100 : 300;
      break;
    case CS::ES:
      trans = [](double x, double y) { return xy( x,-y); };
      shift = ang_right_handed_ ? 0 : 400;
      break;
    case CS::WN:
      trans = [](double x, double y) { return xy(-x, y); };
      shift = ang_right_handed_ ? 200 : 200;
      break;
    };

  while (file_.back() != '.') file_.pop_back();
  file_.pop_back();

  std::ostringstream osstr;
  osstr << "<?xml version='1.0' ?>\n\n"
           "<gama-local xmlns='http://www.gnu.org/software/gama/gama-local'>\n"
           "<network axes-xy = '" +locos+ "' angles='" +angor+ "-handed'>\n\n"
           "<description>\n"
        << file_ <<
           "\n</description>\n\n"
           "<parameters  sigma-apr = '" << sigma_apr_ << "'\n"
           "             conf-pr = '" << conf_pr_<< "'\n"
           "             tol-abs = '" << tol_abs_ << "'\n"
           "             sigma-act = 'aposteriori'\n"
           "             update-constrained-coordinates = 'yes'/>\n\n"
           "<points-observations distance-stdev='" << distance_stdev_ << "'\n"
           "                     direction-stdev='" << direction_stdev_ << "'\n"
           "                     angle-stdev='" << angle_stdev_ << "'\n"
           "                     azimuth-stdev='" << azimuth_stdev_ << "'>\n\n";

  std::string t, s;
  string standpoint;
  istringstream inp(istr);

  char s0;
  while (inp >> s0)
    {
      osstr << fixed << setprecision(3);

      if (s0 == '#')
        {
          while (inp.get(s0) && s0 != '\n');
          continue;
        }
      if (s0 == fixed_point_ || s0 == free_point_)
        {
          bool fixed = (s0 == fixed_point_);
          string id;
          double x, y;
          inp >> id >> x >> y;
          auto t = trans(x,y);

          LocalPoint model, adj;
          if (fixed)
            {
              model.set_xy(x, y);
              model.set_fixed_xy();

              adj.set_xy(t.x, t.y);
              adj.set_fixed_xy();

              osstr << "<point id='" << id <<
                       "' x='" << t.x << "'  y='" << t.y << "' fix='xy' />\n";
            }
          else
            {
              model.set_free_xy();
              adj.set_free_xy();

              osstr << "<point id='" << id << "' adj='xy' />\n";
            }

          if (!fixed) model.set_xy(x, y);
          points_[id] = model;
        }
      else if (s0 == obs_)
        {
          inp >> standpoint;

          osstr << "\n<obs from='" << standpoint << "'>\n";
        }
      else if (s0 == obs_end_)
        {
          osstr << "</obs>\n";
        }
      else if (s0 == direction_)
        {
          string target;
          inp >> target;
          double b, d;
          bearing_distance(points_[standpoint], points_[target], b, d);

          osstr << "   <direction to ='" << target << "' val='"
                << setprecision(4) << trang(b/M_PI*200) << "' />\n";
        }
      else if (s0 == distance_)
        {
          string target;
          inp >> target;
          double b, d;
          bearing_distance(points_[standpoint], points_[target], b, d);

          osstr << "   <distance  to ='" << target
                << "' val='" << d << "' />\n";
        }
      else if (s0 == dist_dir_)
        {
          string target;
          inp >> target;
          double b, d;
          bearing_distance(points_[standpoint], points_[target], b, d);

          osstr << "   <distance  to ='" << target
                << "' val='" << d << "' />\n";
          osstr << "   <direction to ='" << target  << "' val='"
                << setprecision(4) << trang(b/M_PI*200) << "' />\n";
        }
      else if (s0 == angle_)
        {
          string left, right;
          inp >> left >> right;
          double bl, br, d;
          bearing_distance(points_[standpoint], points_[left],  bl, d);
          bearing_distance(points_[standpoint], points_[right], br, d);

          double sign  = ang_right_handed_ ? 1 : -1;
          double angle = sign*(br - bl);

          osstr << "   <angle bs='" << left << "' fs='" << right << "' val='"
                << setprecision(4) << angle/M_PI*200 << "' />\n";
        }
      else if (s0 == azimuth_)
        {
          string target;
          inp >> target;
          double b, d;
          bearing_distance(points_[standpoint], points_[target], b, d);
          double a = M_PI/2 - b;
          if (ang_right_handed_) a = 2*M_PI - a;

          osstr << "   <azimuth  to ='" << target
                << "' val='" << setprecision(4) << a/M_PI*200 << "' />\n";
        }
    }

  osstr << "\n</points-observations>\n"
           "</network>\n"
           "</gama-local>\n";

  {
    std::string name = output_dir_ + file_ + "-" + locos + "-" + angor + ".gkf";
    std::ofstream ofstr(name);
    string s = osstr.str();
    ofstr << s;

    using namespace GNU_gama::local;
    GKFparser gkf(network_);
    gkf.xml_parse(s.c_str(), int(s.size()), 0);
    gkf.xml_parse("", 0, 1);
  }

  network_.remove_inconsistency();

#ifdef A2G_ACORD2
  Acord2 acord2(network_.PD, network_.OD);
  acord2.execute();
#endif
#ifdef A2G_ACORD
  Acord acord(network_.PD, network_.OD);
  acord.execute();
#endif

#ifdef A2G_DEBUG
  bool first_point = true;
#endif

  cout << fixed << setprecision(3);

  double maxdiff = 0;
  for (auto p : network_.PD)
    {
      PointID id = p.first;

      if (!p.second.test_xy())
        {
          cout << "id = " << id << "  unsolved coordines xy " << endl;
          maxdiff++;
          continue;
        }

      double dx = p.second.x();       // adjustment coordinates
      double dy = p.second.y();

      auto t = trans(points_[id].x(), points_[id].y());
      if (network_.consistent())
        {
          dx -=  t.x;
          dy -=  t.y;
        }
      else
        {
          dx -=  t.x;
          dy +=  t.y;
        }

#ifdef A2G_DEBUG
      if (first_point)
        {
          first_point = false;
          cout << endl;
        }
      using std::setw;
      cout << "cons " << network_.consistent()
           << setw(10) << "model "
           << setw(10) << "trans "
           << setw(10) << "adj"
           << setw(10) << "dxy" << endl
           << setw(2)  << id << " x "
           << setw(10) << points_[id].x() << " "
           << setw(10) << t.x << " "
           << setw(10) << p.second.x() << " "
           << setw( 9) << dx << endl
           << setw(2)  << id << " y "
           << setw(10) << points_[id].y() << " "
           << setw(10) << t.y << " "
           << setw(10) << p.second.y() << " "
           << setw( 9) << dy << endl;
#endif

      double diff = sqrt(dx*dx+dy*dy);
      if (diff > maxdiff) maxdiff = diff;
    }

  return (maxdiff < 3e-3 ? 0 : 1);
}
catch (...)
{

  return 1;
}

void A2G::set_output_dir(std::string dir)
{
  output_dir_ = dir;
  if (output_dir_.back() != '/') output_dir_.push_back('/');
}

void A2G::set_input_dir(std::string dir)
{
  input_dir_ = dir;
  if (input_dir_.back() != '/') input_dir_.push_back('/');
}

/* GNU Gama -- testing Acord2 approximate coordinates against Acord class
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

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <memory>

#include <gnu_gama/local/network.h>
#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/local/language.h>
#include "acord.h"
#include <gnu_gama/local/acord/acord2.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

 using namespace GNU_gama::local;

/*#include "check_xyz.h"
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

using GNU_gama::local::LocalNetwork;
using GNU_gama::LocalNetworkAdjustmentResults;
*/

void a2_diff(std::string, double& maxdiff);
string input_dir;
const double a2_toll = 0.2;


int main(int argc, char* argv[])
{
  GNU_gama::local::set_gama_language(GNU_gama::local::en);

  if (argc < 4)
    {
      std::cout << "\nUsage: " << argv[0] << "  output_dir  input_dir  files... \n\n";
      return 1;
    }


  cout << "input  dir: " << argv[2] << endl;
  cout << "output dir: " << argv[1] << "\n\n";

  input_dir = string(argv[2]);
  if (input_dir.back() != '/') input_dir.push_back('/');

  double max_diff = 0;
  for (int i=3; i<argc; i++)
  {
    a2_diff(argv[i], max_diff);
  }
  cout << endl;

  return (max_diff < a2_toll) ? 0 : 1;
}


void a2_diff (string input_file, double& max_diff)
{
  using LocalNetwork = GNU_gama::local::LocalNetwork;

  std::unique_ptr<LocalNetwork> lnet (new LocalNetwork);
  lnet->set_algorithm("gso");

  std::ifstream soubor(input_dir + input_file);
  GNU_gama::local::GKFparser gkf(*lnet);
  try
    {
      char c;
      int  n, konec = 0;
      string radek;
      do
        {
          radek = "";
          n     = 0;
          while (soubor.get(c))
            {
              radek += c;
              n++;
              if (c == '\n') break;
            }
            if (!soubor) konec = 1;

            gkf.xml_parse(radek.c_str(), n, konec);
        }
      while (!konec);
    }
    catch (const GNU_gama::local::ParserException& v) {
      cerr << "\n" << T_GaMa_exception_2a << "\n\n"
           << T_GaMa_exception_2b << v.line << " : " << v.what() << endl;
          //return 3;
      throw;
    }
    catch (const GNU_gama::local::Exception& v) {
      cerr << "\n" <<T_GaMa_exception_2a << "\n"
           << "\n***** " << v.what() << "\n\n";
          //return 2;
      throw;
    }
    catch (...)
    {
      cerr << "\n" << T_GaMa_exception_2a << "\n\n";
      throw;
    }


  lnet->remove_inconsistency();

  Acord acord(lnet->PD, lnet->OD);
  acord.execute();

#if 0
  auto printp = [&lnet]() {
    cout.precision(4);
    cout.setf(std::ios::fixed);
    for (auto pd : lnet->PD)
      {
        auto id = pd.first;
        cout << id;
        auto xy = pd.second;
        if (xy.fixed_xy())
        {
          cout << " fixed ";
        } else if (xy.free_xy()) {
          cout << " free  ";
        }
        if (xy.test_xy()) cout << std::setw(12) << xy.x() << " "
                               << std::setw(12) << xy.y();

        cout << endl;
      }
    cout << endl;
    };
    printp();
#endif

  std::map<PointID, LocalPoint> pmap;

  for (auto& pd : lnet->PD)
    {
      PointID id     = pd.first;
      LocalPoint& lp = pd.second;
      pmap[id] = lp;
      if (lp.free_xy()) lp.unset_xy();
    }
  // printp();

  Acord2 acord2(lnet->PD, lnet->OD);
  acord2.execute();
  // printp();

  double diff = 0;
  for (auto& pd : lnet->PD)
    {
      PointID id    = pd.first;
      LocalPoint lp = pd.second;
      auto iter = pmap.find(id);
      if (iter != pmap.end())
        {
          // the point exists in the Acord solution
          LocalPoint p = iter->second;
          double dx = p.x() - lp.x();
          double dy = p.y() - lp.y();
          double dd = std::sqrt(dx*dx + dy*dy);
          if (dd > diff) diff = dd;
        }
    }
  if (diff > max_diff) max_diff = diff;

  cout.precision(4);
  cout.setf(std::ios::fixed);
  auto sff = [](double x) { return (std::abs(x) > 10 ? std::scientific : std::fixed);};
  cout << std::setw(10) << sff(max_diff) << max_diff << " "
       << std::setw(10) << sff(diff) << diff << "   "
       << input_file << endl;

  if (diff > a2_toll)
    {
      for (auto& pd : lnet->PD)
        {
          PointID id    = pd.first;
          LocalPoint lp = pd.second;
          auto iter = pmap.find(id);
          if (iter != pmap.end())
            {
              // the point exists in the Acord solution
              LocalPoint p = iter->second;
              double dx = p.x() - lp.x();
              double dy = p.y() - lp.y();
              diff = std::sqrt(dx*dx + dy*dy);
              cout << "                        "
                   << id << " "
                   << lp.x() << " " << lp.y() << " "
                   << dx << " " << dy
                   << endl;
            }
    }
      cout << endl;
    }
}

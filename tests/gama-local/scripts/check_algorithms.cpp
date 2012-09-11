/* GNU Gama -- testing adjustment results from different algorithms
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

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
#include <sstream>
#include <vector>
#include <iomanip>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/network_svd.h>
#include <gnu_gama/local/network_gso.h>
#include <gnu_gama/local/network_chol.h>
#include <gnu_gama/local/network_env.h>
#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/local/acord.h>


using GNU_gama::local::LocalNetwork;

std::string version, netconfig, input;
const char* netfile;

std::vector<std::string>   algoname;
std::vector<LocalNetwork*> algorithm;

double condnum;

bool          check(int i, int j);
LocalNetwork* getNet();

int main(int argc, char* argv[])
{
  if (argc != 4) return 1;

  version   = std::string(argv[1]);
  netconfig = std::string(argv[2]);
  netfile   = argv[3];

  std::ifstream inp(argv[3]);
  if (!inp)
    {
      std::cout << "\n   ####  ERROR ON OPENING FILE " << argv[3] << "\n";
      return 1;
    }

  bool failed = false;
  algoname.push_back(" svd ");   algorithm.push_back(getNet());
  algoname.push_back(" gso ");   algorithm.push_back(getNet());
  algoname.push_back(" chol");   algorithm.push_back(getNet());
  algoname.push_back(" env ");   algorithm.push_back(getNet());

  for (int i=0; i<algoname.size(); i++)
    for (int j=i+1; j<algoname.size(); j++)
      {
        if (!check(i,j)) failed = true;
      }

  if (failed) return 1;
}


bool check(int i, int j)
{
  std::cout << "cond.n "
            << std::scientific << std::setprecision(2)
            << condnum;

  double maxdiff = 0;

  using namespace GNU_gama::local;
  LocalNetwork* IS = algorithm[i];
  const int y_sign = GaMaConsistent(IS->PD) ? +1 : -1;
  const Vec&     x = IS->solve();

  LocalNetwork* IS2 = algorithm[j];
  const Vec&     x2 = IS2->solve();

  for (PointData::const_iterator ii=IS->PD.begin(); ii!=IS->PD.end(); ii++)
    {
      const PointID point_id = (*ii).first;
      const LocalPoint&  b   = (*ii).second;

      const LocalPoint&  b2  = IS2->PD[point_id];

      if (b.free_xy() && b.index_x())
        {
          if (!b2.free_xy() || !b2.index_x())
            {
              return false;
            }
          double adj_x = b.x()+x(b.index_x())/1000;
          double adj_y = y_sign*(b.y()+x(b.index_y())/1000);

          double adj_x2 = b2.x()+x2(b2.index_x())/1000;
          double adj_y2 = y_sign*(b2.y()+x2(b2.index_y())/1000);

          double dx = adj_x - adj_x2;
          if (std::abs(dx) > std::abs(maxdiff)) maxdiff = dx;

          double dy = adj_y - adj_y2;
          if (std::abs(dy) > std::abs(maxdiff)) maxdiff = dy;

        }
      if (b.free_z() && b.index_z())
        {
          if (!b2.free_z() || !b2.index_z())
            {
              return false;
            }
          double adj_z = b.z()+x(b.index_z())/1000;
          double adj_z2 = b2.z()+x2(b2.index_z())/1000;

          double dz = adj_z - adj_z2;
          if (std::abs(dz) > std::abs(maxdiff)) maxdiff = dz;
        }
    }

  std::cout << "  max.diff "
            << std::scientific << std::setprecision(3) << std::setw(11)
            << maxdiff << " ";

  std::cout << algoname[i] << algoname[j] << "  " << netconfig;

  if (abs(maxdiff) < 1e-5)
    {
      std::cout << "\n";
      return true;
    }
 
  std::cout << "  !!!\n";
  return false;
}


LocalNetwork* getNet()
{
  LocalNetwork* lnet = 0;

  int N = algorithm.size();
  switch (N)
    {
    case 0:
      lnet = new GNU_gama::local::LocalNetwork_svd;
      break;
    case 1:
      lnet = new GNU_gama::local::LocalNetwork_gso;
      break;
    case 2:
      lnet = new GNU_gama::local::LocalNetwork_chol;
      break;
    case 3:
      lnet = new GNU_gama::local::LocalNetwork_env;
      break;
    }

  using namespace GNU_gama::local;
  using std::cerr;
  using std::endl;

      {
        using std::string;

        GNU_gama::local::set_gama_language(GNU_gama::local::en);

        std::ifstream soubor(netfile);
        GNU_gama::local::GKFparser gkf(lnet->PD, lnet->OD);
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

            lnet->apriori_m_0(gkf.m0_apr );
            lnet->conf_pr    (gkf.konf_pr);
            lnet->tol_abs    (gkf.tol_abs);

            lnet->update_constrained_coordinates(gkf.update_constr);

            if (gkf.typ_m0_apriorni)
              lnet->set_m_0_apriori();
            else
              lnet->set_m_0_aposteriori();

            lnet->description = gkf.description;
            lnet->epoch = gkf.epoch;
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
      }

   try
      {
        if (!GaMaConsistent(lnet->PD))
          {
            // cout << T_GaMa_inconsistent_coordinates_and_angles << "\n\n\n";
          }
        Acord acord(lnet->PD, lnet->OD);
        acord.execute();
        //ReducedObservationsText(lnet,&(acord.RO), cout);

        /*
        if (correction_to_ellipsoid)
          {
            ReduceToEllipsoid reduce_to_el(lnet->PD, lnet->OD, el, latitude);
            reduce_to_el.execute();
            ReducedObservationsToEllipsoidText(lnet, reduce_to_el.getMap(), cout);
          }
        */

        //ApproximateCoordinates(&acord, std::cout);

      }
    catch(GNU_gama::local::Exception e)
      {
        cerr << e.what() << endl;
        //return 1;
        throw;
      }
    catch(...)
      {
        cerr << "Gama / Acord: approximate coordinates failed\n\n";
        //return 1;
        throw;
      }

      lnet->solve();
      if (N == 0) condnum = lnet->cond();

  return lnet;
}

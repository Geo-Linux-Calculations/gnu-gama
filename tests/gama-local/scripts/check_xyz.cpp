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

#include "check_xyz.h"

#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/local/acord.h>
#include <gnu_gama/local/results/text/test_linearization.h>

double xyzMaxDiff(GNU_gama::local::LocalNetwork* lnet1,
		  GNU_gama::local::LocalNetwork* lnet2)
{
  double maxdiff = 0;

  using namespace GNU_gama::local;
  const int y_sign = GaMaConsistent(lnet1->PD) ? +1 : -1;
  const Vec&    x1 = lnet1->solve();
  const Vec&    x2 = lnet2->solve();

  for (PointData::const_iterator
	 ii=lnet1->PD.begin(); ii!=lnet1->PD.end(); ii++)
    {
      const PointID point_id = (*ii).first;
      const LocalPoint&  b   = (*ii).second;

      const LocalPoint&  b2  = lnet2->PD[point_id];

      if (b.free_xy() && b.index_x())
        {
          if (!b2.free_xy() || !b2.index_x())
            {
              //return false;
            }
          double adj_x = b.x()+x1(b.index_x())/1000;
          double adj_y = y_sign*(b.y()+x1(b.index_y())/1000);

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
              //return false;
            }
          double adj_z = b.z()+x1(b.index_z())/1000;
          double adj_z2 = b2.z()+x2(b2.index_z())/1000;

          double dz = adj_z - adj_z2;
          if (std::abs(dz) > std::abs(maxdiff)) maxdiff = dz;
        }
    }

  return maxdiff;
}


GNU_gama::local::LocalNetwork* getNet(int alg, const char* file)
{
  GNU_gama::local::LocalNetwork* lnet = new GNU_gama::local::LocalNetwork;
  switch (alg)
    {
    case 0:
      lnet->set_algorithm("svd");
      break;
    case 1:
      lnet->set_algorithm("gso");
      break;
    case 2:
      lnet->set_algorithm("cholesky");
      break;
    case 3:
      lnet->set_algorithm("envelope");
      break;
    }

  using namespace GNU_gama::local;
  using std::cerr;
  using std::endl;

      {
        using std::string;

        GNU_gama::local::set_gama_language(GNU_gama::local::en);

        std::ifstream soubor(file);
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
      if (lnet->huge_abs_terms()) lnet->remove_huge_abs_terms();

      int iteration = 0;
      do
        {
          if (++iteration > 1) lnet->refine_approx();
        }
      while (GNU_gama::local::TestLinearization(lnet) && iteration < 5);

  return lnet;
}

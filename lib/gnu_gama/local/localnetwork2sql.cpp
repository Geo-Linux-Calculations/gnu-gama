/*
    GNU Gama -- adjudstment of geodetic networks
    Copyright (C) 2010  Ales Cepek <cepek@gnu.org>,
                  2010 Jiri Novak <jiri.novak@petriny.net>,
                  2012, 2013, 2014, 2015, 2017 Ales Cepek <cepek@gnu.org>

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
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#include "localnetwork2sql.h"
#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/version.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/size_to.h>
#include <iostream>

using namespace GNU_gama::local;

namespace {

class WriteSQLVisitor : public GNU_gama::local::AllObservationsVisitor
{
private:
    std::ostream&                  ostr;
    GNU_gama::local::LocalNetwork* netinfo;
    const GNU_gama::local::Vec&    residuals;
    int                            index;
    const double                   y_sign;
    const double                   kki;

public:
    WriteSQLVisitor(std::ostream& outStream, GNU_gama::local::LocalNetwork* net)
        : ostr(outStream), netinfo(net),
          residuals(netinfo->residuals()),
          index(0), y_sign(netinfo->y_sign()),
          kki(netinfo->conf_int_coef())
    {
    }

    void setObservationIndex(int ind) { index = ind; }

    void residualsAndAnalysisOfObservations(Observation* obs)
    {
      const double scale = netinfo->gons() ? 1.0 : 0.324;
      const int  angular = obs->angular() ? 1 : 0;

      double ml  = netinfo->stdev_obs(index);
      if (obs->angular()) ml *= scale;

      double qrr = netinfo->wcoef_res(index);
      double f   = netinfo->obs_control(index);

      ostr << ", " << angular << ", " << ml << ", " << qrr << ", " << f << ", ";

      if (f >= 0.1)
      {
          double sr  = netinfo->studentized_residual(index);
          ostr << sr << ", ";

          if ( (obs->ptr_cluster())->covariance_matrix.bandWidth() == 0 &&
                          (f >=5 || (f >= 0.1 && sr > kki)))
          {
              double em = residuals(index)/(netinfo->wcoef_res(index)*netinfo->weight_obs(index));
              double ev = em - residuals(index);

              ostr << em << ", " << ev;
          }
          else
          {
              ostr << "NULL, NULL";
          }
      }
      else
      {
          ostr << "NULL, NULL, NULL";
      }

      ostr << ");\n";
    }

    void visit(Distance* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'distance', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Direction* obs)
    {
      double m = obs->value()*R2G;
      double r = residuals(index)/10000;

      ostr << "'direction', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Angle* obs)
    {
        double m = obs->value()*R2G;
        double r = residuals(index)/10000;

        ostr << "'angle', '" << obs->from() << "', '" << obs->bs() << "', '"
	     << obs->fs() << "', " << m << ", " << m+r;
    }
    void visit(H_Diff* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'height-diff', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(S_Distance* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'slope-distance', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Z_Angle* obs)
    {
      double m = obs->value()*R2G;
      double r = residuals(index)/10000;

      ostr << "'zenith-angle', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(X* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'coordinate-x', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Y* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'coordinate-y', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Z* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'coordinate-z', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Xdiff* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'dx', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Ydiff* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'dy', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Zdiff* obs)
    {
      double m = obs->value();
      double r = residuals(index)/1000;

      ostr << "'dz', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }
    void visit(Azimuth* obs)
    {
      double m = obs->value()*R2G;
      double r = residuals(index)/10000;

      ostr << "'azimuth', '" << obs->from() << "', '" << obs->to() << "', NULL, " << m << ", " << m+r;
    }

};

}   // unnamed namespace

LocalNetwork2sql::LocalNetwork2sql(LocalNetwork& lnet)
  : localNetwork(lnet),
    points      (lnet.PD),
    observations(lnet.OD)
{
  setDelete(true);
}

void LocalNetwork2sql::readGkf(std::istream& istr)
{
  try
    {
      GKFparser gkf(localNetwork);
      char c;
      int  n, finish = 0;
      std::string line;
      do
        {
          line = "";
          n     = 0;
          while (istr.get(c))
            {
              line += c;
              n++;
              if (c == '\n') break;
            }
          if (!istr) finish = 1;

          gkf.xml_parse(line.c_str(), n, finish);
        }
      while (!finish);
    }
  catch (...)
    {
      throw;
    }
}


void LocalNetwork2sql::write(std::ostream& ostr, std::string conf)
{
  config = conf;
  ostr.setf(std::ios_base::scientific, std::ios_base::floatfield);
  ostr.precision(17);

  ostr << "/* generated by LocalNetwork2sql, configuration: " + config + "\n"
       << " */\n"
       << "begin;\n\n";     // begin transaction

  if (getDelete())
    {
      ostr << "DELETE FROM gnu_gama_local_covmat         WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_descriptions   WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_points         WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_obs            WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_coordinates    WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_vectors        WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_clusters       WHERE conf_id = " << cnfg() << ";\n"

           << "DELETE FROM gnu_gama_local_adj_network_general_parameters  WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_coordinates_summary         WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_observations_summary        WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_project_equations           WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_standard_deviation          WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_coordinates                 WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_orientation_shifts          WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_covmat                      WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_original_indexes            WHERE conf_id = " << cnfg() << ";\n"
           << "DELETE FROM gnu_gama_local_adj_observations                WHERE conf_id = " << cnfg() << ";\n"

           << "DELETE FROM gnu_gama_local_configurations WHERE conf_id = " << cnfg() << ";\n\n";
    }

  std::string axes = "ne";
  switch(localNetwork.PD.local_coordinate_system)
  {
      case LocalCoordinateSystem::EN: axes = "'en', "; break;
      case LocalCoordinateSystem::NW: axes = "'nw', "; break;
      case LocalCoordinateSystem::SE: axes = "'se', "; break;
      case LocalCoordinateSystem::WS: axes = "'ws', "; break;
      case LocalCoordinateSystem::NE: axes = "'ne', "; break;
      case LocalCoordinateSystem::SW: axes = "'sw', "; break;
      case LocalCoordinateSystem::ES: axes = "'es', "; break;
      case LocalCoordinateSystem::WN: axes = "'wn', "; break;
      default:
             axes =  "'ne', "; //break;*/
  }

  {
    ostr <<  "insert into gnu_gama_local_configurations"
         <<  " (conf_id, conf_name, sigma_apr, conf_pr, tol_abs, sigma_act,"
         <<  " update_cc, axes_xy, angles, ang_units,"
         <<  " cov_band, algorithm, epoch, latitude, ellipsoid) values ("
         <<  "(select new_id from (select coalesce(max(conf_id), 0)+1 as "
         <<  "new_id from gnu_gama_local_configurations)x),"
         <<  " '" + config +"', "
         <<  localNetwork.apriori_m_0() << ", "
         <<  localNetwork.conf_pr() << ", "
         <<  localNetwork.tol_abs() << ", "
         << (localNetwork.m_0_apriori()
             ? "'apriori'" : "'aposteriori'") << ", "
         << (localNetwork.update_constrained_coordinates()
             ? "'yes'" : "'no'") << ", "
         <<  axes
         << (localNetwork.PD.left_handed_angles()
             ? "'left-handed'" : "'right-handed'") << ", "
         << (localNetwork.gons() ? 400 : 360) << ", "
         << localNetwork.adj_covband() << ", ";
    // nullable data
    if (localNetwork.has_algorithm()) ostr << "'" << localNetwork.algorithm()
                                           << "', ";
    else                              ostr << "NULL, ";
    if (localNetwork.has_epoch())     ostr << localNetwork.epoch() << ", ";
    else                              ostr << "NULL, ";
    if (localNetwork.has_latitude())  ostr << localNetwork.latitude() << ", ";
    else                              ostr << "NULL, ";
    if (localNetwork.has_ellipsoid()) ostr << "'" << localNetwork.ellipsoid()
                                           << "'";
    else                              ostr << "NULL";
    ostr << ");\n";
  }

  /* <points-observations> atributes */
  // {
  //   double da = gkfparser->implicit_stdev_distance_a();
  //   double db = gkfparser->implicit_stdev_distance_b();
  //   double dc = gkfparser->implicit_stdev_distance_c();
  //   if (da + db != 0)
  //     {
  //    ostr << "insert into gnu_gama_local_atributes"
  //         << " (conf_id, atribute, tag, value) values ("
  //         << cnfg() << ", 'distance-stdev', 'points-observations', '" << da;
  //    if (db)
  //      ostr << " " << db << " " << dc;
  //    ostr << "');\n";
  //     }
  //
  //   if (double dir=gkfparser->implicit_stdev_direction())
  //     {
  //    ostr << "insert into gnu_gama_local_atributes (conf_id, atribute, tag, value) values ("
  //         << cnfg() << ", 'direction-stdev', 'points-observations', '" << dir << "');\n";
  //     }
  //
  //   if (double angle=gkfparser->implicit_stdev_angle())
  //     {
  //    ostr << "insert into gnu_gama_local_atributes (conf_id, atribute, tag, value) values ("
  //         << cnfg() << ", 'angle-stdev', 'points-observations', '" << angle << "');\n";
  //     }
  //
  //     if (double zangle=gkfparser->implicit_stdev_zangle())
  //     {
  //    ostr << "insert into gnu_gama_local_atributes (conf_id, atribute, tag, value) values ("
  //         << cnfg() << ", 'zenith-angle-stdev', 'points-observations', '" << zangle << "');\n";
  //     }
  // }


  /* <description> */
  if (!localNetwork.description.empty())
    {
      const int N = 1000;  // varchar('N') in gnu_gama_local_descriptions table;
      int indx = 0;
      while (indx*N < size_to<int>(localNetwork.description.length()))
        {
          const std::string& s = localNetwork.description.substr(indx*N, N);
          std::string description;
          for (std::string::const_iterator i=s.begin(); i!=s.end(); ++i)
            if (*i == '\'')
              description += "''";
            else
              description += *i;
          ostr << "insert into gnu_gama_local_descriptions"
               << " (conf_id, indx, text) values ("
               << cnfg() << ", " << (indx+1) << ", '" << description << "');\n";
          indx++;
        }
    }


  /* <points-observations><point /> */
  {
    using namespace GNU_gama::local;
    ostr << "\n";
    for (PointData::const_iterator i=points.begin(); i!=points.end(); ++i)
      {
        const PointID&    id = i->first;
        const LocalPoint& pt = i->second;

        std::string atr = "conf_id, id";
        std::ostringstream val;
        val.setf(std::ios_base::fixed, std::ios_base::floatfield);
        val.precision(17);
        val << cnfg() << ", '" << id << "'";

        if (pt.test_xy())
          {
            atr += ", x, y";
            val << ", " << pt.x() << ", " << pt.y();
          }

        if (pt.test_z())
          {
            atr += ", z";
            val << ", " << pt.z();
          }

        if (pt.active_xy())
          {
            atr += ", txy";
            if      (pt.fixed_xy()      )  val << ", 'fixed'";
            else if (pt.constrained_xy())  val << ", 'constrained'";
            else if (pt.free_xy()       )  val << ", 'adjusted'";
          }

        if (pt.active_z())
          {
            atr += ", tz";
            if      (pt.fixed_z()      )  val << ", 'fixed'";
            else if (pt.constrained_z())  val << ", 'constrained'";
            else if (pt.free_z()       )  val << ", 'adjusted'";
          }

        ostr << "insert into gnu_gama_local_points (" << atr << ") "
             << "values (" << val.str() << ");\n";
      }
  }


  /* clusters */
  {
    using namespace GNU_gama::local;

    int cluster = 1;
    for (GNU_gama::local::ObservationData::ClusterList::const_iterator
           i=observations.clusters.begin(); i!=observations.clusters.end(); ++i)
      {
        const Cluster* c = *i;
        if (/*const StandPoint* sp = */ dynamic_cast<const StandPoint*>(c))
          {
            /* xml <obs> atributes from, orientation and from_dh,
             * defined in gama-local.dtd, are ignored in database
             * schema (from_dh is not even implemented in class
             * StandPoint)
             */
            write_cluster(ostr, c, cluster, "obs");

            int index = 1;
            for (ObservationList::const_iterator
                   b = c->observation_list.begin(),
                   e = c->observation_list.end();  b != e;  ++b)
              {
                // tag, from_id, to_id, to_id2, val, stdev, from_dh, to_dh, to_dh2, dist
                if      (const Distance*   m = dynamic_cast<const Distance*  >(*b))
                {
                  ostr << "insert into gnu_gama_local_obs "
                       << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                       << "val, from_dh, to_dh, rejected) values ("
                       << cnfg() << ", " << cluster << ", "
                       << index++ << ", 'distance', '"
                       << m->from() << "', '" << m->to() << "', "
                       << m->value();
                  if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                  if (m->to_dh()  ) ostr << ", " << m->to_dh();   else ostr << ", null";
                  ostr << ", " << rejected(m) << ");\n";
                }
                else if (const Direction*  m = dynamic_cast<const Direction* >(*b))
                {
                  ostr << "insert into gnu_gama_local_obs "
                       << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                       << "val, from_dh, to_dh, rejected) values ("
                       << cnfg() << ", " << cluster << ", "
                       << index++ << ", 'direction', '"
                       << m->from() << "', '" << m->to() << "', "
                       << m->value();
                  if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                  if (m->to_dh()  ) ostr << ", " << m->to_dh();   else ostr << ", null";
                  ostr  << ", " << rejected(m) << ");\n";
                }
                else if (const S_Distance* m = dynamic_cast<const S_Distance*>(*b))
                {
                  ostr << "insert into gnu_gama_local_obs "
                       << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                       << "val, from_dh, to_dh, rejected) values ("
                       << cnfg() << ", " << cluster << ", "
                       << index++ << ", 's-distance', '"
                       << m->from() << "', '" << m->to() << "', "
                       << m->value();
                  if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                  if (m->to_dh()  ) ostr << ", " << m->to_dh();   else ostr << ", null";
                  ostr  << ", " << rejected(m) << ");\n";
                }
                else if (const Z_Angle*    m = dynamic_cast<const Z_Angle*   >(*b))
                {
                  ostr << "insert into gnu_gama_local_obs "
                       << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                       << "val, from_dh, to_dh, rejected) values ("
                       << cnfg() << ", " << cluster << ", "
                       << index++ << ", 'z-angle', '"
                       << m->from() << "', '" << m->to() << "', "
                       << m->value();
                  if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                  if (m->to_dh()  ) ostr << ", " << m->to_dh();   else ostr << ", null";
                  ostr  << ", " << rejected(m) << ");\n";
                }
                else if (const Azimuth*    m = dynamic_cast<const Azimuth*   >(*b))
                {
                  ostr << "insert into gnu_gama_local_obs "
                       << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                       << "val, from_dh, to_dh, rejected) values ("
                       << cnfg() << ", " << cluster << ", "
                       << index++ << ", 'azimuth', '"
                       << m->from() << "', '" << m->to() << "', "
                       << m->value();
                  if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                  if (m->to_dh()  ) ostr << ", " << m->to_dh();   else ostr << ", null";
                  ostr  << ", " << rejected(m) << ");\n";
                }
                else if (const Angle*      m = dynamic_cast<const Angle*     >(*b))
                  {
                    ostr << "insert into gnu_gama_local_obs "
                         << "(conf_id, ccluster, indx, tag, from_id, to_id, to_id2, "
                         << "val, from_dh, to_dh, to_dh2, rejected) values ("
                         << cnfg() << ", " << cluster << ", "
                         << index++ << ", 'angle', '"
                         << m->from() << "', '" << m->bs() << "', '" << m->fs() << "', "
                         << m->value();
                    if (m->from_dh()) ostr << ", " << m->from_dh(); else ostr << ", null";
                    if (m->bs_dh()  ) ostr << ", " << m->bs_dh();   else ostr << ", null";
                    if (m->fs_dh()  ) ostr << ", " << m->fs_dh();   else ostr << ", null";
                    ostr  << ", " << rejected(m) << ");\n";
                  }
                // xsd 0.91 else if (const H_Diff*     m = dynamic_cast<const H_Diff*    >(*b))
                // xsd 0.91   {
                // xsd 0.91     ostr << "insert into gnu_gama_local_obs "
                // xsd 0.91          << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                // xsd 0.91          << "val, dist, rejected) values (" << cnfg() << ", " << cluster << ", "
                // xsd 0.91          << index++ << ", 'dh', '"
                // xsd 0.91          << m->from() << "', '" << m->to() << "', "
                // xsd 0.91          << m->value() << ", ";
                // xsd 0.91     if (m->dist())
                // xsd 0.91       ostr << ", " << m->dist();
                // xsd 0.91     else
                // xsd 0.91       ostr << "null";
                // xsd 0.91     ostr << ", " << rejected(m) << ");\n";
                // xsd 0.91   }
                else
                  {
                  }
              }
          }
        else if (/* const HeightDifferences* sp = */ dynamic_cast<const HeightDifferences*>(c))
          {
            write_cluster(ostr, c, cluster, "height-differences");

            int index = 1;
            for (ObservationList::const_iterator
                   b = c->observation_list.begin(),
                   e = c->observation_list.end();  b != e;  ++b)
              {
                const H_Diff* hd = dynamic_cast<const H_Diff*>(*b);
                ostr << "insert into gnu_gama_local_obs "
                     << "(conf_id, ccluster, indx, tag, from_id, to_id, "
                     << "val, dist, rejected) values (" << cnfg() << ", " << cluster << ", "
                     << index++ << ", 'dh', '"
                     << hd->from() << "', '" << hd->to() << "', "
                     << hd->value() << ", ";
                if (hd->dist())
                  ostr << hd->dist();
                else
                  ostr << "null";
                ostr << ", " << rejected(hd) << ");\n";
              }
          }
        else if (/*const Coordinates* sp = */dynamic_cast<const Coordinates*>(c))
          {
            write_cluster(ostr, c, cluster, "coordinates");
            int index = 1, inc;
            for (ObservationList::const_iterator
                   b = c->observation_list.begin(),
                   e = c->observation_list.end();  b != e;  ++b)
              {
                inc = 0;
                std::string        ats;
                std::ostringstream xyz;
                int                rejected_point = 0;
                xyz.precision(17);
                std::string pointid = (*b)->from().str();
                if (const X* xcoord = dynamic_cast<const X*>(*b))
                  {
                    inc++;
                    ats += ", x";
                    xyz << ", " << xcoord->value();
                    if (rejected(*b)) rejected_point = rejected(*b);
                    ObservationList::const_iterator t = b;
                    ++t;
                    if (t != e)
                      {
                        inc++;
                        ++b;
                        ats += ", y";
                        xyz << ", " << (*b)->value();
                        if (rejected(*b)) rejected_point = rejected(*b);
                        ++t;
                        if (t != e && dynamic_cast<const Z*>(*t)
                            && (*t)->from().str()==pointid)
                          {
                            inc++;
                            ++b;
                            ats += ", z";
                            xyz << ", " << (*b)->value();
                            if (rejected(*b)) rejected_point = rejected(*b);
                          }
                      }
                  }
                else if (const Z* zcoord = dynamic_cast<const Z*>(*b))
                  {
                    inc  = 1;
                    ats += ", z";
                    xyz << ", " << zcoord->value();
                    rejected_point = rejected(zcoord);
                  }
                ostr << "insert into gnu_gama_local_coordinates "
                     << "(conf_id, ccluster, indx, id" << ats
                     << ", rejected) "
                     << "values (" << cnfg() << ", " << cluster << ", "
                     << index << ", '" << pointid << "'"
                     << xyz.str() << ", " << rejected_point << ");\n";
                index += inc;
              }
          }
        else if (/*const Vectors* sp = */ dynamic_cast<const Vectors*>(c))
          {
            write_cluster(ostr, c, cluster, "vectors");
            int index = 1;
            for (ObservationList::const_iterator
                   b = c->observation_list.begin(),
                   e = c->observation_list.end();  b != e;  ++b)
              {
                const Xdiff* xd = dynamic_cast<const Xdiff*>(*b++);
                const Ydiff* yd = dynamic_cast<const Ydiff*>(*b++);
                const Zdiff* zd = dynamic_cast<const Zdiff*>(*b);
                int rejected_point = 0;
                if (rejected(xd)) rejected_point = rejected(xd);
                if (rejected(yd)) rejected_point = rejected(yd);
                if (rejected(zd)) rejected_point = rejected(zd);
                ostr << "insert into gnu_gama_local_vectors "
                     << "(conf_id, ccluster, indx, from_id, to_id, "
                     << "dx, dy, dz, from_dh, to_dh, rejected) values ("
                     << cnfg() << ", " << cluster << ", " << index << ", '"
                     << xd->from().str() << "', '" << xd->to().str() << "', "
                     << xd->value() << ", "
                     << yd->value() << ", "
                     << zd->value() << ", "
                     << " null, null"  // from_dh to_dh (are they really used?)
                     << ", " << rejected_point << ");\n";
                index += 3;
              }
          }
        else
          throw GNU_gama::local::Exception("gkf2sql --- unknown cluster type");

        cluster++;
      }
  }

  /* adjusted results only when the network is adjusted */
  if (localNetwork.is_adjusted())
    {
      LocalNetwork* netinfo = &localNetwork;
      const double y_sign = netinfo->y_sign();
      const Vec& x = netinfo->solve();

      { // general parameters
        ostr << "insert into gnu_gama_local_adj_network_general_parameters "
             << "(conf_id, gmversion, algorithm, compiler, epoch, axes, angles) "
             << "values ("
             << cnfg() << ", '" << GNU_gama::GNU_gama_version() << "', "
             << (netinfo->algorithm().length() ? ("'"+netinfo->algorithm()+"'") : "NULL")
             << ", '" << GNU_gama::GNU_gama_compiler << "', ";

        if (netinfo->has_epoch()) ostr << netinfo->epoch() << ", ";  else  ostr << "NULL, ";

        std::string axes = "ne";
        switch(localNetwork.PD.local_coordinate_system)
          {
          case LocalCoordinateSystem::EN: axes = "'en', "; break;
          case LocalCoordinateSystem::NW: axes = "'nw', "; break;
          case LocalCoordinateSystem::SE: axes = "'se', "; break;
          case LocalCoordinateSystem::WS: axes = "'ws', "; break;
          case LocalCoordinateSystem::NE: axes = "'ne', "; break;
          case LocalCoordinateSystem::SW: axes = "'sw', "; break;
          case LocalCoordinateSystem::ES: axes = "'es', "; break;
          case LocalCoordinateSystem::WN: axes = "'wn', "; break;
          default:
            axes =  "'ne', "; //break;*/
          }

        ostr << axes
             << (localNetwork.PD.left_handed_angles() ? "'left-handed'" : "'right-handed'")
             << ");\n";
      }


      { // summary of coordinates in adjustment
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

        ostr << "insert into gnu_gama_local_adj_coordinates_summary "
             << "(conf_id, adj_xyz, adj_xy, adj_z, con_xyz, con_xy, con_z, fix_xyz, fix_xy, fix_z) "
             << "values ("
             << cnfg() << ", "
             << a_xyz << ", " << a_xy << ", " << a_z << ", "
             << c_xyz << ", " << c_xy << ", " << c_z << ", "
             << f_xyz << ", " << f_xy << ", " << f_z << " "
             << ");\n";
      }


      { // observations summary
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

        ostr << "insert into gnu_gama_local_adj_observations_summary "
             << "(conf_id, distances, directions, angles, xyz_coords, h_diffs, z_angles, s_dists, vectors, azimuths) "
             << "values ("
             << cnfg() << ", "
             << counter.dists   << ", "
             << counter.dirs    << ", "
             << counter.angles  << ", "
             << counter.coords  << ", "
             << counter.hdiffs  << ", "
             << counter.zangles << ", "
             << counter.chords  << ", "
             << counter.vectors << ", "
             << counter.azimuth << ");\n";
      }


      { // project equations
        ostr << "insert into gnu_gama_local_adj_project_equations "
             << "(conf_id, equations, unknowns, deg_freedom, defect, sum_squares, connected) "
             << "values ("
             << cnfg()                        << ", "
             << netinfo->sum_observations()   << ", "
             << netinfo->sum_unknowns()       << ", "
             << netinfo->degrees_of_freedom() << ", "
             << netinfo->null_space()         << ", "
             << netinfo->trans_VWV()          << ", "
             << (netinfo->connected_network() ? 1 : 0) << ");\n";
      }


      { // standard deviation
        const int dof = netinfo->degrees_of_freedom();
        double test=0, lower=0, upper=0;

        test  = netinfo->m_0_aposteriori_value() / netinfo->apriori_m_0();
        if (dof)
          {
            const double alfa_pul = (1 - netinfo->conf_pr())/2;
            lower = sqrt(GNU_gama::Chi_square(1-alfa_pul,dof)/dof);
            upper = sqrt(GNU_gama::Chi_square(  alfa_pul,dof)/dof);
          }

        ostr << "insert into gnu_gama_local_adj_standard_deviation "
             << "(conf_id, apriori, aposteriori, used, probability, ratio, rlower, rupper, passed, conf_scale) "
             << "values ("
             << cnfg() << ", "
             << netinfo->apriori_m_0() << ", "
             << (netinfo->degrees_of_freedom() > 0 ? sqrt(netinfo->trans_VWV()/netinfo->degrees_of_freedom()) : 0) << ", "
             << (netinfo->m_0_aposteriori() ? "'aposteriori'" : "'apriori'") << ", "
             << netinfo->conf_pr() << ", "
             << test  << ", "
             << lower << ", "
             << upper << ", "
             << ((lower < test && test < upper) ? 1 : 0) << ", "
             << netinfo->conf_int_coef() << ");\n";
      }


      { // coordinates
        for (PointData::const_iterator ii=netinfo->PD.begin(); ii!=netinfo->PD.end(); ii++)
          {
            const PointID point_id = (*ii).first;
            const LocalPoint&  b   = (*ii).second;
            if (!b.active()) continue;

            int indx = 0;
            if (b.free_z()  && b.index_z()) indx = b.index_z();
            if (b.free_xy() && b.index_x()) indx = b.index_x();


            ostr << "insert into gnu_gama_local_adj_coordinates "
                 << "(conf_id, indx, id, x, y, z, txy, tz, x_approx, y_approx, z_approx) "
                 << "values ("
                 << cnfg() << ", " << indx << ", '" << point_id << "', ";

            if (b.fixed_xy())
              {
                ostr << b.x() << ", " << b.y() << ", ";
              }
            else if (b.free_xy() && b.index_x())
              {
                double adj_x = b.x()+x(b.index_x())/1000;
                double adj_y = y_sign*(b.y()+x(b.index_y())/1000);
                ostr << adj_x << ", " << adj_y << ", ";
              }
            else
              {
                ostr << "NULL, NULL, ";
              }

            if (b.fixed_z())
              {
                ostr << b.z() << ", ";
              }
            else if (b.free_z() && b.index_z())
              {
                ostr << (b.z()+x(b.index_z())/1000) << ", ";
              }
            else
              {
                ostr << "NULL, ";
              }

            if (b.fixed_xy())            ostr << "'fixed', ";
            else if (b.constrained_xy()) ostr << "'constrained', ";
            else if (b.free_xy())        ostr << "'adjusted', ";
            else                         ostr << "NULL, ";

            if (b.fixed_z())             ostr << "'fixed', ";
            else if (b.constrained_z())  ostr << "'constrained', ";
            else if (b.free_z())         ostr << "'adjusted', ";
            else                         ostr << "NULL, ";

            if (b.free_xy()) {
              ostr << b.x_0() << ", " << y_sign*b.y_0() << ", ";
            }
            else {
              ostr << "NULL, NULL, ";
            }

            if (b.free_z()) {
              ostr << b.z_0() << ");\n";
            }
            else {
              ostr << "NULL);\n";
            }
          }
      }


      { // orientation shifts
        for (int i=1; i<=netinfo->sum_unknowns(); i++)
          {
            if (netinfo->unknown_type(i) != 'R') continue;

            StandPoint* k = netinfo->unknown_standpoint(i);
            double z = y_sign*( k->orientation() )*R2G;
            double c = y_sign*x(i)/10000;

            ostr << "insert into gnu_gama_local_adj_orientation_shifts "
                 << "(conf_id, indx, id, approx, adj) "
                 << "values ("
                 << cnfg() << ", " << i << ", '" << netinfo->unknown_pointid(i) << "', "
                 << z << ", " << (z + c) << ");\n";
          }
      }

      std::vector<int> ind(netinfo->sum_unknowns() + 1);
      {
        int dim = 0;
        for (PointData::const_iterator
               i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
          {
            const LocalPoint& p = (*i).second;
            if (p.active_xy() && p.index_x() != 0) {
              ind[++dim] = p.index_x();
              ind[++dim] = p.index_y();
            }

            if (p.active_z () && p.index_z() != 0) {
              ind[++dim] = p.index_z();
            }
          }

        for (int i=1; i<=netinfo->sum_unknowns(); i++)
          if (netinfo->unknown_type(i) == 'R')
            {
              StandPoint* k = netinfo->unknown_standpoint(i);
              ind[++dim] =  k->index_orientation();
            }
      }


      { // covariance matrix
        int dim  = netinfo->sum_unknowns();
        int band = netinfo->adj_covband();
        if (band < 0) band = dim-1;

        const double m2 = netinfo->m_0() * netinfo->m_0();
        for (int i=1; i<=dim; i++)
          for (int j=i; j<=std::min(dim, i+band); j++)
            {
              ostr << "insert into gnu_gama_local_adj_covmat "
                   << "(conf_id, rind, cind, val) "
                   << "values ("
                   << cnfg() << ", " << i << ", " << j << ", "
                   << m2*netinfo->qxx(ind[i], ind[j])
                   << ");\n";
            }
      }


      { // original indexes
        for (int i=1; i<= netinfo->sum_unknowns(); i++)
          {
            ostr << "insert into gnu_gama_local_adj_original_indexes "
                 << "(conf_id, indx, adj_indx) "
                 << "values ("
                 << cnfg() << ", " << i << ", " << ind[i] << ");\n";
          }
      }


      { // observations
        WriteSQLVisitor writeVisitor(ostr, netinfo);

        for (int indx=1; indx<=netinfo->sum_observations(); indx++)
          {
            Observation* pm = netinfo->ptr_obs(indx);

            ostr << "insert into gnu_gama_local_adj_observations "
                 << "(conf_id, indx, obs_type, from_id, to_id, to_id2, obs, adj, "
                 << "angular, stdev, qrr, f, std_resid, err_obs, err_adj) "
                 << "values ("
                 << cnfg() << ", " << indx << ", ";

            writeVisitor.setObservationIndex(indx);
            pm->accept(&writeVisitor);
            writeVisitor.residualsAndAnalysisOfObservations(pm);
          }

      }
    }

  ostr << "\ncommit;\n";  // commit transaction
}


void LocalNetwork2sql::write_cluster(std::ostream& ostr, const Cluster* c,
                                     int cluster, std::string tag)
{
  const GNU_gama::local::Observation::CovarianceMatrix& covmat = c->covariance_matrix;
  int dim  = covmat.rows();
  int band = covmat.bandWidth();

  ostr << "\ninsert into gnu_gama_local_clusters "
       << "(conf_id, ccluster, dim, band, tag) "
       << "values (" << cnfg() << ", "
       << cluster << ", " << dim << ", " << band << ", '" << tag
       << "');\n";

  for (int i=1; i<=dim; i++)
    for (int j=i; j<=i+band && j <= dim; j++)
      {
        ostr << "insert into gnu_gama_local_covmat "
             << "(conf_id, ccluster, rind, cind, val) "
             << "values (" << cnfg() << ", " << cluster
             << ", " << i << ", " << j << "," << covmat(i,j) << ");\n";
      }
}

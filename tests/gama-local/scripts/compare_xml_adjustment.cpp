/* GNU Gama -- testing adjustment results from different algorithms
   Copyright (C) 2012, 2014, 2018  Ales Cepek <cepek@gnu.org>

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
#include <iomanip>
#include <fstream>
#include <gnu_gama/local/float.h>
#include "compare_xml_adjustment.h"

using GNU_gama::LocalNetworkAdjustmentResults;

int compare_xml_adjustment(GNU_gama::LocalNetworkAdjustmentResults* html,
                           GNU_gama::LocalNetworkAdjustmentResults* xml,
                           double covmat_tol)
{
  int result_gp = 0, rcoord = 0, robs = 0;

  // needs to fix trailing white spaces and HTML tags
  //if (html->description != xml->description) result_gp = 1;

  // general parameters
  {
    bool test =
      html->coordinates_summary.adjusted.xyz    == xml->coordinates_summary.adjusted.xyz   &&
      html->coordinates_summary.adjusted.xy     == xml->coordinates_summary.adjusted.xy    &&
      html->coordinates_summary.adjusted.z      == xml->coordinates_summary.adjusted.z     &&
      html->coordinates_summary.constrained.xyz == xml->coordinates_summary.constrained.xyz&&
      html->coordinates_summary.constrained.xy  == xml->coordinates_summary.constrained.xy &&
      html->coordinates_summary.constrained.z   == xml->coordinates_summary.constrained.z  &&
      html->coordinates_summary.fixed.xyz       == xml->coordinates_summary.fixed.xyz      &&
      html->coordinates_summary.fixed.xy        == xml->coordinates_summary.fixed.xy       &&
      html->coordinates_summary.fixed.z         == xml->coordinates_summary.fixed.z;
    if (!test) {
      std::cout << "         coordinate summary failed\n";
      result_gp = 1;
    }
  }

  {
    bool test =
      html->observations_summary.distances  ==  xml->observations_summary.distances  &&
      html->observations_summary.directions ==  xml->observations_summary.directions &&
      html->observations_summary.angles     ==  xml->observations_summary.angles     &&
      html->observations_summary.xyz_coords ==  xml->observations_summary.xyz_coords &&
      html->observations_summary.h_diffs    ==  xml->observations_summary.h_diffs    &&
      html->observations_summary.z_angles   ==  xml->observations_summary.z_angles   &&
      html->observations_summary.s_dists    ==  xml->observations_summary.s_dists    &&
      html->observations_summary.vectors    ==  xml->observations_summary.vectors;
    if (!test) {
      std::cout << "         observation summary failed\n";
      result_gp = 1;
    }
  }

  {
    bool test =
      html->project_equations.equations          ==  xml->project_equations.equations           &&
      html->project_equations.unknowns           ==  xml->project_equations.unknowns            &&
      html->project_equations.degrees_of_freedom ==  xml->project_equations.degrees_of_freedom  &&
      html->project_equations.defect             ==  xml->project_equations.defect; 
    if (!test) {
      std::cout << "         project equations failed\n";
      result_gp = 1;
    }

    {
      double pvv = std::abs(html->project_equations.sum_of_squares
                            - xml->project_equations.sum_of_squares);
      double qvv = (html->project_equations.sum_of_squares
                    + xml->project_equations.sum_of_squares)/2;
      if (qvv > 1e-5) pvv /= qvv;
      if (pvv > 1e-5)
        {
          std::cout << "         sum of squares failed\n";
          result_gp = 1;
        }
    }
    {
      double mapr = std::abs(html->standard_deviation.apriori
                             - xml->standard_deviation.apriori);
      double qapr = (html->standard_deviation.apriori
                     + xml->standard_deviation.apriori)/2;
      if (qapr) mapr /= qapr;
      if (mapr > 1e-3)
        {
          std::cout << "         apriori standard deviation failed\n";
          result_gp = 1;
        }
    }
    {
      double mapo = std::abs(html->standard_deviation.aposteriori
                             - xml->standard_deviation.aposteriori);
      double qapo = (html->standard_deviation.aposteriori
                     + xml->standard_deviation.aposteriori)/2;
      if (qapo) mapo /= qapo;
      if (mapo > 1e-3)
        {
          std::cout << "         aposteriori standard deviation failed\n";
          result_gp = 1;
        }
    }

    if (html->project_equations.connected_network
        != xml->project_equations.connected_network)
      {
        std::cout << "         network connectivity test failed\n";
        result_gp = 1;
      }

    if (html->standard_deviation.using_aposteriori
        != xml->standard_deviation.using_aposteriori)
      {
        std::cout << "         a priori/a posteriori test failed\n";
        result_gp = 1;
      }

    {
      double mprob = std::abs(html->standard_deviation.probability
                             - xml->standard_deviation.probability);
      double qprob = (html->standard_deviation.probability
                     + xml->standard_deviation.probability)/2;
      if (qprob) mprob /= qprob;
      if (mprob > 1e-5)
        {
          std::cout << "         probability failed\n";
          result_gp = 1;
        }
    }

    if (html->standard_deviation.using_aposteriori)
      {
        if (html->standard_deviation.passed
            != xml->standard_deviation.passed)
          {
            std::cout << "         m0 in (lower, upper) test failed\n";
            result_gp = 1;
          }
        {
          double mratio = std::abs(html->standard_deviation.probability
                                  - xml->standard_deviation.probability);
          double qratio = (html->standard_deviation.probability
                          + xml->standard_deviation.probability)/2;
          if (qratio) mratio /= qratio;
          if (mratio > 1e-5)
            {
              std::cout << "         ratio m0'/m0 failed\n";
              result_gp = 1;
            }
        }
        {
          double mlower = std::abs(html->standard_deviation.lower
                                  - xml->standard_deviation.lower);
          double qlower = (html->standard_deviation.lower
                          + xml->standard_deviation.lower)/2;
          if (qlower) mlower /= qlower;
          if (mlower > 1e-5)
            {
              std::cout << "         lower limit failed\n";
              result_gp = 1;
            }
        }
        {
          double mupper = std::abs(html->standard_deviation.upper
                                  - xml->standard_deviation.upper);
          double qupper = (html->standard_deviation.upper
                          + xml->standard_deviation.upper)/2;
          if (qupper) mupper /= qupper;
          if (mupper > 1e-5)
            {
              std::cout << "         upper limit failed\n";
              result_gp = 1;
            }
        }
        {
          double mcscale = std::abs(html->standard_deviation.confidence_scale
                                  - xml->standard_deviation.confidence_scale);
          double qcscale = (html->standard_deviation.confidence_scale
                          + xml->standard_deviation.confidence_scale)/2;
          if (qcscale) mcscale /= qcscale;
          if (mcscale > 1e-5)
            {
              std::cout << "         confidence scale failed\n";
              result_gp = 1;
            }
        }
      } // if (html->standard_deviation.using_aposteriori)

    if (result_gp == 0) std::cout << "         general parameters"
                                  << "                 passed\n";
  } // general parameters


  // fixed coordinates
  {
    double dfix = 0;
    LocalNetworkAdjustmentResults::Point p, q;

    for (size_t i=0; i<html->fixed_points.size(); i++)
      {
        p = html->fixed_points[i];
        q = html ->fixed_points[i];

        if (p.hxy != q.hxy || p.hz != q.hz)
          {
            rcoord = 1;
          }
        if (p.hxy)
          {
            double d;
            d = p.x - q.x;
            if (std::abs(d) > std::abs(dfix)) dfix = d;
            d = p.y - q.y;
            if (std::abs(d) > std::abs(dfix)) dfix = d;
          }
        if (p.hz)
          {
            double d;
            d = p.z - q.z;
            if (std::abs(d) > std::abs(dfix)) dfix = d;
          }
      }

    std::cout << "         fixed points       "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << dfix << " [m] ";
    if (std::abs(dfix) < 1e-5)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }
  } // fixed coordinates


  bool html_needs_to_be_fixed = html->observations_summary.h_diffs == html->project_equations.equations;

  // adjusted coordinated
  {
    double aprdif = 0;
    double adjdif = 0;

    if (!html_needs_to_be_fixed && html->approximate_points.size() != xml->approximate_points.size() )
      {
        std::cout << "         approximate coordinates dimensions "
                  << html->approximate_points.size() << " "
                  << xml->approximate_points.size()
                  << " failed\n";
        rcoord = 1;
        return rcoord;
      }
    if (!html_needs_to_be_fixed && html->adjusted_points.size() != xml->adjusted_points.size() )
      {
        std::cout << "         adjusted coordinates dimensions "
                  << html->approximate_points.size() << " "
                  << xml->approximate_points.size()
                  << " failed\n";
        rcoord = 1;
        return rcoord;
      }

    for (size_t n=0; n<html->adjusted_points.size(); n++)
      {
        LocalNetworkAdjustmentResults::Point& P=html->adjusted_points[n];
        LocalNetworkAdjustmentResults::Point& Q=xml ->adjusted_points[n];

        LocalNetworkAdjustmentResults::Point& p=html->approximate_points[n];
        LocalNetworkAdjustmentResults::Point& q=xml ->approximate_points[n];

        if (P.id != Q.id)
          {
            std::cout << "         unmatching point ids (index " << n << ") "
                      << P.id << " " << Q.id << " failed\n";
            rcoord = 1;
            return rcoord;
          }
        if (P.id   != Q.id   || p.id   != q.id   ||
            P.hxy  != Q.hxy  || p.hxy  != q.hxy  ||
            P.hz   != Q.hz   || p.hz   != q.hz   ||
            P.cxy  != Q.cxy  ||
            P.cz   != Q.cz   ||
            P.indx != Q.indx ||
            P.indy != Q.indy ||
            P.indz != Q.indz  )
          {
            std::cout << "         unmatching point atttributes failed\n";
            rcoord = 1;
            return rcoord;
          }

        double d;

        if (p.hxy)
          {
            d = p.x - q.x;
            if (std::abs(d) > std::abs(aprdif)) aprdif = d;
            d = p.y - q.y;
            if (std::abs(d) > std::abs(aprdif)) aprdif = d;
          }
        if (p.hz)
          {
            d = p.z - q.z;
            if (std::abs(d) > std::abs(aprdif)) aprdif = d;
            }
        if (P.hxy)
          {
            d = P.x - Q.x;
            if (std::abs(d) > std::abs(adjdif)) adjdif = d;
            d = P.y - Q.y;
            if (std::abs(d) > std::abs(adjdif)) adjdif = d;
          }
        if (P.hz)
          {
            d = P.z - Q.z;
            if (std::abs(d) > std::abs(adjdif)) adjdif = d;
            }

      }
    /* test on approximate coordinates is not relevant
    std::cout << "         approx.coordinates "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << aprdif << " [m] ";
    if (std::abs(aprdif) < 1e-5)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }
    */
    std::cout << "         adjusted coords.   "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << adjdif << " [m] ";
    if (std::abs(adjdif) < 1e-5)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }
  }// adjusted coordinated

  // original index list
  if (!html_needs_to_be_fixed) {
    int tori = 0;
    if (html->original_index.size() != xml->original_index.size()) {
      tori = 1;
    }
    else
      for (size_t i=0; i<html->original_index.size(); i++)
        if (html->original_index[i] != xml->original_index[i])
          {
            tori = 1;
            break;
          }

    if (tori) {
      std::cout << "         original index list failed\n";
      rcoord = 1;
    }
  }  // original index list


  // adjusted orientations
  {
    double oridif = 0;

    if (html->orientations.size() != xml->orientations.size())
      {
        std::cout << "         adj. orientations dimension test failed\n";
        return 1;
      }

    for (size_t i=0; i<html->orientations.size(); i++)
      {
        double d = html->orientations[i].adj - xml->orientations[i].adj;
        if (std::abs(d) > std::abs(oridif)) oridif = d;
      }

    std::cout << "         adj. orientations  "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << oridif << " [g] ";
    if (std::abs(oridif) < 1e-5)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }
  } // adjusted orientations

  // covariance band
  if (!html_needs_to_be_fixed) {
    double dcov = 0;
    double dmax = 1;

    const int dim  = html->cov.dim();
    const int band = std::min(html->cov.bandWidth(), xml->cov.bandWidth());

    for (int i=1; i<=dim; i++)
      for (int j=0; j<=band; j++)
        if (i+j <= dim) {
          double d = html->cov(i,i+j);
          if (d > dmax) dmax = d;

          d -= xml->cov(i,i+j);
          if (std::abs(d) > std::abs(dcov)) dcov = d;
        }

    std::cout << "         cov. matrix band   "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << 100*dcov/dmax << " [%] ";
    if (std::abs(dcov)/dmax < covmat_tol)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }
   } // covariance band


  // observations
  {
    double dang = 0;
    double dlin = 0;
    double fpar = 0;
    double sres = 0;

    if (html->obslist.size() != xml->obslist.size())
      {
        std::cout << "            ###  failed obslist dimension "
                  << html->obslist.size() << " "
                  << xml ->obslist.size() << "\n";
        return 1;
      }

    for (size_t i=0; i<html->obslist.size(); i++)
      {
        LocalNetworkAdjustmentResults::Observation H = html->obslist[i];
        LocalNetworkAdjustmentResults::Observation X = xml ->obslist[i];
        if (H.xml_tag != X.xml_tag)
          {
            std::cout << "            ###  failed xml_tag obs #"
                      << i << " " << H.xml_tag << " " << X.xml_tag << "\n";
            return 1;
          }
        if (H.from != X.from)
          {
            std::cout << "            ###  failed from obs #"
                      << i << " " << H.from << " " << X.from << "\n";
            return 1;
          }
        if (H.to != X.to)
          {
            std::cout << "            ###  failed to obs #"
                      << i << " " << H.to << " " << X.to << "\n";
            return 1;
          }
        if (H.left != X.left)
          {
            std::cout << "            ###  failed left obs #"
                      << i << " " << H.left << " " << X.left << "\n";
            return 1;
          }
        if (H.right != X.right)
          {
            std::cout << "            ###  failed right obs #"
                      << i << " " << H.right << " " << X.right << "\n";
            return 1;
          }

        double dobs = H.obs - X.obs;
        double dadj = H.adj - X.adj;
        if (H.xml_tag == "angle" ||
            H.xml_tag == "direction" ||
            H.xml_tag == "zenith-angle" )
          {
            dobs = std::asin(std::sin(dobs*G2R))*R2G;
            dadj = std::asin(std::sin(dadj*G2R))*R2G;

            if (std::abs(dobs) > std::abs(dang)) dang = dobs;
            if (std::abs(dadj) > std::abs(dang)) dang = dadj;
          }
        else
          {
            if (std::abs(dobs) > std::abs(dlin)) dlin = dobs;
            if (std::abs(dadj) > std::abs(dlin)) dlin = dadj;
          }

        double df = H.f - X.f;
        if (std::abs(df) > std::abs(fpar)) fpar = df;

        double ds = H.std_residual - X.std_residual;
        if (std::abs(ds) > std::abs(sres)) sres = ds;
      }

    std::cout << "         angular obs.       "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << dang << " [g] ";
    if (std::abs(dang) < 1e-6)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }

    std::cout << "         linear observations"
              << std::scientific << std::setprecision(3) << std::setw(11)
              << dlin << " [m] ";
    if (std::abs(dlin) < 1e-5)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }

    std::cout << "         f% obs. checked    "
              << std::scientific << std::setprecision(3) << std::setw(11)
              << fpar << " [%] ";
    if (std::abs(fpar) < 1e-1)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }

    std::cout << "         standardized resid."
              << std::scientific << std::setprecision(3) << std::setw(11)
              << sres << "     ";
    if (std::abs(sres) < 1e-1)
      std::cout << "passed\n";
    else
      {
        std::cout << "failed\n";
        rcoord = 1;
      }

  } // observations

  return result_gp + rcoord + robs;
}

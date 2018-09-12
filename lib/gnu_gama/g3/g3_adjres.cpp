/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2007, 2018  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/g3/g3_adjres.h>
#include <gnu_gama/xml/dataobject.h>

using namespace std;
using namespace GNU_gama::g3;

namespace
{
  bool EOL;

  void xml(std::ostream& out, const std::string data, const std::string& tag)
  {
    if (!data.empty())
      {
        out << "\t<" << tag << ">" << data << "</" << tag << ">";
        if (EOL) out << "\n";
      }

  }
}

void AdjustmentResults::write_xml(std::ostream& out) const
{
  out << GNU_gama::DataObject::Base::xml_begin();

  EOL = true;

  out << "<g3-adjustment-results>\n\n";

  if (!rejected_observations.empty())
    {
      out << "<rejected-observations>\n";
      for (std::list<Observation>::const_iterator
             p=rejected_observations.begin(), e=rejected_observations.end();
           p!=e; ++p)
        {
          out << "\n<rejected> ";
          xml(out, p->ind, "reason");
          out << "\t<"  << p->type << ">\n";

          if (p->type == "vector")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id2,  "to");
              xml(out, p->obs1, "dx");
              xml(out, p->obs2, "dy");
              xml(out, p->obs3, "dz");
            }
          else if (p->type == "xyz")
            {
              xml(out, p->id1,  "id");
              xml(out, p->obs1, "x");
              xml(out, p->obs2, "y");
              xml(out, p->obs3, "z");
            }
          else if (p->type == "distance")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id1,  "to");
              xml(out, p->obs1, "val");
            }
          else if (p->type == "angle")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id1,  "to");
              xml(out, p->obs1, "val");
            }
          else if (p->type == "azimuth")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id1,  "to");
              xml(out, p->obs1, "val");
            }
          else if (p->type == "hdiff")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id1,  "to");
              xml(out, p->obs1, "val");
            }
          else if (p->type == "hobs")
            {
              xml(out, p->id1,  "id");
              xml(out, p->obs1, "val");
            }
          else if (p->type == "zenith")
            {
              xml(out, p->id1,  "from");
              xml(out, p->id1,  "to");
              xml(out, p->obs1, "val");
            }

          out << "\t</" << p->type << ">\n";
          xml(out, p->res1, "flt");
          xml(out, p->res2, "flt");
          xml(out, p->res3, "flt");

          out << "</rejected>\n";
        }
      out << "\n</rejected-observations>\n\n";
    }


  out << "<adjustment-statistics>\n";

  xml(out, algorithm, "algorithm");
  if (!(ell_cap.empty() && ell_id.empty() && ell_a.empty() && ell_b.empty()))
    {
      out << "\t<ellipsoid>\n";
      xml(out, ell_cap, "caption");
      xml(out, ell_id,  "id");
      xml(out, ell_a,   "a");
      xml(out, ell_b,   "b");
      out << "\t</ellipsoid>\n";
    }
  xml(out, parameters,      "parameters");
  xml(out, equations,       "equations");
  xml(out, defect,          "defect");
  xml(out, redundancy,      "redundancy");
  xml(out, sum_of_squares,  "sum-of-squares");
  xml(out, apriori_var,     "apriori-variance");
  xml(out, aposteriori_var, "aposteriori-variance");
  xml(out, variance_factor, "variance-factor-used");
  xml(out, design_m_graph,  "design-matrix-graph");

  out << "</adjustment-statistics>\n\n";

  out << "<adjustment-results>\n";
  for (std::list<Point>::const_iterator
         p=points.begin(), e=points.end(); p!=e; ++p)
    {
      EOL = false;
      out << "\n<point>";
      xml(out, p->id,    "id");
      out << "\n\t<n-" << p->n << "/>";
      xml(out, p->n_dn,  "dn");
      xml(out, p->n_ind, "ind");
      out << "\n";

      out << "\t<e-" << p->e << "/>";
      xml(out, p->e_de,  "de");
      xml(out, p->e_ind, "ind");
      out << "\n";

      out << "\t<u-" << p->u << "/>";
      xml(out, p->u_du,  "du");
      xml(out, p->u_ind, "ind");
      out << "\n";
      EOL = true;

      xml(out, p->cnn, "cnn");
      xml(out, p->cne, "cne");
      xml(out, p->cnu, "cnu");
      xml(out, p->cee, "cee");
      xml(out, p->ceu, "ceu");
      xml(out, p->cuu, "cuu");

      xml(out, p->x_given,      "x-given");
      xml(out, p->x_correction, "x-correction");
      xml(out, p->x_adjusted,   "x-adjusted");
      xml(out, p->y_given,      "y-given");
      xml(out, p->y_correction, "y-correction");
      xml(out, p->y_adjusted,   "y-adjusted");
      xml(out, p->z_given,      "z-given");
      xml(out, p->z_correction, "z-correction");
      xml(out, p->z_adjusted,   "z-adjusted");

      xml(out, p->cnn, "cxx");
      xml(out, p->cne, "cxy");
      xml(out, p->cnu, "cxz");
      xml(out, p->cee, "cyy");
      xml(out, p->ceu, "cyz");
      xml(out, p->cuu, "czz");

      xml(out, p->b_given,      "b-given");
      xml(out, p->b_correction, "b-correction");
      xml(out, p->b_adjusted,   "b-adjusted");
      xml(out, p->l_given,      "l-given");
      xml(out, p->l_correction, "l-correction");
      xml(out, p->l_adjusted,   "l-adjusted");
      xml(out, p->h_given,      "h-given");
      xml(out, p->h_correction, "h-correction");
      xml(out, p->h_adjusted,   "h-adjusted");

      out << "\t</point>\n";
    }
  out << "\n</adjustment-results>\n\n";

  out << "<adjusted-observations>\n";
  for (std::list<Observation>::const_iterator
         p=observations.begin(), e=observations.end(); p!=e; ++p)
    {
      if (p->type == "vector")
        {
          out << "\n<vector>";
          EOL = false;
          xml(out, p->id1, "from");
          xml(out, p->id2, "to");
          EOL = true;
          xml(out, p->ind, "ind");

          xml(out, p->obs1, "dx-observed");
          xml(out, p->res1, "dx-residual");
          xml(out, p->adj1, "dx-adjusted");
          xml(out, p->obs2, "dy-observed");
          xml(out, p->res2, "dy-residual");
          xml(out, p->adj2, "dy-adjusted");
          xml(out, p->obs3, "dz-observed");
          xml(out, p->res3, "dz-residual");
          xml(out, p->adj3, "dz-adjusted");

          xml(out, p->stdev_obs1, "dx-stdev-obs");
          xml(out, p->stdev_adj1, "dx-stdev-adj");
          xml(out, p->stdev_obs2, "dy-stdev-obs");
          xml(out, p->stdev_adj2, "dy-stdev-adj");
          xml(out, p->stdev_obs3, "dz-stdev-obs");
          xml(out, p->stdev_adj3, "dz-stdev-adj");

          xml(out, p->c11, "cxx");
          xml(out, p->c12, "cxy");
          xml(out, p->c13, "cxz");
          xml(out, p->c22, "cyy");
          xml(out, p->c23, "cyz");
          xml(out, p->c33, "czz");

          out << "\t</vector>\n";
        }
      else if (p->type == "xyz")
        {
          out << "\n<xyz>";
          EOL = false;
          xml(out, p->id1, "id");
          EOL = true;
          xml(out, p->ind, "ind");

          xml(out, p->obs1, "x-observed");
          xml(out, p->res1, "x-residual");
          xml(out, p->adj1, "x-adjusted");
          xml(out, p->obs2, "y-observed");
          xml(out, p->res2, "y-residual");
          xml(out, p->adj2, "y-adjusted");
          xml(out, p->obs3, "z-observed");
          xml(out, p->res3, "z-residual");
          xml(out, p->adj3, "z-adjusted");

          xml(out, p->stdev_obs1, "x-stdev-obs");
          xml(out, p->stdev_adj1, "x-stdev-adj");
          xml(out, p->stdev_obs2, "y-stdev-obs");
          xml(out, p->stdev_adj2, "y-stdev-adj");
          xml(out, p->stdev_obs3, "z-stdev-obs");
          xml(out, p->stdev_adj3, "z-stdev-adj");

          xml(out, p->c11, "cxx");
          xml(out, p->c12, "cxy");
          xml(out, p->c13, "cxz");
          xml(out, p->c22, "cyy");
          xml(out, p->c23, "cyz");
          xml(out, p->c33, "czz");

          out << "\t</xyz>\n";
        }
      else if (p->type == "distance")
        {
          out << "\n<distance>";
          EOL = false;
          xml(out, p->id1, "from");
          xml(out, p->id2, "to");
          EOL = true;
          xml(out, p->ind, "ind");

          xml(out, p->obs1, "observed");
          xml(out, p->res1, "residual");
          xml(out, p->adj1, "adjusted");

          xml(out, p->stdev_obs1, "stdev-obs");
          xml(out, p->stdev_adj1, "stdev-adj");

          out << "\t</distance>\n";
        }
      else
        {
          out << "<!-- observation type '" << p->type
              << "' not implemented ->\n";
        }
    }
  out << "\n</adjusted-observations>\n\n";

  out << "</g3-adjustment-results>\n";
  out << GNU_gama::DataObject::Base::xml_end();
}

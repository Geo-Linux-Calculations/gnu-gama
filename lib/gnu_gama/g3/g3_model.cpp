/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/g3/g3_cluster.h>
#include <gnu_gama/outstream.h>
#include <gnu_gama/adj/adj.h>
#include <iomanip>


using namespace std;
using namespace GNU_gama::g3;


Model::Model()
{
  using namespace GNU_gama;

  points         = new PointBase;
  active_obs     = new ObservationList;
  par_list       = new ParameterList;
  adj            = new Adj;
  adj_input_data = 0;

  points->set_common_data(this);
  set(&ellipsoid, ellipsoid_wgs84);

  set_angular_units_gons();

  apriori_sd       = 1.00;
  confidence_level = 0.95;
  tol_abs          = 1e3;

  actual_sd = aposteriori;

  reset();
}


Model::~Model()
{
  delete  points;
  delete  active_obs;
  delete  par_list;
  delete  adj;
  delete  adj_input_data;
}


Point* Model::get_point(const Point::Name& name)
{
  Point* p = points->find(name);
  if (p == 0)
    {
      p = new Point;
      p->name = name;
      points->put(p);
    }

  return p;
}


void Model::write_xml(std::ostream& out) const
{
  using namespace std;

  GNU_gama::SaveFlags sf(out);
  out.setf(ios_base::fixed, ios_base::floatfield);
  out.precision(5);

  out << "<g3-model>\n";

  {
    out << "\n";
    for (Model::PointBase::const_iterator
           b = points->begin(), e = points->end(); b != e; ++b)
      {
        const Point *p = *b;
        out << "<point>\t<id>" << p->name.c_str() << "</id>";

        if (p->has_xyz())
          out << "\n\t"
              << "<x>" << p->X() << "</x> "
              << "<y>" << p->Y() << "</y> "
              << "<z>" << p->Z() << "</z>";

        if (p->has_height())
          out << "\n\t"
              << "<height>" << p->height() << "</height>";

        if (p->unused())
          out << "\n\t"; // <unused/>";
        else if (p->fixed_position())
          out << "\n\t<fixed/>";
        else if (p->free_position() && !p->constr_position())
          out << "\n\t<free/>";
        else if (p->constr_position())
          out << "\n\t<constr/>";
        else {
          if (p->fixed_horizontal_position())
            out << "\n\t<fixed-position/>";
          if (p->free_horizontal_position()&&!p->constr_horizontal_position())
            out << "\n\t<free-position/>";
          if (p->constr_horizontal_position())
            out << "\n\t<constr-position/>";
          if (p->fixed_height())
            out << "\n\t<fixed-height/>";
          if (p->free_height() && !p->constr_height())
            out << "\n\t<free-height/>";
          if (p->constr_height())
            out << "\n\t<constr-height/>";
        }

        out << "</point>\n";
      }
  }

  {
    ClusterList::const_iterator i = obsdata.clusters.begin();
    ClusterList::const_iterator e = obsdata.clusters.end();
    while (i != e)
      {
        if (const g3Cluster* c = dynamic_cast<const g3Cluster*>(*i))
          {
            c->write_xml(out);
          }
        ++i;
      }
  }

  out << "\n</g3-model>\n";
}


void Model::update_index(Parameter& p)
{
  if (!p.has_index())
    {
      if (p.free())
        p.set_index(++dm_cols);
      else
        p.set_index(1);

      par_list->push_back(&p);
    }
}


void Model::update_parameters()
{
  if (!check_init()) update_init();

  for (Model::PointBase::iterator
         i=points->begin(), e=points->end(); i!=e; ++i)
    {
      Point* point = (*i);

      point->N.set_index(0);
      point->E.set_index(0);
      point->U.set_index(0);


      if (point->fixed_horizontal_position())
        {
          point->N.set_fixed();
          point->E.set_fixed();
        }
      else if (point->constr_horizontal_position())
        {
          point->N.set_constr();
          point->E.set_constr();
        }
      else if (point->free_horizontal_position())
        {
          point->N.set_free();
          point->E.set_free();
        }
      else
        {
          point->N.set_unused();
          point->E.set_unused();
        }


      if (point->fixed_height())
        {
          point->U.set_fixed();
        }
      else if (point->constr_height())
        {
          point->U.set_constr();
        }
      else if (point->free_height())
        {
          point->U.set_free();
        }
      else
        {
          point->U.set_unused();
        }
    }

  return next_state_(params_);
}


void Model::update_adjustment()
{
  if (!check_linearization()) update_linearization();

  // parameters 'height' are linked to corresponding parameters 'U'

  for (Model::PointBase::iterator
         i=points->begin(), e=points->end(); i!=e; ++i)
    {
      Parameter& U      = (*i)->U;
      Parameter& height = (*i)->height;

      if      (U.fixed() ) height.set_fixed();
      else if (U.constr()) height.set_constr();
      else if (U.free()  ) height.set_free();
      else                 height.set_unused();

      if (height.free())
        {
          int k = U.index();
          height.set_index(k);
          height.add_correction(adj->x()(k));
        }
    }

  for (ParameterList::iterator
         i=par_list->begin(), e=par_list->end(); i!=e; ++i)
    {
      Parameter* p = *i;
      if (int k = p->index()) p->add_correction(adj->x()(k));
    }

  // ..........................................................

  if (dm_rows == 0 || dm_cols == 0)
    throw GNU_gama::Exception::string("No parameters and/or observations");

  redundancy = dm_rows - dm_cols + adj->defect();

  aposteriori_sd = 0;
  if (redundancy)
    {
      aposteriori_sd = sqrt(adj->rtr()/redundancy);
    }

  if (actual_sd == apriori)
    std_deviation = apriori_sd;
  else
    std_deviation = aposteriori_sd;

  std_variance = std_deviation*std_deviation;

  return next_state_(adjust_);
}


void Model::write_xml_adjustment_input_data(std::ostream& out)
{
  if (!check_linearization()) update_linearization();

  /* see also : GNU_gama::DataParser::xml_start  *
   *            GNU_gama::DataParser::xml_end    */
  adj_input_data->write_xml(out);
}


GNU_gama::E_3 Model::vector(const Point* from, const Point* to) const
{
  GNU_gama::E_3 v;

  v.set(to  ->X(), to  ->Y(), to  ->Z());
  v.sub(from->X(), from->Y(), from->Z());

  return v;
}


GNU_gama::E_3 Model::normal(const Point* p) const
{
  const double B = p->B();
  const double L = p->L();

  return GNU_gama::E_3(std::cos(B)*std::cos(L),
                       std::cos(B)*std::sin(L),
                       std::sin(B)            );
}


GNU_gama::E_3 Model::vertical(const Point* p) const
{
  const double B = p->B() + p->dB();
  const double L = p->L() + p->dL();

  return GNU_gama::E_3(std::cos(B)*std::cos(L),
                       std::cos(B)*std::sin(L),
                       std::sin(B)            );
}

GNU_gama::E_3 Model::instrument(const Point* p, double dh) const
{
  using GNU_gama::E_3;

  E_3 s (p->X(), p->Y(), p->Z());
  E_3 v = vertical(p);
  v *= dh;
  s += v;

  return s;
}


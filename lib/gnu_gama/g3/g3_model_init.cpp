/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005, 2018  Ales Cepek <cepek@gnu.org>

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
#include <gnu_gama/visitor.h>
#include <iomanip>
#include <algorithm>


using namespace std;
using namespace GNU_gama::g3;

using   GNU_gama::ObservationData;
using   GNU_gama::g3::Observation;
using   GNU_gama::g3::Model;
using   ObsList = std::list<Observation*>;


namespace
{
  class Init :
    public GNU_gama::BaseVisitor,
    public GNU_gama::Visitor<Height>,
    public GNU_gama::Visitor<Vector>,
    public GNU_gama::Visitor<XYZ>
  {
  public:

    Init(GNU_gama::g3::Model* m) : model(m)/*, points(model->points)*/ {}

    void visit(Height* p);
    void visit(Vector* p);
    void visit(XYZ* p);
    void approx_xyz_height();

  private:

    GNU_gama::g3::Model*    model;
    std::list<Observation*> a, b;
    ObsList*                obs_in;
    ObsList*                obs_out;
    bool                    updated;
  };


  void Init::approx_xyz_height()
  {
    obs_in  = &a;
    obs_out = &b;

    updated = false;
    ObservationData<Observation>::iterator i=model->obsdata.begin();
    ObservationData<Observation>::iterator e=model->obsdata.end();
    while (i != e)
      {
        (*i++)->accept(this);
      }

    while (updated && !obs_out->empty())
      {
        obs_in->clear();
        std::swap(obs_in, obs_out);

        updated = false;
        for (ObsList::iterator e=obs_in->end(), i=obs_in->begin(); i!=e; i++)
          {
            (*i)->accept(this);
          }
      }
  }


  void Init::visit(Height* p)
  {
    Point::Name id    = p->id;
    Point*      point = model->get_point(id);
    if (!point->has_height())
      {
        point->set_height(p->obs());
        updated = true;
      }
  }


  void Init::visit(Vector* p)
  {
    Point::Name id_from = p->from;
    Point::Name id_to   = p->to;

    Point* from = model->get_point(id_from);
    Point* to   = model->get_point(id_to);

    if      (!from->has_position() && !to->has_position())
      {
        obs_out->push_back(p);
        return;
      }
    else if ( from->has_position() && !to->has_position())
      {
        double x = from->X() + p->dx();
        double y = from->Y() + p->dy();
        double z = from->Z() + p->dz();
        to->set_xyz(x, y, z);
        updated = true;
      }
    else if (!from->has_position() &&  to->has_position())
      {
        double x = to->X() - p->dx();
        double y = to->Y() - p->dy();
        double z = to->Z() - p->dz();
        from->set_xyz(x, y, z);
        updated = true;
      }
  }


  void Init::visit(XYZ* p)
  {
    Point::Name id    = p->id;
    Point*      point = model->get_point(id);
    if (!point->has_position())
      {
        point->set_xyz(p->x(), p->y(), p->z());
        updated = true;
      }
  }
}


void Model::update_init()
{
  rejected_obs.clear();

  Init init(this);
  init.approx_xyz_height();

  return next_state_(init_);
}


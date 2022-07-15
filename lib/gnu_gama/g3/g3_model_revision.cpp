/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005  Ales Cepek <cepek@gnu.org>

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


namespace
{
  class Revision :
    public GNU_gama::BaseVisitor,
    public GNU_gama::Visitor<Angle>,
    public GNU_gama::Visitor<Azimuth>,
    public GNU_gama::Visitor<Distance>,
    public GNU_gama::Visitor<Height>,
    public GNU_gama::Visitor<HeightDiff>,
    public GNU_gama::Visitor<Vector>,
    public GNU_gama::Visitor<XYZ>,
    public GNU_gama::Visitor<ZenithAngle>
  {
  public:

    Revision(GNU_gama::g3::Model* m) : model(m) {}

    void visit(Angle* p)
    {
      model->revision(p);
    }
    void visit(Azimuth* p)
    {
      model->revision(p);
    }
    void visit(Distance* p)
    {
      model->revision(p);
    }
    void visit(Height* p)
    {
      model->revision(p);
    }
    void visit(HeightDiff* p)
    {
      model->revision(p);
    }
    void visit(Vector* p)
    {
      model->revision(p);
    }
    void visit(XYZ* p)
    {
      model->revision(p);
    }
    void visit(ZenithAngle* p)
    {
      model->revision(p);
    }

  private:

    GNU_gama::g3::Model* model;

  };
}



void Model::update_observations()
{
  if (!check_parameters()) update_parameters();

  par_list->clear();
  active_obs->clear();
  dm_rows = dm_cols = dm_floats = 0;   // dimension and size of design matrix


  ::Revision revision(this);
  for (Model::ObservationData::iterator
         i=obsdata.begin(), e=obsdata.end(); i!=e; ++i)
    {
      (*i)->accept(&revision);
    }

  for (Model::ClusterList::iterator ci = obsdata.clusters.begin(),
         ce = obsdata.clusters.end(); ci!=ce; ++ci)
    {
      (*ci)->update();
    }

  return next_state_(obsrvs_);
}



bool Model::revision(Angle* angle)
{
  if (!angle->active()) return false;

  Point* from  = points->find(angle->from);
  Point* left  = points->find(angle->left);
  Point* right = points->find(angle->right);

  if ( from  == 0      ) return angle->set_active(false);
  if ( from->unused()  ) return angle->set_active(false);
  if (!from->has_xyz() ) return angle->set_active(false);

  if ( left  == 0      ) return angle->set_active(false);
  if ( left->unused()  ) return angle->set_active(false);
  if (!left->has_xyz() ) return angle->set_active(false);

  if ( right  == 0     ) return angle->set_active(false);
  if ( right->unused() ) return angle->set_active(false);
  if (!right->has_xyz()) return angle->set_active(false);

  active_obs->push_back(angle);

  update_index(from->N);
  update_index(from->E);
  update_index(from->U);
  update_index(left->N);
  update_index(left->E);
  update_index(left->U);
  update_index(right->N);
  update_index(right->E);
  update_index(right->U);

  dm_rows += angle->dimension();      // design matrix

  if (from ->free_position()) dm_floats += 2;
  if (from ->free_height  ()) dm_floats += 1;
  if (left ->free_position()) dm_floats += 2;
  if (left ->free_height  ()) dm_floats += 1;
  if (right->free_position()) dm_floats += 2;
  if (right->free_height  ()) dm_floats += 1;

  return angle->active();
}



bool Model::revision(Azimuth* a)
{
  if (!a->active()) return false;

  Point* from = points->find(a->from);
  Point* to   = points->find(a->to  );

  if ( from == 0      ||  to == 0      ) return a->set_active(false);
  if ( from->unused() ||  to->unused() ) return a->set_active(false);
  if (!from->has_position()            ) return a->set_active(false);
  if (!to  ->has_position()            ) return a->set_active(false);

  active_obs->push_back(a);

  update_index(from->N);
  update_index(from->E);
  update_index(from->U);
  update_index(to  ->N);
  update_index(to  ->E);
  update_index(to  ->U);

  dm_rows += a->dimension();            // design matrix

  if (from->free_horizontal_position())  dm_floats += 2;
  if (from->free_height())               dm_floats += 1;
  if (to  ->free_horizontal_position())  dm_floats += 2;
  if (to  ->free_height())               dm_floats += 1;

  return a->active();
}



bool Model::revision(Distance* d)
{
  if (!d->active()) return false;

  Point* from = points->find(d->from);
  Point* to   = points->find(d->to  );

  if ( from == 0      ||  to == 0      ) return d->set_active(false);
  if ( from->unused() ||  to->unused() ) return d->set_active(false);
  if (!from->has_position()            ) return d->set_active(false);
  if (!to  ->has_position()            ) return d->set_active(false);

  active_obs->push_back(d);

  update_index(from->N);
  update_index(from->E);
  update_index(from->U);
  update_index(to  ->N);
  update_index(to  ->E);
  update_index(to  ->U);

  dm_rows += d->dimension();            // design matrix

  if (from->free_horizontal_position())  dm_floats += 2;
  if (from->free_height())               dm_floats += 1;
  if (to  ->free_horizontal_position())  dm_floats += 2;
  if (to  ->free_height())               dm_floats += 1;

  return d->active();
}



bool Model::revision(Height* height)
{
  if (!height->active()) return false;

  Point* point = points->find(height->id);

  if ( point == 0                ) return height->set_active(false);
  if ( point->unused()           ) return height->set_active(false);
  if (!point->test_model_height()) return height->set_active(false);

  active_obs->push_back(height);

  update_index(point->U);

  dm_rows += height->dimension();      // design matrix

  if (point->free_height()) dm_floats += 1;

  return height->active();
}



bool Model::revision(HeightDiff* dh)
{
  if (!dh->active()) return false;

  Point* from = points->find(dh->from);
  Point* to   = points->find(dh->to  );

  if ( from == 0      ||  to == 0     ) return dh->set_active(false);
  if ( from->unused() ||  to->unused()) return dh->set_active(false);
  if (!from->test_model_height()      ) return dh->set_active(false);
  if (!to  ->test_model_height()      ) return dh->set_active(false);

  active_obs->push_back(dh);

  update_index(from->U);
  update_index(to  ->U);

  dm_rows += dh->dimension();            // design matrix

  if (from->free_height())               dm_floats += 1;
  if (to  ->free_height())               dm_floats += 1;

  return dh->active();
}



bool Model::revision(Vector* v)
{
  if (!v->active()) return false;

  Point* from = points->find(v->from);
  Point* to   = points->find(v->to  );

  if ( from == 0      ||  to == 0      ) return v->set_active(false);
  if ( from->unused() ||  to->unused() ) return v->set_active(false);
  if (!from->has_position()            ) return v->set_active(false);
  if (!to  ->has_position()            ) return v->set_active(false);

  active_obs->push_back(v);

  update_index(from->N);
  update_index(from->E);
  update_index(from->U);
  update_index(to  ->N);
  update_index(to  ->E);
  update_index(to  ->U);

  dm_rows += v->dimension();            // design matrix

  if (from->free_horizontal_position())  dm_floats += 6;
  if (from->free_height())               dm_floats += 3;
  if (to->free_horizontal_position())    dm_floats += 6;
  if (to->free_height())                 dm_floats += 3;

  return v->active();
}



bool Model::revision(XYZ* xyz)
{
  if (!xyz->active()) return false;

  Point* point = points->find(xyz->id);

  if ( point == 0           ) return xyz->set_active(false);
  if ( point->unused()      ) return xyz->set_active(false);
  if (!point->has_position()) return xyz->set_active(false);

  active_obs->push_back(xyz);

  update_index(point->N);
  update_index(point->E);
  update_index(point->U);

  dm_rows += xyz->dimension();            // design matrix
  if (point->free_horizontal_position())   dm_floats += 6;
  if (point->free_height())                dm_floats += 3;

  return xyz->active();
}



bool Model::revision(ZenithAngle* z)
{
  if (!z->active()) return false;

  Point* from = points->find(z->from);
  Point* to   = points->find(z->to  );

  if ( from == 0      ||  to == 0      ) return z->set_active(false);
  if ( from->unused() ||  to->unused() ) return z->set_active(false);
  if (!from->has_position()            ) return z->set_active(false);
  if (!to  ->has_position()            ) return z->set_active(false);

  active_obs->push_back(z);

  update_index(from->N);
  update_index(from->E);
  update_index(from->U);
  update_index(to  ->N);
  update_index(to  ->E);
  update_index(to  ->U);

  dm_rows += z->dimension();            // design matrix

  if (from->free_horizontal_position())  dm_floats += 2;
  if (from->free_height())               dm_floats += 1;
  if (to  ->free_horizontal_position())  dm_floats += 2;
  if (to  ->free_height())               dm_floats += 1;

  return z->active();
}



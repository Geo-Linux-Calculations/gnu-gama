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
#include <gnu_gama/sparse/smatrix_graph.h>
#include <iomanip>


using namespace std;
using namespace GNU_gama::g3;


namespace
{
  class Linearization :
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

    Linearization(GNU_gama::g3::Model* m) : model(m) {}

    void visit(Angle* p)
    {
      model->linearization(p);
    }
    void visit(Azimuth* p)
    {
      model->linearization(p);
    }
    void visit(Distance* p)
    {
      model->linearization(p);
    }
    void visit(Height* p)
    {
      model->linearization(p);
    }
    void visit(HeightDiff* p)
    {
      model->linearization(p);
    }
    void visit(Vector* p)
    {
      model->linearization(p);
    }
    void visit(XYZ* p)
    {
      model->linearization(p);
    }
    void visit(ZenithAngle* p)
    {
      model->linearization(p);
    }

  private:

    GNU_gama::g3::Model* model;

  };
}



void GNU_gama::g3::Model::update_linearization()
{
  do
    {
      if (!check_observations()) update_observations();

      if (adj_input_data) delete adj_input_data;
      if (A             ) delete A;

      adj_input_data = new AdjInputData;
      A              = new SparseMatrix<>(dm_floats, dm_rows, dm_cols);

      rhs.reset(dm_rows);
      rhs_ind = 0;

      Linearization linearization(this);
      for (ObservationList::iterator
             i=active_obs->begin(), e=active_obs->end(); i!=e; ++i)
        {
          (*i)->accept(&linearization);
        }

    } while (!check_observations());

  if (dm_floats * dm_rows * dm_cols == 0)
    throw GNU_gama::Exception::string("No parameters and/or observations");

  adj_input_data->set_mat(A);
  adj_input_data->set_rhs(rhs);

  {
    GNU_gama::SparseMatrixGraph<> graph(A);

    dm_graph_is_connected = graph.connected();
  }

  {
    int minx=0;
    for (ParameterList::const_iterator
           i=par_list->begin(), e=par_list->end(); i!=e; ++i)
      {
        if ((*i)->constr()) minx++;
      }

    if (minx)
      {
        GNU_gama::IntegerList<>* minlist = new GNU_gama::IntegerList<>(minx);
        GNU_gama::IntegerList<>::iterator m = minlist->begin();
        for (ParameterList::const_iterator
               i=par_list->begin(), e=par_list->end(); i!=e; ++i)
          {
            const Parameter* p = *i;
            if (p->constr())
              {
                *m = p->index();
                ++m;
              }
          }

        adj_input_data->set_minx(minlist);
      }
  }

  {
    int nonzeroes=0, blocks=0;
    for (ClusterList::iterator ci = obsdata.clusters.begin(),
           ce = obsdata.clusters.end(); ci!=ce; ++ci)
      {
        if (int n = (*ci)->activeNonz())
          {
            nonzeroes += n;
            blocks++;
          }
      }

    GNU_gama::BlockDiagonal<>*
      bd = new GNU_gama::BlockDiagonal<>(blocks, nonzeroes);

    for (ClusterList::const_iterator ci = obsdata.clusters.begin(),
           ce = obsdata.clusters.end(); ci!=ce; ++ci)
      {
        CovMat<> C = (*ci)->activeCov();
        C /= (apriori_sd*apriori_sd);      // covariances ==> cofactors
        if (C.dim())
          {
            bd->add_block(C.dim(), C.bandWidth(), C.begin());
          }
      }

    adj_input_data->set_cov(bd);
  }

  adj->set(adj_input_data);

  return next_state_(linear_);
}


void Model::linearization(Angle* pangle)
{
  using namespace std;

  Point* from  = points->find(pangle->from);
  Point* left  = points->find(pangle->left);
  Point* right = points->find(pangle->right);

  const double sca = Angular().scale();
  const double scl = Linear ().scale();
  const double sc  = sca / scl;

  E_3 FI = instrument(from,  pangle->from_dh);
  E_3 FL = instrument(left,  pangle->left_dh);
  E_3 FR = instrument(right, pangle->right_dh);
  E_3 FV = vertical(from);
  FL    -= FI;
  FR    -= FI;
  double ql  = sqrt(FL.e1*FL.e1 + FL.e2*FL.e2 + FL.e3*FL.e3);
  double qr  = sqrt(FR.e1*FR.e1 + FR.e2*FR.e2 + FR.e3*FR.e3);
  if (ql) ql = 1.0/ql;
  if (qr) qr = 1.0/qr;
  FL *= ql;
  FR *= qr;
  E_3 VL, VR;
  VL.cross(FV, FL);
  VR.cross(FV, FR);


  R_3 tran;
  tran.set_rotation(from->B(), from->L());

  E_3 Lxyz(left ->X.init_value() - from->X.init_value(),
           left ->Y.init_value() - from->Y.init_value(),
           left ->Z.init_value() - from->Z.init_value());
  E_3 Rxyz(right->X.init_value() - from->X.init_value(),
           right->Y.init_value() - from->Y.init_value(),
           right->Z.init_value() - from->Z.init_value());

  E_3 Lneu, Rneu;
  tran.inverse(Lxyz, Lneu);
  tran.inverse(Rxyz, Rneu);

  const double dl = sqrt (Lneu.e1*Lneu.e1 + Lneu.e2*Lneu.e2);
  const double dr = sqrt (Rneu.e1*Rneu.e1 + Rneu.e2*Rneu.e2);
  const double sl = atan2(Lneu.e2, Lneu.e1);
  const double sr = atan2(Rneu.e2, Rneu.e1);

  const double psl = sin(sl)/dl;
  const double pcl = cos(sl)/dl;
  const double psr = sin(sr)/dr;
  const double pcr = cos(sr)/dr;

  E_3 Lcoef(-psl,  pcl, 0.0);
  E_3 Rcoef(-psr,  pcr, 0.0);

  E_3 Fcoef( Rcoef );  Fcoef -= Lcoef;  Fcoef *= -1.0;

  tran.rotation(Lcoef, Lxyz);
  tran.rotation(Rcoef, Rxyz);
  tran.set_rotation(left->B(), left->L());
  tran.inverse(Lxyz, Lcoef);
  tran.set_rotation(right->B(), right->L());
  tran.inverse(Rxyz, Rcoef);


  // nonzero derivatives in project equations
  A->new_row();

  if (from->N.free()) A->add_element(Fcoef.e1*sc, from->N.index());
  if (from->E.free()) A->add_element(Fcoef.e2*sc, from->E.index());
  if (from->U.free()) A->add_element(Fcoef.e3*sc, from->U.index());

  if (left->N.free()) A->add_element(Lcoef.e1*sc, left->N.index());
  if (left->E.free()) A->add_element(Lcoef.e2*sc, left->E.index());
  if (left->U.free()) A->add_element(Lcoef.e3*sc, left->U.index());

  if (right->N.free()) A->add_element(Rcoef.e1*sc, right->N.index());
  if (right->E.free()) A->add_element(Rcoef.e2*sc, right->E.index());
  if (right->U.free()) A->add_element(Rcoef.e3*sc, right->U.index());


  // right hand site

  const double angle = GNU_gama::angle(VL, VR);
  rhs(++rhs_ind) = (pangle->obs() - angle)*sca;
}



void Model::linearization(Azimuth* a)
{
  Point* from = points->find(a->from);
  Point* to   = points->find(a->to  );

  E_3 from_vertical, from_to, p1, v1, p2, v2;

  p1.set(from->X(), from->Y(), from->Z());
  v1 = from_vertical = vertical(from);
  v1 *= a->from_dh;
  p1 += v1;                               // instrument

  p2.set(to->X(), to->Y(), to->Z());
  v2  = vertical(to);
  v2 *= a->to_dh;
  p2 += v2;                               // target

  from_to  = p2;
  from_to -= p1;                          // instrument --> target

  R_3 R;
  R.set_rotation(from->B(), from->L());   // dif_NEU --> dif_XYZ
  E_3 local;
  R.inverse(from_to, local);

  // const double h = std::sqrt(local.e1*local.e1 + local.e2*local.e2);

  // pd - partial derivatives for the occupied station
  E_3 pd( std::cos(a->obs()), -std::sin(a->obs()), 0);

  // nonzero derivatives in project equations
  A->new_row();
  if (from->free_horizontal_position())
    {
      A->add_element(pd.e1, from->N.index());
      A->add_element(pd.e2, from->E.index());
    }
  // if (from->free_height())
  //   {
  //     A->add_element(pd.e3, from->U.index());
  //   }

  E_3 tmp;
  R.rotation(pd, tmp);
  tmp *= -1.0;
  R.set_rotation(to->B(),to->L());
  // pd - partial derivatives for the target station
  R.inverse (tmp, pd);

  if (to->free_horizontal_position())
    {
      A->add_element(pd.e1, to->N.index());
      A->add_element(pd.e2, to->E.index());
    }
  if (to->free_height())
    {
      A->add_element(pd.e3, to->U.index());
    }


  // right hand site

  double az = std::atan2(local.e2, local.e1);
  std::cout << "??? azimuth " << (a->obs()*GON_TO_RAD - az)*RAD_TO_GON
            << "\t" << a->obs() << "\t" << az*RAD_TO_GON
            << "\n";
  rhs(++rhs_ind) = a->obs()*GON_TO_RAD - az;
}



void Model::linearization(Distance* d)
{
  Point* from = points->find(d->from);
  Point* to   = points->find(d->to  );

  {
    double dx = to->X() - from->X();
    double dy = to->Y() - from->Y();
    double dz = to->Z() - from->Z();
    if (double dd = std::sqrt(dx*dx + dy*dy + dz*dz))
      {
        dx /= dd;
        dy /= dd;
        dz /= dd;
      }
    from->set_diff_XYZ(-dx, -dy, -dz);
    to  ->set_diff_XYZ( dx,  dy,  dz);
  }



  // nonzero derivatives in project equations
  A->new_row();
  if (from->free_horizontal_position())
    {
      A->add_element(from->diff_N(), from->N.index());
      A->add_element(from->diff_E(), from->E.index());
    }
  if (from->free_height())
    {
      A->add_element(from->diff_U(), from->U.index());
    }

  if (to->free_horizontal_position())
    {
      A->add_element(to->diff_N(), to->N.index());
      A->add_element(to->diff_E(), to->E.index());
    }
  if (to->free_height())
    {
      A->add_element(to->diff_U(), to->U.index());
    }



  // right hand site
  {
    double dx = to->X_dh(d->to_dh) - from->X_dh(d->from_dh);
    double dy = to->Y_dh(d->to_dh) - from->Y_dh(d->from_dh);
    double dz = to->Z_dh(d->to_dh) - from->Z_dh(d->from_dh);
    double D  = std::sqrt(dx*dx + dy*dy + dz*dz);

    double rd = rhs(++rhs_ind) = (d->obs() - D)*Linear().scale();

    if (abs(rd) > tol_abs)
      {
        Model::Rejected robs;

        robs.criterion   = Model::Rejected::rhs;
        robs.observation = d;
        robs.data[0]     = rd;

        rejected_obs.push_back(robs);
        reset_parameters();
        d->set_active(false);
      }
  }
}



void Model::linearization(Height* height)
{
  Point* point = points->find(height->id);

  // nonzero derivatives in project equations
  A->new_row();

  if (point->free_height())  A->add_element(1, point->U.index());

  // right hand site

  rhs(++rhs_ind) = (height->obs() - point->model_height())*Linear().scale();
}



void Model::linearization(HeightDiff* dh)
{
  Point* from = points->find(dh->from);
  Point* to   = points->find(dh->to  );

  // nonzero derivatives in project equations
  A->new_row();

  if (from->free_height())  A->add_element(-1, from->U.index());
  if (to  ->free_height())  A->add_element(+1, to  ->U.index());

  // right hand site

  const double h = to->model_height() - from->model_height();
  rhs(++rhs_ind) = (dh->obs() - h)*Linear().scale();
}



void Model::linearization(Vector* v)
{
  Point* from = points->find(v->from);
  Point* to   = points->find(v->to  );

  for (int i=1; i<=3; i++)
   {
     double tx=0, ty=0, tz=0;
     switch (i) {
     case 1: tx = 1.0; break;
     case 2: ty = 1.0; break;
     case 3: tz = 1.0; break;
     };

     from->set_diff_XYZ(-tx, -ty, -tz);
     to  ->set_diff_XYZ( tx,  ty,  tz);

     // nonzero derivatives in project equations
     A->new_row();

     if (from->free_horizontal_position())
       {
         A->add_element(from->diff_N(), from->N.index());
         A->add_element(from->diff_E(), from->E.index());
       }
     if (from->free_height())
       {
         A->add_element(from->diff_U(), from->U.index());
       }

     if (to->free_horizontal_position())
       {
         A->add_element(to->diff_N(), to->N.index());
         A->add_element(to->diff_E(), to->E.index());
       }
     if (to->free_height())
       {
         A->add_element(to->diff_U(), to->U.index());
       }
   }

   // right hand site
   {
     double rx, dx = to->X_dh(v->to_dh) - from->X_dh(v->from_dh);
     double ry, dy = to->Y_dh(v->to_dh) - from->Y_dh(v->from_dh);
     double rz, dz = to->Z_dh(v->to_dh) - from->Z_dh(v->from_dh);

     const double s = Linear().scale();

     rhs(++rhs_ind) = rx = (v->dx() - dx)*s;
     rhs(++rhs_ind) = ry = (v->dy() - dy)*s;
     rhs(++rhs_ind) = rz = (v->dz() - dz)*s;

     if (abs(rx) > tol_abs || abs(ry) > tol_abs || abs(rz) > tol_abs)
       {
         Model::Rejected robs;

         robs.criterion   = Model::Rejected::rhs;
         robs.observation = v;
         robs.data[0]     = rx;
         robs.data[1]     = ry;
         robs.data[2]     = rz;

         rejected_obs.push_back(robs);
         reset_parameters();
         v->set_active(false);
       }
   }
}



void Model::linearization(XYZ* xyz)
{
  Point* point = points->find(xyz->id);

  for (int i=1; i<=3; i++)
   {
     double tx=0, ty=0, tz=0;
     switch (i) {
     case 1: tx = 1.0; break;
     case 2: ty = 1.0; break;
     case 3: tz = 1.0; break;
     };

     point->set_diff_XYZ(tx, ty, tz);

     // nonzero derivatives in project equations
     A->new_row();

     if (point->free_horizontal_position())
       {
         A->add_element(point->diff_N(), point->N.index());
         A->add_element(point->diff_E(), point->E.index());
       }
     if (point->free_height())
       {
         A->add_element(point->diff_U(), point->U.index());
       }
   }

   // right hand site
   {
     double rx, x = point->X();
     double ry, y = point->Y();
     double rz, z = point->Z();

     const double s = Linear().scale();

     rhs(++rhs_ind) = rx =(xyz->x() - x)*s;
     rhs(++rhs_ind) = ry = (xyz->y() - y)*s;
     rhs(++rhs_ind) = rz = (xyz->z() - z)*s;

     if (abs(rx) > tol_abs || abs(ry) > tol_abs || abs(rz) > tol_abs)
       {
         Model::Rejected robs;

         robs.criterion   = Model::Rejected::rhs;
         robs.observation = xyz;
         robs.data[0]     = rx;
         robs.data[1]     = ry;
         robs.data[2]     = rz;

         rejected_obs.push_back(robs);
         reset_parameters();
         xyz->set_active(false);
       }
   }
}



void Model::linearization(ZenithAngle* z)
{
  Point* from = points->find(z->from);
  Point* to   = points->find(z->to  );

  E_3 from_vertical, from_to, p1, v1, p2, v2;

  p1.set(from->X(), from->Y(), from->Z());
  v1 = from_vertical = vertical(from);
  v1 *= z->from_dh;
  p1 += v1;                               // instrument

  p2.set(to->X(), to->Y(), to->Z());
  v2  = vertical(to);
  v2 *= z->to_dh;
  p2 += v2;                               // target

  from_to  = p2;
  from_to -= p1;                          // instrument --> target

  R_3 R;
  R.set_rotation(from->B(), from->L());   // dif_NEU --> dif_XYZ
  E_3 local;
  R.inverse(from_to, local);

  const double r = std::sqrt(local.e1*local.e1 + local.e2*local.e2);
  const double s = local.e1*local.e1 + local.e2*local.e2 + local.e3*local.e3;
  const double q = 1.0/(r*s);

  // pd - partial derivatives for the occupied station
  E_3 pd( -local.e1*q, -local.e2*q, r/s);


  const double sca = Angular().scale();
  const double scl = Linear ().scale();
  const double sc  = sca / scl;


  // nonzero derivatives in project equations
  A->new_row();
  if (from->free_horizontal_position())
    {
      A->add_element(pd.e1*sc, from->N.index());
      A->add_element(pd.e2*sc, from->E.index());
    }
  if (from->free_height())
    {
      A->add_element(pd.e3*sc, from->U.index());
    }

  E_3 tmp;
  R.rotation(pd, tmp);
  tmp *= -1.0;
  R.set_rotation(to->B(),to->L());
  // pd - partial derivatives for the target station
  R.inverse (tmp, pd);

  if (to->free_horizontal_position())
    {
      A->add_element(pd.e1*sc, to->N.index());
      A->add_element(pd.e2*sc, to->E.index());
    }
  if (to->free_height())
    {
      A->add_element(pd.e3*sc, to->U.index());
    }


  // right hand site

  double za = angle(from_vertical, from_to);

  /************************************************************/
  /*          !!! add refraction correction here !!!          */
  /************************************************************/

  rhs(++rhs_ind) = (z->obs() - za)*sca;
}


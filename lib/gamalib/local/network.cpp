/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: network.cpp,v 1.1 2006/04/09 16:40:24 cepek Exp $
 */

#include <fstream>
#include <iomanip>
#include <cctype>
#include <memory>
#include <set>
#include <typeinfo>

#include <gamalib/local/network.h>
#include <gamalib/local/linearization.h>
#include <gamalib/itstream.h>
#include <gamalib/skipcomm.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/sparse/smatrix_graph.h>
#include <gnu_gama/version.h>

using namespace std;
using namespace GaMaLib;


typedef GNU_gama::List<GNU_gama::Cluster<Observation>*> ClusterList;
typedef GNU_gama::Cluster<Observation>                  Cluster;


LocalNetwork::LocalNetwork()        
  : pocbod_(0), tst_redbod_(false), pocmer_(0), tst_redmer_(false),
    m_0_apr_(10), konf_pr_(0.95), tol_abs_(1000), 
    update_constrained_coordinates_(false), typ_m_0_(empiricka_),
    tst_rov_opr_(false), tst_vyrovnani_(false), min_n_(0), min_x_(0),
    gons_(true)
{
  epoch = 0.0;
  Asp = 0;
}


LocalNetwork::~LocalNetwork()
{
  delete[] min_x_;  
  delete   Asp;
}
 

void LocalNetwork::revision_points()
{
  if (tst_redbod_) return;
  
  undefined_xy_z_.erase(undefined_xy_z_.begin(), undefined_xy_z_.end());
  pocbod_ = 0;
  
  for (PointData::iterator bod=PD.begin(); bod!=PD.end(); ++bod)
    {
      bool ok = true;
      LocalPoint& b = (*bod).second;

      b.set_xyz_0();       // store initial values even for unused points

      if (b.active_xy())
        {
          if (b.test_xy())
            {
              pocbod_++;
            }
          else
            {
              b.unused_xy();
              removed( (*bod).first, rm_missing_xy );
              ok = false;
            }
        }

      if (b.active_z())
        {
          if (b.test_z())
            {
              if (!b.active_xy() || !b.test_xy()) pocbod_++;
            }
          else
            {
              b.unused_z();
              removed( (*bod).first, rm_missing_z );
              ok = false;
            }
        }

      if (!ok) undefined_xy_z_.push_back((*bod).first);
    }

  tst_redbod_ = true;
  update(Observations);
}


void LocalNetwork::revision_observations()
{
  if (!tst_redbod_) revision_points();

  {
    const LocalRevision local_rev(PD);
    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
      {
        Observation* m = *i;
        if (!m->revision(&local_rev)) m->set_passive();
      }
  }

  // test cycle for StandPoint clusters with single direction
  ClusterList& clusters = OD.clusters;
  for (ClusterList::iterator cit=clusters.begin(); cit!=clusters.end(); ++cit)
    {
      if (StandPoint* sp = dynamic_cast<StandPoint*>(*cit))
        {
          // 1.3.08 *** check for directions pointing to the same targer
          set<PointID> targets;

          int active_directions = 0;
          for (ObservationList::iterator i=sp->observation_list.begin();
               i != sp->observation_list.end(); ++i)
            {
              if (const Direction* d = dynamic_cast<const Direction*>(*i))
                if (d->active())
                  {
                    set<PointID>::const_iterator s = targets.find( d->to() );
                    if (s == targets.end()) 
                      {
                        active_directions++;
                        targets.insert( d->to() );
                      }
                  }
            }
          
          if (active_directions < 2)
            {
              for (ObservationList::iterator 
                     i  = sp->observation_list.begin();
                   i != sp->observation_list.end(); ++i)
                if (Direction* d = dynamic_cast<Direction*>(*i))
                  d->set_passive();
            }
        }
      (*cit)->update();
    }
  
  RSM.clear();
  removed_obs.clear();
  for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    {
      Observation* m = *i;
      
      if (m->active()) RSM.push_back(m);
      else             removed_obs.push_back(m);
    }
  pocmer_ = RSM.size();

  tst_redmer_ = true;
  update(Residuals);
}


void LocalNetwork::project_equations()
{
  if (tst_rov_opr_) return;
  if (!tst_redmer_) revision_observations();
  
  for (PointData::iterator bod=PD.begin(); bod!=PD.end(); ++bod)
    {
      LocalPoint& b = (*bod).second;
      if (b.active_xy() || b.active_z())
        {
          // indexes of unknowns in project equations: 1, 2, ...
          b.index_y() = b.index_x() = b.index_z() = 0; 
        }
    }

  ClusterList& clusters = OD.clusters;
  for (ClusterList::iterator cl=clusters.begin(); cl!=clusters.end(); ++cl)
    if (StandPoint* standpoint=dynamic_cast<StandPoint*>(*cl))
      standpoint->index_orientation(0);
  
  {
    LocalLinearization  loclin(PD, m_0_apr_);

    const size_t  V = pocmer_;               // vectors
    const size_t  M = V * loclin.max_size;   // reserved memory 


    GNU_gama::SparseMatrix<Double, Index>* 
      tmp = new GNU_gama::SparseMatrix<Double, Index>(M, V, 0);

    b.reset(pocmer_);   // initialisation of base class OLS
    rhs_.reset(pocmer_);
    
    int  r = 0;
    pocet_neznamych_ = 0;
    for (RevisedObsList::iterator m=RSM.begin(); m!=RSM.end(); ++m)
      {
        (*m)->linearization(&loclin);
        b(++r)  = loclin.rhs;
        rhs_(r) = loclin.rhs; 
        tmp->new_row();
        for (long i=0; i<loclin.size; i++)
            tmp->add_element(loclin.coeff[i], loclin.index[i]);
    }

    pocet_neznamych_ = loclin.unknowns();
    A.reset(pocmer_, pocet_neznamych_);   // initialization of base class OLS
    A.set_zero();

    delete Asp;
    Asp =  tmp->replicate(tmp->nonzeroes(), pocmer_, pocet_neznamych_ );
    delete tmp;

    {
      GNU_gama::SparseMatrixGraph<> graph(Asp);

      design_matrix_graph_is_connected = graph.connected();
    }

    Double* a = Asp->begin(1);
    Index*  i = Asp->ibegin(1);
    for (int n, row=1; row<=pocmer_; row++)
      {
        n = Asp->size(row);
        while (n--)
          A(row, *i++) = *a++;
      }
  }

  seznez_.erase(seznez_.begin(), seznez_.end());
  seznez_.resize(pocet_neznamych_);
  neznama_ nez;
  
  for (ClusterList::iterator 
         clptr=clusters.begin(); clptr!=clusters.end(); ++clptr)
    if (StandPoint* standpoint=dynamic_cast<StandPoint*>(*clptr))
      /*
       * in the following if (...) statement we test index of an orientation
       * to skip stations with single direction (gamalib-1.1.10)
       */
      if (standpoint->test_orientation() && standpoint->index_orientation())
        {
          const LocalPoint& station = PD[standpoint->station];
          if (station.active_xy())
            {
              nez.cb  = standpoint->station;
              nez.osn = standpoint;
              nez.typ = 'R';
              seznez_[standpoint->index_orientation()-1] = nez;
          }
        }

  for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
    {
      const LocalPoint& b = (*i).second;

      if (b.active_xy() && b.index_y())
        {
          nez.cb = (*i).first;
          nez.osn =  0;
          nez.typ = 'X';  seznez_[b.index_x()-1] = nez;
          nez.typ = 'Y';  seznez_[b.index_y()-1] = nez;
        }

      if (b.active_z() && b.index_z())
        {
          nez.cb = (*i).first;
          nez.osn =  0;
          nez.typ = 'Z';  seznez_[b.index_z()-1] = nez;
        }
    }
  
  // ...  initialization of base class OLS  .............................
    
  vybocujici_abscl_ = false;
  {   // for ...
    for (long r=1; r<=pocmer_; r++)
      {
        if (test_abs_term(r)) vybocujici_abscl_ = true;
      }
  }   // for ...
  
  prepareProjectEquations();  // [A, b] scaled by chol. dec. of weight matrix 
  network_data(this);         // supply data for derived reset(A,b) if needed

  if (singular_coords(A))
    {
      update(Points);
      project_equations();
      return;  
    }

  reset(A, b);

  //--ofstream opr("A_scaled.bin", ios_base::trunc); 
  //--int m=A.rows();
  //--int n=A.cols();
  //--opr.write((char*)(&m), sizeof(int));
  //--opr.write((char*)(&n), sizeof(int));
  //--opr.write((char*)(A.begin()), sizeof(Double)*m*n);

  delete[] min_x_;
  min_x_ = 0;
  min_n_ = 0;

  for (PointData::iterator i=PD.begin(); i!=PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.constrained_xy() && p.index_x())  min_n_ += 2;
      if (p.constrained_z()  && p.index_z())  min_n_ += 1;
    }

  if (min_n_)
    {
      min_x_ = new Index[min_n_];
      int n = 0;
      for (PointData::iterator i=PD.begin(); i!=PD.end(); ++i)
        {
          const LocalPoint& p = (*i).second;
          if (p.constrained_xy() && p.index_x())
            {
              min_x_[n++] = p.index_y();
              min_x_[n++] = p.index_x();
            }
          if (p.constrained_z() && p.index_z())
            {
              min_x_[n++] = p.index_z();
            }
        }
      min_x(min_n_, min_x_);
    }
  
  tst_rov_opr_ = true;
  update(Adjustment);
}


bool LocalNetwork::singular_coords(const Mat& A)
{
  bool result = false;

  Double a, b, aa, ab, bb, D;          // testing xy submatrix is sufficient
  Index  indx, indy, r;

  for (PointData::iterator i=PD.begin(); i!=PD.end(); ++i)
    {
      LocalPoint&  p  = (*i).second;
      if (!p.free_xy() || p.index_x()==0) continue;

      indx = p.index_x();
      indy = p.index_y();

      aa = ab = bb = 0;
      for (r=1; r<=A.rows(); r++)
        {
          a = A(r,indx);
          b = A(r,indy);
          aa += a*a;
          ab += a*b;
          bb += b*b;
        }

      D = aa*bb - ab*ab;

      if ((aa == 0) || (fabs(D) <= aa*1e-6))
        {
          result = true;
          p.unused_xy();
          removed( (*i).first, rm_singular_xy );
        }
      
    }

  return result;
}


void LocalNetwork::project_equations(std::ostream& out)
{
  using namespace std;
  project_equations();

  out << "\n" << pocet_neznamych_ << " " << pocmer_ << "\n\n";
  
  Index*  ib;
  Index*  ie;
  Double* nb;
  Double* ne;
  for (Index i=1; i<=Asp->rows(); i++)
    {
      out << Asp->size(i) << ' ';     // number of nonzeroes in the i-th row
      ib = Asp->ibegin(i);
      ie = Asp->iend(i);
      while (ib != ie)
        {
          out << *ib << ' ';          // indexes
          ++ib;
        }
      out << endl;

      out << weight_obs(i) << ' ';    // weight
      out << rhs(i) << ' ';           // abs. term

      nb = Asp->begin(i);
      ne = Asp->end(i);
      while (nb != ne)
        {
          out << *nb << ' ';          // coefficients
          ++nb;
        }
      out << endl;
    }
}


void LocalNetwork::project_equations(Mat& A_, Vec& b_, Vec& w_)
{
  project_equations();
  
  A_.reset(A.rows(), A.cols());
  A_.set_zero();
  b_.reset(A.rows());
  w_.reset(A.rows());
  
  Index*  ib;
  Double* nb;
  Double* ne;
  for (Index i=1; i<=Asp->rows(); i++)
    {
      ib = Asp->ibegin(i);
      nb = Asp-> begin(i);
      ne = Asp-> end(i);
      while (nb != ne)
        {
          A_(i, *ib) = *nb;
          ++nb;
          ++ib;
        }
      
      b_(i) = rhs(i);
      w_(i) = weight_obs(i);
    }
}


void LocalNetwork::conf_pr(Double p)
{
  if (p <= 0 || p >= 1) 
    throw GaMaLib::Exception(T_LN_undefined_confidence_level);
  konf_pr_ = p;
}


Double LocalNetwork::m_0()
{
  if (m_0_apriori())
    return m_0_apr_;
  else if (m_0_aposteriori())
    {
      vyrovnani_();
      const int nadb = degrees_of_freedom();
      if (nadb > 0)
        return sqrt(trans_VWV()/nadb);
      else
        return 0;
    }
  else
    {
      throw GaMaLib::Exception(T_LN_undefined_type_of_actual_sigma);
    }
}


Double LocalNetwork::conf_int_coef()
{
  using namespace GaMaLib;
  
  Double pravdepodobnost = (1 - konf_pr_)/2;
  if (m_0_apriori())
    return GNU_gama::Normal(pravdepodobnost);
  else if (m_0_aposteriori())
    {
      const int nadb = degrees_of_freedom();
      if ( nadb > 0)
        return GNU_gama::Student(pravdepodobnost, nadb);
      else
        return 0;
    }
  else
    {
      throw GaMaLib::Exception(T_LN_undefined_type_of_actual_sigma);
    }
}


void LocalNetwork::update(Update etapa)
{
  switch(etapa)
    {
    default:
    case Points:        tst_redbod_    = false;
    case Observations:  tst_redmer_    = false;
    case Residuals:     tst_rov_opr_   = false;
    case Adjustment:    tst_vyrovnani_ = false;
    }
}


Double LocalNetwork::test_abs_term(Index indm)
{
  const Observation* m = RSM[indm-1];

  // 2005-12-28 added test for coordinates and vectors
  //
  // if (dynamic_cast<const Coordinates*>(m->ptr_cluster())) return 0;
  // if (dynamic_cast<const Vectors*>(m->ptr_cluster())) return 0;

  const LocalPoint& stan = PD[m->from()];
  const LocalPoint& cil  = PD[m->to()];   // ignoring second angle target here

  if (const H_Diff* h = dynamic_cast<const H_Diff*>(m))
    {
      const Double h0 = cil.z() - stan.z();
      if (fabs(h->value() - h0)*1000 > tol_abs_)
        return b(indm);
      else
        return 0;
    }

  { 
    Double dx, dy, d0;
    if (stan.test_xy() && cil.test_xy())
      {
        dy = stan.y() - cil.y();
        dx = stan.x() - cil.x();
        d0 = sqrt(dy*dy + dx*dx);
      }
    
    if      (typeid(*m) == typeid(Distance))
      {
        if (fabs(m->value() - d0)*1000 > tol_abs_)
          return b(indm);
        else
          return 0;
      }
    else if (typeid(*m) == typeid(Direction))
      {
        if (fabs(b(indm)*d0/(10*R2G)) > tol_abs_)
          return b(indm);
        else
          return 0;
      }
    else if (typeid(*m) == typeid(Angle))
      {
        if (fabs(b(indm)*d0/(10*R2G)) > tol_abs_)
          return b(indm);
        else
          return 0;
      }
    else if (typeid(*m) == typeid(Z_Angle))
      {
        Double dz = stan.z() - cil.z();
        Double d3 = sqrt(dz*dz + d0*d0);
        if (fabs(b(indm)*d3/(10*R2G)) > tol_abs_)
          return b(indm);
        else
          return 0;
      }
    else if (typeid(*m) == typeid(S_Distance))
      {
        Double dz = stan.z() - cil.z();
        Double d3 = sqrt(dz*dz + d0*d0);
        if (fabs(d3 - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;   
      }
    else if (typeid(*m) == typeid(Xdiff))
      {
        const Double dx = cil.x() - stan.x();
        if (fabs(dx - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
    else if (typeid(*m) == typeid(Ydiff))
      {
        const Double dy = cil.y() - stan.y();
        if (fabs(dy - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
    else if (typeid(*m) == typeid(Zdiff))
      {
        const Double dz = cil.z() - stan.z();
        if (fabs(dz - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
    else if (typeid(*m) == typeid(X))
      {
        if (fabs(stan.x() - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
    else if (typeid(*m) == typeid(Y))
      {
        if (fabs(stan.y() - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
    else if (typeid(*m) == typeid(Z))
      {
        if (fabs(stan.z() - m->value())*1000 > tol_abs_)
          return b(indm);
        else
          return 0;           
      }
  } 

  throw GaMaLib::Exception("LocalNetwork::test_abs_term() - unknown observation");

}


void LocalNetwork::remove_huge_abs_terms()
{
  if (!huge_abs_terms()) return;

  Index r = 0;
  for (RevisedObsList::iterator m = RSM.begin(); m!=RSM.end(); ++m)
    if (test_abs_term(++r))
      (*m)->set_passive();
  
  update(Observations);
}


int LocalNetwork::null_space()
{
  try 
    { 
      vyrovnani_(); 
    } 
  catch(const MatVecException& vs) 
    {
      if (vs.error != GNU_gama::Exception::BadRegularization) throw;
      
      for (Index i=1; i<=sum_unknowns(); i++)
        if (lindep(i))
          {
            const char    type = unknown_type(i);
            const PointID id   = unknown_pointid(i);
            
            LocalPoint& p = PD[id];
            if (type == 'X' || type == 'Y' || type == 'R' )
              {
                p.unused_xy();
                removed(id, rm_singular_xy );
              }
            else if (type == 'Z' )
              {
                p.unused_z();
                removed(id, rm_missing_z );
              }
            
            return null_space();
          }
    } 
  
  return defect();
}


void LocalNetwork::std_error_ellipse(const PointID& cb, 
                                     Double& a, Double& b, Double& alfa)
{
  using namespace std;

  const LocalPoint& bod = PD[cb];
  int iy = bod.index_y();
  int ix = bod.index_x();
  Double cyy = q_xx(iy,iy);
  Double cyx = q_xx(iy,ix);
  Double cxx = q_xx(ix,ix); 
  Double c = sqrt((cxx-cyy)*(cxx-cyy) + 4*cyx*cyx);
  b = (cyy+cxx-c)/2;
  if (b < 0) b = 0;

  Double m = m_0();
  a = m * sqrt(b+c);
  b = m * sqrt(b);
  if (c == 0) {
     alfa = 0;
     return;
  }
  alfa = atan2(2*cyx, cxx-cyy)/2;
  if (alfa < 0) alfa += M_PI;
}

// 1.7.09 added optional update of constrained coordinates; inspired
// by adjustment of photographic observation by Jim Sutherland

void LocalNetwork::refine_approx()
{
  for (int i=1; i<=sum_unknowns(); i++)
    if (unknown_type(i) == 'X')
      {
        const PointID& cb = unknown_pointid(i);
        LocalPoint& b = PD[cb];
        if (!b.constrained_xy() || update_constrained_coordinates())
            b.set_xy(b.x() + x(i)/1000, b.y() + x(i+1)/1000);
      }
    else if (unknown_type(i) == 'Z')
      {
        const PointID& cb = unknown_pointid(i);
        LocalPoint& b = PD[cb];
        if (!b.constrained_z() || update_constrained_coordinates())
          b.set_z(b.z() + x(i)/1000);
      }
    else if (unknown_type(i) == 'R')
      {
        StandPoint* standpoint = unknown_standpoint(i);
        Double ori = ( standpoint->orientation() )*R2G + x(i)/10000;
        standpoint->set_orientation( ori*G2R );
      }

  update(Residuals);
}


// preparing for project equations - Cholesky decomposition of
// covariance matrix

void LocalNetwork::cholesky(CovMat& chol)
{
  chol.cholDec();

  using namespace std;
  const Index N = chol.rows();
  const Index b = chol.bandWidth();

  for (Index m, j, i=1; i<=N; i++)
    {
      double d = sqrt(chol(i,i));
      chol(i,i) = d;

      m = i+b;  if(N < m) m = N;    // m = min(N, i+b);
      
      for (j=i+1; j<=m; j++) chol(i,j) *= d;
    }
}

void LocalNetwork::forwardSubstitution(const CovMat& chol, Vec& v)
{
  using namespace std;
  const Index N = chol.rows();
  const Index b = chol.bandWidth();

  for (Index m, i=1; i<=N; i++)
    {
      if (i > b+1) m = i - b;
      else         m = 1;
      for (Index j=m; j<i; j++) v(i) -= chol(i,j)*v(j);

      v(i) /= chol(i,i);
    }
}

// void LocalNetwork::backwardSubstitution(const Cov& chol, Vec& v)
// {
//   using namespace std;
//   const Index N = chol.rows();
//   const Index b = chol.bandWidth();
// 
//   for (Index i=N; i>0; i--)
//     {
//       Index m = min(N, i+b);
//       for (Index j=i+1; j<=m; j++) v(i) -= chol(i,j)*v(j);
// 
//       v(i) /= chol(i,i);
//     }
// }

void LocalNetwork::prepareProjectEquations()
{
  Index ind_0 = 0;

  for (ClusterList::const_iterator 
         cluster=OD.clusters.begin(); cluster!=OD.clusters.end(); ++cluster)
    if (const Index N = (*cluster)->activeObs())
        {
          Vec t(N);
          CovMat C = (*cluster)->activeCov();
          C /= (m_0_apr_*m_0_apr_);        // covariances ==> cofactors
          cholesky(C);                     // cofactors   ==> weights

          for (Index j=1; j<=A.cols(); j++)
            {
              bool empty = true;
              for (Index k=1; k<=N; k++)
                {
                  Double tmp = A(ind_0+k,j);
                  t(k) = tmp;
                  if (tmp) empty = false;
                }
                if (empty) continue;
                
                forwardSubstitution(C, t);
                for (Index l=1; l<=N; l++) A(ind_0+l,j) = t(l);
            }
          
          for (Index k=1; k<=N; k++) t(k) = b(ind_0+k);
          forwardSubstitution(C, t);
          for (Index l=1; l<=N; l++) b(ind_0+l) = t(l);

          ind_0 += N;
        }
}


void LocalNetwork::vyrovnani_()
{
  using namespace GaMaLib;
  if (tst_vyrovnani_) return;

  project_equations();
  if (sum_unknowns()     == 0) 
    throw GaMaLib::Exception(T_GaMa_No_unknowns_defined);
  if (sum_observations() == 0)
    throw GaMaLib::Exception(T_GaMa_No_observations_available);
  if (sum_points()      == 0)
    throw GaMaLib::Exception(T_GaMa_No_points_available);

  GNU_gama::AdjBase<Double, GaMaLib::MatVecException>::solve();

  tst_vyrovnani_ = true;

  { /* ----------------------------------------------------------------- */
    // check for huge covariances / indefinite coordinates

    for (PointData::iterator i=PD.begin(); i!=PD.end(); ++i)
      {
        LocalPoint&     P  = (*i).second;
        const PointID&  id = (*i).first;

        if (!P.free_xy() && !P.free_z()) continue;

        Double tx=0, ty=0, tz=0;
        if (int ix = P.index_x()) tx = m_0_apr_ * sqrt( q_xx(ix,ix) );
        if (int iy = P.index_y()) ty = m_0_apr_ * sqrt( q_xx(iy,iy) );
        if (int iz = P.index_z()) tz = m_0_apr_ * sqrt( q_xx(iz,iz) );

        bool bxy = (tx > 1e4) || (ty > 1e4);
        bool bz  = (tz > 1e4);
 
        if (bxy && bz) 
          { 
            P.unused_xy();
            P.unused_z(); 
            removed(id, rm_huge_cov_xyz);
            tst_vyrovnani_ = false;
          }
        else if (bxy)
          {
            P.unused_xy();
            removed(id, rm_huge_cov_xy);
            tst_vyrovnani_ = false;
          }
        else if (bz)
          {
            P.unused_z();
            removed(id, rm_huge_cov_z);
            tst_vyrovnani_ = false;
          }
      }

    if (!tst_vyrovnani_) 
      {
        vyrovnani_();
        return;
      }
  }

  { /* ----------------------------------------------------------------- */
    r = GNU_gama::AdjBase<Double, GaMaLib::MatVecException>::residuals();
    suma_pvv_ = 0;

    Double tmp;
    Index  ind_0 = 0;
    
    for (ClusterList::const_iterator 
           cluster=OD.clusters.begin(); cluster!=OD.clusters.end(); ++cluster)
      if (const Index N = (*cluster)->activeObs())
        {
          Vec t(N), u(N);
          CovMat C = (*cluster)->activeCov();
          C /= (m_0_apr_*m_0_apr_);
          cholesky(C);
          
          for (Index k=1; k<=N; k++)
            {
              tmp  = r(ind_0+k); 
              t(k) = tmp;
              suma_pvv_ += tmp*tmp;
            }
          const CovMat&  CC = C;
          const Index b  = CC.bandWidth();
          {
            for (Index m, i=1; i<=N; i++)
              {
                Double s=0;
                if (i > b+1) m = i - b;
                else         m = 1;
                for (Index j=m; j<=i; j++) s += CC(i,j)*t(j);
                u(i) = s;
              }
          }
          for (Index l=1; l<=N; l++) r(ind_0+l) = u(l);
          
          ind_0 += N;
        }
  }

  { /* ----------------------------------------------------------------- */
    sigma_L.reset(pocmer_);

    Double MM = m_0() / m_0_apr_;
    Index ind_0 = 0;

    for (ClusterList::const_iterator 
           cit=OD.clusters.begin(); cit!=OD.clusters.end(); ++cit)
      if (const Index N = (*cit)->activeObs())
        {
          const Cluster& cluster = *(*cit);
          // ??? if (cluster.covariance_matrix.bandWidth())
          // ???   {
          // ???     // vypocet pro korelovana mereni zatim chybi !!!
          // ???   }
          // ??? else
            {
              Index n = ind_0+1;
              for (ObservationList::const_iterator 
                     i = cluster.observation_list.begin();
                     i!= cluster.observation_list.end(); ++i)
                if ((*i)->active())
                  { 
                    // sigma_L = m0() * sqrt(q_bb(n,n)) / weight_l
                    sigma_L(n) = MM * sqrt(q_bb(n,n)) * (*i)->stdDev();
                    n++;
                  }
            }

          ind_0 += N;
        }
  }


  { /* ----------------------------------------------------------------- */ 
    vahkopr.reset(pocmer_);
    
    for (int i=1; i<=pocmer_; i++) 
      {
        // F.Charamza: Geodet/PC p. 171
        // 1.1.56 Double  qv = (1.0 - q_bb(i, i))/w(i); 
        Double  qv = (1.0 - q_bb(i, i))/ weight_obs(i); 
        vahkopr(i) = (qv >= 0) ? qv : 0;       // removing noise 
      }
  }

}






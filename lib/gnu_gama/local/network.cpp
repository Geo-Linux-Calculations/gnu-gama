/* GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999, 2006, 2010  Ales Cepek <cepek@fsv.cvut.cz>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>
                  2012, 2013, 2014, 2015, 2018  Ales Cepek <cepek@gnu.org>

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

/** \file network.cpp
 * \brief #GNU_gama::local::LocalNetwork class implementation
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <memory>
#include <set>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/local_linearization.h>
#include <gnu_gama/local/itstream.h>
#include <gnu_gama/local/skipcomm.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/sparse/smatrix_graph.h>
#include <gnu_gama/version.h>
#include <gnu_gama/ellipsoids.h>

using namespace std;
using namespace GNU_gama::local;


typedef std::list<GNU_gama::Cluster<Observation>*> ClusterList;
typedef GNU_gama::Cluster<Observation>             Clust_r;  // 1.10

namespace
{
  const char* UNKNOWN_ALGORITHM="### network.cpp : unknown algorithm ###";
}

/** \brief %Visitor for absolute term testing.
 *
 * Visit methods determine absolute term.
 * Computed value is accessible through method value().
 *
 * \note Before first (and probably each) visit
 * setIndex() and setFromTo() should be called.
 *
 * Example:
 * \code
 * // ...
 * TestAbsTermVisitor testVisitor(b, tolerance);
 * // ... get observation
 * // set index and observation from and to
 * testVisitor.setIndex(indm);
 * testVisitor.setFromTo(stan, cil);
 * // do visit
 * m->accept(&testVisitor);
 * // use result value
 * Double myValue = testVisitor.value();
 * \endcode
 *
 */
class TestAbsTermVisitor : public GNU_gama::local::AllObservationsVisitor
{
public:
    /**
     *
     * \note Before first (and probably each) visit
     * setIndex() and setFromTo() should be called.
     */
    TestAbsTermVisitor(const Vec& bVector, Double tolerance)
        : indm(0), stan(0), cil(0),
          b(bVector), tol_abs_(tolerance),
          val(0), d0(0)
    {
    }

    /** Result value of last visited observation. */
    Double value() { return val; }

    /** Sets observation index in vector \a b (given in constructor). */
    void setIndex(Index observationIndex)
    {
        indm = observationIndex;
    }

    /** Sets from point and to point. */
    void setFromTo(const LocalPoint& from, const LocalPoint& to)
    {
        stan = &from;
        cil = &to;

        Double dx, dy;
        if (stan->test_xy() && cil->test_xy())
            {
                dy = stan->y() - cil->y();
                dx = stan->x() - cil->x();
                d0 = sqrt(dy*dy + dx*dx);
            }
    }

    void visit(Distance* obs)
    {
        check(fabs(obs->value() - d0)*1000);
    }
    void visit(Direction* /*obs*/)
    {
        check(fabs(b(indm)*d0/(10*R2G)));
    }
    void visit(Angle* /*obs*/)
    {
        check(fabs(b(indm)*d0/(10*R2G)));
    }
    void visit(H_Diff* obs)
    {
        const Double h0 = cil->z() - stan->z();
        check(fabs(obs->value() - h0)*1000);
    }

    void visit(S_Distance* obs)
    {
        Double dz = stan->z() - cil->z();
        Double d3 = sqrt(dz*dz + d0*d0);
        check(fabs(d3 - obs->value())*1000);
    }

    void visit(Z_Angle* /*obs*/)
    {
        Double dz = stan->z() - cil->z();
        Double d3 = sqrt(dz*dz + d0*d0);
        check(fabs(b(indm)*d3/(10*R2G)));
    }

    void visit(X* obs)
    {
        check(fabs(stan->x() - obs->value())*1000);
    }

    void visit(Y* obs)
    {
        check(fabs(stan->y() - obs->value())*1000);
    }
    void visit(Z* obs)
    {
        check(fabs(stan->z() - obs->value())*1000);
    }

    void visit(Xdiff* obs)
    {
        const Double dx = cil->x() - stan->x();
        check(fabs(dx - obs->value())*1000);
    }

    void visit(Ydiff* obs)
    {
        const Double dy = cil->y() - stan->y();
        check(fabs(dy - obs->value())*1000);
    }

    void visit(Zdiff* obs)
    {
        const Double dz = cil->z() - stan->z();
        check(fabs(dz - obs->value())*1000);
    }

    void visit(Azimuth* /*obs*/)
    {
        check(fabs(b(indm)*d0/(10*R2G)));
    }

private:
    Index indm;
    const LocalPoint* stan;
    const LocalPoint* cil;
    const Vec& b;
    Double tol_abs_;
    Double val;
    Double d0;

    void check(Double value)
    {
        if (value > tol_abs_)
            val = b(indm);
        else
            val = 0;
    }
};



//##########################################################################



LocalNetwork::LocalNetwork()
  : pocbod_(0), tst_redbod_(false), pocmer_(0), tst_redmer_(false),
    m_0_apr_(10), konf_pr_(0.95), tol_abs_(1000),
    update_constrained_coordinates_(false), typ_m_0_(empiricka_),
    tst_rov_opr_(false), tst_vyrovnani_(false), min_n_(0), min_x_(0),
    gons_(true)
{
  least_squares = nullptr;
  Asp = nullptr;

  set_adj_covband();
  set_max_linearization_iterations();
  // epoch_         = 0.0;
  // has_epoch_     = false;
  // latitude_      = M_PI/4.0;
  // has_latitude_  = false;
  // ellipsoid_     = "";
  // has_ellipsoid_ = false;
  clear_nullable_data();

  removed_inconsistency_ = false;
}


LocalNetwork::~LocalNetwork()
{
  delete[] min_x_;
  delete   Asp;
}


Double LocalNetwork::cond()
{
  return least_squares->cond();
}


bool LocalNetwork::lindep(Index i)
{
  return least_squares->lindep(i);
}


std::string LocalNetwork::algorithm() const
{
  return algorithm_;
}


bool LocalNetwork::has_algorithm() const
{
  return has_algorithm_;
}


void LocalNetwork::set_algorithm(std::string alg)
{
  typedef GNU_gama::local::MatVecException     MVE;
  typedef GNU_gama::AdjEnvelope<Double, Index, MVE> OLS_env;
  typedef GNU_gama::AdjGSO     <Double,        MVE> OLS_gso;
  typedef GNU_gama::AdjSVD     <Double,        MVE> OLS_svd;
  typedef GNU_gama::AdjCholDec <Double,        MVE> OLS_chol;

  AdjBase* adjb;
  if      (alg == "gso" )     adjb = new OLS_gso;
  else if (alg == "svd" )     adjb = new OLS_svd;
  else if (alg == "cholesky") adjb = new OLS_chol;
  else if (alg == "envelope") adjb = new OLS_env;
  else
    {
      alg  = "envelope";
      adjb = new OLS_env;
    }
  algorithm_ = alg;
  has_algorithm_ = true;

  if (least_squares != nullptr) delete least_squares;
  least_squares = adjb;

  update(Points);
}


int LocalNetwork::adj_covband() const
{
  return adj_covband_;
}


void LocalNetwork::set_adj_covband(int val)
{
  if (val < -1) val = -1;
  adj_covband_ = val;
}


int LocalNetwork::max_linearization_iterations() const
{
  return max_linearization_iterations_;
}


void LocalNetwork::set_max_linearization_iterations(int value)
{
  if (value < 0) value = 0;
  max_linearization_iterations_ = value;
}


double LocalNetwork::epoch() const
{
  return epoch_;
}


bool LocalNetwork::has_epoch() const
{
  return has_epoch_;
}


void LocalNetwork::set_epoch(double val)
{
  has_epoch_ = true;
  epoch_ = val;
}


double LocalNetwork::latitude() const
{
  return latitude_;
}


bool LocalNetwork::has_latitude() const
{
  return has_latitude_;
}


void LocalNetwork::set_latitude(double val)
{
  has_latitude_ = true;
  latitude_ = val;
}


std::string LocalNetwork::ellipsoid() const
{
  return ellipsoid_;
}


bool LocalNetwork::has_ellipsoid() const
{
return has_ellipsoid_;
}

void LocalNetwork::set_ellipsoid(std::string e)
{
  if (GNU_gama::ellipsoid(e.c_str())) ellipsoid_ = e;
  else                                set_ellipsoid();
  has_ellipsoid_ = true;
}


bool LocalNetwork::correction_to_ellipsoid() const
{
  return has_latitude_ || has_ellipsoid_;
}


void LocalNetwork::clear_nullable_data()
{
  has_algorithm_ = has_epoch_ = has_latitude_ = has_ellipsoid_ = false;
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
              b.set_unused_xy();
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
              b.set_unused_z();
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
    LocalRevision local_rev(PD);
    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
      {
        Observation* m = *i;
        m->accept(&local_rev);
      }
  }

  // test cycle for StandPoint clusters with single direction
  ClusterList& clusters = OD.clusters;
  for (ClusterList::iterator cit=clusters.begin(); cit!=clusters.end(); ++cit)
    {
      if (StandPoint* sp = dynamic_cast<StandPoint*>(*cit))
        {
          // 1.3.08 *** check for directions pointing to the same targer
          std::set<PointID> targets;

          int active_directions = 0;
          for (ObservationList::iterator i=sp->observation_list.begin();
               i != sp->observation_list.end(); ++i)
            {
              if (const Direction* d = dynamic_cast<const Direction*>(*i))
                if (d->active())
                  {
                    std::set<PointID>::const_iterator s = targets.find( d->to() );
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
  // removed_obs.clear();  1.17b
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
        Observation* obs = *m;
        obs->accept(&loclin);
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
       * to skip stations with single direction (gnu_gama/local-1.1.10)
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

  if (singular_coords(A))
    {
      update(Points);
      project_equations();
      return;
    }

  if (AdjBaseFull* full = dynamic_cast<AdjBaseFull*>(least_squares))
    {
      full->reset(A, b);
    }
  else if (AdjBaseSparse* sparse = dynamic_cast<AdjBaseSparse*>(least_squares))
    {
      /*********************************/
      /*  reset AdjInputData structure */
      /*********************************/

      // design matrix

      input.set_mat(Asp);
      Asp = nullptr;

      // ---  cofactors  --------------------------------------------------

      Index count = 0;   // number of diagonal blocks
      Index msize = 0;   // memory size

      for (ClusterList::const_iterator
             cluster=OD.clusters.begin(); cluster!=OD.clusters.end(); ++cluster)
        if (const Index N = (*cluster)->activeObs())
        {
          Index W = (*cluster)->covariance_matrix.bandWidth();
          count++;
          msize += 1+(N+N-W)*(W+1)/2;   // += number of band matrix elements
        }

      GNU_gama::BlockDiagonal<> *bd = new GNU_gama::BlockDiagonal<>(count, msize);

      for (ClusterList::const_iterator
             cluster=OD.clusters.begin(); cluster!=OD.clusters.end(); ++cluster)
        if (const Index N = (*cluster)->activeObs())
        {
          CovMat C = (*cluster)->activeCov();
          C /= (m_0_apr_*m_0_apr_);        // covariances ==> cofactors
          bd->add_block(N, C.bandWidth(), C.begin());
        }

      input.set_cov(bd);
      bd = 0;

      // ---  right-hand side  --------------------------------------------

      GNU_gama::Vec<> bb(b.dim());
      for (Index i=1; i<=b.dim(); i++) bb(i) = rhs_(i);
      input.set_rhs(bb);     // right-hand side before homogenization

      sparse->reset(&input);
    }
  else
    {
      throw GNU_gama::local::Exception(UNKNOWN_ALGORITHM);
    }

  //--ofstream opr("A_scaled.bin", ios_base::trunc);
  //--int m=A.rows();
  //--int n=A.cols();
  //--opr.write((char*)(&m), sizeof(int));
  //--opr.write((char*)(&n), sizeof(int));
  //--opr.write((char*)(A.begin()), sizeof(Double)*m*n);

  delete[] min_x_;
  min_x_ = nullptr;
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
    }
  least_squares->min_x(min_n_, min_x_);

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
      if (p.fixed_xy() || !p.active_xy()) continue;

      if(p.index_x() == 0 || p.index_y() == 0)
      {
          result = true;
          p.set_unused_xy();
          removed( (*i).first, rm_singular_xy );
          continue;
      }

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

      if (bb > aa) std::swap(aa, bb);
      if (aa == 0)
        D = 0;
      else
        D = 1 - std::abs(ab)/std::sqrt(aa*bb);     // 1 - |cos(a,b)|

      if (D < 1e-12)
        {
          result = true;
          p.set_unused_xy();
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
    throw GNU_gama::local::Exception(T_LN_undefined_confidence_level);
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
      throw GNU_gama::local::Exception(T_LN_undefined_type_of_actual_sigma);
    }
}


Double LocalNetwork::m_0_aposteriori_value()
{
  vyrovnani_();
  const int nadb = degrees_of_freedom();
  if (nadb > 0)
    return sqrt(trans_VWV()/nadb);
  else
    return 0;
}


Double LocalNetwork::conf_int_coef()
{
  using namespace GNU_gama::local;

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
      throw GNU_gama::local::Exception(T_LN_undefined_type_of_actual_sigma);
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
  Observation* m = RSM[indm-1];

  const LocalPoint& stan = PD[m->from()];
  const LocalPoint& cil  = PD[m->to()];   // ignoring second angle target here

  TestAbsTermVisitor testVisitor(b, tol_abs_);
  testVisitor.setIndex(indm);
  testVisitor.setFromTo(stan, cil);

  m->accept(&testVisitor);

  return testVisitor.value();
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

      for (int i=1; i<=sum_unknowns(); i++)
        if (lindep(i))
          {
            const char    type = unknown_type(i);
            const PointID id   = unknown_pointid(i);

            LocalPoint& p = PD[id];
            if (type == 'X' || type == 'Y' || type == 'R' )
              {
                p.set_unused_xy();
                removed(id, rm_singular_xy );
              }
            else if (type == 'Z' )
              {
                p.set_unused_z();
                removed(id, rm_missing_z );
              }

            return null_space();
          }
    }

  return least_squares->defect();
}


void LocalNetwork::std_error_ellipse(const PointID& cb,
                                     Double& a, Double& b, Double& alfa)
{
  using namespace std;

  const LocalPoint& bod = PD[cb];
  int iy = bod.index_y();
  int ix = bod.index_x();
  Double cyy = least_squares->q_xx(iy,iy);
  Double cyx = least_squares->q_xx(iy,ix);
  Double cxx = least_squares->q_xx(ix,ix);
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
  const Vec& x = least_squares->unknowns();

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
        Double ori =
          ( standpoint->orientation() )*R2G + x(i)/10000;
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
  using namespace GNU_gama::local;
  if (tst_vyrovnani_) return;

  do {

    project_equations();
    if (sum_unknowns()     == 0)
      throw GNU_gama::local::Exception(T_GaMa_No_unknowns_defined);
    if (sum_observations() == 0)
      throw GNU_gama::local::Exception(T_GaMa_No_observations_available);
    if (sum_points()      == 0)
      throw GNU_gama::local::Exception(T_GaMa_No_points_available);

    tst_vyrovnani_ = true;

    /* ----------------------------------------------------------------- */
    // check for huge covariances / indefinite coordinates

    for (PointData::iterator i=PD.begin(); i!=PD.end(); ++i)
      {
        LocalPoint&     P  = (*i).second;
        const PointID&  id = (*i).first;

        if (!P.free_xy() && !P.free_z()) continue;

        Double tx=0, ty=0, tz=0;
        if (int ix = P.index_x()) tx =m_0_apr_*sqrt(least_squares->q_xx(ix,ix));
        if (int iy = P.index_y()) ty =m_0_apr_*sqrt(least_squares->q_xx(iy,iy));
        if (int iz = P.index_z()) tz=m_0_apr_*sqrt(least_squares->q_xx(iz,iz));

        bool bxy = (tx > 1e4) || (ty > 1e4);
        bool bz  = (tz > 1e4);

        if (bxy && bz)
          {
            P.set_unused_xy();
            P.set_unused_z();
            removed(id, rm_huge_cov_xyz);
            update(Points);
          }
        else if (bxy)
          {
            P.set_unused_xy();
            removed(id, rm_huge_cov_xy);
            update(Points);
          }
        else if (bz)
          {
            P.set_unused_z();
            removed(id, rm_huge_cov_z);
            update(Points);
          }
      }

  } while (!tst_vyrovnani_);

  if (AdjBaseFull* full = dynamic_cast<AdjBaseFull*>(least_squares))
  { /* ----------------------------------------------------------------- */
    // r = least_squares->residuals();
    r = full->residuals();
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
  else if (AdjBaseSparse* sparse = dynamic_cast<AdjBaseSparse*>(least_squares))
    {
      r = sparse->residuals();
      suma_pvv_ = sparse->sum_of_squares();
    }
  else
    {
      throw GNU_gama::local::Exception(UNKNOWN_ALGORITHM);
    }


  { /* ----------------------------------------------------------------- */
    sigma_L.reset(pocmer_);

    Double MM = m_0() / m_0_apr_;
    Index ind_0 = 0;

    for (ClusterList::const_iterator
           cit=OD.clusters.begin(); cit!=OD.clusters.end(); ++cit)
      if (const Index N = (*cit)->activeObs())
        {
          const Clust_r& cluster = *(*cit);
          // ??? if (cluster.covariance_matrix.bandWidth())
          // ???   {
          // ???     // calculation for correlated observations missing !!!
          // ???   }
          // ??? else
            {
              Index n = ind_0+1;
              for (ObservationList::const_iterator
                     i = cluster.observation_list.begin();
                     i!= cluster.observation_list.end(); ++i)
                if ((*i)->active())
                  {
                    // sigma_L = m0() * sqrt(least_squares->q_bb(n,n)) / weight_l
                    sigma_L(n) = MM * sqrt(least_squares->q_bb(n,n)) * (*i)->stdDev();
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
        Double  qv = (1.0 - least_squares->q_bb(i, i))/ weight_obs(i);
        vahkopr(i) = (qv >= 0) ? qv : 0;       // removing noise
      }
  }

}


std::string LocalNetwork::updated_xml()
{
  if (!is_adjusted()) std::string();

  std::string xml =
    "<?xml version=\"1.0\" ?>\n"
    "<gama-local xmlns=\"http://www.gnu.org/software/gama/gama-local\">\n"
    "<network axes-xy=";

  switch(PD.local_coordinate_system)
    {
    case LocalCoordinateSystem::EN: xml += "\"en\""; break;
    case LocalCoordinateSystem::NW: xml += "\"nw\""; break;
    case LocalCoordinateSystem::SE: xml += "\"se\""; break;
    case LocalCoordinateSystem::WS: xml += "\"ws\""; break;
    case LocalCoordinateSystem::NE: xml += "\"ne\""; break;
    case LocalCoordinateSystem::SW: xml += "\"sw\""; break;
    case LocalCoordinateSystem::ES: xml += "\"es\""; break;
    case LocalCoordinateSystem::WN: xml += "\"wn\""; break;
    default:
      xml +=  "\"ne\""; break;
    }

  xml += " angles=";
  xml += PD.left_handed_angles() ? "\"left-handed\"" : "\"right-handed\"";
  if (has_epoch()) xml += " epoch=\"" + std::to_string(epoch()) + "\"";
  xml += ">\n";


  if (!description.empty())
    xml += "\n<description>" + description + "</description>\n";

  xml += "\n<parameters\n";
  xml += "  sigma-apr=\"" + std::to_string(apriori_m_0()) + "\"\n";
  xml += "  conf-pr=\""   + std::to_string(conf_pr())     + "\"\n";
  xml += "  tol-abs=\""   + std::to_string(tol_abs())     + "\"\n";
  xml += "  sigma-act=\"";
  xml +=         m_0_apriori() ? "apriori\"\n" : "aposteriori\"\n";
  xml += "  update-constrained-coordinates=\"";
  xml +=         update_constrained_coordinates() ? "yes\"\n" : "no\"\n";
  xml += "  angles=\"" + std::string(gons() ? "400" : "360") + "\"\n";
  if (has_algorithm()) xml += "  algorithm=\"" + algorithm() + "\"\n";
  if (has_latitude())
    xml += "  latitude=\"" + std::to_string(latitude()) + "\"\n";
  if (has_ellipsoid()) xml += "  ellipsoid=\"" + ellipsoid() + "\"\n";
  xml += "  cov-band=\"" + std::to_string(adj_covband()) + "\"\n";
  // iterations ... not implemented in XML input
  // language .....
  // encoding .....
  xml += "/>\n";


  xml += "\n<points-observations";
  // implicit parameters
  xml += ">\n\n";


  for (auto p=PD.begin(); p!=PD.end(); ++p)
    {
      PointID    id    = p->first;
      LocalPoint point = p->second;
      if (!point.active()) continue;

      xml += "<point id=\"" + id.str() + "\"";

      if (point.test_xy()) {
        xml += " x=\"" + std::to_string(point.x()) + "\"";
        xml += " y=\"" + std::to_string(point.y()) + "\"";
      }

      if (point.test_z()) {
          xml += " z=\"" + std::to_string(point.z()) + "\"";
      }

      std::string fix {}, adj {};
      if      (point.fixed_xy())       fix += "xy";
      else if (point.constrained_xy()) adj += "XY";
      else if (point.free_xy())        adj += "xy";

      if      (point.fixed_z())        fix += "z";
      else if (point.constrained_z())  adj += "Z";
      else if (point.free_z())         adj += "z";

      if (!fix.empty()) xml += " fix=\"" + fix + "\"";
      if (!adj.empty()) xml += " adj=\"" + adj + "\"";

      xml += " />\n";
    }


  DisplayObservationVisitor info(this);

  for (auto c=OD.clusters.begin(); c!=OD.clusters.end(); ++c) {
    if (auto cluster = dynamic_cast<StandPoint*>(*c))
      {
        xml += "\n<obs from=\"" + cluster->station.str() + "\">\n";

        for (auto p  = cluster->observation_list.begin();
                  p != cluster->observation_list.end(); ++p) {
          Observation* obs = *p;
          obs->accept(&info);

          xml += "<" +  info.xml_name;
          if (!info.str_to.empty()) {
            xml += " to=\"" + info.str_to + "\"";
            double fdh = obs->from_dh();
            double tdh = obs->to_dh();
            if (fdh) xml += " from_dh=\"" + std::to_string(fdh) + "\"";
            if (tdh) xml +=   " to_dh=\"" + std::to_string(tdh) + "\"";
          }
          else if (!info.str_bs.empty()) {
            xml += " bs=\"" + info.str_bs + "\"";
            xml += " fs=\"" + info.str_fs + "\"";

            Angle* a = static_cast<Angle*>(obs);
            double rdh = a->from_dh();
            double bdh = a->bs_dh();
            double fdh = a->fs_dh();
            if (rdh) xml += " from_dh=\"" + std::to_string(rdh) + "\"";
            if (bdh) xml +=   " bs_dh=\"" + std::to_string(bdh) + "\"";
            if (fdh) xml +=   " fs_dh=\"" + std::to_string(fdh) + "\"";
          }
          xml += " val=\"" + info.str_val + "\"";
          xml += " stdev=\"" + info.str_stdev + "\"";
          xml += " />\n";
        }

        updated_xml_covmat(xml, cluster->covariance_matrix, false);
         xml += "</obs>\n";
      }
    else if (auto cluster = dynamic_cast<HeightDifferences*>(*c))
      {
        xml += "\n<height-differences>\n";
        for (auto p  = cluster->observation_list.begin();
                  p != cluster->observation_list.end(); ++p) {
          Observation* obs = *p;
          obs->accept(&info);

         xml += "<" +  info.xml_name;
         xml += " from=\"" + info.str_from + "\"";
         xml += " to=\"" + info.str_to + "\"";
         xml += " val=\"" + info.str_val + "\"";

         double dist = 0;
         if (H_Diff* p = dynamic_cast<H_Diff*>(obs)) dist = p->dist();
         if (dist > 0)
           xml += " dist=\"" +std::to_string(dist) + "\"";
         else
           xml += " stdev=\"" + info.str_stdev + "\"";
         xml += "/>\n";
        }

        updated_xml_covmat(xml, cluster->covariance_matrix, false);
        xml += "</height-differences>\n";
      }
    else if (auto cluster = dynamic_cast<Coordinates*>(*c))
      {
        xml += "\n<coordinates>\n";
        for (auto p  = cluster->observation_list.begin();
                  p != cluster->observation_list.end(); ++p) {
          Observation* obs = *p;
          obs->accept(&info);
          std::string from = info.str_from;
          xml += "<point id=\"" + from + "\"";
          if (info.xml_name == "x")
            {
              xml += " x=\"" + info.str_val + "\"";
              ++p;
              (*p)->accept(&info);
              xml += " y=\"" + info.str_val + "\"";
              auto q = p;
              ++q;
              if (q != cluster->observation_list.end())
                {
                  DisplayObservationVisitor tmp(this);
                  (*q)->accept(&tmp);
                  if (tmp.str_from == from && tmp.xml_name == "z")
                    {
                      ++p;
                      (*p)->accept(&info);
                    }
                }
            }
          if (info.xml_name == "z")
            {
              xml += " z=\"" + info.str_val + "\"";
            }
          xml += " />\n";
        }

        updated_xml_covmat(xml, cluster->covariance_matrix, true);
        xml += "</coordinates>\n";
      }
    else if (auto cluster = dynamic_cast<Vectors*>(*c))
      {
        xml += "\n<vectors>\n";

        for (auto p  = cluster->observation_list.begin();
                  p != cluster->observation_list.end(); ++p) {
          Observation* obs = *p;
          obs->accept(&info);

          xml += "<vec from=\"" +info.str_from+ "\" to=\"" +info.str_to+ "\"";
          obs->accept(&info);
          xml += " dx=\"" + info.str_val + "\"";
          (*++p)->accept(&info);
          xml += " dy=\"" + info.str_val + "\"";
          (*++p)->accept(&info);
          xml += " dz=\"" + info.str_val + "\"";

          // double fdh = (*p)->from_dh(); ... unrealistic
          // double tdh = (*p)->to_dh();
          // if (fdh) xml += " from_dh=\"" + std::to_string(fdh) + "\"";
          // if (tdh) xml +=   " to_dh=\"" + std::to_string(tdh) + "\"";
          xml += " />\n";
        }

        updated_xml_covmat(xml, cluster->covariance_matrix, true);
        xml += "\n</vectors>\n";
      }
    else
      {
        xml += "\n### Undefined cluster\n";
      }
  }


  xml +=
    "\n</points-observations>\n"
    "\n</network>\n"
    "</gama-local>\n";

  return xml;
}

void LocalNetwork::updated_xml_covmat(std::string& xml, const CovMat& C,
                                      bool always)
{
  Index  dim  = C.dim();
  Index  band = C.bandWidth();
  if (!always && band == 0) return;

  xml += "\n<cov-mat dim=\"" + std::to_string(dim) + "\"";
  xml += " band=\"" + std::to_string(band) + "\">\n";
  for (Index i=1; i<=dim; i++)
    {
      std::ostringstream out;
      out.setf(ios_base::scientific, ios_base::floatfield);
      out.precision(16);
      out << "\n";
      for (Index n=1, j=i; j<=i+band && j <=dim; j++, n++)
        {
          out << setw(24) << C(i,j) << ((n%3 == 0 && j != dim) ? "\n" : " ");
        }
      out << "\n";
      xml += out.str();
    }
  xml += "\n</cov-mat>\n";
}

bool LocalNetwork::consistent() const
{
  return PD.left_handed_coordinates() == PD.left_handed_angles();
}

double LocalNetwork::y_sign() const
{
  return consistent() ? +1.0 : -1.0;
}

void LocalNetwork::remove_inconsistency()
{
  if (consistent()) return;
  if (removed_inconsistency_) return;

  change_y_signs_for_inconsistent_system_();
  removed_inconsistency_ = true;
}

void LocalNetwork::return_inconsistency()
{
  if (!removed_inconsistency_) return;

  change_y_signs_for_inconsistent_system_();
  removed_inconsistency_ = false;
}

void LocalNetwork::change_y_signs_for_inconsistent_system_()
{
  for (PointData::iterator ii=PD.begin(); ii!=PD.end(); ++ii)
    {
      LocalPoint& p = (*ii).second;

      if (p.test_xy()) p.set_xy(p.x(), -p.y());
    }

  for (ObservationData::ClusterList::iterator
         ci=OD.clusters.begin(), ei=OD.clusters.end(); ci!=ei; ++ci)
    {
      ObservationData::ClusterType *cluster = *ci;
      for (ObservationList::iterator
             m = cluster->observation_list.begin(),
             e = cluster->observation_list.end()  ; m!=e; ++m)
        {
          Observation *obs = *m;
          bool b = false;

          if      (dynamic_cast<Y*>    (obs))  b = true;
          else if (dynamic_cast<Ydiff*>(obs))  b = true;

          if (b)  obs->set_value( -obs->value() );
        }
    }
}

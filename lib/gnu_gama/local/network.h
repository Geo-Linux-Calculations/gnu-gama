/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 1999, 2006, 2012, 2014, 2015, 2017  Ales Cepek <cepek@gnu.org>

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


// LocalNetwork - Network Informations class
// =========================================


#ifndef gama_local_LocalNetwork_h
#define gama_local_LocalNetwork_h

#include <fstream>
#include <iomanip>
#include <list>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/adj/adj_basefull.h>
#include <gnu_gama/adj/adj_basesparse.h>
#include <gnu_gama/local/cluster.h>
#include <gnu_gama/local/local_revision.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/adj/adj.h>

namespace GNU_gama { namespace local
{

  class LocalNetwork
  {
    typedef std::vector<GNU_gama::local::Observation*>       RevisedObsList;
    typedef GNU_gama::AdjBase<Double, Index, Vec>            AdjBase;
    typedef GNU_gama::AdjBaseFull<Double, MatVecException>   AdjBaseFull;
    typedef GNU_gama::AdjBaseSparse<Double, Index, Vec,
                                    GNU_gama::AdjInputData>  AdjBaseSparse;

    AdjBase                *least_squares;
    GNU_gama::AdjInputData  input;

  public:

    LocalNetwork();
    virtual ~LocalNetwork();

    PointData        PD;      // point list
    ObservationData  OD;      // observation list

    std::string   description;


    // ...  information on points removed from adjustment  .................

    enum rm_points {
      rm_missing_xyz, rm_missing_xy,   rm_missing_z,
      rm_singular_xy, rm_singular_z,
      rm_huge_cov_xyz, rm_huge_cov_xy, rm_huge_cov_z
    };

    GNU_gama::local::PointIDList  removed_points;
    std::list<rm_points>  removed_code;

    void removed(const PointID& id, rm_points rm)
    {
      removed_points.push_back(id);
      removed_code  .push_back(rm);
      update(Points);
    }


    // ...  network configuration updates  .................................

    void update_points()       { update(Points);       }
    void update_observations() { update(Observations); }
    void update_residuals()    { update(Residuals);    }
    void update_adjustment()   { update(Adjustment);   }


    // ...  revision of points and observations  ...........................

    void  revision_points();
    const PointIDList& undefined_coordinates() const
    {
      return undefined_xy_z_;
    }
    int sum_points()
    {
      revision_points(); return pocbod_;
    }
    void  revision_observations();
    const ObservationList& sum_rejected_observations() const
    {
      return removed_obs;
    }
    int sum_observations() const
    {
      return pocmer_;
    }
    void project_equations();
    void project_equations(std::ostream&);
    void project_equations(Mat& A, Vec& b, Vec& w);
    Double conf_int_coef();
    int min_n() const
    {
      return min_n_;
    }


    // ...  unknowns  ......................................................

    PointID     unknown_pointid   (int i) const { return seznez_[i-1].cb;  }
    char        unknown_type      (int i) const { return seznez_[i-1].typ; }
    StandPoint* unknown_standpoint(int i) const { return seznez_[i-1].osn; }
    Double      unknown_stdev     (int i)
    {
      using namespace std;
      return m_0()*sqrt(least_squares->q_xx(i, i));
    }


    // ...  observations  ..................................................

    Observation* ptr_obs(int i)
    {
      return RSM[i-1];
    }
    Double weight_obs(int i)
    {
      Double p = m_0_apr_/RSM[i-1]->stdDev();
      return p*p;
    }
    Double test_abs_term(Index i);                   // 0 or abs_term(i)
    bool huge_abs_terms()
    {
      project_equations(); return vybocujici_abscl_;
    }
    void remove_huge_abs_terms();
    Double rhs(Index i) const
    {
      return rhs_(i);
    }


    // ...  adjustment  ....................................................

    bool is_adjusted() const
    {
      return tst_vyrovnani_;
    }
    const Vec& solve()
    {
      vyrovnani_();
      return least_squares->unknowns();
    }
    const Vec& residuals()
    {
      vyrovnani_();
      return r;
    }
    int sum_unknowns()
    {
      project_equations();
      return A.cols();
    }
    int sum_observations()
    {
      project_equations();
      return A.rows();
    }
    int degrees_of_freedom()
    {
      vyrovnani_();
      return A.rows() - A.cols() + least_squares->defect();
    }
    int null_space();

    Double trans_VWV()           { vyrovnani_(); return suma_pvv_; }
    Double m_0();
    Double apriori_m_0() const   { return m_0_apr_; }

    Double qxx(Index i, Index j) { return least_squares->q_xx(i,j); }
    Double qbb(Index i, Index j) { return least_squares->q_bb(i,j); }
    Double qbx(Index i, Index j) { return least_squares->q_bx(i,j); }

    Double cond();
    bool lindep(Index i);

    void refine_approx();

    void   apriori_m_0(Double m)  { m_0_apr_ = m; }
    void   tol_abs(Double m)      { tol_abs_ = m; }
    Double tol_abs() const        { return tol_abs_; }

    void   update_constrained_coordinates(bool par)
    {
      update_constrained_coordinates_ = par;
    }
    bool   update_constrained_coordinates() const
    {
      return update_constrained_coordinates_;
    }

    Double stdev_obs(int i) { return sigma_L(i); }
    Double wcoef_res(int i) { return vahkopr(i); }
    Double stdev_res(int i)
    {
      using namespace std;
      return m_0()*sqrt(fabs(wcoef_res(i)));
    }

    Double studentized_residual(int i)
    {
      return residuals()(i)/stdev_res(i);
    }
    Double obs_control(int i)
    {
      /*
       * It is supposed that standard deviation mL of adjusted
       * observation is derived from apriori reference standard
       * deviation m0 (ml is standard deviation of the
       * observation). F. Charamza: Geodet/PC, Zdiby 1990
       * p. 180.
       *
       *      f = 100*(ml - mL)/ml
       *
       *      f < 0.1%     uncontrolled observation
       *      f < 5%       weakly controlled observation
       */
      // 1.1.20 return 100*fabs((1-sqrt(q_bb(i,i)*w(i))));
      using namespace std;
      return 100*fabs(1-sqrt(least_squares->q_bb(i,i)));
    }
    void std_error_ellipse(const PointID&, Double& a,
                           Double& b, Double& alfa);


    // ...  parameters of statistic analysis  ...............................

    bool m_0_apriori    () const { return typ_m_0_ == apriorni_;  }
    bool m_0_aposteriori() const { return typ_m_0_ == empiricka_; }
    Double m_0_aposteriori_value();

    void set_m_0_apriori    ()   { typ_m_0_ = apriorni_; }
    void set_m_0_aposteriori()   { typ_m_0_ = empiricka_; }

    Double conf_pr() const       { return konf_pr_; }
    void   conf_pr(Double p);


    // ...  formated output  ...............................................

    int maxw_id () const { return 12; } // max width of point id.
    int maxw_unk() const { return  3; } // max width of index of unknown
    int maxw_obs() const { return  4; } // max width of index of observation

    bool gons()    const { return  gons_; }
    bool degrees() const { return !gons_; }
    void set_gons()      { gons_ = true;  Observation::gons = true;  }
    void set_degrees()   { gons_ = false; Observation::gons = false; }

    // ...  connected network  .............................................

    bool connected_network() const { return design_matrix_graph_is_connected; }

    std::string algorithm() const;
    bool        has_algorithm() const;
    void        set_algorithm(std::string alg = std::string());
    int         adj_covband() const;
    void        set_adj_covband(int value=-1);
    double      epoch() const;
    bool        has_epoch() const;
    void        set_epoch(double=0.0);
    double      latitude() const;
    bool        has_latitude() const;
    void        set_latitude(double);
    std::string ellipsoid() const;
    bool        has_ellipsoid() const;
    void        set_ellipsoid(std::string="wgs84");
    bool        correction_to_ellipsoid() const;
    void        clear_nullable_data();

    // ... linearization iterations ........................................

    void set_max_linearization_iterations(int value=5);
    int  max_linearization_iterations() const;
    int  clear_linearization_iterations() { return (iterations_ = 0); }
    int  linearization_iterations() const { return iterations_; }
    int  increment_linearization_iterations() { return ++iterations_; }
    bool next_linearization_iterations() const { return iterations_ < max_linearization_iterations_;}

    // ...  input data with free coordinates updated after adjustment
    //      and rejected observations removed ..............................

    std::string updated_xml();

    // #####################################################################

    bool   consistent() const;
    double y_sign() const;
    void   remove_inconsistency();
    void   return_inconsistency();

  private:

    void updated_xml_covmat(std::string& xml, const CovMat& C, bool always);

    int         adj_covband_;         // output XML xyz cov bandWidth
    int         max_linearization_iterations_;
    int         iterations_ {};
    std::string algorithm_;           // algorithm name or empty string
    bool        has_algorithm_;
    double      epoch_;
    bool        has_epoch_;
    double      latitude_;
    bool        has_latitude_;
    std::string ellipsoid_;
    bool        has_ellipsoid_;

    RevisedObsList          RSM;

    PointIDList undefined_xy_z_;      // revision of points
    int pocbod_;
    bool tst_redbod_;

    ObservationList removed_obs;      // revision of observations
    int pocmer_;
    bool tst_redmer_;

    // parameters of statistical analysis

    Double m_0_apr_;         // a priori reference standard deviation
    Double konf_pr_;         // (confidence) probability
    Double tol_abs_;         // tollerance for testing absolute terms
    bool update_constrained_coordinates_;
    enum ApEm_ { apriorni_, empiricka_ };
    ApEm_ typ_m_0_;          // type of reference standard deviation

    bool tst_rov_opr_;       // project equations
    bool vybocujici_abscl_;  // outlying abs. terms in project equations

    int pocet_neznamych_;

    enum Update { Points, Observations, Residuals, Adjustment };
    void update(Update);

    struct neznama_ {
      PointID cb;
      char typ;
      StandPoint* osn;       // zero or pointer for orientaion (typ='R')
    };

    std::vector<neznama_> seznez_;  // list of unknowns

    Mat A;
    Vec b;
    Vec rhs_;             // right-hand side
    Vec r;
    Vec sigma_L;          // standard deviation of adjusted observation
    Vec vahkopr;          // weight coefficient of residuals
    Double suma_pvv_;
    GNU_gama::SparseMatrix<Double, Index>*  Asp;

    bool design_matrix_graph_is_connected;

    // solution of Least Squares

    bool tst_vyrovnani_;
    void vyrovnani_();

    Index  min_n_;           // regularization of free network
    Index* min_x_;

    bool   gons_;

    // preparation for design matrix

    void cholesky(CovMat& chol);
    void forwardSubstitution(const CovMat& chol, Vec& v);
    // void backwardSubstitution(const Cov& chol, Vec& v);
    void prepareProjectEquations();
    bool singular_coords(const Mat&);

    // fixing inconsitent systems

    void change_y_signs_for_inconsistent_system_();
    bool removed_inconsistency_ {false};


  };     /* class LocalNetwork */

}}         /* namespace GNU_gama::local */

#endif

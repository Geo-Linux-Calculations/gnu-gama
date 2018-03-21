/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_g3_model_h_gnugamag3modelh_gnu_gama_g3model
#define GNU_gama_g3_model_h_gnugamag3modelh_gnu_gama_g3model

#include <gnu_gama/model.h>
#include <gnu_gama/pointbase.h>
#include <gnu_gama/obsdata.h>
#include <gnu_gama/ellipsoids.h>
#include <gnu_gama/g3/g3_point.h>
#include <gnu_gama/g3/g3_observation.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/sbdiagonal.h>
#include <gnu_gama/adj/adj.h>
#include <gnu_gama/e3.h>
#include <list>

namespace GNU_gama {

  /** \brief Adjustment model on ellipsoid */
  namespace g3 {

  /** g3 adjustment model. */

  class Model : public GNU_gama::Model<g3::Observation>
  {
  public:

    Model();
    virtual ~Model();

    typedef GNU_gama::ObservationData<g3::Observation>  ObservationData;
    typedef std::list<Observation*>                     ObservationList;
    typedef GNU_gama::PointBase<g3::Point>              PointBase;
    typedef std::list<Parameter*>                       ParameterList;
    typedef GNU_gama::Adj                               Adj;


    PointBase           *points;
    GNU_gama::Ellipsoid  ellipsoid;

    Point* get_point(const Point::Name&);
    void   write_xml(std::ostream& out) const;

    void reset()               { state_ = init_; }
    void reset_parameters()    { if (params_ < state_) state_ = params_; }
    void reset_observations()  { if (obsrvs_ < state_) state_ = obsrvs_; }
    void reset_linearization() { if (linear_ < state_) state_ = linear_; }
    void reset_adjustment()    { if (adjust_ < state_) state_ = adjust_; }

    bool check_parameters()    const { return state_ > params_; }
    bool check_observations()  const { return state_ > obsrvs_; }
    bool check_linearization() const { return state_ > linear_; }
    bool check_adjustment()    const { return state_ > adjust_; }

    void update_parameters();
    void update_observations();
    void update_linearization();
    void update_adjustment();

    bool revision(Angle*      );
    bool revision(Azimuth*    );
    bool revision(Distance*   );
    bool revision(Height*     );
    bool revision(HeightDiff* );
    bool revision(Vector*     );
    bool revision(XYZ*        );
    bool revision(ZenithAngle*);

    void linearization(Angle*      );
    void linearization(Azimuth*    );
    void linearization(Distance*   );
    void linearization(Height*     );
    void linearization(HeightDiff* );
    void linearization(Vector*     );
    void linearization(XYZ*        );
    void linearization(ZenithAngle*);

    void write_xml_adjusted(std::ostream&, const Angle*,      Index);
    void write_xml_adjusted(std::ostream&, const Azimuth*,    Index);
    void write_xml_adjusted(std::ostream&, const Distance*,   Index);
    void write_xml_adjusted(std::ostream&, const Vector*,     Index);
    void write_xml_adjusted(std::ostream&, const Height*,     Index);
    void write_xml_adjusted(std::ostream&, const HeightDiff*, Index);
    void write_xml_adjusted(std::ostream&, const XYZ*,        Index);
    void write_xml_adjusted(std::ostream&, const ZenithAngle*,Index);

    void set_algorithm(Adj::algorithm a) { adj->set_algorithm(a); }

    void   set_apriori_sd(double s) { apriori_sd = s;          }
    double get_apriori_sd() const   { return apriori_sd;       }
    void   set_conf_level(double c) { confidence_level = c;    }
    double get_conf_level() const   { return confidence_level; }
    void   set_tol_abs   (double c) { tol_abs = c;             }
    double get_tol_abs   () const   { return tol_abs;          }

    double standard_deviation() const { return std_deviation; }
    double standard_variance () const { return std_variance; }
    double cov_xx(Index i, Index j) { return std_variance*adj->q_xx(i,j); }
    double cov_bb(Index i, Index j) { return std_variance*adj->q_bb(i,j); }

    bool   graph_is_connected() const  { return dm_graph_is_connected; }

    void write_xml_adjustment_input_data(std::ostream&);
    void write_xml_adjustment_results   (std::ostream&);

    bool angular_units_degrees() const { return !gons_; }
    bool angular_units_gons   () const { return  gons_; }
    void set_angular_units_degrees()   { gons_ = false; }
    void set_angular_units_gons()      { gons_ = true; }

    GNU_gama::E_3 vector    (const Point* from, const Point* to) const;
    GNU_gama::E_3 normal    (const Point* p) const;
    GNU_gama::E_3 vertical  (const Point* p) const;
    GNU_gama::E_3 instrument(const Point* p, double dh) const;


    struct Rejected
    {
      enum rejection { rhs };

      rejection     criterion;
      Observation*  observation;
      double        data[3];
    };

    typedef std::list<Rejected>  RejectedObs;
    RejectedObs     rejected_obs;


  private:   /*-----------------------------------------------------------*/

    Model(const Model&);
    Model& operator=(const Model&);

    // active observations list (active observations used in the adjustment)
    ObservationList*  active_obs;

    // parameter list
    ParameterList*  par_list;
    void update_index(Parameter&);

    // basic revision steps
    enum State_{init_, params_, obsrvs_, linear_, adjust_, ready_} state_;

    void next_state_(int s) { state_ = State_(++s); }
    bool check_init() const { return state_ > init_; }
    void update_init();


    // design matrix
    int dm_rows, dm_cols, dm_floats;
    SparseMatrix <>*  A;
    Vec          <>   rhs;
    int               rhs_ind;
    BlockDiagonal<>*  B;
    GNU_gama::AdjInputData*  adj_input_data;


    // adjustment
    Adj*              adj;

    size_t redundancy;
    enum { apriori, aposteriori } actual_sd;
    double aposteriori_sd;
    double std_deviation;
    double std_variance;

    bool   dm_graph_is_connected;   // design matrix graph


    // constants
    double apriori_sd;
    double confidence_level;
    double tol_abs;

    bool   gons_;


    void write_xml_rejected_observations          (std::ostream&);
    void write_xml_adjustment_results_statistics  (std::ostream&);
    void write_xml_adjustment_results_points      (std::ostream&);
    void write_xml_adjustment_results_observations(std::ostream&);

    void write_xml_adjusted_stdev  (const char*,
                                    std::ostream&, const Observation*,
                                    Index, Index);

    void write_xml_adjusted_cov_xyz(std::ostream&, const Observation*,
                                    Index);
  };

}}

#endif

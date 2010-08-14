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

#ifndef GNU_gama_____g3______parameter______h__________GNUgamag3parameterh
#define GNU_gama_____g3______parameter______h__________GNUgamag3parameterh


#include <cstddef>
#include <gnu_gama/model.h>
#include <gnu_gama/list.h>
#include <gnu_gama/radian.h>


namespace GNU_gama { namespace g3 {


  /** g3 base XML helper class. */

  class ParXML {
  public:

    ParXML() : owner(0), finished(false) {}
    virtual ~ParXML() {}

    virtual void write_xml(std::ostream& ostr)
    {
      if (!finished && owner) owner->write_xml(ostr);
    }
    virtual void write_xml_done()  { finished = true;   }
    void         write_xml_init()  { finished = false;  }
    void set_owner(ParXML* parxml) { owner    = parxml; }

  private:
    ParXML* owner;
    bool    finished;

  };


  /** g3 base parameter class. */

  class Parameter : public ParXML {
  public:

    Parameter() : val(0), cor(0), dif(0.05), state_(unused_) {}
    virtual ~Parameter() {}

    double operator()() const { return val + cor; }

    double init_value() const { return val; }
    double correction() const { return cor; }
    double step_size () const { return dif; }
    bool   has_index () const { return ind; }
    std::size_t index() const { return free() ? ind : 0; }

    void set_init_value(double p) { val = p; cor = 0; }
    void set_correction(double p) { cor = p; }
    void set_step_size (double p) { dif = p; }
    void set_index(std::size_t t) { ind = t; }

    void add_correction(double p) { cor += p/scale(); }
    virtual double scale() const  { return 1.0; }

    void set_unused() { state_ = unused_; }
    void set_fixed () { state_ = fixed_;  }
    void set_free  () { state_ = free_;   }
    void set_constr() { state_ = constr_; }

    bool active() const { return state_ != unused_; }
    bool unused() const { return state_ == unused_; }
    bool fixed () const { return state_ == fixed_;  }
    bool free  () const { return state_ &  free_;   }
    bool constr() const { return state_ == constr_; }

    void set_state(const Parameter& p) { state_ = p.state_; }
    bool cmp_state(const Parameter& p) const { return state_ == p.state_; }

  private:

    double val;
    double cor;
    double dif;
    std::size_t ind;

    enum
      {
        unused_ = 0,
        fixed_  = 1,
        free_   = 2,
        constr_ = 4 + free_
      } state_;

  };


  /** g3 angular observations base class */

  class Angular : public Parameter {
  public:
    double scale() const { return RAD_TO_CC; }
  };


  /** g3 linear observations base class */

  class Linear  : public Parameter {
  public:
    double scale() const { return 1e3; }
  };

}}

#endif

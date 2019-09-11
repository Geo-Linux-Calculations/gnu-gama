/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 1999, 2007, 2011, 2012, 2014, 2017, 2018, 2019
                  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Matrix/Vector template library.

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

#ifndef GNU_gama_gMatVec_IndexErr_h
#define GNU_gama_gMatVec_IndexErr_h

#include <cstddef>
#include <exception>

namespace GNU_gama {

  inline const char* matvec_version() { return "3.02"; }

  /** Exception \brief Matrix/vector exceptions
   */

  namespace Exception {

    enum
      {
        BadRank,
        BadIndex,
        Singular,
        BadRegularization,
        NoConvergence,
        ZeroDivision,
        NonPositiveDefinite,
        NotImplemented,
        StreamError
      };

    class base : public std::exception
    {
    public:
      // virtual ~base() {} ... C++11 std:exception has virtual destructor
      virtual base* clone() const = 0;
      virtual void  raise() const = 0;
    };

    class matvec : public base
    {
    public:
      matvec(int e, const char* t) : error_(e), description_(t) {}

      matvec* clone() const override { return new matvec(*this); }
      void    raise() const override { throw *this; }

      const char* what()  const noexcept override { return description_; }
      int         error() const { return error_; }

    private:
      const int    error_;
      const char*  description_;
    };
  }
}

#endif

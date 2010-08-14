/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseSparse_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseSparse_h

#include <gnu_gama/adj/adj_base.h>

namespace GNU_gama {

  /** \brief Base adjustment class for sparse matrix solutions. */

  template <typename Float, typename Index, typename Vector,
            typename AdjInputData>
  class AdjBaseSparse : public AdjBase<Float, Index, Vector>
  {
  public:

    AdjBaseSparse() : input(0), stage(0)
    {
    }

    AdjBaseSparse(const AdjInputData *data) : input(data), stage(0)
    {
    }

    virtual ~AdjBaseSparse()
    {
    }

    virtual void reset(const AdjInputData *data)
    {
      input = data;
      stage = 0;
    }

  protected:

    const AdjInputData* input;
    int                 stage;

  };


}
#endif


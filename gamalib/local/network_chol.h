/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2005  Ales Cepek <cepek@fsv.cvut.cz>

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
 *  $Id: network_chol.h,v 1.2 2005/05/18 09:53:41 cepek Exp $
 */


#ifndef GaMaLib_LocalNetwork_chol_h
#define GaMaLib_LocalNetwork_chol_h

#include <gamalib/local/network.h>
#include <gnu_gama/adj/adj_chol.h>

namespace GaMaLib 
{
  class LocalNetwork_chol 
    : 
    public LocalNetwork, 
    GNU_gama::AdjCholDec<Double, GaMaLib::MatVecException>  
    {
      typedef GNU_gama::AdjCholDec<Double, GaMaLib::MatVecException> OLS_chol;

      bool   lindep(Index i) { return OLS_chol::lindep(i); }
      Double cond()          { return OLS_chol::cond();    } 

      const char* const algorithm() const { return "chol"; }
    };
}

#endif










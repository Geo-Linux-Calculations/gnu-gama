/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                  2001  Ales Cepek  <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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

/*
 *  $Id: g2d_exception.h,v 1.2 2007/06/26 15:04:06 cepek Exp $
 */

/******************************************************************
 * local exception for GaMaLib (Median)                           *
 ******************************************************************/
 
#ifndef GaMaLib_g2d_exception_h__GaMaLib_Median_Vyjimky_H
#define GaMaLib_g2d_exception_h__GaMaLib_Median_Vyjimky_H

#include <gamalib/exception.h>
#include <gamalib/language.h>

namespace GaMaLib 
{

  class g2d_exc : public GaMaLib::Exception
    {
    public:
      g2d_exc(const std::string& description) : 
        GaMaLib::Exception(T_IE_internal_error+std::string(" ")+description) 
        {
        }
    };
    
}  // GaMaLib

#endif


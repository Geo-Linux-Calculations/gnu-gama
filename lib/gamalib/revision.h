/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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
 *  $Id: revision.h,v 1.2 2007/06/26 15:04:04 cepek Exp $
 */

#ifndef GaMaLib_Revision_______GaMaLib_____Revision__
#define GaMaLib_Revision_______GaMaLib_____Revision__


namespace GaMaLib {

  class Observation;

  class Revision {
  public:

    Revision() {}
    virtual ~Revision() {}

    virtual bool revision (const Observation*) const = 0;
  };

}

#endif

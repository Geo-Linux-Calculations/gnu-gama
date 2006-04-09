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
 *  $Id: capi_private_exception.h,v 1.1 2006/04/09 16:40:24 cepek Exp $
 */

#ifndef GNU__GaMa__C_API_Cgama_private_exception_functions_header_file____
#define GNU__GaMa__C_API_Cgama_private_exception_functions_header_file____

#include <gamalib/exception.h>

void Cgama_private_set_exception(const GaMaLib::Exception&);
void Cgama_private_set_unknown_exception();

#endif


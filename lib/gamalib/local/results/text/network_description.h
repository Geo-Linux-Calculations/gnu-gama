/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

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
 *  $Id: network_description.h,v 1.2 2007/06/26 15:04:07 cepek Exp $
 */

#ifndef GaMa_GaMaProg_Prehled_Popis_Site_h_
#define GaMa_GaMaProg_Prehled_Popis_Site_h_

#include <gamalib/local/network.h>
#include <gamalib/language.h>
#include <cctype>

namespace GaMaLib {

template <typename OutStream>
void NetworkDescription(const std::string& description, OutStream& out)
{
   using namespace std;
   using namespace GaMaLib;

   if (description.empty()) return;

   out << T_GaMa_network_description << '\n'
       << underline(T_GaMa_network_description, '*') << '\n' 
       << description << "\n\n";

   if ((*description.rbegin()) != '\n')
     out << '\n';

   out.flush();
}

}

#endif

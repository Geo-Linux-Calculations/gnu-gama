/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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
 *  $Id: direction.cpp,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */

#include <iostream>
#include <iomanip>
#include <gamalib/observation.h>
#include <gamalib/local/pobs/bearing.h>
#include <gamalib/local/pobs/format.h>
#include <gnu_gama/gon2deg.h>

using namespace GaMaLib;
using namespace std;

void Direction::write(std::ostream& out, bool print_at) const
{
  out << "<direction";
  if (print_at)
    out << " from=\"" << from() << '"';

  out << " to=\"" << to() << '"' << " val=\"";   
  if (Observation::gons)
    out << setprecision(Format::gon_p()  ) << value()*R2G;
  else
    out << GNU_gama::gon2deg(value()*R2G, 2, Format::gon_p());
  out << '"';

  if (check_std_dev())
    {
      double stddev = Observation::gons ? stdDev() : stdDev()*0.324;
      out << " stdev=\"" << setprecision(Format::stdev_p()) << stddev << '"';
    }

  out << " />";
}





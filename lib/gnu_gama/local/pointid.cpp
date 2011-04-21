/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>,
                  2001  Ales Cepek <cepek@fsv.cvut.cz>,
                        Jan Pytel  <pytel@gama.fsv.cvut.cz>
                  2011  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/intfloat.h>
#include <gnu_gama/local/pointid.h>
#include <gnu_gama/utf8.h>
#include <cstdlib>
#include <sstream>

using namespace GNU_gama::local;

void PointID::init(const std::string& s)
{
  std::string::const_iterator b=s.begin();
  std::string::const_iterator e=s.end();
  GNU_gama::TrimWhiteSpaces(b, e);
  sid = std::string(b,e);
  iid = 0;

  if ( !GNU_gama::IsInteger(b, e) ) return;

  PointInt tmp = -1;
  std::istringstream inp(sid);
  inp >> tmp;
  if (tmp < 0) return;

  std::ostringstream out;
  out << tmp;
  if (out.str() != sid) return;

  iid = tmp;      // numeric ID
}

std::size_t PointID::lengthUtf8() const
{
  return GNU_gama::Utf8::length(sid);
}


bool PointID::operator==(const PointID& p) const
{
  return iid == p.iid && sid == p.sid;
}

bool PointID::operator!=(const PointID& p) const
{
  return iid != p.iid || sid != p.sid;
}

bool PointID::operator< (const PointID& p) const
{
  if      (iid != 0 && p.iid != 0) return iid < p.iid;
  else if (iid != 0 && p.iid == 0) return true;
  else if (iid == 0 && p.iid != 0) return false;
  else
    return sid < p.sid;
}

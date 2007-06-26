/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>,
                  2001  Ales Cepek <cepek@fsv.cvut.cz>,
                        Jan Pytel  <pytel@gama.fsv.cvut.cz>

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
 *  $Id: pointid.h,v 1.2 2007/06/26 15:04:04 cepek Exp $
 */

// PointID - point identification 

#ifndef GaMaLib_Point_Identification__h
#define GaMaLib_Point_Identification__h

#include <string>
#include <cstddef>
#include <gnu_gama/intfloat.h>

namespace GaMaLib 
{

  class PointID 
    {
    
      int          iid;   // positive integer representation if available or 0 
      std::string  sid;
    
    public:
      
      PointID()
        {
        }
      
      PointID(const char* c)
        {
          if (c == 0)
            {
              PointID t(std::string(""));
              iid = t.iid;
              sid = t.sid;
            }
          else
            {
              const std::string s(c);
              PointID t(s);
              iid = t.iid;
              sid = t.sid;
            }
        }
      
      PointID(const std::string& s);
      
      bool operator==(const PointID& p) const
      {
        return iid == p.iid && sid == p.sid;
      }
      
      bool operator!=(const PointID& p) const
      {
        return iid != p.iid || sid != p.sid;
      }
      
      bool operator< (const PointID& p) const
        {
          if      (iid != 0 && p.iid != 0) return iid < p.iid;
          else if (iid != 0 && p.iid == 0) return true;
          else if (iid == 0 && p.iid != 0) return false;
          else 
            return sid < p.sid;
        }
      
      std::string str() const
        {
          return sid;
        }

      const char* c_str()  const
        {
          return sid.c_str();
        }
      
      #ifndef _MSC_VER
      std::   
      #endif
           size_t length() const
        {
          return sid.length();
        }
      
    };   // class PointID
  
  
  inline std::ostream& operator<<(std::ostream& ostr, const PointID& p)
    {
      return ostr << p.c_str();
    }
  
  inline std::string operator+(const char* s, const PointID& p)
    {
      return std::string(s)+p.c_str();
    }
  
  inline std::string operator+(const PointID& p, const char* s)
    {
      return p.c_str()+std::string(s);
    }
  
  
}      // namespace GaMaLib

#endif







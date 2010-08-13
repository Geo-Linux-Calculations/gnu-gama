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

#ifndef GaMaLib__LocalRevision______GaMaLib___Local___Revision_____
#define GaMaLib__LocalRevision______GaMaLib___Local___Revision_____

#include <gamalib/local/gamadata.h>
#include <gamalib/revision.h>

namespace GaMaLib {

  class LocalRevision : public Revision 
    {
    
    public:

      LocalRevision(const PointData& pd) : PD(pd) {}
      
      bool revision(const Observation* o) const
        {
          if     (const Direction  *dir = dynamic_cast<const Direction *>(o))
            return      direction  (dir);
          else if(const Distance   *dis = dynamic_cast<const Distance  *>(o))
            return      distance   (dis);
          else if(const Angle      *ang = dynamic_cast<const Angle     *>(o))
            return      angle      (ang);
          else if(const H_Diff     *h_d = dynamic_cast<const H_Diff    *>(o))
            return      h_diff     (h_d);
          else if(const S_Distance *s_d = dynamic_cast<const S_Distance*>(o))
            return      s_distance (s_d);
          else if(const Z_Angle    *z_a = dynamic_cast<const Z_Angle   *>(o))
            return      z_angle    (z_a);
          else if(const X          *x__ = dynamic_cast<const X         *>(o))
            return      x          (x__);
          else if(const Y          *y__ = dynamic_cast<const Y         *>(o))
            return      y          (y__);
          else if(const Z          *z__ = dynamic_cast<const Z         *>(o))
            return      z          (z__);
          else if(const Xdiff      *xdf = dynamic_cast<const Xdiff     *>(o))
            return      xdiff      (xdf);
          else if(const Ydiff      *ydf = dynamic_cast<const Ydiff     *>(o))
            return      ydiff      (ydf);
          else if(const Zdiff      *zdf = dynamic_cast<const Zdiff     *>(o))
            return      zdiff      (zdf);
          
          return false;
        }
      
    private:
      
      const PointData& PD;
      
      bool direction  (const Direction  *obs) const;
      bool distance   (const Distance   *obs) const;
      bool angle      (const Angle      *obs) const;
      bool h_diff     (const H_Diff     *obs) const;
      bool s_distance (const S_Distance *obs) const;
      bool z_angle    (const Z_Angle    *obs) const;
      bool x          (const X          *obs) const;
      bool y          (const Y          *obs) const;
      bool z          (const Z          *obs) const;
      bool xdiff      (const Xdiff      *obs) const;
      bool ydiff      (const Ydiff      *obs) const;
      bool zdiff      (const Zdiff      *obs) const;
    };
  
}

#endif






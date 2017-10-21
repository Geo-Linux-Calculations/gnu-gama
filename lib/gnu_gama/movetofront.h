/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2007  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library

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

#ifndef GNU_Gama_MoveToFront_gnu_gama_move_to_front_movetofront_h
#define GNU_Gama_MoveToFront_gnu_gama_move_to_front_movetofront_h

#include <algorithm>

namespace GNU_gama {

  template<size_t N, typename Key, typename Buffer>
  class MoveToFront
  {
  public:
    MoveToFront(Buffer b=Buffer());
    MoveToFront(Buffer b[]);

    std::pair<Buffer,bool> get(Key key);
    void   erase()      { active = 0; }
    size_t size() const { return N;   }

  private:
    Key    key_[N];
    Buffer buf_[N];
    size_t active;
  };


  template<size_t N, typename Key, typename Buffer>
  MoveToFront<N,Key,Buffer>::MoveToFront(Buffer b)
  {
    for (size_t i=0; i<N; i++) buf_[i] = b++;
    active = 0;
  }

  template<size_t N, typename Key, typename Buffer>
  MoveToFront<N,Key,Buffer>::MoveToFront(Buffer b[])
  {
    for (size_t i=0; i<N; i++) buf_[i] = b[i];
    active = 0;
  }

  template<size_t N, typename Key, typename Buffer>
  std::pair<Buffer,bool> MoveToFront<N,Key,Buffer>::get(Key key)
  {
    bool   good = false;
    size_t imin = N-1;

    for (size_t i=0; i<active; i++)
      if (key_[i] == key)
        {
          imin = i;
          good = true;
          break;
        }

    if (!good && active < N) imin = active++;

    Buffer buf = buf_[imin];
    for (size_t i=imin; i>0; i--)
      {
        key_[i] = key_[i-1];
        buf_[i] = buf_[i-1];
      }
    key_[0] = key;
    buf_[0] = buf;

    return std::pair<Buffer,bool>(buf, good);
  }

}  // namespace GNU_gama

#endif

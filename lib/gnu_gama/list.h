/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

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

#include <vector>
#include <iterator>


#ifndef GNU_gama_pointer_list_h_gnugamapointerlist_gnu_gama_pointerlist
#define GNU_gama_pointer_list_h_gnugamapointerlist_gnu_gama_pointerlist


namespace GNU_gama {


  template <typename T> class List;

  template <typename T> class List<T*>
    {
      typedef std::vector<void*> Vector;
      Vector  vec;

      List(const List& cod);
      // List& operator=(const List& cod);

    public:

      class iterator;
      class const_iterator;

      List() {}

      std::size_t size()  const   { return vec.size();  }
      bool        empty() const   { return vec.empty(); }
      void        push_back(T* t) { vec.push_back(t);   }
      void        pop_back()      { vec.pop_back();     }
      void        clear()         { vec.clear();        }


      class const_iterator
        // : public std::iterator <std::forward_iterator_tag, T>
        {
        public:

          const_iterator()
            {
            }
          const_iterator(const typename Vector::const_iterator& p) : vit(p)
            {
            }
          bool operator==(const const_iterator& x) const
            {
              return vit==x.vit;
            }
          bool operator!=(const const_iterator& x) const
            {
              return vit!=x.vit;
            }
          const_iterator& operator++()
            {
              ++vit;
              return *this;
            }
          const_iterator operator++(int)
            {
              const_iterator tmp(vit);
              ++vit;
              return tmp;
            }
          const T* operator*() const
            {
              return static_cast<T*>(*vit);
            }

        private:
          typename Vector::const_iterator vit;

        };

      const_iterator  begin() const { return vec.begin(); }
      const_iterator  end  () const { return vec.end  (); }


      class iterator
        // : public std::iterator <std::forward_iterator_tag, T>
        {
        public:

          iterator()
            {
            }
          iterator(const typename Vector::iterator& p) : vit(p)
            {
            }
          operator const_iterator() const
            {
              return const_iterator(vit);
            }
          bool operator==(const iterator& x) const
            {
              return vit==x.vit;
            }
          bool operator!=(const iterator& x) const
            {
              return vit!=x.vit;
            }
          iterator& operator++()
            {
              ++vit;
              return *this;
            }
          iterator operator++(int)
            {
              iterator tmp(vit);
              ++vit;
              return tmp;
            }
          T* operator*() const
            {
              return static_cast<T*>(*vit);
            }

        private:
          typename Vector::iterator vit;

        };

      iterator  begin() { return vec.begin(); }
      iterator  end  () { return vec.end  (); }
    };

}

#endif

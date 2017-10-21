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

#include <map>
#include <iterator>


#ifndef GNU_gama_point_data_h_gnugamapointbase_gnu_gama_pointbase
#define GNU_gama_point_data_h_gnugamapointbase_gnu_gama_pointbase


namespace GNU_gama {


  template <typename Point>
    class PointBase
    {
    private:

      typedef  std::map<typename Point::Name, Point*>  Points;
      Points   points;
      typename Point::Common* common;

    public:

      PointBase() : common(0) {}
      PointBase(const PointBase& cod);
      ~PointBase();

      PointBase& operator=(const PointBase& cod);

      void put(const Point&);
      void put(Point*&);

      Point*       find(const typename Point::Name&);
      const Point* find(const typename Point::Name&) const;

      void erase(const typename Point::Name&);
      void erase();

      class const_iterator
        // : public std::iterator <std::forward_iterator_tag, Point>
        {
        public:

          const_iterator(const typename Points::const_iterator& p) : pit(p)
            {
            }
          bool operator==(const const_iterator& x) const
            {
              return pit==x.pit;
            }
          bool operator!=(const const_iterator& x) const
            {
              return pit!=x.pit;
            }
          const_iterator& operator++()
            {
              ++pit;
              return *this;
            }
          const_iterator operator++(int)
            {
              const_iterator tmp(pit);
              ++pit;
              return tmp;
            }
          const Point* operator*() const
            {
              return (*pit).second;
            }

        private:
          typename Points::const_iterator pit;

        };

      const_iterator  begin() const { return points.begin(); }
      const_iterator  end  () const { return points.end  (); }


      class iterator
        // : public std::iterator <std::forward_iterator_tag, Point>
        {
        public:

          iterator(const typename Points::iterator& p) : pit(p)
            {
            }
          operator const_iterator() const
            {
              return const_iterator(pit);
            }
          bool operator==(const iterator& x) const
            {
              return pit==x.pit;
            }
          bool operator!=(const iterator& x) const
            {
              return pit!=x.pit;
            }
          iterator& operator++()
            {
              ++pit;
              return *this;
            }
          iterator operator++(int)
            {
              iterator tmp(pit);
              ++pit;
              return tmp;
            }
          Point* operator*() const
            {
              return (*pit).second;
            }

        private:
          typename Points::iterator pit;

        };

      iterator  begin() { return points.begin(); }
      iterator  end  () { return points.end  (); }

      typename Point::Common* common_data() const { return common; }
      void set_common_data(typename Point::Common*);
    };


  template <typename Point>
    PointBase<Point>::~PointBase()
    {
      erase();
    }


  template <typename Point>
    PointBase<Point>::PointBase(const PointBase& cpd)
    {
      common = cpd.common;
      for (const_iterator p=cpd.begin(), e=cpd.end(); p!=e; ++p)
        {
          put( **p );
        }
    }


  template <typename Point>
    PointBase<Point>& PointBase<Point>::operator=(const PointBase& cpd)
    {
      if (this != &cpd)
        {
          erase();
          common = cpd.common;
          for (const_iterator p=cpd.begin(), e=cpd.end(); p!=e; ++p)
            {
              put( **p );
            }
        }

      return *this;
    }


  template <typename Point>
    void PointBase<Point>::put(const Point& point)
    {
      Point* ptr = find(point.name);

      if (ptr)
        {
          *ptr = point;
        }
      else
        {
          ptr = new Point(point);
          points[ptr->name] = ptr;
        }

      ptr->common = common;
    }


  template <typename Point>
    void PointBase<Point>::put(Point*& point_ptr)
    {
      typename Points::iterator t = points.find(point_ptr->name);

      if (t != points.end())
        {
          Point* ptr = (*t).second;

          if (ptr != point_ptr)
            {
              *ptr = *point_ptr;
              delete  point_ptr;
              point_ptr = ptr;
            }
        }
      else
        {
          points[point_ptr->name] = point_ptr;
        }

      point_ptr->common = common;
    }


  template <typename Point>
    Point* PointBase<Point>::find(const typename Point::Name& name)
    {
      typename Points::iterator t = points.find(name);
      if (t != points.end())
        {
          return (*t).second;
        }

      return 0;
    }


  template <typename Point>
    const Point* PointBase<Point>::find(const typename Point::Name& name) const
    {
      typename Points::const_iterator t = points.find(name);
      if (t != points.end())
        {
          return (*t).second;
        }

      return 0;
    }


  template <typename Point>
    void PointBase<Point>::erase(const typename Point::Name& name)
    {
      typename Points::iterator t = points.find(name);
      if (t != points.end())
        {
          delete (*t).second;
          points.erase(t);
        }
    }


  template <typename Point>
    void PointBase<Point>::erase()
    {
      typename Points::iterator t = points.begin();
      typename Points::iterator e = points.end();
      while (t != e)
        {
          delete (*t).second;
          ++t;
        }

      points.erase(points.begin(), points.end());
    }


  template <typename Point>
    void PointBase<Point>::set_common_data(typename Point::Common* com)
    {
      common = com;

      typename Points::iterator t = points.begin();
      typename Points::iterator e = points.end();
      while (t != e)
        {
          (*t).second->common = common;
          ++t;
        }

    }

}

#endif

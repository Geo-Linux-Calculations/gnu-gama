/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2011  Ales Cepek <cepek@gnu.org>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>

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

/** \file visitor.h
 * \brief Generic implementation of acyclic visitor pattern
 *
 * \author Ales Cepek
 * \author Vaclav Petras
 */

#ifndef GNU_gama_visior_h_gnugamavisitor_gnu_gama_visior
#define GNU_gama_visior_h_gnugamavisitor_gnu_gama_visior

#include <iostream>

namespace GNU_gama {

/** \brief Marking interface for acyclic visitor design pattern.
 *
 * BaseVisitor is a completely degenerated class having only
 * the virtual destructor.
 *
 * Methods \c accept in visitable classes take pointer to BaseVisitor
 * and return \c true if visit was successful, \c false otherwise.
 * Caller is not expected to check it.
 * However, sometimes is better to know it.
 *
 * Example of accept method from base visitable class:
 * \code
 * virtual bool accept(BaseVisitor* visitor) = 0;
 * \endcode
 *
 * Example of using visitor pattern:
 * \code
 * // base visitable class
 * class Observation
 * {
 * public:
 *     virtual bool accept(BaseVisitor* visitor) = 0;
 * };
 *
 * // concrete visitable class
 * class Distance : public Observation
 * {
 * public:
 *     bool accept(BaseVisitor* visitor)
 *     {
 *         if (Visitor<Distance>* v = dynamic_cast<Visitor<Derived>*>(visitor))
 *          {
 *            v->visit(this);
 *          }
 *     }
 *
 * // concrete visitor class
 * class LocalRevision : public BaseVisitor,
 *                       public Visitor<Direction>
 * {
 * public:
 *   void visit(Direction* dir)  {
 *   // ...
 *   }
 * };
 * \endcode
 * Note that every derived visitable class has to have its own \c accept method.
 * This function is almost the same for all classes.
 * For convenience it can be implemented by a macro or a template.
 * In GNU Gama there is Accept template class which will provide \c accept method implementation
 * to its subclasses.
 *
 * \sa Visitor, Accept
 */

class BaseVisitor
{
public:
  virtual ~BaseVisitor() {}
};


/** \brief Abstract visitor class (design pattern 'acyclic visitor')
 *
 * Concrete visitor class typically inherits more than one template class Visitor.
 * It also inherits BaseVisitor to mark itself as visitor.
 * Visitor template parameter \a Element is class which should be visited.
 *
 * Example of concrete visitor class:
 * \code
 *  class LocalRevision : public BaseVisitor,
 *                        public Visitor<Direction>,
 *                        public Visitor<Distance>
 *                        // ...
 *  {
 *  public:
 *    void visit(Direction* element)  {
 *    // ... method implementation
 *    }
 *    void visit(Distance* element)   {
 *    // ... method implementation
 *    }
 *  private:
 *  // ... implementation
 *  };
 * \endcode
 */

template <typename Element> class Visitor
{
public:
  virtual ~Visitor() {}
  virtual void visit(Element* element) = 0;
};


/** Helper intermediate template class Accept defines method
 *  accept() for derived element classes in acyclic visitor pattern.
 *
 * Example:
 *
 * \code
 *  //  g3 distance class
 *
 *  class Distance : public Accept<Distance, Observation>,
 *                   public FromTo, public Value {
 *  public:
 *
 *    Distance() {}
 *    Distance(double d) : Value(d) {}
 *
 *    int dimension() const { return 1; }
 *  };
 * \endcode
 *
 * \note When visitor is unknown no operation is done
 * (this is default expected behavior in visitor pattern).
 * More particulary, \c accept returns \c false in this case.
 *
 * \note It is not possible to do visit on const object.
 */

template <typename Derived, typename Base>
  class Accept : public Base {
public:
  /** \brief Default implementation of accept method.
   */
  void accept(BaseVisitor* visitor)
  {
    if (Visitor<Derived>* v = dynamic_cast<Visitor<Derived>*>(visitor))
      {
        v->visit(static_cast<Derived*>(this));
      }
  }

};

} // namespace GNU_gama

#endif // GNU_gama_visior_h_gnugamavisitor_gnu_gama_visior

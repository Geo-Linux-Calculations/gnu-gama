/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local_GaMa_XML_Data_Object_object_h_
#define gama_local_GaMa_XML_Data_Object_object_h_

#include <string>
#include <sstream>
#include <gnu_gama/adj/adj.h>
#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/g3/g3_adjres.h>

namespace GNU_gama {

  /** \brief Data Object sub-namespace */

  namespace DataObject {

  /** \brief Abstract data object class */

  class Base {
  public:

    virtual ~Base()
      {
      }

    /** XML representation of the object  */
    virtual std::string xml() const = 0;

    static  std::string xml_begin();
    static  std::string xml_end();
  };


  /** \brief Text object (tad <text>) */

  class Text : public Base {
  public:

    std::string text;

    Text()
      {
      }
    Text(std::string s) : text(s)
      {
      }
    std::string xml() const
      {
        if (!text.empty())
          return "\n<text>" + text + "</text>\n";

        return "";
      }
  };


  /** \brief Adjustment Data Input object */

  class AdjInput : public Base {
  public:

    GNU_gama::AdjInputData *data;

    AdjInput() : data(0)
      {
      }
    AdjInput(GNU_gama::AdjInputData *d) : data(d)
      {
      }
    std::string xml() const
      {
        if (data)
          {
            std::stringstream out;
            data->write_xml(out);

            return out.str();
          }

        return "";
      }
  };


  /** \brief g3_model adjustment XML input data */

  class g3_model : public Base {
  public:

    GNU_gama::g3::Model *model;

    g3_model() : model(0)
      {
      }
    g3_model(GNU_gama::g3::Model *m) : model(m)
      {
      }
    std::string xml() const
      {
        if (model)
          {
            std::stringstream out;
            model->write_xml(out);

            return out.str();
          }

        return "";
      }
  };


  /** \brief g3_model adjustment results */

  class g3_adj_results : public Base {
  public:

    GNU_gama::g3::AdjustmentResults *adjres;

    g3_adj_results() : adjres(0)
      {
      }
    g3_adj_results(GNU_gama::g3::AdjustmentResults *m) : adjres(m)
      {
      }
    std::string xml() const
      {
        if (adjres)
          {
            std::stringstream out;
            adjres->write_xml(out);

            return out.str();
          }

        return "";
      }
  };

}}       // namespace GNU_gama::DataObject


#endif


















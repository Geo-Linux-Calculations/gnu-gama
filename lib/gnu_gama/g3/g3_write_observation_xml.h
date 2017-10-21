/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005, 2011  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_g3_WriteObservationXML_g3_xml_write_observation_h
#define GNU_gama_g3_WriteObservationXML_g3_xml_write_observation_h


#include <gnu_gama/g3/g3_observation.h>
#include <gnu_gama/outstream.h>
#include <iomanip>



namespace GNU_gama { namespace g3 {

  /** g3 visitor class for writing observation data in XML. */

  class WriteObservationXML :
    public GNU_gama::BaseVisitor,
    public GNU_gama::Visitor<Angle>,
    public GNU_gama::Visitor<Azimuth>,
    public GNU_gama::Visitor<Distance>,
    public GNU_gama::Visitor<Height>,
    public GNU_gama::Visitor<HeightDiff>,
    public GNU_gama::Visitor<Vector>,
    public GNU_gama::Visitor<XYZ>,
    public GNU_gama::Visitor<ZenithAngle>
  {
  private:

    std::ostream& out;

  public:

    WriteObservationXML(std::ostream& ostr) : out(ostr) {}

    void visit(Angle*);
    void visit(Azimuth*);
    void visit(Distance*);
    void visit(Height*);
    void visit(HeightDiff*);
    void visit(Vector*);
    void visit(XYZ*);
    void visit(ZenithAngle*);

  };


}}

#endif

/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/g3/g3_write_observation_xml.h>
#include <iomanip>


using namespace std;
using namespace GNU_gama::g3;


void WriteObservationXML::visit(Angle* a)
{
  out << "<angle>"
      << " <from>"  << a->from  << "</from>"
      << " <left>"  << a->left  << "</left>"
      << " <right>" << a->right << "</right>\n"
      << "        <val>" << a->obs() << "</val>\n"
      << "        </angle>\n";
}

void WriteObservationXML::visit(Azimuth* a)
{
  out << "<azimuth>"
      << " <from>" << a->from << "</from> <to>" << a->to << "</to>\n"
      << "        <val>" << a->obs() << "</val>\n"
      << "        </azimuth>\n";
}

void WriteObservationXML::visit(Distance* d)
{
  out << "<distance>"
      << " <from>" << d->from << "</from> <to>" << d->to << "</to>\n"
      << "        <val>" << d->obs() << "</val>\n"
      << "        </distance>\n";
}

void WriteObservationXML::visit(Height* h)
{
  out << "<height>"
      << " <id>" << h->id << "</id>\n"
      << "        <val>" << h->obs() << "</val>\n"
      << "        </height>\n";
}

void WriteObservationXML::visit(HeightDiff* hd)
{
  out << "<height-diff>"
      << " <from>" << hd->from << "</from> <to>" << hd->to << "</to>\n"
      << "        <val>" << hd->obs() << "</val>\n"
      << "        </height-diff>\n";
}

void WriteObservationXML::visit(Vector* v)
{
  out << "<vector>"
      << " <from>" << v->from << "</from> <to>" << v->to << "</to>\n"
      << "        <dx>" << v->dx() << "</dx>\n"
      << "        <dy>" << v->dy() << "</dy>\n"
      << "        <dz>" << v->dz() << "</dz>\n"
      << "        </vector>\n";
}

void WriteObservationXML::visit(XYZ* xyz)
{
  out << "<xyz>"
      << " <id>" << xyz->id << "</id>\n"
      << "        <x>" << xyz->x() << "</x>\n"
      << "        <y>" << xyz->y() << "</y>\n"
      << "        <z>" << xyz->z() << "</z>\n"
      << "        </xyz>\n";
}

void WriteObservationXML::visit(ZenithAngle* za)
{
  out << "<zenith-angle>"
      << " <from>" << za->from << "</from> <to>" << za->to << "</to>\n"
      << "        <val>" << za->obs() << "</val>\n"
      << "        </zenith-angle>\n";
}

/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2011  Ales Cepek <cepek@gnu.org>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>
                  2013  Ales Cepek <cepek@gnu.org>

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

/** \file writevisitor.h
 * \brief #GNU_gama::local::WriteVisitor header file
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef GNU_gama__local_write_writewisitor_h
#define GNU_gama__local_write_writewisitor_h

#include <gnu_gama/gon2deg.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/pobs/bearing.h>
#include <gnu_gama/local/pobs/format.h>
#include <iomanip>

namespace GNU_gama { namespace local {

template <typename OutStream>
class WriteVisitor : public AllObservationsVisitor
{
public:
    WriteVisitor(OutStream& out, bool print_at) : out_(out), print_at_(print_at) {}

    void visit(Angle *obs) { write(*obs, out_, print_at_); }
    void visit(Direction *obs) { write(*obs, out_, print_at_); }
    void visit(Distance *obs) { write(*obs, out_, print_at_); }
    void visit(H_Diff *obs) { write(*obs, out_, print_at_); }
    void visit(S_Distance *obs) { write(*obs, out_, print_at_); }
    void visit(X *obs) { write(*obs, out_, print_at_); }
    void visit(Y *obs) { write(*obs, out_, print_at_); }
    void visit(Z *obs) { write(*obs, out_, print_at_); }
    void visit(Xdiff *obs) { write(*obs, out_, print_at_); }
    void visit(Ydiff *obs) { write(*obs, out_, print_at_); }
    void visit(Zdiff *obs) { write(*obs, out_, print_at_); }
    void visit(Z_Angle *obs) { write(*obs, out_, print_at_); }
    void visit(Azimuth *obs) { write(*obs, out_, print_at_); }

    void write(const Angle& obs, OutStream& out, bool print_at) const
    {
      out << "<angle";
      if (print_at)
        out << " from=\"" << obs.from() << '"';

      out << " bs=\"" << obs.bs()  << '"' << " fs=\"" << obs.fs() << '"' << " val=\"";
      if (Observation::gons)
        out << std::setprecision(Format::gon_p()) << obs.value()*R2G;
      else
        out << GNU_gama::gon2deg(obs.value()*R2G, 2, Format::gon_p());
      out << '"';

      if (obs.check_std_dev())
        {
          double stddev = Observation::gons ? obs.stdDev() : obs.stdDev()*0.324;
          out << " stdev=\"" << std::setprecision(Format::stdev_p()) << stddev << '"';
        }

      out << " />";
    }

    void write(const Direction& obs, OutStream& out, bool print_at) const
    {
      out << "<direction";
      if (print_at)
        out << " from=\"" << obs.from() << '"';

      out << " to=\"" << obs.to() << '"' << " val=\"";
      if (Observation::gons)
        out << std::setprecision(Format::gon_p()  ) << obs.value()*R2G;
      else
        out << GNU_gama::gon2deg(obs.value()*R2G, 2, Format::gon_p());
      out << '"';

      if (obs.check_std_dev())
        {
          double stddev = Observation::gons ? obs.stdDev() : obs.stdDev()*0.324;
          out << " stdev=\"" << std::setprecision(Format::stdev_p()) << stddev << '"';
        }

      out << " />";
    }

    void write(const Distance& obs, OutStream& out, bool print_at) const
    {
      using namespace std;
      out << "<distance";
      if (print_at)
        out << " from=\"" << obs.from() << '"';
      out << " to=\"" << obs.to() << '"'
          << " val=\""   << std::setprecision(Format::coord_p()) << obs.value()  << '"';
      if (obs.check_std_dev())
        out << " stdev=\"" << std::setprecision(Format::stdev_p()) << obs.stdDev() << '"';
      out << " />";
    }

    void write(const H_Diff& obs, OutStream& out, bool /* print_at */) const
    {
      out << "<dh";
      // if (print_at) ... always print from="..."
      out << " from=\"" << obs.from() << '"';
      out << " to=\"" << obs.to() << '"'
          << " val=\""   << std::setprecision(Format::coord_p()) << obs.value()  << '"';
      if (obs.check_std_dev())
        out << " stdev=\"" << std::setprecision(Format::stdev_p()) << obs.stdDev() << '"';;
      if (obs.dist())
        out << " dist=\"" << obs.dist() << "\"";
      out << " />";
    }

    void write(const S_Distance& obs, OutStream& out, bool print_at) const
    {
      using namespace std;
      out << "<s-distance";
      if (print_at)
        out << " from=\"" << obs.from() << '"';
      out << " to=\"" << obs.to() << '"'
          << " val=\""   << std::setprecision(Format::coord_p()) << obs.value()  << '"';
      if (obs.check_std_dev())
        out << " stdev=\"" << std::setprecision(Format::stdev_p()) << obs.stdDev() << '"';
      out << " />";
    }

    void write(const X& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- " << obs.from() << " x = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }


    void write(const Y& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- " << obs.from() << " y = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }


    void write(const Z& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- " << obs.from() << " z = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }

    void write(const Xdiff& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- from='" << obs.from() << "' to='" << obs.to() << "'"
          << " diff x = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }


    void write(const Ydiff& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- from='" << obs.from() << "' to='" << obs.to() << "'"
          << " diff y = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }


    void write(const Zdiff& obs, OutStream& out, bool) const
    {
      using namespace std;
      out << "<!-- from='" << obs.from() << "' to='" << obs.to() << "'"
          << " diff z = "
          << std::setprecision(Format::coord_p()) << obs.value() << " --!>";
    }

    void write(const Z_Angle& obs, OutStream& out, bool print_at) const
    {
      out << "<z-angle";
      if (print_at)
        out << " from=\"" << obs.from() << '"';

      out << " to=\"" << obs.to() << '"' << " val=\"";
      if (Observation::gons)
        out << std::setprecision(Format::gon_p()) << obs.value()*R2G;
      else
        out << GNU_gama::gon2deg(obs.value()*R2G, 2, Format::gon_p());
      out << '"';

      if (obs.check_std_dev())
        {
          double stddev = Observation::gons ? obs.stdDev() : obs.stdDev()*0.324;
          out << " stdev=\"" << std::setprecision(Format::stdev_p()) << stddev << '"';
        }

      out << " />";
    }

    void write(const Azimuth& obs, OutStream& out, bool print_at) const
    {
      out << "<azimuth";
      if (print_at)
        out << " from=\"" << obs.from() << '"';

      out << " to=\"" << obs.to() << '"' << " val=\"";
      if (Observation::gons)
        out << std::setprecision(Format::gon_p()  ) << obs.value()*R2G;
      else
        out << GNU_gama::gon2deg(obs.value()*R2G, 2, Format::gon_p());
      out << '"';

      if (obs.check_std_dev())
        {
          double stddev = Observation::gons ? obs.stdDev() : obs.stdDev()*0.324;
          out << " stdev=\"" << std::setprecision(Format::stdev_p()) << stddev << '"';
        }

      out << " />";
    }

private:
    OutStream& out_;
    bool print_at_;
};

}} // namespace GNU_gama local

#endif

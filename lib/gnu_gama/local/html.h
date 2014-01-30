/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/** \file html.h
 * \brief #GNU_gama::local::GamaLocalHTML class header file
 *
 * \author Ales Cepek
 */

#ifndef GAMA_LOCAL_HTML__Gama_Local_Html__gama_local_html__h
#define GAMA_LOCAL_HTML__Gama_Local_Html__gama_local_html__h

#include <gnu_gama/local/network.h>
#include <string>

namespace GNU_gama { namespace local {

class GamaLocalHTML
{
public:
  GamaLocalHTML(LocalNetwork* local_network=0);

  void set_local_network(GNU_gama::local::LocalNetwork* local_network);
  GNU_gama::local::LocalNetwork* get_local_network() const { return lnet; }

  void exec();

  void html(std::ostream&) const;
  std::string str() const;

  std::string html_begin       () const { return begin       .text(); }
  std::string html_info        () const { return info        .text(); }
  std::string html_unknowns    () const { return unknowns    .text(); }
  std::string html_observations() const { return observations.text(); }
  std::string html_residuals   () const { return residuals   .text(); }
  std::string html_rejected    () const { return rejected    .text(); }
  std::string html_end         () const { return end         .text(); }

  void set_info        (bool p) { info        .active = p; }
  void set_unknowns    (bool p) { unknowns    .active = p; }
  void set_observations(bool p) { observations.active = p; }
  void set_residuals   (bool p) { residuals   .active = p; }
  void set_rejected    (bool p) { rejected    .active = p; }

  bool get_info        () const { return info        .active; }
  bool get_unknowns    () const { return unknowns    .active; }
  bool get_observations() const { return observations.active; }
  bool get_residuals   () const { return residuals   .active; }
  bool get_rejected    () const { return rejected    .active; }

  void set_all_parts_active()
  {
    info.active = unknowns.active = observations.active =
      residuals.active = rejected.active = true;
  }

  void set_title(std::string s) { title = s;    }
  std::string get_title() const { return title; }

  void set_h1(std::string s) { h1 = s;    }
  std::string get_h1() const { return h1; }

  void set_style(std::string style) { html_style = style; }
  std::string style() const { return html_style; }

private:
  LocalNetwork* lnet;

  struct HtmlPart {
        HtmlPart() : active(true) {}
        std::string text() const { return active ? str : std::string(); }
        bool active;
        std::string str;
    };

  HtmlPart begin, info, unknowns, observations, residuals, rejected, end;

  std::string html_style;
  std::string title, h1;

  void htmlBegin       ();
  void htmlInfo        ();
  void htmlUnknowns    ();
  void htmlObservations();
  void htmlResiduals   ();
  void htmlRejected    ();
  void htmlEnd         ();
};

}} // namespace GNU_gama::local

#endif // HTML_H

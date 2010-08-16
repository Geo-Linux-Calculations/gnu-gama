/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

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

#include <gnu_gama/local/c/api/capi_exception.h>
#include <gnu_gama/local/c/api/capi_private_exception.h>
#include <gnu_gama/local/exception.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/xml/gkfparser.h>
#include <cstring>

// exception data is accessible only through the selected functions
// initialization and clean up si done by ctor/dtor

namespace {

  struct GaMa_C_API_exception_handling_data
  {
    GaMa_C_API_exception_handling_data()
    {
      text[0] = 0;
    }
    ~GaMa_C_API_exception_handling_data()
    {
      clean();
    }
    void clean()
    {
      if (text[0])
        {
          text[0]  = 0;
          unknown  = false;
          xml_line = 0;
        }
    }


    char  text[256];
    bool  unknown;
    int   xml_line;
  };

  GaMa_C_API_exception_handling_data  c_api_data;
}

// ------  C API  ----------------------------------------------------------

extern "C" {

  void Cgama_init(int lang)
  {
    using namespace GNU_gama::local;
    set_gama_language(gama_language(lang));
  }

  const char* Cgama_exception()
  {
    if (c_api_data.text[0]) return c_api_data.text;

    return 0;
  }

  void Cgama_exception_clean()
  {
    c_api_data.clean();
  }

  int Cgama_exception_unknown()
  {
    return c_api_data.unknown ? 1 : 0;
  }

  int Cgama_exception_GKF_parser_line()
  {
    return c_api_data.xml_line;
  }

}    // extern "C"

// -----  private functions  -----------------------------------------------

void Cgama_private_set_exception(const GNU_gama::local::Exception& e)
{
  c_api_data.clean();
  strncpy(c_api_data.text, e.text.c_str(),256);
  c_api_data.text[255] = 0;

  using namespace GNU_gama::local;
  if (const GNU_gama::local::ParserException*
      g = dynamic_cast<const GNU_gama::local::ParserException*>(&e))
    {
      c_api_data.xml_line = g->line;
    }
}

void Cgama_private_set_unknown_exception()
{
  using namespace std;

  c_api_data.clean();
  strcpy(c_api_data.text, "unknown exception");
  c_api_data.unknown   = true;
}












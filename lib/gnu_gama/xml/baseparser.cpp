/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2014  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/xml/baseparser.h>
#include <gnu_gama/xml/encoding.h>
#include <cstdlib>

using namespace std;
using namespace GNU_gama;


// ===========================================================================

namespace {
  extern "C" {

    void characterDataHandler(void *userData, const char* s, int len)
    {
      using namespace GNU_gama;
      CoreParser* gexp = static_cast<CoreParser*>(userData);

      gexp->characterDataHandler(s, len);
    }

    void startElement(void *userData, const char *cname, const char **atts)
    {
      using namespace GNU_gama;
      CoreParser* gexp = static_cast<CoreParser*>(userData);

      gexp->startElement(cname, atts);
    }

    void endElement(void *userData, const char *cname)
    {
      using namespace GNU_gama;
      CoreParser* gexp = static_cast<CoreParser*>(userData);

      gexp->endElement(cname);
    }

  }   // extern "C"
}     // unnamed namespace

// ===========================================================================



CoreParser::CoreParser()
{
  state = -1;
  errCode = errLineNumber = 0;

  parser  = XML_ParserCreate(0);

  XML_SetUserData              (parser, this);
  XML_SetElementHandler        (parser, ::startElement, ::endElement);
  XML_SetCharacterDataHandler  (parser, ::characterDataHandler);
  XML_SetUnknownEncodingHandler(parser, UnknownEncodingHandler, 0);
}

CoreParser::~CoreParser()
{
  XML_ParserFree(parser);
}


bool CoreParser::toDouble(const std::string& s, double& d) const
{
  using namespace std;        // Visual C++ doesn't know std::atof ???

  if (IsFloat(s))
    {
      d = atof(s.c_str());
      return true;
    }
  else
    return false;
}


bool CoreParser::toInteger(const std::string& s, int& value) const
{
  if (IsInteger(s))
    {
      using namespace std;
      value =atoi(s.c_str());
      return true;
    }
  else
    {
      return false;
    }
}


bool CoreParser::toIndex(const std::string& s, int& index) const
{
  for (std::string::const_iterator i=s.begin(); i!=s.end(); ++i)
    if (!isspace(*i) && !isdigit(*i))
      return false;

  double d;
  if (toDouble(s, d))
    {
      index = static_cast<int>(d);
      return true;
    }
  else
    return false;
}


int CoreParser::error(const char* text)
{
  // store only the first detected error
  if(errCode) return 1;

  errString = std::string(text);
  errCode   = -1;
  errLineNumber = XML_GetCurrentLineNumber(parser);
  state = 0;     /*  state_error is 0  */
  return 1;
}




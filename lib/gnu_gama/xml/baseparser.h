/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: baseparser.h,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */

#ifndef GaMaLib_GaMa__XML__BASE_Base_base__PARSER_Parser_parser__h_
#define GaMaLib_GaMa__XML__BASE_Base_base__PARSER_Parser_parser__h_


// BaseParser is just a simple C++ wrapper for XML parser expat

#include <gnu_gama/xml_expat.h>
#include <gnu_gama/xml/dataobject.h>
#include <gnu_gama/intfloat.h>
#include <string>
#include <list>

namespace GNU_gama {
  
  class CoreParser 
  {
  public:
    
    CoreParser();
    virtual ~CoreParser();
    
    // expat parser interface
    
    virtual void xml_parse(const char *s, int len, int  isFinal) = 0;
    void xml_parse(const std::string& s, bool isFinal) 
    {
      xml_parse(s.c_str(), s.length(), isFinal ? 1 : 0);
    }
    virtual int characterDataHandler(const char* s, int len) = 0;
    virtual int startElement(const char *cname, const char **atts) = 0;
    virtual int endElement(const char * name) = 0;

  protected: 
    
    XML_Parser  parser;
    int         state;      /*  state_error must be 0  */
    
    int error(const char* text);
    int error(const std::string& s)   { return error(s.c_str()); }

    bool toDouble(const std::string&, double&     ) const;
    bool toIndex (const std::string&, std::size_t&) const;
      
    // private:

    std::string errString;
    int         errLineNumber;  
    int         errCode;              // -1 bad data in gkf; 0 OK; >0 expat

  };  // class CoreParser




  template<typename ParserException> class BaseParser : public CoreParser 
  {
  public:
    
    void xml_parse(const char *s, int len, int  isFinal) 
    { 
      int err = XML_Parse(parser, s, len, isFinal);
      if (err == 0)
        {
          // fatal error
          
          errString=std::string(XML_ErrorString(XML_GetErrorCode(parser)));
          errCode  =XML_GetErrorCode(parser);
          errLineNumber = XML_GetCurrentLineNumber(parser);
          
          throw ParserException(errString, errLineNumber, errCode);
        }
      
      if (state == 0)     /*  state_error must be 0  */
        {
          // errLineNumber is set by function  error("...");    
          errCode = -1;
          throw ParserException(errString, errLineNumber, errCode);
        }
    }
    
  };








}     // namespace GNU_gama


#endif


















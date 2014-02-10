/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2010  Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#include <gnu_gama/local/localnetwork2sql.h>
#include <gnu_gama/version.h>
#include <gnu_gama/exception.h>
#include <iostream>
#include <fstream>


int help()
{
  using std::cerr;
  
  cerr << "Usage: gama-local-xml2sql configuration (xml_input|-) [sql_output|-]\n\n" 
       << "Convert XML adjustment input of gama-local to SQL\n\n";
  
  return 1;
}



int parameters(int argc, char* argv[], std::istream*& xml, std::ostream*& sql)
{
  if (argc < 2 || argc > 4) return help();
  
  if (argv[1][0] == '-' ) return help();

  const char* inp = "-";
  const char* out = "-";

  switch (argc)
    {
    case 4 : out = argv[3];
    case 3 : inp = argv[2]; break;
    default: return help();
    }

  if (std::string(inp) == "-")
    xml = &std::cin;
  else
    xml = new std::ifstream(inp);

  if (std::string(out) == "-")
    sql = &std::cout;
  else
    sql = new std::ofstream(out);

  return 0;
}


int main(int argc, char* argv[])
{
  std::istream* inp;
  std::ostream* out;

  GNU_gama::local::LocalNetwork lnet;

  try
    {
      if (const int k = parameters(argc, argv, inp, out)) return k;

      GNU_gama::local::LocalNetwork2sql ln2sql(lnet);
      ln2sql.readGkf(*inp);
      ln2sql.write  (*out, argv[1]);
      out->flush(); 
   }
  catch (GNU_gama::Exception::parser perr)
    {
      std::cerr << "parser error : " << perr.error_code 
                << "  line : "       << perr.line
                << "  text : "       << perr.str
                << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << "unknown exception\n";
    }
}



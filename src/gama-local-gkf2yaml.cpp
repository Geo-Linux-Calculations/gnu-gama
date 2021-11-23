/* Gkf2yaml --- conversion from yaml to gkf input format
   Copyright (C) 2020 Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   Class Gkf2yaml is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Gkf2yaml is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Gama.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <memory>
#include <fstream>
#include <string>
#include <regex>
#include <gnu_gama/local/network.h>
#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/local/xmlerror.h>
#include <gnu_gama/local/gkf2yaml.h>

using namespace std;
using namespace GNU_gama::local;


int main(int argc, char* argv[])
{
  if (argc != 2 && argc != 3)
  {
    cout << "\ngama-local-gkf2yaml input.gkf [output.yaml]\n\n";
    return 1;
  }

  LocalNetwork  locnet;
  GKFparser     gkf(locnet);
  GNU_gama::local::XMLerror xmlerr;

  try
  {
       string line;
       ifstream ifstr(argv[1]);

        while(getline(ifstr, line))
        {
            line.push_back('\n');
            gkf.xml_parse(line.c_str(), line.length(), 0);
        }
        gkf.xml_parse("", 0, 1);

   }
   catch (const GNU_gama::local::ParserException& v)
   {
     if (xmlerr.isValid()) {}
     {
       xmlerr.setDescription(T_GaMa_exception_2a);
       std::string t, s = v.what();
       for (std::string::iterator i=s.begin(), e=s.end(); i!=e; ++i)
       {
         if      (*i == '<') t += "\"";
         else if (*i == '>') t += "\"";
         else                t += *i;
       }
       xmlerr.setDescription(t);
       xmlerr.setLineNumber (v.line);
       xmlerr.setDescription(T_GaMa_exception_2b);
       return xmlerr.write_xml("gamaLocalParserError");
      }
    }


  std::unique_ptr<Gkf2yaml> gkf2yaml;
  std::ofstream  outf;

  if (argc == 3)
  {
    outf.open(argv[2]);
    gkf2yaml.reset( new Gkf2yaml(gkf, outf) );
  }
  else
  {
    gkf2yaml.reset( new Gkf2yaml(gkf, std::cout) );
  }


  gkf2yaml->run();

  return 0;
}

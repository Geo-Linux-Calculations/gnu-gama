/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004, 2018  Ales Cepek <cepek@fsv.cvut.cz>

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
    along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/version.h>

namespace
{
  const char* arg_input     = nullptr;
  const char* arg_output    = nullptr;
  const char* arg_algorithm = nullptr;
  const char* arg_projeq    = nullptr;

  GNU_gama::Adj::algorithm algorithm;

  int error(const char* s) { std::cerr << s << "\n"; return 1; }

  int help(int argc, char* argv[])
  {
    const std::string usage =
      "\n"
      "Usage:  gama-g3  [ options ] input  [ output ] \n\n"

      " input      xml data file name\n"
      " output     optional output data file name\n\n"

      " --algorithm  envelope | gso | svd | cholesky\n"

      " --project-equations file"
      "     optional output of project equations in XML\n"

      "\n"
      " -h         help (this text)\n"

      "\n";


    if (argc <= 1)   // no arguments
      {
        std::cerr << usage;
        return 1;
      }

    bool ok = true;

    for (int n=0, i=1; ok && i<argc; i++)
      {
        // '-arg' is equivalent to '--arg' in gama-g3
        const std::string a =
          (*argv[i] == '-' && *(argv[i]+1) == '-') ? argv[i]+1 : argv[i];


        if (a == "-h" || a == "-help")
          {
            std::cerr << usage;
            return 1;
          }
        if (a == "-version")
          {
            GNU_gama::version("gama-g3", "Ales Cepek");
            return 1;
          }
        if (a == "-algorithm")
          {
            if (++i < argc)
              arg_algorithm = argv[i];
            else
              ok = false;

            const std::string arg = arg_algorithm;
            if      (arg == "envelope") algorithm = GNU_gama::Adj::envelope;
            else if (arg == "gso"     ) algorithm = GNU_gama::Adj::gso;
            else if (arg == "svd"     ) algorithm = GNU_gama::Adj::svd;
            else if (arg == "cholesky") algorithm = GNU_gama::Adj::cholesky;
            else
              ok = false;

            continue;
          }
        if (a == "-project-equations")
          {
            if (++i < argc)
              arg_projeq = argv[i];
            else
              ok = false;

            continue;
          }

        ++n;
        if      (n == 1) arg_input  = argv[i];
        else if (n == 2) arg_output = argv[i];
        else
          {
            ok = false;
          }
      }

    if (ok) return 0;

    std::cerr << "\nWrong argument(s)\n";
    std::cerr << usage;
    return 1;
  }

  GNU_gama::g3::Model* get_xml_input(const char* file)
  {
    using namespace GNU_gama::g3;

    std::ifstream input(file);
    if (!input) return 0;

    std::list<GNU_gama::DataObject::Base*> objects;
    GNU_gama::DataParser parser(objects);

    try
      {
        std::string text;
        while (std::getline(input, text))
          {
            parser.xml_parse(text.c_str(), text.length(), 0);
            parser.xml_parse("\n", 1, 0);
          }
        parser.xml_parse("", 0, 1);
      }
    catch(const GNU_gama::Exception::parser& p)
      {
        std::cerr << "\nXML parser error on line " << p.line
                  << " of input data  "
                  << "\t(error code " << p.error_code << ")\n"
                  << p.str << "\n\n";
        return 0;
      }
    catch(...)
      {
        error("catch ... ");
        return 0;
      }

    GNU_gama::g3::Model* model = 0;
    std::list<GNU_gama::DataObject::Base*>::iterator i = objects.begin();
    std::list<GNU_gama::DataObject::Base*>::iterator e = objects.end();
    while (i != e)
      {
         if (GNU_gama::DataObject::g3_model* m
            = dynamic_cast<GNU_gama::DataObject::g3_model*>(*i))
          {
            if (model) delete model;   // this should never happen
            model =  m->model;
          }

         // std::cerr << (*i)->xml() << std::endl;
         delete *i;
         ++i;
      }

    return model;
  }
}

int main_g3()
{
  using namespace std;
  using namespace GNU_gama::g3;

  Model* model = get_xml_input(arg_input);
  if (model == 0) return error("error on reading XML input data");

  if (arg_algorithm) model->set_algorithm(algorithm);

  model->update_linearization();

  if (arg_projeq)
    {
      std::ofstream out(arg_projeq);
      out.precision(16);
      out << GNU_gama::DataObject::Base::xml_begin();
      model->write_xml_adjustment_input_data(out);
      out << GNU_gama::DataObject::Base::xml_end();
    }

  model->update_adjustment();

  if (arg_output)
    {
      ofstream file(arg_output);
      if (file)
        model->write_xml_adjustment_results(file);
      else
        std::cerr << "\n****** error on opening file " << arg_output << "\n\n";
    }
  else
    {
      model->write_xml_adjustment_results(std::cout);
    }

  delete model;
  return 0;
}

// ----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  using namespace GNU_gama::g3;

  if (help(argc, argv)) return 1;

  try
    {
      return main_g3();
    }
  catch (GNU_gama::Exception::matvec m)
    {
      std::cerr <<  "\n### gama-g3 : "
                << m.description << " (" << m.error << ")\n";
    }
  catch (GNU_gama::Exception::string s)
    {
      std::cerr << "\n### gama-g3 : " << s.str << "\n";
    }
  catch (...)
    {
      std::cerr << "\n### gama-g3 : unknown exception\n";
    }

  return 1;
}

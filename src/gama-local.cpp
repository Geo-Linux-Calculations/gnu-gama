/* GNU Gama C++ library
   Copyright (C) 1999, 2002, 2003, 2010, 2011, 2012, 2014, 2018, 2020, 2021
                 Ales Cepek <cepek@gnu.org>

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

#ifdef   GNU_GAMA_LOCAL_SQLITE_READER
#include <gnu_gama/local/sqlitereader.h>
#endif

#include <gnu_gama/outstream.h>

#include <cstring>
#include <gnu_gama/version.h>
#include <gnu_gama/intfloat.h>
#include <gnu_gama/xml/localnetworkoctave.h>
#include <gnu_gama/xml/localnetworkxml.h>
#include <gnu_gama/xml/gkfparser.h>

#include <gnu_gama/local/language.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/acord/acordstatistics.h>
#include <gnu_gama/local/svg.h>
#include <gnu_gama/local/html.h>

#include <gnu_gama/local/results/text/approximate_coordinates.h>
#include <gnu_gama/local/results/text/reduced_observations.h>
#include <gnu_gama/local/results/text/network_description.h>
#include <gnu_gama/local/results/text/general_parameters.h>
#include <gnu_gama/local/results/text/fixed_points.h>
#include <gnu_gama/local/results/text/adjusted_observations.h>
#include <gnu_gama/local/results/text/adjusted_unknowns.h>
#include <gnu_gama/local/results/text/outlying_abs_terms.h>
#include <gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h>
#include <gnu_gama/local/results/text/residuals_observations.h>
#include <gnu_gama/local/results/text/error_ellipses.h>
#include <gnu_gama/local/test_linearization_visitor.h>
#include <gnu_gama/local/xmlerror.h>

namespace {
int help()
{
  using namespace std;
  using namespace GNU_gama::local;

  cout << "\n"
       << "Adjustment of local geodetic network"
       << "        version: "<< GNU_gama::GNU_gama_version()
       << " / " << GNU_gama::GNU_gama_compiler() << "\n"
       << "************************************\n"
       << "https://www.gnu.org/software/gama/\n\n";

  cout <<
    "Usage: gama-local  [--input-xml] input.xml  [options]\n"

#ifdef   GNU_GAMA_LOCAL_SQLITE_READER
    "       gama-local  [--input-xml] input.xml  --sqlitedb sqlite.db"
            "  --configuration name  [options]\n"
    "       gama-local  --sqlitedb sqlite.db  --configuration name  [options]\n"
    "       gama-local  --sqlitedb sqlite.db"
            "  --readonly-configuration name  [options]\n"
#endif

    "\nOptions:\n\n"

    "--algorithm  gso | svd | cholesky | envelope\n"
    "--language   en | ca | cz | du | es | fi | fr | hu | ru | ua | zh\n"
    "--encoding   utf-8 | iso-8859-2 | iso-8859-2-flat | cp-1250 | cp-1251\n"
    "--angular    400 | 360\n"
    "--latitude   <latitude>\n"
    "--ellipsoid  <ellipsoid name>\n"
    "--text       adjustment_results.txt\n"
    "--html       adjustment_results.html\n"
    "--xml        adjustment_results.xml\n"
    "--octave     adjustment_results.m\n"
    "--svg        network_configuration.svg\n"
    "--cov-band   covariance matrix of adjusted parameters in XML output\n"
    "             n  = -1  for full covariance matrix (implicit value)\n"
    "             n >=  0  covariances are computed only for bandwidth n\n"
    "--iterations maximum number of iterations allowed in the linearized\n"
    "             least squares algorithm (implicit value is 5)\n"
    "--export     updated input data based on adjustment results\n"
    "--verbose    [yes | no]\n"
    "--version\n"
    "--help\n\n";

//  "--obs        observation_equations.txt (obsolete format)\n"

  cout <<
    "Report bugs to: <bug-gama@gnu.org>\n"
    "GNU gama home page: <https://www.gnu.org/software/gama/>\n"
    "General help using GNU software: <https://www.gnu.org/gethelp/>\n\n";

  return 0;
}
}  // unnamed namespace


GNU_gama::local::XMLerror xmlerr;

int main(int argc, char **argv)
  try {

    if (argc == 1) return help();

    using namespace std;
    using namespace GNU_gama::local;

    const char* c;
    const char* argv_1 = nullptr;           // xml input or sqlite db name
    const char* argv_input_xml = nullptr;
    const char* argv_algo = nullptr;
    const char* argv_lang = nullptr;
    const char* argv_enc  = nullptr;
    const char* argv_angular = nullptr;
    const char* argv_ellipsoid = nullptr;
    const char* argv_latitude = nullptr;
    const char* argv_txtout = nullptr;
    const char* argv_htmlout = nullptr;
    const char* argv_xmlout = nullptr;
    const char* argv_octaveout = nullptr;
    const char* argv_svgout = nullptr;
    const char* argv_obsout = nullptr;
    const char* argv_covband = nullptr;
    const char* argv_iterations = nullptr;
    const char* argv_export_xml = nullptr;
    bool verbose_output { false };

    // handle --help and --version as special cases
    if (argc == 2 && strcmp(argv[1], "-") && strlen(argv[1]) > 2)
      {
        c = argv[1];
        if(*c == '-') c++;
        if(*c == '-') c++;

        if (!strcmp(c, "help"))    return help();
        if (!strcmp(c, "version")) return
            GNU_gama::version("gama-local", "Ales Cepek et al.");
      }

  int indopt = 1;

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    const char* argv_confname = nullptr;
    const char* argv_readonly = nullptr;
    const char* argv_sqlitedb = nullptr;

    if (argc >= 4)
    {
        // --sqlitedb
        const char* db = argv[1];
        if (*db == '-') db++;
        if (*db == '-') db++;
        if (!strcmp(db, "sqlitedb"))
          {
            argv_sqlitedb = argv[2];

            // --configuration or --readonly-configuration
            const char* conf = argv[3];
            if (*conf == '-') conf++;
            if (*conf == '-') conf++;
            if (!strcmp(conf, "configuration")) argv_confname = conf;
            if (!strcmp(conf, "readonly-configuration")) argv_readonly = conf;

            if (argv_confname || argv_readonly) indopt = 5;
          }
     }
#endif


    for (int i=indopt; i<argc; i++)
      {
        if (argv[i] == nullptr) break;  // this should never happen

        c = argv[i];
        if (argv_1 == nullptr && !strcmp(argv[i], "-"))
          {
            argv_1 = c;   // hyphen (-) can be used to read gkf input from std::cin
            continue;
          }

        if (*c != '-')    // **** one or two parameters (file names) ****
          {
            if (!argv_1) {
              argv_1 = c;
              continue;
            }
            return help();
          }

        // ****  options  ****

        if(*c == '-') c++;
        if(*c == '-') c++;
        const char* name = c;        // option
        c = argv[++i];               // value

        if      (!strcmp("input-xml",   name)) argv_input_xml = c;
        else if (!strcmp("algorithm",   name)) argv_algo = c;
        else if (!strcmp("language",    name)) argv_lang = c;
        else if (!strcmp("encoding",    name)) argv_enc  = c;
        else if (!strcmp("angular",     name)) argv_angular = c;
        else if (!strcmp("ellipsoid",   name)) argv_ellipsoid = c;
        else if (!strcmp("latitude",    name)) argv_latitude = c;
        else if (!strcmp("text",        name)) argv_txtout = c;
        else if (!strcmp("html",        name)) argv_htmlout = c;
        else if (!strcmp("xml",         name)) argv_xmlout = c;
        else if (!strcmp("octave",      name)) argv_octaveout = c;
        else if (!strcmp("svg",         name)) argv_svgout = c;
        else if (!strcmp("obs",         name)) argv_obsout = c;
        else if (!strcmp("cov-band",    name)) argv_covband = c;
        else if (!strcmp("iterations",  name)) argv_iterations = c;
        else if (!strcmp("export",      name)) argv_export_xml = c;
        else if (!strcmp("verbose",     name))
          {
            std::string argverb(c ? c : "");
            if (argverb == "yes" || argverb == "no"  )
              {
                verbose_output = (argverb == "yes");
              }
            else
              { // implicit argument, --verbose without a value
                verbose_output = true;
                --i;
              }
          }
        else
          return help();
      }

    // implicit output
    if (!argv_txtout && !argv_htmlout && !argv_xmlout) argv_xmlout = "-";

    if (argv_xmlout) xmlerr.setXmlOutput(argv_xmlout);

    if (argv_input_xml)
      {
        if (argv_1) return help(); // input already defined

        argv_1 = argv_input_xml;
      }

    if (!argv_lang)
      set_gama_language(en);
    else
      {
        if      (!strcmp("en", argv_lang)) set_gama_language(en);
        else if (!strcmp("ca", argv_lang)) set_gama_language(ca);
        else if (!strcmp("cs", argv_lang)) set_gama_language(cz);
        else if (!strcmp("cz", argv_lang)) set_gama_language(cz);
        else if (!strcmp("du", argv_lang)) set_gama_language(du);
        else if (!strcmp("du", argv_lang)) set_gama_language(du);
        else if (!strcmp("es", argv_lang)) set_gama_language(es);
        else if (!strcmp("fr", argv_lang)) set_gama_language(fr);
        else if (!strcmp("fi", argv_lang)) set_gama_language(fi);
        else if (!strcmp("hu", argv_lang)) set_gama_language(hu);
        else if (!strcmp("ru", argv_lang)) set_gama_language(ru);
        else if (!strcmp("ua", argv_lang)) set_gama_language(ua);
        else if (!strcmp("zh", argv_lang)) set_gama_language(zh);
        else return help();
      }

    ostream* output = nullptr;

    ofstream fcout;
    if (argv_txtout)
      {
        if (!strcmp(argv_txtout, "-"))
          {
            output = &std::cout;
          }
        else
          {
            fcout.open(argv_txtout);
            output = &fcout;
          }
      }

    GNU_gama::OutStream cout(output);

    if (argv_enc)
      {
        using namespace GNU_gama;

        if (!strcmp("utf-8", argv_enc))
          cout.set_encoding(OutStream::utf_8);
        else if (!strcmp("iso-8859-2", argv_enc))
          cout.set_encoding(OutStream::iso_8859_2);
        else if (!strcmp("iso-8859-2-flat", argv_enc))
          cout.set_encoding(OutStream::iso_8859_2_flat);
        else if (!strcmp("cp-1250", argv_enc))
          cout.set_encoding(OutStream::cp_1250);
        else if (!strcmp("cp-1251", argv_enc))
          cout.set_encoding(OutStream::cp_1251);
        else
          return help();
      }

    if (argv_algo)
      {
        const std::string algorithm = argv_algo;
        if (algorithm != "gso"      &&
            algorithm != "svd"      &&
            algorithm != "cholesky" &&
            algorithm != "envelope" ) return help();
      }

    LocalNetwork* IS = new LocalNetwork;
    if (verbose_output) IS->set_verbose();

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    if (argv_sqlitedb)
      {
        GNU_gama::local::sqlite_db::SqliteReader reader(argv_sqlitedb);

        const char* conf = argv_confname;
        if (argv_readonly) conf = argv_readonly;
        reader.retrieve(IS, conf);
      }
    else
#endif
      {
        std::shared_ptr<std::istream>  inxml {};

        if (argv_1 == std::string("-"))
            inxml.reset(&std::cin, [](std::istream*){}); // is the deleter necessary?
        else
            inxml.reset( new std::ifstream(argv_1) );

        GKFparser gkf(*IS);
        try
          {
            char c;
            int  n, finish = 0;
            string  line;
            do
              {
                line.clear();
                n = 0;
                while (inxml->get(c))
                  {
                    line += c;
                    n++;
                    if (c == '\n') break;
                  }

                if (inxml->eof() || !inxml->good()) finish = 1;

                gkf.xml_parse(line.c_str(), n, finish);
              }
            while (!finish);
          }
        catch (const GNU_gama::local::ParserException& v) {
          if (xmlerr.isValid())
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
              //xmlerr.setDescription(T_GaMa_exception_2b);
              return xmlerr.write_xml("gamaLocalParserError");
            }

          cerr << "\n" << T_GaMa_exception_2a << "\n\n"
               << T_GaMa_exception_2b << v.line << " : " << v.what() << endl;
          return 3;
        }
        catch (const GNU_gama::local::Exception& v) {
          if (xmlerr.isValid())
            {
              xmlerr.setDescription(T_GaMa_exception_2a);
              xmlerr.setDescription(v.what());
              return xmlerr.write_xml("gamaLocalException");
            }

          cerr << "\n" <<T_GaMa_exception_2a << "\n"
               << "\n***** " << v.what() << "\n\n";
          return 2;
        }
        catch (...)
          {
            cerr << "\n" << T_GaMa_exception_2a << "\n\n";
            throw;
          }
      }

    if (argv_algo)
      {
        IS->set_algorithm(argv_algo);
      }

    if (!IS->has_algorithm()) IS->set_algorithm();

    if (argv_angular)
      {
        if (!strcmp("400", argv_angular))
          IS->set_gons();
        else if (!strcmp("360", argv_angular))
          IS->set_degrees();
        else
          return help();
      }

    if (argv_covband)
      {
        std::istringstream istr(argv_covband);
        int band = -1;
        if (!(istr >> band) || band < -1) return help();
        char c;
        if (istr >> c) return help();

        IS->set_adj_covband(band);
      }

    if (argv_iterations)
      {
        std::istringstream istr(argv_iterations);
        int iter = IS->max_linearization_iterations();
        if (!(istr >> iter) || iter < 0) return help();
        char c;
        if (istr >> c) return help();

        IS->set_max_linearization_iterations(iter);
      }

    if (argv_latitude)
      {
        double latitude;
        if (!GNU_gama::deg2gon(argv_latitude, latitude))
          {
            if (!GNU_gama::IsFloat(string(argv_latitude)))
              return help();

            latitude = atof(argv_latitude);
          }

        IS->set_latitude(latitude * M_PI/200);
      }


    if (argv_ellipsoid)
      {
        using namespace GNU_gama;

        gama_ellipsoid gama_el = ellipsoid(argv_ellipsoid);
        if  (gama_el == ellipsoid_unknown) return help();

        IS->set_ellipsoid(argv_ellipsoid);
      }


    {
      cout << T_GaMa_Adjustment_of_geodetic_network << "        "
           << T_GaMa_version << GNU_gama::GNU_gama_version()
           << "-" << IS->algorithm()
           << " / " << GNU_gama::GNU_gama_compiler() << "\n"
           << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
           << "http://www.gnu.org/software/gama/\n\n";
    }

    if (IS->PD.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_points_available);

    if (IS->OD.clusters.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_observations_available);

    try
      {
        // if (!GaMaConsistent(IS->PD))
        //   {
        //      cout << T_GaMa_inconsistent_coordinates_and_angles << "\n\n";
        //   }
        IS->remove_inconsistency();

        AcordStatistics stats(IS->PD, IS->OD);

        /* Acord2 class for computing approximate values of unknown paramaters
         * (needed for linearization of project equations) superseded previous
         * class Acord.
         *
         * Acord2 apart from other algorithms relies on a class
         * ApproximateCoordinates (used also in Acord) for solving unknown xy
         * by intersections and local transformations. Original author
         * of ApproximateCoordinates is Jiri Vesely.
         *
         * Class Acord2 was introduced for better handling of traverses.
         */

        Acord2 acord2(IS->PD, IS->OD);
        acord2.execute();
        refine_obsdh_reductions(IS);

        if (IS->correction_to_ellipsoid())
          {
            using namespace GNU_gama;
            gama_ellipsoid elnum = ellipsoid(IS->ellipsoid().c_str());
            Ellipsoid el {};
            GNU_gama::set(&el, elnum);
            ReduceToEllipsoid reduce_to_el(IS->PD, IS->OD, el, IS->latitude());
            reduce_to_el.execute();
            ReducedObservationsToEllipsoidText(IS, reduce_to_el.getMap(), cout);
          }

        stats.execute();
        ApproximateCoordinates(&stats, cout);
      }
    catch(GNU_gama::local::Exception& e)
      {
        if (xmlerr.isValid())
          {
            xmlerr.setDescription(e.what());
            return xmlerr.write_xml("gamaLocalException");
          }

        cerr << e.what() << endl;
        return 1;
      }
    catch(...)
      {
        if (xmlerr.isValid())
          {
            xmlerr.setDescription("Gama / Acord: "
                                  "approximate coordinates failed");
            return xmlerr.write_xml("gamaLocalApproximateCoordinates");
          }

        cerr << "Gama / Acord: approximate coordinates failed\n\n";
        return 1;
      }


    if (IS->points_count() == 0 || IS->unknowns_count() == 0)
      {
        throw GNU_gama::local::Exception(T_GaMa_No_network_points_defined);
      }

    //else ... do not use else after throw
      {
        if (IS->huge_abs_terms())
          {
            OutlyingAbsoluteTerms(IS, cout);
            IS->remove_huge_abs_terms();
            cout << T_GaMa_Observatios_with_outlying_absolute_terms_removed
                 << "\n\n";
          }

        if (!IS->connected_network())
          cout  << T_GaMa_network_not_connected << "\n\n";

        bool network_can_be_adjusted {false};
        {
          std::ostringstream tmp_out;
          network_can_be_adjusted = GeneralParameters(IS, tmp_out);
          if (!network_can_be_adjusted)
            {
              NetworkDescription(IS->description, cout);
              cout << tmp_out.str();

              // delete IS;
              return 1;
            }
        }

        if (network_can_be_adjusted)
          {
            // update dh reductions and approximate coordinates if needed
            bool refined = IS->refine_adjustment();

            if (refined)
              {
                cout << T_GaMa_Approximate_coordinates_replaced << "\n"
                     << underline(T_GaMa_Approximate_coordinates_replaced,'*')
                     << "\n\n"
                     << T_GaMa_Number_of_linearization_iterations
                     << IS->linearization_iterations() << "\n\n";
              }

            if (!TestLinearization(IS, cout)) cout << "\n";

            if (IS->verbose()) ReducedObservations  (IS, cout);
            NetworkDescription   (IS->description, cout);
            GeneralParameters    (IS, cout);
            FixedPoints          (IS, cout);
            AdjustedUnknowns     (IS, cout);
            ErrorEllipses        (IS, cout);
            AdjustedObservations (IS, cout);
            ResidualsObservations(IS, cout);
          }

        if (argv_svgout)
          {
            GamaLocalSVG svg(IS);
            if (!strcmp(argv_svgout, "-"))
              {
                svg.draw(std::cout);
              }
            else
              {
                ofstream file(argv_svgout);
                svg.draw(file);
              }
          }

        if (argv_obsout)
          {
            ofstream opr(argv_obsout);
            IS->project_equations(opr);
          }

        if (argv_htmlout)
          {
            GNU_gama::local::GamaLocalHTML html(IS);
            html.exec();

            if (!strcmp(argv_htmlout, "-"))
              {
                html.html(std::cout);
              }
            else
              {
                ofstream file(argv_htmlout);
                html.html(file);
              }
          }

        if (argv_xmlout)
          {
            IS->set_gons();

            GNU_gama::LocalNetworkXML xml(IS);

            if (!strcmp(argv_xmlout, "-"))
              {
                xml.write(std::cout);
              }
            else
              {
                ofstream file(argv_xmlout);
                xml.write(file);
              }
          }

        if (argv_octaveout)
          {
            IS->set_gons();

            GNU_gama::LocalNetworkOctave octave(IS);

            if (!strcmp(argv_octaveout, "-"))
              {
                octave.write(std::cout);
              }
            else
              {
                ofstream file(argv_octaveout);
                octave.write(file);
              }
          }

        if (network_can_be_adjusted && argv_export_xml)
          {
            std::string ver = "<!-- created by gama-local "
                + GNU_gama::GNU_gama_version() + " -->\n";
            std::string xml = IS->export_xml(ver);

            if (!strcmp(argv_export_xml, "-"))
              {
                std::cout << xml;
              }
            else
              {
                ofstream file(argv_export_xml);
                file << xml;
              }
          }
      }

    delete IS;
    return 0;

  }
#ifdef GNU_GAMA_LOCAL_SQLITE_READER
  catch(const GNU_gama::Exception::sqlitexc& gamalite)
    {
      if (xmlerr.isValid())
        {
          xmlerr.setDescription(gamalite.what());
          return xmlerr.write_xml("gamaLocalSqlite");
        }

      std::cout << "\n" << "****** " << gamalite.what() << "\n\n";
      return 1;
    }
#endif
  catch(const GNU_gama::Exception::adjustment& choldec)
    {
      using namespace GNU_gama::local;

      if (xmlerr.isValid())
        {
          xmlerr.setDescription(T_GaMa_solution_ended_with_error);
          xmlerr.setDescription(choldec.str);
          return xmlerr.write_xml("gamaLocalAdjustment");
        }

      std::cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
                << "****** " << choldec.str << "\n\n";
      return 1;
    }
  catch (const GNU_gama::local::Exception& V)
    {
      using namespace GNU_gama::local;

      if (xmlerr.isValid())
        {
          xmlerr.setDescription(T_GaMa_solution_ended_with_error);
          xmlerr.setDescription(V.what());
          return xmlerr.write_xml("gamaLocalException");
        }

      std::cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
                << "****** " << V.what() << "\n\n";
      return 1;
    }
  catch (std::exception& stde) {
    if (xmlerr.isValid())
      {
        xmlerr.setDescription(stde.what());
        return xmlerr.write_xml("gamaLocalStdException");
      }

    std::cout << "\n" << stde.what() << "\n\n";
  }
  catch(...) {
    using namespace GNU_gama::local;

    if (xmlerr.isValid())
      {
        return xmlerr.write_xml("gamaLocalUnknownException");
      }

    std::cout << "\n" << T_GaMa_internal_program_error << "\n\n";
    return 1;
  }

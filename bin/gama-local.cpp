/* GNU Gama C++ library
   Copyright (C) 1999, 2002, 2003, 2010, 2011, 2012, 2014
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
#include <gnu_gama/xml/localnetworkxml.h>
#include <gnu_gama/xml/gkfparser.h>

#include <gnu_gama/local/language.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/acord.h>
#include <gnu_gama/local/svg.h>
#include <gnu_gama/local/html.h>

#include <gnu_gama/local/results/text/approximate_coordinates.h>
#include <gnu_gama/local/results/text/network_description.h>
#include <gnu_gama/local/results/text/general_parameters.h>
#include <gnu_gama/local/results/text/fixed_points.h>
#include <gnu_gama/local/results/text/adjusted_observations.h>
#include <gnu_gama/local/results/text/adjusted_unknowns.h>
#include <gnu_gama/local/results/text/outlying_abs_terms.h>
#include <gnu_gama/local/results/text/reduced_observations.h>
#include <gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h>
#include <gnu_gama/local/results/text/residuals_observations.h>
#include <gnu_gama/local/results/text/error_ellipses.h>
#include <gnu_gama/local/results/text/test_linearization.h>
#include <gnu_gama/local/xmlerror.h>

namespace {
int help()
{
  using namespace std;
  using namespace GNU_gama::local;

  cerr << "\n"
       << "Adjustment of local geodetic network"
       << "        version: "<< GNU_gama::GNU_gama_version
       << " / " << GNU_gama::GNU_gama_compiler << "\n"
       << "************************************\n"
       << "http://www.gnu.org/software/gama/\n\n";

#ifndef   GNU_GAMA_LOCAL_SQLITE_READER
  cerr <<
    "Usage: gama-local  input.xml  [options]\n\n"

    "Options:\n\n"

    "--algorithm  gso | svd | cholesky | envelope\n"
    "--language   en | ca | cz | du | es | fi | fr | hu | ru | ua | zh\n"
    "--encoding   utf-8 | iso-8859-2 | iso-8859-2-flat | cp-1250 | cp-1251\n"
    "--angles     400 | 360\n"
    "--latitude   <latitude>\n"
    "--ellipsoid  <ellipsoid name>\n"
    "--text       adjustment_results.txt\n"
    "--html       adjustment_results.html\n"
    "--xml        adjustment_results.xml\n"
    "--svg        network_configuration.svg\n"
    "--cov-band   covariance matrix of adjusted parameters in XML output\n"
    "             n  = -1  for full covariance matrix (implicit value)\n"
    "             n >=  0  covariances are computed only for bandwidth n\n"
    "--version\n"
    "--help\n\n";

  //"--obs        observation_equations.txt (obsolete format)\n"
#else
  cerr <<
    "Usage: gama-local  input.xml  [options]\n"
    "       gama-local  input.xml  --sqlitedb sqlite.db"
            "  --configuration name  [options]\n"
    "       gama-local  --sqlitedb sqlite.db  --configuration name  [options]\n"
    "       gama-local  --sqlitedb sqlite.db"
            "  --readonly-configuration name  [options]\n"
    "\n"

    "Options:\n\n"

    "--algorithm  gso | svd | cholesky | envelope\n"
    "--language   en | ca | cz | du | es | fi | fr | hu | ru | ua | zh\n"
    "--encoding   utf-8 | iso-8859-2 | iso-8859-2-flat | cp-1250 | cp-1251\n"
    "--angles     400 | 360\n"
    "--latitude   <latitude>\n"
    "--ellipsoid  <ellipsoid name>\n"
    "--text       adjustment_results.txt\n"
    "--html       adjustment_results.html\n"
    "--xml        adjustment_results.xml\n"
    "--svg        network_configuration.svg\n"
    "--cov-band   covariance matrix of adjusted parameters in XML output\n"
    "             n  = -1  for full covariance matrix (implicit value)\n"
    "             n >=  0  covariances are computed only for bandwidth n\n"
    "--version\n"
    "--help\n\n";
#endif

  return 1;
}

GNU_gama::local::XMLerror xmlerr;

}  // unnamed namespace



int main(int argc, char **argv)
  try {

    if (argc == 1) return help();

    using namespace std;
    using namespace GNU_gama::local;

    const char* c;
    const char* argv_1 = 0;           // xml input or sqlite db name
    const char* argv_algo = 0;
    const char* argv_lang = 0;
    const char* argv_enc  = 0;
    const char* argv_angles = 0;
    const char* argv_ellipsoid = 0;
    const char* argv_latitude = 0;
    const char* argv_txtout = 0;
    const char* argv_htmlout = 0;
    const char* argv_xmlout = 0;
    const char* argv_svgout = 0;
    const char* argv_obsout = 0;
    const char* argv_covband = 0;

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    const char* argv_confname = 0;
    const char* argv_readonly = 0;
    const char* argv_sqlitedb = 0;
#endif


    for (int i=1; i<argc; i++)
      {
        c = argv[i];
        if (*c != '-')    // **** one or two parameters (file names) ****
          {
            if (!argv_1) {
              argv_1 = c;
              continue;
            }
            return help();
          }

        // ****  options  ****

        if(*c && *c == '-') c++;
        if(*c && *c == '-') c++;
        string name = string(c);
        c = argv[++i];

        if      (name == "help"      ) return help();
        else if (name == "version"   )
                        return GNU_gama::version("gama-local", "Ales Cepek");
        else if ( i   ==  argc       ) return help();
        else if (name == "algorithm" ) argv_algo = c;
        else if (name == "language"  ) argv_lang = c;
        else if (name == "encoding"  ) argv_enc  = c;
        else if (name == "angles"    ) argv_angles = c;
        else if (name == "ellipsoid" ) argv_ellipsoid = c;
        else if (name == "latitude"  ) argv_latitude = c;
        else if (name == "text"      ) argv_txtout = c;
        else if (name == "html"      ) argv_htmlout = c;
        else if (name == "xml"       ) argv_xmlout = c;
        else if (name == "svg"       ) argv_svgout = c;
        else if (name == "obs"       ) argv_obsout = c;
        else if (name == "cov-band"  ) argv_covband = c;
#ifdef GNU_GAMA_LOCAL_SQLITE_READER
        else if (name == "sqlitedb")               argv_sqlitedb = c;
        else if (name == "configuration")          argv_confname = c;
        else if (name == "readonly-configuration") argv_readonly = c;
#endif
        else
          return help();
      }

    // implicit output
    if (!argv_txtout && !argv_htmlout && !argv_xmlout) argv_xmlout = "-";

    if (argv_xmlout) xmlerr.setXmlOutput(argv_xmlout);

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    if (argv_confname && argv_readonly) return help();
    if (!argv_1 && !argv_sqlitedb) return help();
#endif

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

    ostream* output = 0;

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
        ifstream soubor(argv_1);
        GKFparser gkf(*IS);
        try
          {
            char c;
            int  n, konec = 0;
            string radek;
            do
              {
                radek = "";
                n     = 0;
                while (soubor.get(c))
                  {
                    radek += c;
                    n++;
                    if (c == '\n') break;
                  }
                if (!soubor) konec = 1;

                gkf.xml_parse(radek.c_str(), n, konec);
              }
            while (!konec);
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

    if (argv_angles)
      {
        if (!strcmp("400", argv_angles))
          IS->set_gons();
        else if (!strcmp("360", argv_angles))
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

        IS->set_xml_covband(band);
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
           << T_GaMa_version << GNU_gama::GNU_gama_version
           << "-" << IS->algorithm()
           << " / " << GNU_gama::GNU_gama_compiler << "\n"
           << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
           << "http://www.gnu.org/software/gama/\n\n\n";
    }

    if (IS->PD.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_points_available);

    if (IS->OD.clusters.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_observations_available);

    try
      {
        if (!GaMaConsistent(IS->PD))
          {
            // cout << T_GaMa_inconsistent_coordinates_and_angles << "\n\n\n";
          }
        Acord acord(IS->PD, IS->OD);
        acord.execute();
        ReducedObservationsText(IS,&(acord.RO), cout);

        if (IS->correction_to_ellipsoid())
          {
            using namespace GNU_gama;
            gama_ellipsoid elnum = ellipsoid(IS->ellipsoid().c_str());
            Ellipsoid el;
            GNU_gama::set(&el, elnum);
            ReduceToEllipsoid reduce_to_el(IS->PD, IS->OD, el, IS->latitude());
            reduce_to_el.execute();
            ReducedObservationsToEllipsoidText(IS, reduce_to_el.getMap(), cout);
          }

        ApproximateCoordinates(&acord, cout);

      }
    catch(GNU_gama::local::Exception e)
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


    if (IS->sum_points() == 0 || IS->sum_unknowns() == 0)
      {
        throw GNU_gama::local::Exception(T_GaMa_No_network_points_defined);
      }
    else
      {
        if (IS->huge_abs_terms())
          {
            OutlyingAbsoluteTerms(IS, cout);
            IS->remove_huge_abs_terms();
            cout << T_GaMa_Observatios_with_outlying_absolute_terms_removed
                 << "\n\n\n";
          }

        if (!IS->connected_network())
          cout  << T_GaMa_network_not_connected << "\n\n\n";

        bool network_can_be_adjusted;
        {
          std::ostringstream tmp_out;
          if (!(network_can_be_adjusted = GeneralParameters(IS, tmp_out)))
            {
              NetworkDescription(IS->description, cout);
              cout << tmp_out.str();
            }
        }

        if (network_can_be_adjusted)
          {
            int iteration = 0;
            do
              {
                if(++iteration > 1)
                  {
                    // removed in version 1.13
                    // cout << "\n         ******  "
                    //     << iteration << T_GaMa_adjustment_iteration
                    //     << "  ******\n\n"
                    //     << T_GaMa_Approximate_coordinates_replaced << "\n"
                    //     << underline(T_GaMa_Approximate_coordinates_replaced,
                    //                  '*') << "\n\n\n";

                    IS->refine_approx();
                    // GeneralParameters(IS, cout);
                  }
                // FixedPoints     (IS, cout);
                // AdjustedUnknowns(IS, cout);
              }
            while (TestLinearization(IS) && iteration < 5);

            // from version 1.13 running results are not printed
            if (iteration > 1)
              {
                cout << T_GaMa_Approximate_coordinates_replaced << "\n"
                     << underline(T_GaMa_Approximate_coordinates_replaced,'*')
                     << "\n\n"
                     << T_GaMa_Number_of_linearization_iterations
                     << iteration << "\n\n";

                if (!TestLinearization(IS, cout)) cout << "\n";
              }

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

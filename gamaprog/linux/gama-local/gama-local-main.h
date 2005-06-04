/*  
    GNU Gama C++ library
    Copyright (C) 1999, 2002, 2003  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.
    
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
 *  $Id: gama-local-main.h,v 1.26 2005/06/04 21:06:43 cepek Exp $
 */

#ifndef GAMA_MAIN__gama_main__gm_mn__g_m__g______________________________h___
#define GAMA_MAIN__gama_main__gm_mn__g_m__g______________________________h___

#include <gnu_gama/outstream.h>

#include <cstring>
#include <gnu_gama/version.h>
#include <gnu_gama/intfloat.h>
#include <gamalib/language.h>
#include <gamalib/local/gamadata.h>
#include <gamalib/xml/gkfparser.h>
#include <gamalib/local/network_svd.h>
#include <gamalib/local/network_gso.h>
#include <gamalib/local/network_chol.h>
#include <gamalib/local/acord.h>

#include <gamalib/xml/adjusted/coordinates.h>

/* functions for printing of formatted output text were moved to new
 * location. it is planned for future versions to produce adjustment
 * results in XML (functions in directory local/results/xml) and
 * current text functions shell be rewritten to process the XML data
 * as their input */

#include <gamalib/local/results/text/approximate_coordinates.h>
#include <gamalib/local/results/text/network_description.h>
#include <gamalib/local/results/text/general_parameters.h>
#include <gamalib/local/results/text/fixed_points.h>
#include <gamalib/local/results/text/adjusted_observations.h>
#include <gamalib/local/results/text/adjusted_unknowns.h>
#include <gamalib/local/results/text/outlying_abs_terms.h>
#include <gamalib/local/results/text/reduced_observations.h>
#include <gamalib/local/results/text/reduced_observations_to_ellipsoid.h>
#include <gamalib/local/results/text/residuals_observations.h>
#include <gamalib/local/results/text/error_ellipses.h>
#include <gamalib/local/results/text/test_linearization.h>


//---------------------------------------------------------------------------

int help()
{
  using namespace std;
  using namespace GaMaLib;
  
  cerr << "\n" 
       << "Adjustment of local geodetic network"
       << "        version: "<< GNU_gama::GNU_gama_version
       << " / " << GNU_gama::GNU_gama_compiler << "\n"
       << "************************************\n"
       << "http://www.gnu.org/software/gama/\n\n"
       << "Usage: " << /*argv[0]*/"gama-local" 
       << " [options]"
       << " xml_input_file.gkf "
       << " [ output. ]\n\n";
  cerr << "Options:\n"
       << "\n";
  cerr << "--algorithm  svd | gso | cholesky\n" 
       << "--language   en | ca | cz | du | fi | hu\n"
       << "--encoding   utf-8 | iso-8859-2 | iso-8859-2-flat | cp-1250\n"
       << "--angles     400 | 360\n"  
       << "--latitude   <latitude>\n"
       << "--ellipsoid  <ellipsoid name>\n"
       << "--version\n"
       << "--help\n";
  cerr << endl;
  return 1;
}


int version()
{
  using namespace std;
  using namespace GaMaLib;
  
  cout << GNU_gama::GNU_gama_version << endl;

  return 0;
}


int GaMa_Main(int argc, char **argv)
{
  using namespace std;
  using namespace GaMaLib;
  
  string description, xml_output, file_stx, file_txt, file_opr;
  const char* c;
  const char* argv_1 = 0;
  const char* argv_2 = 0;
  const char* argv_algo = 0;
  const char* argv_lang = 0;
  const char* argv_enc  = 0;
  const char* argv_angles = 0;
  const char* argv_ellipsoid = 0;
  const char* argv_latitude = 0;

  bool correction_to_ellipsoid = false;
  GNU_gama::Ellipsoid el;
  double latitude = M_PI/4.0;
  
  for (int i=1; i<argc; i++)
    {
      c = argv[i];
      if (*c != '-')    // **** one or two parameters (file names) ****
        { 
          if (!argv_1) {
              argv_1 = c;
              continue;
            }
          if (!argv_2) {
              argv_2 = c;
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
      else if (name == "version"   ) return version();
      else if ( i   ==  argc       ) return help();
      else if (name == "algorithm" ) argv_algo = c; 
      else if (name == "language"  ) argv_lang = c;
      else if (name == "encoding"  ) argv_enc  = c;
      else if (name == "angles"    ) argv_angles = c;
      else if (name == "ellipsoid" ) argv_ellipsoid = c;
      else if (name == "latitude"  ) argv_latitude = c;
      else
          return help();
    }
  
  if (!argv_1) return help();
  if (!argv_lang)
    set_gama_language(en);
  else
    {
      if (!strcmp("en", argv_lang)) set_gama_language(en);
      else if (!strcmp("ca", argv_lang)) set_gama_language(ca);
      else if (!strcmp("cs", argv_lang)) set_gama_language(cz);
      else if (!strcmp("cz", argv_lang)) set_gama_language(cz);
      else if (!strcmp("du", argv_lang)) set_gama_language(du);
      else if (!strcmp("fi", argv_lang)) set_gama_language(fi);
      else if (!strcmp("hu", argv_lang)) set_gama_language(hu);
      else return help();
    }

  LocalNetwork* IS;
  ofstream fcout;
  GNU_gama::OutStream cout(fcout);
  
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
      else
        return help();
    }

  
  try {
    
    try {
      
      if (!argv_algo) 
        {
          IS = new LocalNetwork_svd;        // implicit algorithm
        }
      else {
        if (     !strcmp("svd",      argv_algo)) IS = new LocalNetwork_svd;
        else if (!strcmp("gso",      argv_algo)) IS = new LocalNetwork_gso;
        else if (!strcmp("cholesky", argv_algo)) IS = new LocalNetwork_chol;
        else return help();
      }

      if (argv_angles)
        {
          if (!strcmp("400", argv_angles))
            IS->set_gons();
          else if (!strcmp("360", argv_angles))
            IS->set_degrees();
          else
            return help();
        }

    }
    catch (...) {
      throw 
        GaMaLib::Exception(T_GaMa_exception_1);
    }
    

    if (argv_latitude)
    {
        if ( !GNU_gama::IsFloat(string(argv_latitude)) )
            return help();

        latitude = atof(argv_latitude) * M_PI / (IS->gons() ? 200 : 180);

	correction_to_ellipsoid = true;
    }

    GNU_gama::set(&el, GNU_gama::ellipsoid_wgs84);

    if (argv_ellipsoid)
    {
        using namespace GNU_gama;
        
        gama_ellipsoid gama_el = ellipsoid(argv_ellipsoid);
        if  ( (gama_el == ellipsoid_unknown) || GNU_gama::set(&el,  gama_el) )
            return help();
	    
	correction_to_ellipsoid = true;
    }

    
    ifstream soubor(argv_1);
    
    //# removed in version 1.3.24
    //# #########################
    //#
    //# /* GKFparser does not allow anything before tag <?xml version="1.0" ?> 
    //#  * neither after the end of XML document.  In email version of
    //#  * gama it is useful to skip the email header and automatically
    //#  * added signatures etc and I have added the following loop in
    //#  * which they are "cut off". Thus GaMa would accept even formally
    //#  * invalid XML documents, ie gkf files.
    //#  */
    //# 
    //# char c;
    //# while ((soubor >> c) && (c != '<' || soubor.peek() != '?'))
    //#   ;
    //# soubor.putback(c);
    
    {
      GKFparser gkf(IS->PD, IS->OD);
      try 
        {
          //# // when </gama-xml> is found, anything that follows is ignored
          //# int i, k, n;  
          //# /* 
          //#  * ??? while (konec == 0 && getline(soubor, radek)) ???
          //#  *
          //#  * input is processed by lines (to find the closing tag) but
          //#  * function getline is not well implemented in bcc32 (it
          //#  * returns '\0' byte as an active character in radek); thus
          //#  * it is read here unefectively by characters.
          //#  */
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
              
              //# for (i=10; i<n; i++)
              //#   {
              //#     k = i;
              //#     if (radek[k--] != '>') continue;
              //#     if (radek[k--] != 'l') continue;
              //#     if (radek[k--] != 'm') continue;
              //#     if (radek[k--] != 'x') continue;
              //#     if (radek[k--] != '-') continue;
              //#     if (radek[k--] != 'a') continue;
              //#     if (radek[k--] != 'm') continue;
              //#     if (radek[k--] != 'a') continue;
              //#     if (radek[k--] != 'g') continue;
              //#     if (radek[k--] != '/') continue;
              //#     if (radek[k--] != '<') continue;
              //#     n = i+1;
              //#     konec = 1;
              //#     break;
              //#   }
              
              gkf.xml_parse(radek.c_str(), n, konec);
            }
          while (!konec);
          
          IS->apriori_m_0(gkf.m0_apr );
          IS->conf_pr    (gkf.konf_pr);
          IS->tol_abs    (gkf.tol_abs);

          // added in 1.7.09
          IS->update_constrained_coordinates(gkf.update_constr);
 
          if (gkf.typ_m0_apriorni)
            IS->set_m_0_apriori();
          else
            IS->set_m_0_aposteriori();
          
          description = gkf.description;
        }
      catch (...) 
        {
          throw;         // should be added later ???
        }
    }
    
    if (argv_2)         // to be used in email version 
      {
        string jmeno(argv_2);
        
        if (*jmeno.rbegin() == '.')
          {
            file_txt = jmeno + "txt";
            file_stx = jmeno + "stx";
            file_opr = jmeno + "opr";
            
            xml_output = jmeno + "xml";
          }
        else
          {
            file_txt = jmeno;
            file_stx = "";
            file_opr = "";
          }
      }
    
    if (file_txt == "")  file_txt = string(argv_1) 
                          + "-" + string(GNU_gama::GNU_gama_version)
                          + "-" + string(IS->algorithm());

    fcout.open(file_txt.c_str());
    
  }
  catch (const GaMaLib::ParserException& v) {
    cerr << "\n" << T_GaMa_exception_2a << "\n\n"
         << T_GaMa_exception_2b << v.line << " : " << v.text << endl;
    return 3;      
  }
  catch (const GaMaLib::Exception& v) {
    cerr << "\n" <<T_GaMa_exception_2a << "\n"
         << "\n***** " << v.text << "\n\n";
    return 2;
  }
  catch (...) 
    {
      cerr << "\n" << T_GaMa_exception_2a << "\n\n";
      throw;
      // return 3;
    }
  
  
  try {
    
    {
      cout << T_GaMa_Adjustment_of_geodetic_network << "        "
           << T_GaMa_version << GNU_gama::GNU_gama_version 
           << "-" << IS->algorithm()
           << " / " << GNU_gama::GNU_gama_compiler << "\n"
           << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
           << "http://www.gnu.org/software/gama/\n\n\n";
    }

    if (IS->PD.empty())
      throw GaMaLib::Exception(T_GaMa_No_points_available);
    
    if (IS->OD.clusters.empty())
      throw GaMaLib::Exception(T_GaMa_No_observations_available);
    
    try
      {
        if (!Consistent(IS->PD))
          {
            cout << T_GaMa_inconsistent_coordinates_and_angles << "\n\n\n";
          }
        Acord acord(IS->PD, IS->OD);
        acord.execute();
	ReducedObservationsText(IS,&(acord.RO), cout);

        if (correction_to_ellipsoid)
        {
            ReduceToEllipsoid reduce_to_el(IS->PD, IS->OD, el, latitude);
            reduce_to_el.execute();
            ReducedObservationsToEllipsoidText(IS, reduce_to_el.getMap(), cout);
        }

        ApproximateCoordinates(&acord, cout);

      }
    catch(...)
      {
        cerr << "Gama / Acord: approximate coordinates failed\n\n";
        return 1;
      }
    
    // cerr << IS->PD << IS->OD << "\n\n";
    
    if (IS->sum_points() == 0 || IS->sum_unknowns() == 0)
      {
        throw GaMaLib::Exception(T_GaMa_No_network_points_defined);
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

        NetworkDescription(description, cout);
        if (GeneralParameters(IS, cout))
          {
            int iterace = 0;
            do 
              {
                if(++iterace > 1)
                  {
                    cout << "\n         ******  " 
                         << iterace << T_GaMa_adjustment_iteration 
                         << "  ******\n\n"
                         << T_GaMa_Approximate_coordinates_replaced << "\n"
                         << underline(T_GaMa_Approximate_coordinates_replaced,
                                      '*') << "\n\n\n";
                    
                    IS->refine_approx();
                    GeneralParameters(IS, cout);
                  }
                FixedPoints     (IS, cout);
                AdjustedUnknowns(IS, cout);
              } 
            while (TestLinearization(IS, cout) && iterace < 3);
            
            ErrorEllipses        (IS, cout);
            AdjustedObservations (IS, cout);
            ResidualsObservations(IS, cout);

          }
        
        if (file_opr != "")
          {
            ofstream opr(file_opr.c_str());
            IS->project_equations(opr);
          }
        
        if (xml_output != "")
          {
            ofstream xml(xml_output.c_str());
            
            XML_adjusted_coordinates(IS, xml, true);
          }
                
      }
    
    delete IS; 
    return 0;   
    
  }
  catch (const GaMaLib::Exception& V) 
    {
      cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
           << "****** " << V.text << "\n\n";
    }
  catch(const GNU_gama::Exception::adjustment& choldec)
    {
      cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
           << "****** " << choldec.str << "\n\n";
    }
  catch(...)
    {
      cout << "\n" << T_GaMa_internal_program_error << "\n\n";
    }
  
  return 1;
}

//---------------------------------------------------------------------------

#endif








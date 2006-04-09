/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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
 *  $Id: capi_gkfparser.h,v 1.1 2006/04/09 16:40:24 cepek Exp $
 */

#ifndef GNU__GaMa__C_API_Cgama_gkf_parser_handling_functions_header_file____
#define GNU__GaMa__C_API_Cgama_gkf_parser_handling_functions_header_file____

#ifdef __cplusplus
extern "C" {
#endif

  void*  Cgama_GKF_parser(void* local_network);
  void   Cgama_GKF_parser_dtor (void*);
  void   Cgama_GKF_parser_parse(void*, const char* text, int len, int isfinal);
  
  double Cgama_GKF_parser_apriori_m0(void*);
  double Cgama_GKF_parser_conf_pr(void*);
  double Cgama_GKF_parser_tol_abs(void*);
  int    Cgama_GKF_parser_m0_apriori(void*);
  char*  Cgama_GKF_parser_description(void*);

#ifdef __cplusplus
}
#endif

#endif





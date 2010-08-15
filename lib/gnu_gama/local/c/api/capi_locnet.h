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

#ifndef GNU__GaMa__C_API_Cgama_local_network_handling_functions_header_file____
#define GNU__GaMa__C_API_Cgama_local_network_handling_functions_header_file____

#ifdef __cplusplus
extern "C" {
#endif

  /* constructors */

  void* Cgama_LocalNetwork_svd();
  void* Cgama_LocalNetwork_gso();

  /* virtual destructor */

  void  Cgama_LocalNetwork_dtor(void*);

  /* public member functions */

  const char* Cgama_LocalNetwork_algorithm(void*);
  void Cgama_LocalNetwork_set_apriori_m0(void*, double);
  void Cgama_LocalNetwork_set_conf_pr   (void*, double);
  void Cgama_LocalNetwork_set_tol_abs   (void*, double);
  void Cgama_LocalNetwork_set_type_refsd(void*, int);
  int  Cgama_LocalNetwork_PointData_empty(void*);
  int  Cgama_LocalNetwork_ObservationData_empty(void*);
  int  Cgama_LocalNetwork_sum_points(void*);
  int  Cgama_LocalNetwork_sum_unknowns(void*);
  int  Cgama_LocalNetwork_huge_abs_terms(void*);
  void Cgama_LocalNetwork_remove_huge_abs_terms(void*);
  void Cgama_LocalNetwork_refine_approx(void*);

#ifdef __cplusplus
}
#endif

#endif



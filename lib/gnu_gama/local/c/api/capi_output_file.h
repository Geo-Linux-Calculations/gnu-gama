/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local__C_API_internal_output_file_class____capi_output_file__H__
#define gama_local__C_API_internal_output_file_class____capi_output_file__H__


#ifdef __cplusplus

#include <fstream>
#include <gnu_gama/local/local/network.h>

namespace GNU_gama { namespace local {

  class C_API_output_file {
  public:

    C_API_output_file(LocalNetwork* is, std::ofstream* outp)
      : IS(is), out(outp)
      {
      }
    ~C_API_output_file()
      {
        delete out;
      }

    LocalNetwork*  IS;
    std::ofstream* out;

  };

}}

#endif


#ifdef __cplusplus
extern "C" {
#endif

  /* C API output file constructor */
  void* Cgama_output_file(void* local_network, const char* file_name);
  /* C API output file destructor */
  void  Cgama_output_file_close(void* object);

  const char* Cgama_gnu_gama_local_version();

  /* formatted output */

  void Cgama_of_string(void*, const char* string);

  void Cgama_of_adjusted_observations(void*);
  void Cgama_of_adjusted_unknowns(void*);
  void Cgama_of_approximate_coordinates(void*);
  void Cgama_of_error_ellipses(void*);
  void Cgama_of_fixed_points(void*);
  int  Cgama_of_general_parameters(void*);
  void Cgama_of_network_description(void*, char*);
  void Cgama_of_outlying_abs_terms(void*);
  void Cgama_of_residuals_observations(void*);
  int  Cgama_of_test_linearization(void*);


#ifdef __cplusplus
}
#endif


#endif

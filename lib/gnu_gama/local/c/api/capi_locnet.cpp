/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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

#include <gnu_gama/local/c/api/capi_locnet.h>
#include <gnu_gama/local/c/api/capi_private_exception.h>
#include <gnu_gama/local/local/network.h>
#include <gnu_gama/local/local/network_svd.h>
#include <gnu_gama/local/local/network_gso.h>

using namespace GaMaLib;

extern "C" {

  // LocalNetwork constructors

  void* Cgama_LocalNetwork_svd()
  {
    try
      {
        return new LocalNetwork_svd;
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 0;
  }
  void* Cgama_LocalNetwork_gso()
  {
    try
      {
        return new LocalNetwork_gso;
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 0;
  }

  // virtual destructor

  void  Cgama_LocalNetwork_dtor(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        delete locnet;
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }

  // member functions

  const char* Cgama_LocalNetwork_algorithm(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->algorithm();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }

    return "";
  }
  void Cgama_LocalNetwork_set_apriori_m0(void* ptr, double p)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        locnet->apriori_m_0(p);
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }
  void Cgama_LocalNetwork_set_conf_pr   (void* ptr, double p) 
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        locnet->conf_pr(p);
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }
  void Cgama_LocalNetwork_set_tol_abs   (void* ptr, double p) 
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        locnet->tol_abs(p);
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }
  void Cgama_LocalNetwork_set_type_refsd(void* ptr, int p)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        if(p)
          locnet->set_m_0_apriori();
        else
          locnet->set_m_0_aposteriori();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }
  int Cgama_LocalNetwork_PointData_empty(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->PD.empty();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 1;
  }
  int Cgama_LocalNetwork_ObservationData_empty(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->OD.clusters.empty();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 1;
  }
  int Cgama_LocalNetwork_sum_points(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->sum_points();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 0;
  }
  int Cgama_LocalNetwork_sum_unknowns(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->sum_unknowns();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 0;
  }
  int Cgama_LocalNetwork_huge_abs_terms(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        return locnet->huge_abs_terms();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
    return 0;
  }
  void Cgama_LocalNetwork_remove_huge_abs_terms(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        locnet->remove_huge_abs_terms();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }

  void Cgama_LocalNetwork_refine_approx(void* ptr)
  {
    try
      {
        LocalNetwork* locnet = static_cast<LocalNetwork*>(ptr);
        locnet->refine_approx();
      }
    catch (const GaMaLib::Exception& e)
      {
        Cgama_private_set_exception(e);
      }
    catch(...)
      {
        Cgama_private_set_unknown_exception();
      }
  }

}












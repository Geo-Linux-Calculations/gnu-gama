/* GNU Gama -- testing adjustment results from different algorithms
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

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

#ifndef CHECK_XYZ_H_Check_Xyz_H_check_xyz_h
#define CHECK_XYZ_H_Check_Xyz_H_check_xyz_h

#include <gnu_gama/local/network.h>

enum {alg_svd, alg_gso, alg_chol, alg_env};

double                     xyzMaxDiff(GNU_gama::local::LocalNetwork* lnet1, 
				      GNU_gama::local::LocalNetwork* lnet2);
GNU_gama::local::LocalNetwork* getNet(int alg, const char* file);

#endif

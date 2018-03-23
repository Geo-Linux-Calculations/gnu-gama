/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Petr Doubrava <petr@gepro.cz>

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

#include <string.h>
#include <gnu_gama/xml/encoding.h>

#ifdef __cplusplus
namespace GNU_gama {
#endif


int UnknownEncodingHandler(void *userData, const char *name,XML_Encoding *info)
{
 if      (!strcmp(name,"cp-1250"))
   cp1250_unicode(info->map);
 else if (!strcmp(name,"windows-1250"))
   cp1250_unicode(info->map);             /* this is probably correct */
 else if (!strcmp(name,"cp-1251"))
   cp1251_unicode(info->map);
 else if (!strcmp(name,"windows-1251"))
   cp1251_unicode(info->map);             /* this is probably correct */
 else if (!strcmp(name,"iso-8859-2"))
   iso_8859_2_unicode(info->map);
 else
   ascii(info->map);
 return 1;
}


#ifdef __cplusplus
}   //  namespace GNU_gama
#endif










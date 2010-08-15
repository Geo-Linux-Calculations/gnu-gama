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

int cp1250_unicode(int* tab){
   int cp1250_unicode[]={
         0x00000080,   /* 0x80 */
         0x00000081,   /* 0x81  ? */
         0x0000201A,   /* 0x82 */
         0x00000083,   /* 0x83  ? */
         0x0000201E,   /* 0x84 */
         0x00002026,   /* 0x85 */
         0x00002020,   /* 0x86 */
         0x00002021,   /* 0x87 */
         0x00000088,   /* 0x88 */
         0x00002030,   /* 0x89 */
         0x00000160,   /* 0x8A */
         0x00002039,   /* 0x8B */
         0x0000015A,   /* 0x8C */
         0x00000164,   /* 0x8D */
         0x0000017D,   /* 0x8E */
         0x00000179,   /* 0x8F */
         0x00000090,   /* 0x90  ? */
         0x00002018,   /* 0x91 */
         0x00002019,   /* 0x92 */
         0x0000201C,   /* 0x93 */
         0x0000201D,   /* 0x94 */
         0x00002022,   /* 0x95 */
         0x00002013,   /* 0x96 */
         0x00002014,   /* 0x97 */
         0x00000098,   /* 0x98  ? */
         0x00002122,   /* 0x99 */
         0x00000161,   /* 0x9A */
         0x0000203A,   /* 0x9B */
         0x0000015B,   /* 0x9C */
         0x00000165,   /* 0x9D */
         0x0000017E,   /* 0x9E */
         0x0000017A,   /* 0x9F */
         0x000000A0,   /* 0xA0 */
         0x000002C7,   /* 0xA1 */
         0x000002D8,   /* 0xA2 */
         0x00000141,   /* 0xA3 */
         0x000000A4,   /* 0xA4 */
         0x00000104,   /* 0xA5 */
         0x000000A6,   /* 0xA6 */
         0x000000A7,   /* 0xA7 */
         0x000000A8,   /* 0xA8 */
         0x000000A9,   /* 0xA9 */
         0x0000015E,   /* 0xAA */
         0x000000AB,   /* 0xAB */
         0x000000AC,   /* 0xAC */
         0x000000AD,   /* 0xAD */
         0x000000AE,   /* 0xAE */
         0x0000017B,   /* 0xAF */
         0x000000B0,   /* 0xB0 */
         0x000000B1,   /* 0xB1 */
         0x000002DB,   /* 0xB2 */
         0x00000142,   /* 0xB3 */
         0x000000B4,   /* 0xB4 */
         0x000000B5,   /* 0xB5 */
         0x000000B6,   /* 0xB6 */
         0x000000B7,   /* 0xB7 */
         0x000000B8,   /* 0xB8 */
         0x00000105,   /* 0xB9 */
         0x0000015F,   /* 0xBA */
         0x000000BB,   /* 0xBB */
         0x0000013D,   /* 0xBC */
         0x000002DD,   /* 0xBD */
         0x0000013E,   /* 0xBE */
         0x0000017C,   /* 0xBF */
         0x00000154,   /* 0xC0 */
         0x000000C1,   /* 0xC1 */
         0x000000C2,   /* 0xC2 */
         0x00000102,   /* 0xC3 */
         0x000000C4,   /* 0xC4 */
         0x00000139,   /* 0xC5 */
         0x00000106,   /* 0xC6 */
         0x000000C7,   /* 0xC7 */
         0x0000010C,   /* 0xC8 */
         0x000000C9,   /* 0xC9 */
         0x00000118,   /* 0xCA */
         0x000000CB,   /* 0xCB */
         0x0000011A,   /* 0xCC */
         0x000000CD,   /* 0xCD */
         0x000000CE,   /* 0xCE */
         0x0000010E,   /* 0xCF */
         0x00000110,   /* 0xD0 */
         0x00000143,   /* 0xD1 */
         0x00000147,   /* 0xD2 */
         0x000000D3,   /* 0xD3 */
         0x000000D4,   /* 0xD4 */
         0x00000150,   /* 0xD5 */
         0x000000D6,   /* 0xD6 */
         0x000000D7,   /* 0xD7 */
         0x00000158,   /* 0xD8 */
         0x0000016E,   /* 0xD9 */
         0x000000DA,   /* 0xDA */
         0x00000170,   /* 0xDB */
         0x000000DC,   /* 0xDC */
         0x000000DD,   /* 0xDD */
         0x00000162,   /* 0xDE */
         0x000000DF,   /* 0xDF */
         0x00000155,   /* 0xE0 */
         0x000000E1,   /* 0xE1 */
         0x000000E2,   /* 0xE2 */
         0x00000103,   /* 0xE3 */
         0x000000E4,   /* 0xE4 */
         0x0000013A,   /* 0xE5 */
         0x00000107,   /* 0xE6 */
         0x000000E7,   /* 0xE7 */
         0x0000010D,   /* 0xE8 */
         0x000000E9,   /* 0xE9 */
         0x00000119,   /* 0xEA */
         0x000000EB,   /* 0xEB */
         0x0000011B,   /* 0xEC */
         0x000000ED,   /* 0xED */
         0x000000EE,   /* 0xEE */
         0x0000010F,   /* 0xEF */
         0x00000111,   /* 0xF0 */
         0x00000144,   /* 0xF1 */
         0x00000148,   /* 0xF2 */
         0x000000F3,   /* 0xF3 */
         0x000000F4,   /* 0xF4 */
         0x00000151,   /* 0xF5 */
         0x000000F6,   /* 0xF6 */
         0x000000F7,   /* 0xF7 */
         0x00000159,   /* 0xF8 */
         0x0000016F,   /* 0xF9 */
         0x000000FA,   /* 0xFA */
         0x00000171,   /* 0xFB */
         0x000000FC,   /* 0xFC */
         0x000000FD,   /* 0xFD */
         0x00000163,   /* 0xFE */
         0x000002D9,   /* 0xFF   */
   };
   unsigned int i,j;
   for (i=0;i<0x00000080;i++)tab[i]=(int)i;
   for (j=0;i<256;i++) tab[i]=cp1250_unicode[j++];
   return 1;
}

int iso_8859_2_unicode(int* tab){
   int iso_8859_2_unicode[]={
         0x00000104,   /* 0xA1 */
         0x000002D8,   /* 0xA2 */
         0x00000141,   /* 0xA3 */
         0x000000A4,   /* 0xA4 */
         0x0000013D,   /* 0xA5 */
         0x0000015A,   /* 0xA6 */
         0x000000A7,   /* 0xA7 */
         0x000000A8,   /* 0xA8 */
         0x00000160,   /* 0xA9 */
         0x0000015E,   /* 0xAA */
         0x00000164,   /* 0xAB */
         0x00000179,   /* 0xAC */
         0x000000AD,   /* 0xAD */
         0x0000017D,   /* 0xAE */
         0x0000017B,   /* 0xAF */
         0x000000B0,   /* 0xB0 */
         0x00000105,   /* 0xB1 */
         0x000002DB,   /* 0xB2 */
         0x00000142,   /* 0xB3 */
         0x000000B4,   /* 0xB4 */
         0x0000013E,   /* 0xB5 */
         0x0000015B,   /* 0xB6 */
         0x000002C7,   /* 0xB7 */
         0x000000B8,   /* 0xB8 */
         0x00000161,   /* 0xB9 */
         0x0000015F,   /* 0xBA */
         0x00000165,   /* 0xBB */
         0x0000017A,   /* 0xBC */
         0x000002DD,   /* 0xBD */
         0x0000017E,   /* 0xBE */
         0x0000017C,   /* 0xBF */
         0x00000154,   /* 0xC0 */
         0x000000C1,   /* 0xC1 */
         0x000000C2,   /* 0xC2 */
         0x00000102,   /* 0xC3 */
         0x000000C4,   /* 0xC4 */
         0x00000139,   /* 0xC5 */
         0x00000106,   /* 0xC6 */
         0x000000C7,   /* 0xC7 */
         0x0000010C,   /* 0xC8 */
         0x000000C9,   /* 0xC9 */
         0x00000118,   /* 0xCA */
         0x000000CB,   /* 0xCB */
         0x0000011A,   /* 0xCC */
         0x000000CD,   /* 0xCD */
         0x000000CE,   /* 0xCE */
         0x0000010E,   /* 0xCF */
         0x00000110,   /* 0xD0 */
         0x00000143,   /* 0xD1 */
         0x00000147,   /* 0xD2 */
         0x000000D3,   /* 0xD3 */
         0x000000D4,   /* 0xD4 */
         0x00000150,   /* 0xD5 */
         0x000000D6,   /* 0xD6 */
         0x000000D7,   /* 0xD7 */
         0x00000158,   /* 0xD8 */
         0x0000016E,   /* 0xD9 */
         0x000000DA,   /* 0xDA */
         0x00000170,   /* 0xDB */
         0x000000DC,   /* 0xDC */
         0x000000DD,   /* 0xDD */
         0x00000162,   /* 0xDE */
         0x000000DF,   /* 0xDF */
         0x00000155,   /* 0xE0 */
         0x000000E1,   /* 0xE1 */
         0x000000E2,   /* 0xE2 */
         0x00000103,   /* 0xE3 */
         0x000000E4,   /* 0xE4 */
         0x0000013A,   /* 0xE5 */
         0x00000107,   /* 0xE6 */
         0x000000E7,   /* 0xE7 */
         0x0000010D,   /* 0xE8 */
         0x000000E9,   /* 0xE9 */
         0x00000119,   /* 0xEA */
         0x000000EB,   /* 0xEB */
         0x0000011B,   /* 0xEC */
         0x000000ED,   /* 0xED */
         0x000000EE,   /* 0xEE */
         0x0000010F,   /* 0xEF */
         0x00000111,   /* 0xF0 */
         0x00000144,   /* 0xF1 */
         0x00000148,   /* 0xF2 */
         0x000000F3,   /* 0xF3 */
         0x000000F4,   /* 0xF4 */
         0x00000151,   /* 0xF5 */
         0x000000F6,   /* 0xF6 */
         0x000000F7,   /* 0xF7 */
         0x00000159,   /* 0xF8 */
         0x0000016F,   /* 0xF9 */
         0x000000FA,   /* 0xFA */
         0x00000171,   /* 0xFB */
         0x000000FC,   /* 0xFC */
         0x000000FD,   /* 0xFD */
         0x00000163,   /* 0xFE */
         0x000002D9,   /* 0xFF */
   };
   unsigned int i,j;
   for (i=0;i<0x000000A1;i++)tab[i]=(int)i;
   for (j=0;i<256;i++) tab[i]=iso_8859_2_unicode[j++];
   return 1;
}

int ascii(int* tab){
   for (int i=0;i<256;i++)tab[i]=i;
   return 1;
}

int Utf8Decode(int& u, unsigned char *buf){
 unsigned char c=*buf;
 if (c<0x80){u=c;return 1;}
 int i=0;
 while (c&0x80){
  i++;
  c<<=1;
 }
 if (i==2){
  u=((*buf)&0x3f)<<6;
  u+=(*(buf+1))&0x7f;
  return 2;
 }
 if (i==3){
  u=((*buf)&0x1f)<<12;
  u+=((*(buf+1))&0x7f)<<6;
  u+=(*(buf+2))&0x7f;
  return 3;
 }
 u=c;      /* leaving it here and skip */
 return 1;
}

char* utf8_cp1250(char *buf){
  static int tab[256];
  // static int itab=cp1250_unicode((int*)tab);  ... rewritten to avoid warning
  static bool init_tab = true;
  if (init_tab)
    {
      cp1250_unicode((int*)tab);
      init_tab = false;
    }
  unsigned int u;
  char *p,*q;
  p=q=buf;
  while (*p){
          p+=Utf8Decode((int&)u,(unsigned char*)p);
          if (u>0x80){
             int i;
             for (i=0x80;i<256;i++)
                 if (tab[i]==(int)u){*q=i;break;}
             if (i==256)*q=u;
          }else *q=u;
          q++;
  }
  *q=0;
  return buf;
}

char* utf8_iso_8859_2(char *buf){
  static int tab[256];
  // static int itab=iso_8859_2_unicode((int*)tab);  ... avoid warning
  static bool init_tab = true;
  if (init_tab)
    {
      iso_8859_2_unicode((int*)tab);
      init_tab = false;
    }
  unsigned int u;
  char *p,*q;
  p=q=buf;
  while (*p){
          p+=Utf8Decode((int&)u,(unsigned char*)p);
          if (u>0x80){
             int i;
             for (i=0x80;i<256;i++)
                 if (tab[i]==(int)u){*q=i;break;}
             if (i==256)*q=u;
          }else *q=u;
          q++;
  }
  *q=0;
  return buf;
}

char* utf8_ascii(char *buf){
  unsigned int u;
  char *p,*q;
  p=q=buf;
  while (*p){
          p+=Utf8Decode((int&)u,(unsigned char*)p);
          *q++=u;
  }
  *q=0;
  return buf;
}


// moved to file encoding_unknown_handler.cpp
//
// int UnknownEncodingHandler(void *userData, const char *name,XML_Encoding *info)
// {
//  if (!strcmp(name,"cp-1250")) cp1250_unicode(info->map);
//  else if (!strcmp(name,"windows-1250")) cp1250_unicode(info->map); /* this is probably correct */
//  else if (!strcmp(name,"iso-8859-2")) iso_8859_2_unicode(info->map);
//  else ascii(info->map);
//  return 1;
// }

#ifdef __cplusplus
}   //  namespace GNU_gama
#endif










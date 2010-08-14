/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003, 2006  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/outstream.h>

namespace {

  unsigned char ascii_table[256] = {0};

  void init_ascii(unsigned char t[])
  {
    for (int i=0; i<256; i++) t[i] = i;

    /*
     * man iso-8859-2:
     * ===============
     *
     * ISO 8859-2 Characters
     *     The  following  table  displays the characters in ISO 8859
     *     Latin-2, which are printable and unlisted in the  ascii(7)
     *     manual page.
     *
     *
     * Dec             Oct  Hex Char     Description
     * ------------------------------------------------------------------ */

    t[160]=' ';     // 240  A0     NO-BREAK SPACE
    t[161]='A';     // 241  A1  ¡  LATIN CAPITAL LETTER A WITH OGONEK
    t[162]=' ';     // 242  A2  ¢  BREVE
    t[163]='L';     // 243  A3  £  LATIN CAPITAL LETTER L WITH STROKE
    t[164]='$';     // 244  A4  ¤  CURRENCY SIGN
    t[165]='L';     // 245  A5  ¥  LATIN CAPITAL LETTER L WITH CARON
    t[166]='S';     // 246  A6  ¦  LATIN CAPITAL LETTER S WITH ACUTE
    t[167]=' ';     // 247  A7  §  SECTION SIGN
    t[168]=' ';     // 250  A8  ¨  DIAERESIS
    t[169]='S';     // 251  A9  ©  LATIN CAPITAL LETTER S WITH CARON
    t[170]='S';     // 252  AA  ª  LATIN CAPITAL LETTER S WITH CEDILLA
    t[171]='T';     // 253  AB  «  LATIN CAPITAL LETTER T WITH CARON
    t[172]='Z';     // 254  AC  ¬  LATIN CAPITAL LETTER Z WITH ACUTE
    t[173]=' ';     // 255  AD  ­  SOFT HYPHEN
    t[174]='Z';     // 256  AE  ®  LATIN CAPITAL LETTER Z WITH CARON
    t[175]='Z';     // 257  AF  ¯  LATIN CAPITAL LETTER Z WITH DOT ABOVE
    t[176]=' ';     // 260  B0  °  DEGREE SIGN
    t[177]='z';     // 261  B1  ±  LATIN SMALL LETTER A WITH OGONEK
    t[178]=' ';     // 262  B2  ²  OGONEK
    t[179]='l';     // 263  B3  ³  LATIN SMALL LETTER L WITH STROKE
    t[180]=' ';     // 264  B4  ´  ACUTE ACCENT
    t[181]='l';     // 265  B5  µ  LATIN SMALL LETTER L WITH CARON
    t[182]='s';     // 266  B6  ¶  LATIN SMALL LETTER S WITH ACUTE
    t[183]=' ';     // 267  B7  ·  CARON
    t[184]=' ';     // 270  B8  ¸  CEDILLA
    t[185]='s';     // 271  B9  ¹  LATIN SMALL LETTER S WITH CARON
    t[186]='s';     // 272  BA  º  LATIN SMALL LETTER S WITH CEDILLA
    t[187]='t';     // 273  BB  »  LATIN SMALL LETTER T WITH CARON
    t[188]='z';     // 274  BC  ¼  LATIN SMALL LETTER Z WITH ACUTE
    t[189]=' ';     // 275  BD  ½  DOUBLE ACUTE ACCENT
    t[190]='z';     // 276  BE  ¾  LATIN SMALL LETTER Z WITH CARON
    t[191]='z';     // 277  BF  ¿  LATIN SMALL LETTER Z WITH DOT ABOVE
    t[192]='R';     // 300  C0  À  LATIN CAPITAL LETTER R WITH ACUTE
    t[193]='A';     // 301  C1  Á  LATIN CAPITAL LETTER A WITH ACUTE
    t[194]='A';     // 302  C2  Â  LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    t[195]='A';     // 303  C3  Ã  LATIN CAPITAL LETTER A WITH BREVE
    t[196]='A';     // 304  C4  Ä  LATIN CAPITAL LETTER A WITH DIAERESIS
    t[197]='L';     // 305  C5  Å  LATIN CAPITAL LETTER L WITH ACUTE
    t[198]='C';     // 306  C6  Æ  LATIN CAPITAL LETTER C WITH ACUTE
    t[199]='C';     // 307  C7  Ç  LATIN CAPITAL LETTER C WITH CEDILLA
    t[200]='C';     // 310  C8  È  LATIN CAPITAL LETTER C WITH CARON
    t[201]='E';     // 311  C9  É  LATIN CAPITAL LETTER E WITH ACUTE
    t[202]='E';     // 312  CA  Ê  LATIN CAPITAL LETTER E WITH OGONEK
    t[203]='E';     // 313  CB  Ë  LATIN CAPITAL LETTER E WITH DIAERESIS
    t[204]='E';     // 314  CC  Ì  LATIN CAPITAL LETTER E WITH CARON
    t[205]='I';     // 315  CD  Í  LATIN CAPITAL LETTER I WITH ACUTE
    t[206]='I';     // 316  CE  Î  LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    t[207]='D';     // 317  CF  Ï  LATIN CAPITAL LETTER D WITH CARON
    t[208]='D';     // 320  D0  Ð  LATIN CAPITAL LETTER D WITH STROKE
    t[209]='N';     // 321  D1  Ñ  LATIN CAPITAL LETTER N WITH ACUTE
    t[210]='N';     // 322  D2  Ò  LATIN CAPITAL LETTER N WITH CARON
    t[211]='O';     // 323  D3  Ó  LATIN CAPITAL LETTER O WITH ACUTE
    t[212]='O';     // 324  D4  Ô  LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    t[213]='O';     // 325  D5  Õ  LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    t[214]='O';     // 326  D6  Ö  LATIN CAPITAL LETTER O WITH DIAERESIS
    t[215]='x';     // 327  D7  ×  MULTIPLICATION SIGN
    t[216]='R';     // 330  D8  Ø  LATIN CAPITAL LETTER R WITH CARON
    t[217]='U';     // 331  D9  Ù  LATIN CAPITAL LETTER U WITH RING ABOVE
    t[218]='U';     // 332  DA  Ú  LATIN CAPITAL LETTER U WITH ACUTE
    t[219]='U';     // 333  DB  Û  LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    t[220]='U';     // 334  DC  Ü  LATIN CAPITAL LETTER U WITH DIAERESIS
    t[221]='Y';     // 335  DD  Ý  LATIN CAPITAL LETTER Y WITH ACUTE
    t[222]='T';     // 336  DE  Þ  LATIN CAPITAL LETTER T WITH CEDILLA
    t[223]='s';     // 337  DF  ß  LATIN SMALL LETTER SHARP S
    t[224]='r';     // 340  E0  à  LATIN SMALL LETTER R WITH ACUTE
    t[225]='a';     // 341  E1  á  LATIN SMALL LETTER A WITH ACUTE
    t[226]='a';     // 342  E2  â  LATIN SMALL LETTER A WITH CIRCUMFLEX
    t[227]='a';     // 343  E3  ã  LATIN SMALL LETTER A WITH BREVE
    t[228]='a';     // 344  E4  ä  LATIN SMALL LETTER A WITH DIAERESIS
    t[229]='l';     // 345  E5  å  LATIN SMALL LETTER L WITH ACUTE
    t[230]='c';     // 346  E6  æ  LATIN SMALL LETTER C WITH ACUTE
    t[231]='c';     // 347  E7  ç  LATIN SMALL LETTER C WITH CEDILLA
    t[232]='c';     // 350  E8  è  LATIN SMALL LETTER C WITH CARON
    t[233]='e';     // 351  E9  é  LATIN SMALL LETTER E WITH ACUTE
    t[234]='e';     // 352  EA  ê  LATIN SMALL LETTER E WITH OGONEK
    t[235]='e';     // 353  EB  ë  LATIN SMALL LETTER E WITH DIAERESIS
    t[236]='e';     // 354  EC  ì  LATIN SMALL LETTER E WITH CARON
    t[237]='i';     // 355  ED  í  LATIN SMALL LETTER I WITH ACUTE
    t[238]='i';     // 356  EE  î  LATIN SMALL LETTER I WITH CIRCUMFLEX
    t[239]='d';     // 357  EF  ï  LATIN SMALL LETTER D WITH CARON
    t[240]='d';     // 360  F0  ð  LATIN SMALL LETTER D WITH STROKE
    t[241]='n';     // 361  F1  ñ  LATIN SMALL LETTER N WITH ACUTE
    t[242]='n';     // 362  F2  ò  LATIN SMALL LETTER N WITH CARON
    t[243]='o';     // 363  F3  ó  LATIN SMALL LETTER O WITH ACUTE
    t[244]='o';     // 364  F4  ô  LATIN SMALL LETTER O WITH CIRCUMFLEX
    t[245]='o';     // 365  F5  õ  LATIN SMALL LETTER O WITH DOUBLE ACUTE
    t[246]='o';     // 366  F6  ö  LATIN SMALL LETTER O WITH DIAERESIS
    t[247]='/';     // 367  F7  ÷  DIVISION SIGN
    t[248]='r';     // 370  F8  ø  LATIN SMALL LETTER R WITH CARON
    t[249]='u';     // 371  F9  ù  LATIN SMALL LETTER U WITH RING ABOVE
    t[250]='u';     // 372  FA  ú  LATIN SMALL LETTER U WITH ACUTE
    t[251]='u';     // 373  FB  û  LATIN SMALL LETTER U WITH DOUBLE ACUTE
    t[252]='u';     // 374  FC  ü  LATIN SMALL LETTER U WITH DIAERESIS
    t[253]='y';     // 375  FD  ý  LATIN SMALL LETTER Y WITH ACUTE
    t[254]='t';     // 376  FE  þ  LATIN SMALL LETTER T WITH CEDILLA
    t[255]=' ';     // 377  FF  ÿ  DOT ABOVE
  }

}

using namespace GNU_gama;

OutStream::OutStream(std::ostream* s) : str(s), encoding(utf_8)
{
  if (ascii_table[1] == 0) init_ascii(ascii_table);
}

const char* OutStream::recode(const char* s)
{
  if (encoding == utf_8) return s;

  text = "";
  while (*s) text += *s++;
  unsigned char* p;

  switch (encoding)
    {
    case iso_8859_2:
      utf8_iso_8859_2((char*)text.c_str());
      break;
    case iso_8859_2_flat:
      utf8_iso_8859_2((char*)text.c_str());
      p = (unsigned char*)text.c_str();
      while(*p) { *p = ascii_table[*p]; p++; }
      break;
    case cp_1250:
      utf8_cp1250((char*)text.c_str());
      break;
    case cp_1251:
      utf8_cp1251((char*)text.c_str());
      break;
    default:
      break;
    }

  return text.c_str();
}


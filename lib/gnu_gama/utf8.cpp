/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2011 Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#include <gnu_gama/utf8.h>

/* http://en.wikipedia.org/wiki/UTF-8
   ----------------------------------
   Bits Last code point Byte 1     Byte 2    Byte 3    Byte 4
     7  U+007F          0xxx xxxx
    11  U+07FF          110x xxxx  10xxxxxx
    16  U+FFFF          1110 xxxx  10xxxxxx  10xxxxxx
    21  U+1FFFFF        1111 0xxx  10xxxxxx  10xxxxxx  10xxxxxx
*/


std::size_t GNU_gama::Utf8::length(std::string s)
{
  const unsigned char bits7  = 0X80;
  const unsigned char bits11 = 0X20;
  const unsigned char bits16 = 0X10;
  const unsigned char bits21 = 0X08;

  int N = 0;
  for (std::size_t i=0; i<s.length(); )
    {
      const unsigned char c = static_cast<unsigned char>(s[i]);

      int bytes = 1;
      if      ((c &  bits7) == 0 ) bytes = 1;
      else if ((c & bits11) == 0 ) bytes = 2;
      else if ((c & bits16) == 0 ) bytes = 3;
      else if ((c & bits21) == 0 ) bytes = 4;

      i += bytes;
      N++;
   }

  return N;
}


std::string GNU_gama::Utf8::leftPad(std::string s, std::size_t N, char p)
{
  std::string t;
  std::size_t n = length(s);
  while (n++ < N)
    {
      t += p;
    }
  t += s;
  return t;
}


std::string GNU_gama::Utf8::rightPad(std::string s, std::size_t N, char p)
{
  std::string t = s;
  std::size_t n = length(s);
  while (n++ < N)
    {
      t += p;
    }
  return t;
}

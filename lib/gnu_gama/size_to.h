/*
  GNU Gama C++ library
  Copyright (C) 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GNU_gama_size_to_h
#define GNU_gama_size_to_h

#include <cstddef>

/** size_to() \brief The template function size_to() returns a) its
 *  parameter 'from' unchanged, b) conversion int(size_t) in the
 *  template specialization size_to<int, size_t>. The only reason to
 *  use the template is to make int(size_t) explicit conversion more
 *  explicit and detectable in the GNU_gama project.
 */

namespace GNU_gama
{
  template <typename To, typename From>
  To size_to(From f) { return f; }

  template<>
  int size_to<int, size_t>(std::size_t f) { return int(f); }
}

#endif

/*
  GNU Gama -- Abstract base class of Acord2 algorithms family
  Copyright (C) 2019  Petra Millarova <petramillarova@gmail.com>

  This file is part of the GNU Gama C++ library.

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

#ifndef GAMA_LOCAL_ACORD2_ALGORITHM_H_
#define GAMA_LOCAL_ACORD2_ALGORITHM_H_

namespace GNU_gama {  namespace local
  {

    class AcordAlgorithm
    {
    public:

      virtual ~AcordAlgorithm();

      virtual void prepare() = 0;
      virtual void execute() = 0;
      virtual const char* className() const = 0;

      inline bool completed() const
      {
        return completed_;
      }

    protected:
      bool prepared_  { false };
      bool completed_ { false };

    };

  }} //namespace GNU_gama::local

#endif

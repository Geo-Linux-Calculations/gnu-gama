/* Yaml2gkf --- conversion from yaml to gkf input format
   Copyright (C) 2020 Ales Cepek <cepek@gnu.org> and
                      Petra Millarova <millapet@gnu.org>

   This file is part of the GNU Gama C++ library.

   Class Yaml2gkf is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Yaml2gkf is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Gama.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef gama_local_Yaml2gkf_h_
#define gama_local_Yaml2gkf_h_

#include <ostream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace GNU_gama { namespace local {

class Yaml2gkf {
public:
  Yaml2gkf(YAML::Node& node, std::ostream& outstream);
  ~Yaml2gkf();

  int run();

  std::string version() { return "0.91"; }

  void enable_warnings () { warnings_ = true;  }
  void disable_warnings() { warnings_ = false; }

private:
  YAML::Node&   config_;
  std::ostream& ostream_;

  int           exit_{0};
  bool          warnings_{false};

  std::string   atts_[3];


  void test_top_nodes();
  void defaults();
  void network();
  void description();
  void parameters();
  void points_observations();
  void points();
  void observations();
  void observations_obs    (const YAML::Node&);
  void observations_hdiffs (const YAML::Node&);
  void observations_vectors(const YAML::Node&);
  void observations_coords (const YAML::Node&);
  void finish();

  std::string cov_mat(const YAML::Node&);

  std::string obs_list(const YAML::Node&);

  void error  (std::string text,
               std::string key=std::string(),
               std::string val=std::string());
  void message(std::string text,
               std::string key=std::string(),
               std::string val=std::string());

  std::string tostr_(const YAML::Node&);
  std::string keyval(std::string, std::string);
  int    words_count(std::string);

  using Handler
    = std::string (Yaml2gkf::*)(std::string key,
                                       std::string val);

  // *******************************************************
  // *  key / value handlers                               *
  // *******************************************************
  std::string nop        (std::string key, std::string val);
  std::string number     (std::string key, std::string val);
  std::string positive   (std::string key, std::string val);
  std::string probability(std::string key, std::string val);
  std::string axes_xy    (std::string key, std::string val);
  std::string angles     (std::string key, std::string val);
  std::string sigma_act  (std::string key, std::string val);
  std::string algorithm  (std::string key, std::string val);
  std::string language   (std::string key, std::string val);
  std::string encoding   (std::string key, std::string val);
  std::string angular    (std::string key, std::string val);
  std::string pointid    (std::string key, std::string val);
  std::string attrxyz    (std::string key, std::string val);
  std::string variant     (std::string key, std::string val);
  bool observation_is_angular_{};
};

}}
#endif

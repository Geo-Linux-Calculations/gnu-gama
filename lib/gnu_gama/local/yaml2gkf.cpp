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

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <sstream>
#include <cctype>

#include <gnu_gama/local/yaml2gkf.h>

using namespace GNU_gama::local;

using Cset = const std::unordered_set<std::string>;
using Cmap = const std::unordered_map<std::string, Cset>;
using Cint = const std::unordered_map<std::string, int>;


Yaml2gkf::Yaml2gkf(YAML::Node& config, std::ostream& ostr)
  : config_(config), ostream_(ostr)
{
}


Yaml2gkf::~Yaml2gkf()
{
}


int Yaml2gkf::run()
{
  /* Optional nodes are 'defaults' and 'description',
   * mandatory nodes are 'points' and 'observations'.
   * Order of the nodes is free, but nodes cannot be repeated.
   */
  test_top_nodes();
  if (exit_) return exit_;

  defaults();
  network();
  description();
  parameters();
  points_observations();
  finish();

  return exit_;
}


void Yaml2gkf::test_top_nodes()
{
  exit_ = 0;

  int count_defaults {0}, count_description  {0},
      count_points   {0}, count_observations {0};

  atts_[0].clear();    // <network> attributes
  atts_[1].clear();    // <parameters>
  atts_[2].clear();    // <poinst-observations>

  for (auto p : config_)
    {
      auto key = tostr_(p.first);

      if      (key == "defaults"    ) count_defaults++;
      else if (key == "description" ) count_description++;
      else if (key == "points"      ) count_points++;
      else if (key == "observations") count_observations++;
      else
      {
        error("unknown node", key);
        ++exit_;
      }
    }

    if (count_defaults > 1) {
        error("optional node 'defaults' can be used only once", "");
        ++exit_;
      }
    if (count_description > 1) {
        error("optional node 'description' can be used only once", "");
        ++exit_;
      }
    if (count_points != 1) {
        error("mandatory node 'points' must be used exactly once");
        ++exit_;
      }
    if (count_observations != 1) {
        error("mandatory node 'observations' must be used exactly once");
        ++exit_;
      }

    // if (exit_ == 0) message("top nodes are OK");
}


void Yaml2gkf::defaults()
{
  Cint attr_ind
    {
      // xml <network> attributes

      {"axes-xy", 0},
      {"angles",  0},
      {"epoch",   0},

      // xml <parameters> attributes

      {"sigma-apr", 1},
      {"conf-pr",   1},
      {"tol-abs",   1},
      {"sigma-act", 1},
      {"algorithm", 1},
      {"language",  1},
      {"encoding",  1},
      {"angles",    1},
      {"latitude",  1},
      {"ellipsoid", 1},
      {"cov-band",  1},

      // xml <points-observations> attributes

      {"distance-stdev",     2},
      {"direction-stdev",    2},
      {"angle-stdev",        2},
      {"zenith-angle-stdev", 2},
      {"azimuth-stdev",      2}
    };

  Cmap cmap_def
    {
      // xml <network> attributes

      {"axes-xy", {"ne", "sw", "es", "wn", "en", "nw", "se", "ws"}},
      {"angles",  {"left-handed", "right-handed"}},
      {"epoch",   {}},

      // xml <parameters> attributes

      {"sigma-apr", {}},
      {"conf-pr",   {}},
      {"tol-abs",   {}},
      {"sigma-act", {"aposteriori", "apriori"}},
      {"algorithm", {"gso", "svd", "cholesky", "envelope"}},
      {"language",  {"en", "ca", "cz", "du", "es", "fi", "fr", "hu", "ru",
                     "ua","zh"}},
      {"encoding",  {"utf-8", "iso-8859-2", "iso-8859-2-flat",
                     "cp-1250", "cp-1251"}},
      {"angles",    {"400", "360"}},
      {"latitude",  {}},
      {"ellipsoid", {}},
      {"cov-band",  {}},

      // xml <points-observations> attributes

      {"distance-stdev",     {}},
      {"direction-stdev",    {}},
      {"angle-stdev",        {}},
      {"zenith-angle-stdev", {}},
      {"azimuth-stdev",      {}}
    };

  if (YAML::Node def = config_["defaults"])
    for (auto d : def)
    {
      auto key = tostr_(d.first);
      auto val = tostr_(d.second);

      // message("defaults",  key, val);

      // *** test key and val  ***
      auto k = cmap_def.find(key);
      if (k == cmap_def.end())
        {
          error("key not found", key);
          continue;
        }
      if (! k->second.empty())
        {
          auto v = k->second.find(val);
          if (v == k->second.end())
            {
              error("value not found", key, val);
              continue;
            }
        }


      // attributes for XML tags network, patameters and points-observations

      int index = attr_ind.find(key)->second;
      std::string s = "\n   " + key + "=\"" + val + "\"";
      atts_[index] += s;
    }
}


void Yaml2gkf::network()
{
  /* GNU Gama : xml/gama-local.xsd */

  ostream_ <<
    "<?xml version=\"1.0\" ?>\n"
    "<gama-local xmlns=\"http://www.gnu.org/software/gama/gama-local\">\n"
    "<!-- Generated by Yaml2gkf version " << version() << " -->\n\n";

  ostream_ << "<network" << atts_[0] << ">\n\n";
}


void Yaml2gkf::description()
{
  auto desc = config_["description"];
  if (!desc.IsDefined()) return;

  auto dstr = tostr_(desc);
  if (dstr.empty()) return;

  ostream_ << "<description>\n";
  std::istringstream istr(dstr);
  std::string word, line;
  while (istr >> word)
    {
      if (line.length() + word.length() > 66)   // 65 + one space
        {
          if (std::isspace(line.back())) line.pop_back();
          ostream_ << line << "\n";
          line.clear();
        }

      line += word + " ";
    }

  if (std::isspace(line.back())) line.pop_back();
  ostream_ << line << "\n";
  ostream_ << "</description>\n\n";
}


void Yaml2gkf::parameters()
{
  ostream_ << "<parameters" << atts_[1] << " />\n\n";
}


void Yaml2gkf::points_observations()
{
  ostream_ << "<points-observations" + atts_[2] + ">\n\n";

  points();
  observations();

  ostream_ << "</points-observations>\n\n";
}


void Yaml2gkf::points()
{
  Cmap cmap_pnt
    {
      // xml <network> attributes

      {"id", {}},
      {"x",  {}},
      {"y",  {}},
      {"z",  {}},
      {"fix",{"xy","XY","z","Z","xyz","XYZ","XYz","xyZ"}},
      {"adj",{"xy","XY","z","Z","xyz","XYZ","XYz","xyZ"}}
    };

  auto pnts = config_["points"];
  if (!pnts.IsDefined()) return;

  for (auto p : pnts)
    {
      ostream_ << "<point";

      for (auto a : p)
        {
          auto key = tostr_(a.first);
          auto val = tostr_(a.second);

          auto k = cmap_pnt.find(key);
          if (k == cmap_pnt.end())
            {
              error("key not found", key);
              continue;
            }
          if (!k->second.empty())
            {
              auto v = k->second.find(val);
              if (v == k->second.end())
              {
                error("value not found", key, val);
                continue;
              }
            }

          ostream_ << " " << key << "=\"" << val << "\"";
        }

      ostream_ << " />\n";
    }

  ostream_ << "\n";
}


void Yaml2gkf::observations()
{
  /* observations node is one of the top level nodes:
   *    * defaults
   *    * description
   *    * points
   *    * observations
   *
   * observations node contains lists of child nodes describing <obs>,
   * <height-differences>, <vectors> and <coordinates> XML tag.
   */

  YAML::Node top_level_nodes = config_["observations"];
  if (!top_level_nodes.IsDefined()) return;

  for (auto obs_map : top_level_nodes)
    {
      std::string attr = tostr_(obs_map.begin()->first);

      if (attr == "height-differences") {
        observations_hdiffs(obs_map);
      }
      else if (attr == "vectors") {
        observations_vectors(obs_map);
      }
      else if (attr == "coordinates")  {
        observations_coords(obs_map);
      }
      else {
        observations_obs(obs_map);
      }
    }
}


void Yaml2gkf::observations_obs    (const YAML::Node& node)
{
  Cint obs_ae  // <obs> attributes and elements, see gama-local.xsd
  {
    {"from",        0},   // attributes
    {"from_dh",     0},
    {"orientation", 0},
    {"obs",         1}   // list of measurements
  };

  std::string obs_attributes {};
  std::string obs_measurements {};
  for (auto p : node)
    {
      auto key = tostr_(p.first);
      auto val = tostr_(p.second);

      auto k = obs_ae.find(key);
      if (k == obs_ae.end())
        {
          error("key not found", key);
          continue;
        }

      switch (k->second) {
      case 0:
        obs_attributes += " " + key + "=\"" + val + "\"";
        break;
      case 1:
        obs_measurements += obs_list(p.second);
        break;
      default:
        break;   // this should never happen
      }
    }

  ostream_ << "<obs" << obs_attributes << ">\n";
  ostream_ << obs_measurements;
  ostream_ << "</obs>\n\n";
}


void Yaml2gkf::observations_hdiffs (const YAML::Node& hdiff_node)
{
  ostream_ << "<height-differences>\n";

  Cset hdf_ae
  {
   "from", "to", "val", "stdev", "dist", "extern"
  };

  for (auto dh_node : hdiff_node.begin()->second)
    {
      std::string observation {"   <dh "};

      auto iter = dh_node.begin();
      if (iter == dh_node.end())
        {
          continue;
        }

      if (tostr_(iter->first) == "cov-mat")
        {
          ostream_ << cov_mat(iter->second);
          continue;
        }

      for (auto dh : iter->second)
        {
          auto key = tostr_(dh.first);
          auto val = tostr_(dh.second);

          auto iter = hdf_ae.find(key);
          if (iter == hdf_ae.end())
            {
              error("<dh /> unknown key", key);
            }

          observation += " " + key + "=\"" + val + "\"";
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</height-differences>\n\n";
}


void Yaml2gkf::observations_vectors(const YAML::Node& vector_node)
{
  ostream_ << "<vectors>\n";

  Cset vec_atr
  {
    "from", "to", "dx", "dy", "dz", "extern"
  };

  for (auto vec_node : vector_node.begin()->second)
    {
      std::string observation {"   <vec "};

      auto iter = vec_node.begin();
      if (iter == vec_node.end())
        {
          continue;
        }

      if (tostr_(iter->first) == "cov-mat")
        {
          ostream_ << cov_mat(iter->second);
          continue;
        }

      for (auto dh : iter->second)
        {
          auto key = tostr_(dh.first);
          auto val = tostr_(dh.second);

          auto iter = vec_atr.find(key);
          if (iter == vec_atr.end())
            {
              error("<vec /> unknown key", key);
            }

          observation += " " + key + "=\"" + val + "\"";
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</vectors>\n\n";
}


void Yaml2gkf::observations_coords (const YAML::Node& coords_node)
{
  ostream_ << "<coordinates>\n";

  Cset coords_atr
  {
    "id", "x", "y", "z", "extern"
  };

  for (auto cnode : coords_node.begin()->second)
    {
      std::string observation {"   <point "};

      auto iter = cnode.begin();
      if (iter == cnode.end())
        {
          continue;
        }

      if (tostr_(iter->first) == "cov-mat")
        {
          ostream_ << cov_mat(iter->second);
          continue;
        }

      for (auto pnt : cnode)
        {
          auto key = tostr_(pnt.first);
          auto val = tostr_(pnt.second);

          auto iter = coords_atr.find(key);
          if (iter == coords_atr.end())
            {
              error("<point /> unknown key", key);
            }

          observation += " " + key + "=\"" + val + "\"";
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</coordinates>\n\n";

}


std::string Yaml2gkf::obs_list(const YAML::Node& node)
{
  Cint observations  // <obs> attributes and elements, see gama-local.xsd
  {
    {"direction",   0},
    {"distance",    0},
    {"angle",       1},
    {"s-distance",  0},
    {"z-angle",     0},
    {"azimuth",     0}
  };

  std::string measurements;
  bool covmat {};

  for (auto e : node)  // e.IsMap()
    {
      std::string type, atts;
      bool observation_is_angle = false;

      for (auto f : e)
        {
          covmat = false;

          auto key = tostr_(f.first);
          auto val = tostr_(f.second);
          if (key == "type")
            {
              type = val;
              auto obs = observations.find(val);
              if (obs == observations.end())
                {
                  error("unknown observation type", val);
                }
              else
                {
                  observation_is_angle = obs->second;
                }

              continue;
            }

          if (key == "cov-mat")
            {
              covmat = true;
              measurements += cov_mat(f.second);
              continue;
            }

          auto set_keyval = [&](Cset& attributes)
          {
            auto iter = attributes.find(key);
            if (iter == attributes.end()) {
                error("uknown attribute", key, val);
              }
            atts += " " + key + "=\"" + val + "\"";
          };

          // observation attributes, see gama-local.xsd
          if (observation_is_angle)
            {
              Cset obsl_angle {
                "to", "val", "stdev", "from_dh", "to_dh", "extern",
                "bs", "fs", "bs_dh", "fs_dh"
              };
              set_keyval(obsl_angle);
            }
          else
            {
              Cset obsl_others{
                "to", "val", "stdev", "from_dh", "to_dh", "extern"
              };
              set_keyval(obsl_others);
            }
        }

      if (!covmat)
        {
          if ( observations.find(type) == observations.end() ) {
            error("unknown observation type", type);
          }
          if (type.empty()) {
            error("observation type is missing", atts);
          }

          std::string m = "   <" + type + atts + " />\n";
          measurements += m;
        }
    }

  return measurements;
}


void Yaml2gkf::finish()
{
  ostream_ <<
    "</network>\n"
    "</gama-local>\n";
}


std::string Yaml2gkf::cov_mat(const YAML::Node& node)
{
  std::string dim, band, cov;

  for (auto n : node)
    {
      auto key = tostr_(n.first);
      auto val = tostr_(n.second);
      if (key == "dim") dim = val;
      else if (key == "band") band = val;
      else if (key == "upper-part") cov = val;
      else {
        error("key not found", key);
      }
    }

  std::string cline, cword, cpad("      ");
  std::istringstream istr(cov);

  cov.erase();
  cline = cpad;
  while (istr >> cword)
    {
      if (cline.size() + cword.size() > 66)
        {
          if (std::isspace(cline.back())) cline.pop_back();

          cov += cline + "\n";
          cline = cpad;
        }
      cline += cword + " ";
    }

  if (std::isspace(cline.back())) cline.pop_back();
  cov += cline;

  return
    "   <cov-mat dim=\"" + dim +  "\" band=\"" + band  + "\">\n" + cov + "\n" +
    "   </cov-mat>\n";
}


void Yaml2gkf::error(std::string text,
                     std::string key, std::string val)
{
  ostream_ << "<!-- ERROR: " << text;
  if (!key.empty()) ostream_ << " key=" << key;
  if (!val.empty()) ostream_ << " val=" << val;
  ostream_ << " -->\n";

  ++exit_;
}

void Yaml2gkf::message(std::string text,
                       std::string key, std::string val)
{
  if (warnings_)
    {
      ostream_ << "<!-- MESSAGE: " << text;
      if (!key.empty()) ostream_ << " key=" << key;
      if (!val.empty()) ostream_ << " val=" << val;
      ostream_ << " -->\n";
    }
}

std::string Yaml2gkf::tostr_(const YAML::Node& n)
{
  return n.IsScalar() ? n.as<std::string>() : std::string();
}

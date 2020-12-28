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
#include <utility>
#include <gnu_gama/local/yaml2gkf.h>
#include <gnu_gama/gon2deg.h>

using namespace GNU_gama::local;


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
  const std::unordered_map<std::string,
      std::pair<int, Handler>> attr_ind
  {
    // xml <network> attributes

    {"axes-xy", {0, &Yaml2gkf::axes_xy}},
    {"angles",  {0, &Yaml2gkf::angles}},
    {"epoch",   {0, &Yaml2gkf::number}},

    // xml <parameters> attributes

    {"sigma-apr", {1, &Yaml2gkf::positive}},
    {"conf-pr",   {1, &Yaml2gkf::probability}},
    {"tol-abs",   {1, &Yaml2gkf::positive}},
    {"sigma-act", {1, &Yaml2gkf::sigma_act}},
    {"algorithm", {1, &Yaml2gkf::algorithm}},
    {"language",  {1, &Yaml2gkf::language}},
    {"encoding",  {1, &Yaml2gkf::encoding}},
    {"angular",   {1, &Yaml2gkf::angular}},
    {"latitude",  {1, &Yaml2gkf::number}},
    {"ellipsoid", {1, &Yaml2gkf::nop}},
    {"cov-band",  {1, &Yaml2gkf::nop}},

    // xml <points-observations> attributes

    {"distance-stdev",     {2, &Yaml2gkf::nop}},  // up to three values
    {"direction-stdev",    {2, &Yaml2gkf::positive}},
    {"angle-stdev",        {2, &Yaml2gkf::positive}},
    {"zenith-angle-stdev", {2, &Yaml2gkf::positive}},
    {"azimuth-stdev",      {2, &Yaml2gkf::positive}}
  };

  if (YAML::Node def = config_["defaults"])
    for (auto d : def)
      {
        auto key = tostr_(d.first);
        auto val = tostr_(d.second);

        // attributes for XML tags network, patameters and points-observations

        auto iter = attr_ind.find(key);
        if (iter == attr_ind.end())
          {
            error("unknown defaults' attribute", key, val);
            continue;
          }

        int index = iter->second.first;
        Handler f = iter->second.second;
        std::string s = "\n   " + (this->*f)(key, val);
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
  const std::unordered_map<std::string,  Handler> cset_pnt
  {
    // xml <network> attributes

    {"id",  &Yaml2gkf::pointid},
    {"x",   &Yaml2gkf::number},
    {"y",   &Yaml2gkf::number},
    {"z",   &Yaml2gkf::number},
    {"fix", &Yaml2gkf::attrxyz},
    {"adj", &Yaml2gkf::attrxyz}
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

          auto k = cset_pnt.find(key);
          if (k == cset_pnt.end())
            {
              error("key not found", key, val);
              continue;
            }

          Handler f = cset_pnt.find(key)->second;
          ostream_ << (this->*f)(key, val);
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
  const std::unordered_map<std::string, std::pair<int, Handler>> obs_ae
  {
    {"from",        {0, &Yaml2gkf::pointid}},   // attributes
    {"from_dh",     {0, &Yaml2gkf::number}},
    {"orientation", {0, &Yaml2gkf::number}},
    {"obs",         {1, &Yaml2gkf::nop}}        // list of measurements
  };

  std::string obs_attributes {};
  std::string obs_measurements {};
  for (auto p : node)
    {
      auto key = tostr_(p.first);
      auto val = tostr_(p.second);

      // in <obs> tag from attribute may not defined in yaml
      if (key == "from" && val.empty()) continue;

      auto k = obs_ae.find(key);
      if (k == obs_ae.end())
        {
          error("key not found", key);
          continue;
        }

      int index = k->second.first;
      Handler f = k->second.second;

      switch (index) {
      case 0:
        obs_attributes += (this->*f)(key, val);
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

  const std::unordered_map<std::string, Handler> hdf_ae
  {
    {"from",   &Yaml2gkf::pointid},
    {"to",     &Yaml2gkf::pointid},
    {"val",    &Yaml2gkf::number},
    {"stdev",  &Yaml2gkf::positive},
    {"dist",   &Yaml2gkf::positive},
    {"extern", &Yaml2gkf::nop}
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
              continue;
            }

          Handler f = iter->second;
          observation += (this->*f)(key, val);
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</height-differences>\n\n";
}


void Yaml2gkf::observations_vectors(const YAML::Node& vector_node)
{
  ostream_ << "<vectors>\n";

  const std::unordered_map<std::string, Handler> vec_atr
  {
    {"from",   &Yaml2gkf::pointid},
    {"to",     &Yaml2gkf::pointid},
    {"dx",     &Yaml2gkf::number},
    {"dy",     &Yaml2gkf::number},
    {"dz",     &Yaml2gkf::number},
    {"extern", &Yaml2gkf::nop}
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

          Handler f = iter->second;
          observation += (this->*f)(key, val);
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</vectors>\n\n";
}


void Yaml2gkf::observations_coords (const YAML::Node& coords_node)
{
  ostream_ << "<coordinates>\n";

  const std::unordered_map<std::string, Handler> coords_atr
  {
    {"id",     &Yaml2gkf::pointid},
    {"x",      &Yaml2gkf::number},
    {"y",      &Yaml2gkf::number},
    {"z",      &Yaml2gkf::number},
    {"extern", &Yaml2gkf::nop}
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

          Handler f = iter->second;
          observation += (this->*f)(key, val);
        }
      observation += " />\n";

      ostream_ << observation;
    }

  ostream_ << "</coordinates>\n\n";

}


std::string Yaml2gkf::obs_list(const YAML::Node& node)
{
  // <obs> attributes and elements, see gama-local.xsd
  const std::unordered_map<std::string, int> observations
  {
    {"distance",    0},
    {"s-distance",  0},
    {"angle",       1},
    {"direction",   2},
    {"z-angle",     2},
    {"azimuth",     2}
  };

  std::string measurements;
  bool covmat {};

  for (auto e : node)  // e.IsMap()
    {
      std::string type, atts;
      bool observation_is_angle = false;
      observation_is_angular_ = false;

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
                  observation_is_angle = (obs->second == 1);
                  observation_is_angular_ = (obs->second > 0);
                }

              continue;
            }

          if (key == "cov-mat")
            {
              covmat = true;
              measurements += cov_mat(f.second);
              continue;
            }

          // observation attributes, see gama-local.xsd
          if (observation_is_angle)
            {
              const std::unordered_map<std::string, Handler> obsl_angle
              {
                {"from",    &Yaml2gkf::pointid},
                {"to",      &Yaml2gkf::pointid},
                {"val",     &Yaml2gkf::variant},
                {"stdev",   &Yaml2gkf::positive},
                {"from_dh", &Yaml2gkf::pointid},
                {"to_dh",   &Yaml2gkf::number},
                {"extern",  &Yaml2gkf::nop},
                {"bs",      &Yaml2gkf::pointid},
                {"fs",      &Yaml2gkf::pointid},
                {"bs_dh",   &Yaml2gkf::number},
                {"fs_dh",   &Yaml2gkf::number}
              };

              auto iter = obsl_angle.find(key);
              if (iter == obsl_angle.end())
              {
                error("uknown attribute", key, val);
              }
              else
              {
                Handler f = iter->second;
                atts += (this->*f)(key, val);
              }
            }
          else
            {
              const std::unordered_map<std::string, Handler>  obsl_others
              {
                {"from",    &Yaml2gkf::pointid},
                {"to",      &Yaml2gkf::pointid},
                {"val",     &Yaml2gkf::variant},
                {"stdev",   &Yaml2gkf::positive},
                {"from_dh", &Yaml2gkf::number},
                {"to_dh",   &Yaml2gkf::number},
                {"extern",  &Yaml2gkf::nop}
              };

              auto iter = obsl_others.find(key);
              if (iter == obsl_others.end())
              {
                error("uknown attribute", key, val);
              }
              else
              {
                Handler f = iter->second;
                atts += (this->*f)(key, val);
              }
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

std::string Yaml2gkf::keyval(std::string key, std::string val)
{
  return " " + key + " = \"" + val + "\"";
}

int Yaml2gkf::words_count(std::string s)
{
  bool previous {true}, current {};

  int count {0};
  for (char c : s)
    {
      current = std::isspace(c);
      if (previous && !current) count++;
      previous = current;
    }

  return count;
}

// *******************************************************
// *   key / value handlers                              *
// *******************************************************

std::string Yaml2gkf::nop(std::string key, std::string val)
{
  return keyval(key, val);
}

std::string Yaml2gkf::number(std::string key, std::string val)
{
  double result {};
  std::istringstream inp(val);
  inp >> result;

  bool test = !inp.fail() && inp.eof();

  if (!test) error("not a number value", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::positive(std::string key, std::string val)
{
  double result {};
  std::istringstream inp(val);
  inp >> result;

  bool test = !inp.fail() && inp.eof();
  if (test && result <= 0) test = false;

  if (!test) error("zero or negative value", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::probability(std::string key, std::string val)
{
  double result {};
  std::istringstream inp(val);
  inp >> result;

  bool test = !inp.fail() && inp.eof();
  if (test && result <= 0) test = false;
  if (test && result >  1) test = false;

  if (!test) error("not a probability value", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::axes_xy(std::string key, std::string val)
{
  const std::unordered_set<std::string> axes
  {
    "ne", "sw", "es", "wn", "en", "nw", "se", "ws"
  };

  bool test = axes.find(val) != axes.end();
  if (!test) error("bad axes definition", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::angles(std::string key, std::string val)
{
  bool test = val == "left-handed" || val == "right-handed";
  if (!test) error("bad angles definition", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::sigma_act(std::string key, std::string val)
{
  bool test = val == "aposteriori" || val == "apriori";
  if (!test) error("bad sigma act definition", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::algorithm(std::string key, std::string val)
{
  const std::unordered_set<std::string> algo
  {
    "gso", "svd", "cholesky", "envelope"
  };

  bool test = algo.find(val) != algo.end();
  if (!test) error("unknown algorithm", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::language(std::string key, std::string val)
{
  const std::unordered_set<std::string> lang
  {
    "en", "ca", "cz", "du", "es", "fi", "fr", "hu", "ru",
    "ua","zh"
  };

  bool test = lang.find(val) != lang.end();
  if (!test) error("unknown language", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::encoding(std::string key, std::string val)
{
  const std::unordered_set<std::string> enc
  {
    "utf-8", "iso-8859-2", "iso-8859-2-flat", "cp-1250", "cp-1251"
  };

  bool test = enc.find(val) != enc.end();
  if (!test) error("unknown encoding", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::angular(std::string key, std::string val)
{
  bool test = val == "400" || val == "360";
  if (!test) error("bad angular units", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::pointid(std::string key, std::string val)
{
  bool test = words_count(val) == 1;
  if (!test) error("bad point id", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::attrxyz(std::string key, std::string val)
{
  const std::unordered_set<std::string> coordinates
  {
    "xy", "XY", "z", "Z", "xyz", "XYZ", "XYz", "xyZ"
  };

  bool test = coordinates.find(val) != coordinates.end();
  if (!test) error("unknown xyz definition", key, val);

  return keyval(key, val);
}

std::string Yaml2gkf::variant(std::string key, std::string val)
{
  if (observation_is_angular_)
  {
    double gon;
    std::istringstream istr(val);
    int d, m;
    double s;
    char c1, c2;
    istr >> d >> c1 >> m >> c2 >> s;

    bool test = !istr.fail() && istr.eof() &&
                c1 == '-' && c2 == '-';  /* && m <= 60 && s <= 60; */
    if (test) return keyval(key, val);
  }

  return number(key, val);
}

/* Krumm2gama-local -- conversion from F. Krumm format to XML gama-local
   Copyright (C) 2022 Ales Cepek <cepek@gnu.org>

   This file is part of Krumm2gama-local.

   Krumm2gama-local is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Krumm2gama-local is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Krumm2gama-local. If not, see <https://www.gnu.org/licenses/>.
*/

#include <krumm/input.h>
#include <krumm/common.h>
#include <krumm/k2gkf.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cctype>

using namespace GNU_gama::local;

Input::Input(std::istream& inp, Common& c) : inp_(inp), common(c)
{
  input();
}

void Input::input()
{
  std::string line;

  while (std::getline(inp_, line))
    {
      auto p = line.find('%');
      if (p != std::string::npos) line.erase(p);
      p = line.find('#');
      if (p != std::string::npos) line.erase(p);

      while (!line.empty() && std::isspace(line[0])) line.erase(0,1);
      while (!line.empty() && std::isspace(line.back())) line.pop_back();
      if (line.empty()) continue;

      if (line[0] == '[')
        {
          auto iter = common.state_map.find(line);
          if (iter == common.state_map.end())
            common.state = Common::State::unknown;
          else
            {
            common.state = std::get<0>(iter->second);

            int dim =  std::get<1>(iter->second);
            common.dimension = std::max(common.dimension, dim);
            }
          continue;
        }

      std::istringstream istrline(line);
      std::vector<std::string> tokens;
      std::string token;
      while (istrline >> token) tokens.push_back(token);

      switch (common.state)
        {
        case Common::State::project:
          common.project += line + "\n";
          break;
        case Common::State::source:
          common.source += line + "\n";
          break;
        case Common::State::sigma0:
          sigma_0(tokens);
          break;
        case Common::State::coordinates:
          coordinates(line);
          break;
        case Common::State::datum:
          datum(line);
          break;
        case Common::State::spatial_distances:
          spatial_distances(line);
          break;
        case Common::State::vertical_angles:
          vertical_angles(line);
          break;
        case Common::State::zenith_angles:
          zenith_angles(line);
          break;
        case Common::State::angles:
          angles(line);
          break;
        case Common::State::angles_dms:
          angles_dms(line);
          break;
        case Common::State::d3_base_line:
          vectors(line);
          break;
        case Common::State::directions:
          directions(line);
          break;
        case Common::State::distances:
          distances(line);
          break;
        case Common::State::approximate_orientation:
          approximate_orientation(line);
          break;
        case Common::State::levelled_height_differences:
          levelled_height_differences(line);
          break;
        case Common::State::azimuth:
          azimuths(line);
          break;
        case Common::State::azimuth_dms:
          azimuths_dms(line);
          break;
        case Common::State::grid_bearings_dms:
          grid_bearings_dms(line);
          break;
        default:
          break;
        }
    }
}

void Input::coordinates(std::string line)
{
  if (line.empty()) return;

  std::istringstream istr(line);
  Common::Point p;
  std::string pid;
  auto tokens = get_tokens(line);

  if (tokens.size() == 4)
    {
      pid = tokens[0];
      p.x = tokens[1];
      p.y = tokens[2];
      p.z = tokens[3];
      p.dim = 3;
    }
  else if (tokens.size() == 3)
    {
      pid = tokens[0];
      p.x = tokens[1];
      p.y = tokens[2];
      p.dim = 2;
    }
  else if (tokens.size() == 2)
    {
      pid = tokens[0];
      p.z = tokens[1];
      p.dim = 1;
    }
  else
    {
      // ERROR
    }

  // common.dimension = std::max(common.dimension, p.dim);
  common.point_map[pid] = p;
}

void Input::datum(std::string line)
{
  std::istringstream istr(line);
  std::string token;
  while (istr >> token)
    {
      if (token == "dyn")
        {
          datum_dyn();
          return;
        }
      common.datum_tokens.push_back(token);
    }
}

void Input::datum_dyn()
{
  std::string line;
  int row = 0;
  while (std::getline(inp_, line))
    {
      auto tokens = tokenize(line);
      if (tokens.empty())   // we expect the empty line to end covariances
        {
          for (int i=1; i<=common.datum_dyn_cov.rows(); i++)
            {
              auto id = common.datum_dyn_cov[i][0];
              if (id[0] == 'y') continue;       // 2D network : x20 y20 ...
              if (id[0] == 'x') id.erase(0,1);  // 1D         : 20 ...
              auto& pt = common.point_map[id];
              pt.datum_dyn = true;
            }

          return;
        }

      row++;
      for (int col=0; col<tokens.size(); col++)
        {
          common.datum_dyn_cov[row][col] = tokens[col];
        }
    }
}

void Input::spatial_distances(std::string line)
{
  //      3         4           6
  // from to val [stdev [from_dh to_dh]]
  auto tokens = tokenize(line);
  std::string part3, part4, part6;

  switch (tokens.size())
    {
    case 6:
      part6 =
          " from_dh='" +tokens[4]+ "' to_dh='" +tokens[5]+ "'";
    case 4:
      common.spatial_dist_sigma = tokens[3];
    case 3:
      part3 =
          " from='" +tokens[0]+ "' to='" +tokens[1]+ "' val='" +tokens[2]+ "'"
          " stdev='" + mgon2cc(common.spatial_dist_sigma) + "'";
      break;
    default:
      common.spatial_dist_list +=
          "<!-- ERROR -- Wrong number of tokens '" + line + "' -->";
      common.set_error();
      return;
    };

    std::string sdist = "<s-distance" + part3 + part6 + " />\n";
    common.spatial_dist_list += sdist;
}

void Input::vertical_angles(std::string line)
{
  // vertical angles are replaced by zenith angles
  std::istringstream istr(line);
  std::string from, to, val, stdev;
  istr >> from >> to >> val >> stdev;
  if (!stdev.empty())
    common.vertical_angles_sigma = gon2cc(stdev);

  val = std::to_string(100 - std::stod(val));
  common.vertical_angles_list +=
      "<z-angle from=\"" + from + "\"" +
      " to=\"" + to + "\"" +
      " val=\"" + val + "\"" +
      " stdev=\"" + common.vertical_angles_sigma + "\" />\n";
}

void Input::zenith_angles(std::string line)
{
  //      3         4           6
  // from to val [stdev [from_dh to_dh]]
  auto tokens = tokenize(line);
  std::string part3, part4, part6;

  switch (tokens.size())
    {
    case 6:
      part6 =
          " from_dh='" +tokens[4]+ "' to_dh='" +tokens[5]+ "'";
    case 4:
      common.zenith_angles_sigma = gon2cc(tokens[3]);
    case 3:
      part3 =
          " from='" +tokens[0]+ "' to='" +tokens[1]+ "' val='" +tokens[2]+ "'"
          " stdev='" + common.zenith_angles_sigma + "'";
      break;
    default:
      common.zenith_angles_list +=
          "<!-- ERROR -- Wrong number of tokens '" + line + "' -->";
      common.set_error();
      return;
    };

    std::string zangle = "<z-angle" + part3 + part6 + " />\n";
    common.zenith_angles_list += zangle;
}

void Input::angles(std::string line, bool dms)
{
  std::istringstream istr(line);
  std::string from, bs, fs, val, stdev;
  istr >> from >> bs >> fs >> val >> stdev;
  if (!stdev.empty()) {
    if (dms) {
        common.angles_sigma = stdev;
      }
    else {
        common.angles_sigma = gon2cc(stdev);
      }
  }
  common.angles_list +=
      "<angle from=\"" + from + "\"" +
      " bs=\"" + bs + "\"" +
      " fs=\"" + fs + "\"" +
      " val=\"" + val + "\"" +
      " stdev=\"" + common.angles_sigma + "\" />\n";
}

void Input::angles_dms(std::string line)
{
  angles(utf8_to_dms(line), true);
}

void Input::vectors(std::string line)
{
    std::istringstream istr(line);
    std::string from, to, dx, dy, dz;
    std::vector<std::string> scov;
    istr >> from >> to >> dx >> dy >> dz;
    std::string s;
    while (istr >> s) scov.push_back(s);

    std::string covmat, error;
    if (scov.size() == 3)
      {
        std::ostringstream ostr;
        double cx = std::stod(m2mm(scov[0]));
        double cy = std::stod(m2mm(scov[1]));
        double cz = std::stod(m2mm(scov[2]));
        ostr.precision(16);
        ostr << cx*cx << " 0 0\n"
             << cy*cy << " 0\n"
             << cz*cz << "\n";
        covmat = ostr.str();
      }
    else if (scov.size() == 6)
      {
        std::ostringstream ostr;
        ostr << scov[0] << " " << scov[1] << " " << scov[2] << "\n"
             << scov[3] << " " << scov[4] << "\n"
             << scov[5] << "\n";
        covmat += ostr.str();
      }
    else
      {
        error = "<!-- "
          "ERROR -- Wrong number of vector covariances\n      "
          + line + " -->\n";
        common.set_error();
      }

    std::string vector = error
        + "<vec from=\"" + from + "\" to=\"" + to + "\""
        + " dx=\"" + dx + "\" dy=\"" + dy + "\" dz=\"" + dz + "\" />\n"

        + "<cov-mat dim=\"3\" band=\"2\">\n"
        + covmat +
        "</cov-mat>\n";

    common.vectors.push_back(vector);
}

void Input::directions(std::string line)
{
  std::istringstream istr(line);
  std::string from, to, dir, stdev;
  istr >> from >> to >> dir >> stdev;
  if (!stdev.empty())
    common.directions_sigma = gon2cc(stdev);

  std::string direction =
      "<direction"
      " to=\""    + to  + "\"" +
      " val=\""   + dir + "\"" +
      " stdev=\"" + common.directions_sigma +
      "\" />\n";

  common.direction_map[from].directions.push_back(direction);
}

void Input::distances(std::string line)
{
  std::istringstream istr(line);
  std::string from, to, dist, stdev;
  istr >> from >> to >> dist >> stdev;
  if (!stdev.empty())
    common.distances_sigma = m2mm(stdev);

  std::string distance =
      "<distance"
      " from=\"" + from + "\""
      " to=\""   + to    + "\""
      " val=\""   + dist + "\"" +
      " stdev=\"" + common.distances_sigma +
      "\" />\n";

  common.distances_list += distance;
}

void Input::approximate_orientation(std::string line)
{
  std::istringstream istr(line);
  std::string from, appr;
  istr >> from >> appr;

  common.direction_map[from].orientation = appr;
}
/*
 X A  5.10 4000 0.001
 42 A Y  2.34 3000
 43 Y C -1.25 2000
 44 C X -6.13 3000
 45 A B -0.68 2000
 46 Y B -3.00 2000
 47 B C  1.70 2000

*/

void Input::levelled_height_differences(std::string line)
{
  std::string from, to, val, dist, stdev;
  std::istringstream istr(line);
  istr >> from >> to >> val >> dist >> stdev;

  if (!stdev.empty()) common.levell_hdiffs_sigma = m2mm(stdev);

  double s =
      std::stod(common.levell_hdiffs_sigma) * std::sqrt(std::stod(dist)/1000);

  std::string lhdiff =
    "<dh from='" + from + "' to='" + to + "' val='" + val + "'"
    " stdev='" + std::to_string(s) + "' />\n";

  common.levell_hdiffs_list += lhdiff;
}

void Input::azimuths(std::string line, bool dms)
{
  std::istringstream istr(line);
  std::string from, to, val, stdev;
  istr >> from >> to >> val >> stdev;
  if (!stdev.empty()) {
    if (dms) {
        common.azimuths_sigma = stdev;
      }
    else {
        common.azimuths_sigma = mgon2cc(stdev);
      }
  }
  common.azimuths_list +=
      "<azimuth from=\"" + from + "\"" +
      " to=\"" + to + "\"" +
      " val=\"" + val + "\"" +
      " stdev=\"" + common.azimuths_sigma + "\" />\n";
}

void Input::azimuths_dms(std::string line)
{
  azimuths(utf8_to_dms(line),true);
}

void Input::grid_bearings_dms(std::string line)
{
  azimuths(utf8_to_dms(line), true);
}

void Input::sigma_0(const std::vector<std::string>& tokens)
{
  if (tokens.size() == 1)
    {
      common.sigma0 = tokens[0];
    }
  else if (tokens.size() == 2)
    {
      if      (tokens[1] == "gon" ) common.sigma0 = gon2cc(tokens[0]);
      else if (tokens[1] == "mgon") common.sigma0 = mgon2cc(tokens[0]);
      else if (tokens[1] == "m"   ) common.sigma0 = m2mm(tokens[0]);
      else if (tokens[1] == "cm"  ) common.sigma0 = cm2mm(tokens[0]);
      else common.sigma0 = tokens[0];
    }
}

std::string Input::utf8_to_dms(std::string line)
{
  // utf8 DEGREE SIGN to '-'
  std::string str;
  char p{0};
  for (char c : line)
    {
      if (p == char(0302) && c == char(0260)) // utf-8 DEGREE SIGN
        {
          str.pop_back();
          c = '-';
        }
      else if (c == '\'')
        {
          c = '-';
        }
      else if (c == '"')
        {
          c = ' ';
        }
      p = c;
      str += c;

    }
  return str;
}

/*int main()
{
  std::cout << "Hello UTF-8 Degree Sign\n\n"
            << "E D F 205°13'51.7\"" << " XXX\n"
            << utf8_to_dms("E D F 205°13'51.7\"")  << " XXX\n";

  return 0;
}*/

std::vector<std::string> Input::tokenize(std::string line)
{
  std::vector<std::string> tokens;
  std::istringstream istr(line);
  std::string token;
  while (istr >> token)
    {
      tokens.push_back(token);
    }

  return tokens;
}

![GitHub release (latest by date)](https://img.shields.io/github/v/release/Geo-Linux-Calculations/gama-work)
![GitHub Release Date](https://img.shields.io/github/release-date/Geo-Linux-Calculations/gama-work)
![GitHub repo size](https://img.shields.io/github/repo-size/Geo-Linux-Calculations/gama-work)
![GitHub all releases](https://img.shields.io/github/downloads/Geo-Linux-Calculations/gama-work/total)
![GitHub](https://img.shields.io/github/license/Geo-Linux-Calculations/gama-work)

# GNU Gama

|   |   |
| - | - |
| ![logo](https://www.gnu.org/software/gama/g-logo.png) | GNU Gama is a Free Software package for geodesy and surveying, available under the under the GNU General Public License, version 3 or later. The Gama project was started in 1998, and in 2001 it became a GNU package. |

|   |   |
| - | - |
| ![Network](https://www.gnu.org/software/gama/gama-local.svg) | ![in Krizikova station](https://www.gnu.org/software/gama/2020-barta.png) |

## Description

[GNU Gama](https://www.gnu.org/software/gama/manual/index.html)
is a project dedicated to adjustment of surveying and
geodetic networks.  It is intended for use with traditional surveying
instruments, which are still in widespread use despite the advent of
Global Navigation Satellite Systems.  Traditional instruments are
necessary for mm-level precision in local areas, and for underground
or interior measurements.  Gama also has some support for adjustment
of networks containing GNSS observations.

Adjustment in local Cartesian coordinate systems is fully supported by
a command-line program gama-local that adjusts geodetic (free)
networks of observed distances, directions, angles, height
differences, 3D vectors and observed coordinates (coordinates with
given variance-covariance matrix). Adjustment in global coordinate
systems is supported only partly as a gama-g3 program.

## Documentation

As a GNU program, documenttion is available in texinfo format,
resulting in a pdf manual and info pages.

## Building gama

Note that the version number of the package is defined in
configure.ac.

### Prerequisites

Gama assumes a system complying with POSIX, and a compiler that
spports C++11.  Gama intentionally avoids C++14 or later constructs
for improved portability.

### Build and Test

Beyond that, the standard autotools instructions, available in
INSTALL, apply.  Note that Gama's tests may be run via "make test".

## More information

More information on GNU Gama can be found at

                 http://www.gnu.org/software/gama

This includes links to the project page on savannah, where one can
find mailinglists, a bug tracker, and source code repository.  GNU
Gama does not have any repository mirrors.

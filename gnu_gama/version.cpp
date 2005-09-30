/*  
    GNU Gama --- Geodesy and Mapping C++ library 
    Copyright (C) 1999, 2003, 2005  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: version.cpp,v 1.82 2005/09/30 08:28:52 cepek Exp $
 */


#include <gnu_gama/version.h>

namespace GNU_gama {

const char* GNU_gama_version  = "1.7.14";

const char* GNU_gama_compiler =
              #if   defined (__GNUC__)
              "GNU g++"             // g++ 3.3 / 3.4
              // #elif defined (__BORLANDC__) && (__linux__)
              // "kylix-bc++"          // 5.7
              #elif defined (__BORLANDC__)
              "win32-borland"       // 5.6
              #elif defined (_MSC_VER)
              "win32-msvc"          // 1300
              #else
              #error GNU_gama - has not been tested with your compiler
              #endif
              ;
}


/* GNU Gama uses James Clark's parser Expat for XML data processing
 *
 *    Expat is subject to the Mozilla Public License Version 1.1. 
 *    Alternatively you may use expat under the GNU General Public
 *    License instead.
 *
 *              ftp://ftp.jclark.com/pub/xml/expat.zip
 *
 * Normally GNU Gama is linked with Expat version 1.95.2 (or later)
 * shared library.  It is possible to compile and build Gama with old
 * expat version 1.1.  In such a case scripts for compiling GNU Gama
 * and linking the program gama-local expect Expat 1.1 library to be
 * in the same directory as GNU Gama

=============================================================================

1.7.14 2005-09-30

    - Stephane Kaloustian <stephane.kaloustian@laposte.net> translated
      *.lang files to French

    - Boris Pihtin <cyb@bendery.md> translated gama-local *.lang files
      to Russian

    - fixed bugs in printing adjustment results in gama-local

      Index: adjusted_unknowns.h
      ===================================================================
      RCS file: /cvsroot/gama/gama/gamalib/local/results/text/adjusted_unknowns.h,v
      retrieving revision 1.10
      diff -u -r1.10 adjusted_unknowns.h
      --- adjusted_unknowns.h	7 May 2005 18:06:20 -0000	1.10
      +++ adjusted_unknowns.h	29 Aug 2005 17:40:04 -0000
      @@ -78,8 +78,6 @@
       
                 if (b.free_xy() && b.index_x())
                   {
      -              int i = b.index_x();
      -              
                     out.width(IS->maxw_unk());
                     out << " " << " ";
                     out.width(IS->maxw_id());
      @@ -88,13 +86,13 @@
                     else
                       out << " ";
                     prev_id = point_id;
      -              Double mx = IS->unknown_stdev(i);
      -              Double my = IS->unknown_stdev(i+1);
      +              Double mx = IS->unknown_stdev(b.index_x());
      +              Double my = IS->unknown_stdev(b.index_y());
                     mp = sqrt(my*my+mx*mx);
                     out << '\n';
                     
                     out.width(IS->maxw_unk());
      -              out << i << " ";
      +              out << b.index_x() << " ";
                     out.width(IS->maxw_id());
                     if (b.constrained_xy())
                       out << "X" << " * ";
      @@ -102,7 +100,7 @@
                       out << "x" << "   ";
                     out.precision(5);
                     out.width(13);
      -              Double adj_x = b.x()+x(i)/1000;
      +              Double adj_x = b.x()+x(b.index_x())/1000;
                     out << b.x_0() << " ";
                     out.width(9);
                     out << (adj_x - b.x_0()) << " ";
      @@ -117,7 +115,7 @@
                     
                     out.flush();
                     out.width(IS->maxw_unk());
      -              out << (i+1) << " ";
      +              out << b.index_y() << " ";
                     out.width(IS->maxw_id());
                     if (b.constrained_xy())
                       out << "Y" << " * ";
      @@ -125,7 +123,7 @@
                       out << "y" << "   ";
                     out.precision(5);
                     out.width(13);
      -              Double adj_y = y_sign*(b.y()+x(i+1)/1000);
      +              Double adj_y = y_sign*(b.y()+x(b.index_y())/1000);
                     out << y_sign*b.y_0() << " ";
                     out.width(9);
                     out << (adj_y - y_sign*b.y_0()) << " ";
      @@ -140,8 +138,6 @@
                   }
                 if (b.free_z() && b.index_z())
                   {
      -              int i = b.index_z();
      -
                     if (!b.free_xy())
                       {
                         out.width(IS->maxw_unk());
      @@ -156,7 +152,7 @@
                     prev_id = point_id;
                     
                     out.width(IS->maxw_unk());
      -              out << i << " ";
      +              out << b.index_z() << " ";
                     out.width(IS->maxw_id());
                     if (b.constrained_z())
                       out << "Z" << " * ";
      @@ -164,18 +160,18 @@
                       out << "z" << "   ";
                     out.precision(5);
                     out.width(13);
      -              Double adj_z = b.z()+x(i)/1000;
      +              Double adj_z = b.z()+x(b.index_z())/1000;
                     out << b.z_0() << " ";
                     out.width(9);
                     out << (adj_z - b.z_0()) << " ";
                     out.width(13);
                     out << adj_z << " ";
      -              double mv = IS->unknown_stdev(i);
      +              double mz = IS->unknown_stdev(b.index_z());
                     out.precision(1);
                     out.width(7);
      -              out << mv << " ";
      +              out << mz << " ";
                     out.width(7);
      -              out << mv*kki;
      +              out << mz*kki;
                     out << "\n";
                   }
       
      Index: error_ellipses.h
      ===================================================================
      RCS file: /cvsroot/gama/gama/gamalib/local/results/text/error_ellipses.h,v
      retrieving revision 1.11
      diff -u -r1.11 error_ellipses.h
      --- error_ellipses.h	7 May 2005 18:06:20 -0000	1.11
      +++ error_ellipses.h	29 Aug 2005 17:40:05 -0000
      @@ -98,14 +98,15 @@
              for (PointData::const_iterator 
                     point=IS->PD.begin(); point!=IS->PD.end(); ++point)
                if ((*point).second.free_xy())
      -           if (int i = (*point).second.index_x())
      +           if ((*point).second.index_x())
                    {
                      const PointID point_id  = (*point).first;	     
                      out.width(IS->maxw_id());
                      out << point_id.c_str() << ' ';
                      
      -               Double my = IS->unknown_stdev(i);
      -               Double mx = IS->unknown_stdev(i+1);
      +               const LocalPoint& p = (*point).second; 
      +               Double my = IS->unknown_stdev(p.index_y());
      +               Double mx = IS->unknown_stdev(p.index_x());
                      
                      Double mp = sqrt(my*my+mx*mx);
                      if (mp < 1000)     
      @@ -172,8 +173,8 @@
                          out << bk << ' ';
                          
                          Double g  = 0;
      -                   Double dx = x( i );
      -                   Double dy = y_sign*x(i+1);
      +                   Double dx = x( p.index_x() );
      +                   Double dy = y_sign*x( p.index_y() );
                          Double p1 = (dx*cos(alfa) + dy*sin(alfa));
                          Double p2 = (dy*cos(alfa) - dx*sin(alfa));
                          if (ak > 0 && bk > 0 && bk > ak*1e-4) 


    - fixed bug in dataparser_adj.cpp

      Index: dataparser_adj.cpp
      ===================================================================
      RCS file: /cvsroot/gama/gama/gnu_gama/xml/dataparser_adj.cpp,v
      retrieving revision 1.3
      diff -r1.3 dataparser_adj.cpp
      60a61,67
      >   // .....  <adj-input-data>  ........................................
      > 
      >   init(s_gama_data, t_adj_input_data, 
      >        s_adj_input_data_1, s_adj_input_data_5, 0,
      >        &DataParser::adj_input_data, 0, &DataParser::adj_input_data,
      >        s_adj_input_data_4);
      >

    - scripts/gnu_gama_dep.cpp: updated parameters for projects build
      under MS Visual C++ 2005 Express Edition


    - fixed possible undefined behavior of bad regularization during
      revision of points

      Index: gamalib/local/network.cpp
      ===================================================================
      RCS file: /cvsroot/gama/gama/gamalib/local/network.cpp,v
      retrieving revision 1.20
      diff -u -r1.20 network.cpp
      --- gamalib/local/network.cpp	7 May 2005 18:06:19 -0000	1.20
      +++ gamalib/local/network.cpp	20 Jun 2005 20:31:00 -0000
      @@ -610,15 +610,40 @@
       
       int LocalNetwork::null_space()
       {
      -  try { 
      -    vyrovnani_(); 
      -  } 
      -  catch(const MatVecException& vs) {
      -    if (vs.error != GNU_gama::Exception::BadRegularization) throw;
      -  } 
      +  try 
      +    { 
      +      vyrovnani_(); 
      +    } 
      +  catch(const MatVecException& vs) 
      +    {
      +      if (vs.error != GNU_gama::Exception::BadRegularization) throw;
      +      
      +      for (Index i=1; i<=sum_unknowns(); i++)
      +        if (lindep(i))
      +          {
      +            const char    type = unknown_type(i);
      +            const PointID id   = unknown_pointid(i);
      +            
      +            LocalPoint& p = PD[id];
      +            if (type == 'X' || type == 'Y' || type == 'R' )
      +              {
      +                p.unused_xy();
      +                removed(id, rm_singular_xy );
      +              }
      +            else if (type == 'Z' )
      +              {
      +                p.unused_z();
      +                removed(id, rm_missing_z );
      +              }
      +            
      +            return null_space();
      +          }
      +    } 
      +  
         return defect();
       }

       
    - fixed a bug in local network linearization (possible impact of
      this bug used to be adjusted by various data checks in 'gama-local')

      Index: gamalib/local/linearization/xyzdiff.h
      ===================================================================
      RCS file: /cvsroot/gama/gama/gamalib/local/linearization/xyzdiff.h,v
      retrieving revision 1.3
      diff -u -r1.3 xyzdiff.h
      --- gamalib/local/linearization/xyzdiff.h	7 May 2005 18:06:20 -0000 1.3
      +++ gamalib/local/linearization/xyzdiff.h	19 Jun 2005 11:11:03 -0000
      @@ -96,14 +96,14 @@
         rhs = (obs->value() - df)*1e3;
       
         size = 0;
      -  if (spoint.free_xy())
      +  if (spoint.free_z())
           {
             if (!spoint.index_z()) spoint.index_z() = ++maxn;
             index[ size ] =  spoint.index_z();
             coeff[ size ] = -1;
             size++;
           }
      -  if (tpoint.free_xy())
      +  if (tpoint.free_z())
           {
             if (!tpoint.index_z()) tpoint.index_z() = ++maxn;
             index[ size ] =  tpoint.index_z();



1.7.13 2005-06-13

    - added Cholesky decomposition as another adjustement algorithm
      (template class AdjCholDec)

    - removed unused method AdjBase<>::trwr()

    - FSF's new address

      find . -type f -exec sed -ie 's/59 Temple Place, Suite 330/51 Franklin
      Street, Fifth Floor/;s/02111-1307/02110-1301/;' {} ';'

    - three classes BaseOLS, OLSsvd and OLSgso (namespace GaMaLib,
      directory gamalib/ls) renamed to AdjBase, AdjSVD and AdjGSO and
      placed into the namespace GNU_gama (directory gnu_gama/adj).

      From now there is no function, nor class, defined in the old
      namespace GaMaLib referenced from the namespace GNU_gama.

      Rather obsolete namespace GaMaLib is kept for the backward
      compatibility and is mainly dedicated to the command line
      program gama-local. 

    - matrix/vector template library moved to the directory 'matvec'
     


1.7.12 2005-03-27

    - MatVec templete library moved to namespace GNU_gama

        s/gMatVec/GNU_gama/g

    - a bug in the output of coordiantes in the XML format

        diff -r1.6 coordinates.h
        68c68,69
        <         if (!b.active_xy()) continue;
        ---
        >         if (!b.active_xy() && !b.active_z()) continue;
        > 

    - a bug in the second GSO constructor

        <  *  $Id: version.cpp,v 1.82 2005/09/30 08:28:52 cepek Exp $
        ---
        >  *  $Id: version.cpp,v 1.82 2005/09/30 08:28:52 cepek Exp $
        80,83c80
        <   GSO(Mat<Float, Exc>& a, Index m, Index n)
        <     : pA(0), M(0), N(0), sc(true), tol_(0),
        <     minx_n(0), minx(0), clist(0), rlist(0) 
        <   { 
        ---
        >   GSO(Mat<Float, Exc>& a, Index m, Index n) : sc(true), tol_(0) { 
        122,125d118
        <   template <typename T> inline const T ABS(const T& x)
        <     {
        <       return (x >= T(0)) ? x : -x ;
        <     }


1.7.11 2004-09-02

    - simplified version of GNU_gama::List<> template

    - observation lists in gamalib/local/median rewritten to conform
      the standard C++ library

    - a lot of changes based on warnings reported by g++-3.4

    - a bug in gkfparser.cpp reported by Jan Pytel

            kwak$ diff gamalib/xml/gkfparser.cpp gkfparser.cpp.~1.16.~
            58,59c58
            <              state == state_vectors_cov ||
            <              state == state_hdiffs_cov )
            ---
            >              state == state_vectors_cov )

    - all occurrences of keyword 'class' in template type definitions
      replaced by 'typeneme' (new gmatvec version 0.9.23)

    - in the template class ObservationData (gnu_gama/obsdata.h) the
      data member CL was renamed to 'clusters'

    - first draft of zenith and azimuth angles in gama-g3 (NOT DEBUGGED YET!)

    - gnu_gama/xml/dataparser.cpp split to three files dataparser.cpp,
      dataparser_adj.cpp and dataparser_g3.cpp (corresponding to the
      three functional modules)


1.6.02 2004-05-20      ******  stable version 1.6  ******

    - CVS tag gama-1-6 moved to point to the current development branch

                        cvs tag -F gama-1-6

                            **********************  


1.7.10 2004-05-01

    - Catalan   translation of gama-local by Diego Berge
 
    - Hungarian translation of gama-local by Zoltan Faludi



    - files statan.{h|cpp} and rand.{h|cpp} moved from gamalib to
      gnu_gama

    - bug in orientation of semimajor axes of error ellipses for
      inconsistent coordinate systems (internally changed all y
      coordinates to -y); see  GaMaLib::Network::ErrorEllipses()


1.7.09 2004-04-07

    - added new attribute zenith-angle-stdev="..." in the tag
      <points-observations /> in gama-local input XML data

    - added new parameter update-constrained-coordinates="yes | no"
      into XML tag <parameters ... /> (input data for gama-local)
      and all related changes in network.{h|cpp} and gkfparser.{h|cpp}

    - added static data member 'bool gons' in GaMaLib::Observation
      class to enable simple selection of output format in virtual
      functions GaMaLib::Observation::write (this is a dirty hack but
      I want to minimize changes in old GaMaLib)

      The value of Observation::gons is set in gama-local on startup
      by GaMaLib::Network functions set_gons() and/or set_degrees()

      Implicit value of Observation::gons is true

    - three bugs removed by Jan Pytel (approx. coordinates and new 360
      degrees output):

         * accord.cpp 
         * outlying_abs_terms.h 
           reduced_observations.h (reported by Zoltan Faludi)

    - a bug in "active covariance matrix" in obsdata.h

         --- obsdata.h-bug	Thu Mar 18 11:57:06 2004
         +++ obsdata.h	Thu Mar 18 11:58:08 2004
         @@ -345,8 +345,10 @@
                const Index i_size = observation_list.size();
                Index active_band  = covariance_matrix.bandWidth();
          
         -      if (N && N-1 < active_band) 
         -        active_band = N-1;
         +      if (N)
         +        {
         +          if (N-1 < active_band) active_band = N-1;
         +        }
                else
                  active_band = 0;
 
    - a bug in gkfparser.cpp reported by Jan Pytel (wrong error
      message for "bad zenith anlge")

    - gama-local: support for input of angular observables in 360
      degrees (implicitly 400 grades)

    - gama-local: optional output of adjustment results in degrees
      (grades are used implicitly)

    - scripts/gama_dep.cpp (1.01): added conditional usage of option
      '-pipe' in makefiles generated for GNU Linux platform

      bc++ compiler from Kylix3 can now use the makefiles generated in
      the project for GNU compilers



1.6.01 2004-03-03      ******  stable version 1.6  ******

    - CVS tag gama-1-6 moved to point to the current development branch

                        cvs tag -F gama-1-6

    - added observation types Dimension and XYZ ("observed" coordinates)

    - added observation dimensions into Cluster::activeCov (activeCount 
      renamed to activeObs)

                            **********************  


1.7.08 2004-01-26

   - visitor pattern used in description of mathematical model (model.h)

   - into the template class Cluster added function activeNonz()
     returning the size of an active covariance matrix (number of
     nonzero elements)

   - removed conditional compile for _MSC_VER in gnu_gama/list.h

   - ftp download: development branch 1.7    ftp://alpha.gnu.org/gnu/gama/
                   stable branch 1.6         ftp://ftp.gnu.org/gnu/gama/


1.7.07 2003-11-06

   - in template class GNU_gama::ObservationData

     a) added iterators  
     b) removed template function for_each (and all related function
        objects rewritten using iterators)

   - added check for valid covariance matrix in virtual functions
     derived from GaMaLib::Observation::write(ostream&, bool)

   - added support for computation of approximate coordinates from
     vector data by Jan Pytel

   - Makefiles changed to enable Gama to use expat 1.95.2 (or later)
     XML parsing library as shipped with Debian GNU/Linux.  

     Old version 1.1 of expat is still available if for any reason
     shared library of expat was not available (currently expat
     version 1.1 is used for Gama build under Windows)


1.7.06 2003-08-09

   - removed a bug in definition of regularization condition in
     LocalNetwork::project_equations() in file network.cpp.

   - gmatvec 0.9.22 (tested with g++ 3.3)


1.7.05 2003-07-24

   - removed two bugs in gama-local found by Jan Pytel: a)

        *** gama/gamalib/local/linearization/xyz.h.bug
        --- gama/gamalib/local/linearization/xyz.h
        ***************
        *** 78,84 ****
             rhs = (obs->value() - point.z())*1e3;
          
             size = 0;
        !    if (point.free_xy())
             {
                if (!point.index_z()) point.index_z() = ++maxn;
                index[ size ] = point.index_z();
        --- 78,84 ----
             rhs = (obs->value() - point.z())*1e3;
          
             size = 0;
        !    if (point.free_z())
             {
                if (!point.index_z()) point.index_z() = ++maxn;
                index[ size ] = point.index_z();

     b) in the case inconsistent coordinates sign of Y coordinates was
     internally changed but not for "observed coordinates" and/or vectors.


   - from this version g++-3.0 (or higher) is necessary (g++ version 2.95.4
     would not compile Gama). 

     depreated name std::ios has been replaced by C++ standard std::ios_base

   - first testing version of vectors in gama-g3


1.7.04 2003-05-11

   - classes BaseParser and Dataparser moved from namespace GaMaLib to
     namespace GNU_gama.


1.7.03 2003-03-29

   - first draft of implementation of (g3) classes for computing
     derivatives of Observations by Parameters

   - files gamalib/version.[h|cpp] (ChangeLog) moved to directory gnu_gama

   - bash scripts for generating makefiles replaced by C++ program
     scripts/gnu_gama_dep.cpp

     * for building makefiles under win32 see scripts/win32-makefiles.bat  

     * gamalib.a renamed to libgama.a

   - adjustments needed by MSVC compiler (in files gnu_gama/list.h and
     gamalib/cluster.h)


1.7.02 2003-03-13

   - added template class PointBase (gnu_gama/pointbase.h)

   - added template class template <class T> class List<T*> (file
     gnu_gama/list.h); shall replace all pointer lists used in Gama

   - removed bug in template BlockDiagonal (sparse/sbdiagonal.h)
     repoted by Jan Bilek

   - removed bug in SparserMatrix (gnu_gama/sparse/smatrix.h) reported
     by Jan Bilek; not enough memory was allocated for rptr array in
     SparseMatrix(const SparseMatrix* sm)

   - added missing 'typenames' into gnu_gama/obsdata.h


1.7.01 2003-02-28 

   - observation data structures (formerly defined in gamalib/cluster.*
     and gamalib/local/gamadata.*) rewritten as template functions 
     and moved into gnu_gama/obsdata.h

   - directory `sparse' moved from `gamalib' to `gnu_gama', added
     template class SparseVec (added directory <gnu_gama/...> for
     processing in scripts/gamalaib_dep.cpp)


1.7.00 2003-02-16

     #####################################################################
     #                                                                   #
     #  unstable version 1.7 (main trunk at CVS savannah.gnu.org)        #
     #                                                                   #
     #####################################################################

   - for the new development branch (g3) is introduced new namespace
     GNU_gama; this name is going to replace old name GaMaLib
     (similarly directory gnu_gama is going to replace directory
     gamalib)

   - added output encoding option into gama-local program. Output encoding
     is handled by class GNU_gama::OutStream

   - removed bugs in functions `underline' and `set_width' (bad text
     length in utf-8 encoding)


1.6.00 2003-02-01

     #####################################################################
     #                                                                   #
     #  stable version 1.6 (branch gama-1-6 at CVS savannah.gnu.org)     #
     #                                                                   #
     #####################################################################

   - at CVS savannah.gnu.org stable version 1.6 (adjustment in local
     coordinate system, program gama-local) has been started as a
     separate development branch with label gama-1-6; development of
     unstable version 1.7 (aimed to adjustment in geocentric
     coordinates, program gama-g3) is going to continue in the CVS
     main trunk.

    - Jan Pytel: reduction of observations with nonzero heights of
      instrument and/or target in gama-local


1.5.08 2002-01-18

    - Jan Pytel: bug in zangle.h  (missing test for coordinates xy)

    - added data object "AdjInputDataObject" into DataParser

    - first draft of classes DataParser, AdjInputData and Adj


1.5.07 2002-12-18

    - added missing declarations "using namespace std;" in various files
      (apart from g++ 2.95.4 tested with bcc32 5.6 and cl 13.00.9466).

         Note: Makefiles for win platform are not converted using unix2dos

    - call to std:fmod() in g2d_helper.h replaced with expresion "x%2"


1.5.06 2002-12-14

    - added reductions for basic observation types (Jan Pytel)

    - Dutch translation of output texts produced by gama-local (John Dedrum)


1.5.05 2002-11-21

    - removed bug in text output of fixed coordinates in gama-local
      reported by Carl Verheyen (in the case of "inconsistent
      coordinate system" internally changed sign of y coordinates was
      not transformed back in the output).  File: fixed_points.h

    - gmatvec-0.9.21 (removed forgotten calls to fabs() )

    - some minor changes in scripts/GaMaLib_archive to reflect new
      archive file names in format 'gama-xx.yy.zz.tar.gz'


1.5.04 2002-10-25

    - updated documantation (added references to 'gama-local')

    - removed command line program 'gama' 


1.5.03 2002-10-24

    - tar archive file names changed from 'gamalib-xx.yy.zz.tar.gz' to
      'gama-xx.yy.zz.tar.gz'

    - class Point renamed to LocalPoint (file local/point.h renamed to
      local/lpoint.h)

    - command line program 'gama' renamed to 'gama-local' (all previous
      scripts needed for building 'gama' are left in 1.5.03 archive
      tar and will be removed in a next version)

    - general parts of GKFparser moved into the new class BaseParser.
      Class BaseParser is public base class to GKFparser and a new
      GaMa XML parser DataParser

    - added a test if dos2unix utility is available into
      scripts/Build_GaMa (Jan Pytel)


1.5.02 2002-09-29

   - support for the Finish language

   - added scripts/kylix.sh for compilation GaMa with bc++ compilere
     from Kylix3 Open Edition under Linux


1.5.01 2002-09-15

   - new version of BandMat<> class (gmatvec 0.9.20)

   - initial version of new class BlockDiagonal (in gamalib/sparse)
     for symmetric block diagonal matrix (planned for new adjustment
     model)

   - expat source included into gamalib archive as a convenience for
     GaMa users

   - initial draft of the Ellipsoid class and a helper program
     ellipsoids_xml.cpp for generating files ellipsoids.[h|cpp] from
     xml/ellipsoids.xml

   - merged changes from 1.4.01:

     -- changes concerning heights of instrument  and/or reflector
     -- removed unused public data member `tag' from the class Cluster
        (in files cluster.h and observation.[h|cpp])

   - removed bug reported by Jan Bilek in utf8_cp1250(char *buf); file
     gamalib/xml/encoding.cpp:

         310c310
         <       iso_8859_2_unicode((int*)tab);
         ---
         >       cp1250_unicode((int*)tab);


1.5.00 2002-06-11

     #####################################################################
     #                                                                   #
     #  unstable version 1.5 (main trunk at CVS savannah.gnu.org)        #
     #                                                                   #
     #####################################################################

   - unstable version 1.5 is aimed at adjustment of geodetic network
     in geocentric coordinate system. Apart from this comment, version
     1.5.00 is identical to 1.4.00.


1.4.00 2002-06-11

     #####################################################################
     #                                                                   #
     #  stable version 1.4 (branch gama-1-4 at CVS savannah.gnu.org)     #
     #                                                                   #
     #####################################################################

   - at CVS savannah.gnu.org stable version 1.4 (adjustment in local
     coordinate system) has been started as a separate branch with label
     gama-1-4; development of unstable version 1.5 (aimed to adjustment
     in geocentric coordinates) is going to continue in the CVS main
     trunk.


1.3.39 2002-06-09

   - various bugs reported by g++ 3.0.4 (Jan Pytel); files
     observation.h, g2d_coordinates.cpp, statan.cpp, write.cpp.
     
   - a bug in helper program gamalib_dep.cpp (Jan Pytel)

   - some minor changes in makefiles; makefiles and language.[h|cpp] 
     added to CVS repository

   - directory gamalib/gamaxml renamed to gamalib/xml


1.3.38 2002-05-29

   - class Point : added functions x_0(), y_0(), z_0() and set_xyz_0()
     used for correct output of dx/dy/dz corrections in the case of
     iterated adjustment (output of coordinates corrections for the
     initial approximation and not to the last iteration).

   - removed directory gamalib/doc and files scripts/build-doc-html and
     build scripts/gamalib2xhtml*

     automaticaly generated html files for browsing gamalib sources were
     neither maintained nor used


1.3.37 2002-05-24

   - new attributes bs="..." and fs="..." in the definition of <angle />

     Chuck Ghilani pointed out that XML description of angles was not
     natural; thus attribute `to' was renamed to `bs' (backsight) and
     `rs' was renamed to `fs' (foresight)

     Former attributes `to' and `rs' of the tag <angle /> was left as
     an undocumented feature in the class  GKFparser

   - function Angle::rs() was renamed to Angle::fs() and added new
     function Angle::bs() which is a synonym for Angle::to()


1.3.36 2002-05-15

   - in the class GKFparser implicitly all covariance matrices are
     checked for positive-definiteness. This test can be suppressed
     if the function GKFparser::check_covariances(false) is called.
     
     This dark feature was introduced to GKFparser by Jan Pytel. If
     you use it you go on your own risk. You have been warned!


1.3.35 2002-04-06

   - acord.cpp inproved by Jan Pytel (removed unnecessary iterations)


1.3.34 2002-04-05

   - approximate coordinates are computed in iterations if necessary
     (gamalib/local/acord.cpp); max. number of iterations is set to 5.


1.3.33 2002-04-03

   - new scripts for generating Makefiles (inspired by qmake Makefiles
     and Qt project files). All platform dependent environment variables
     are defined in a single file scripts/platforms.defs.


1.3.32 2002-03-19

   - gmatvec 0.9.17 (a bug in BandMat::cholDec(); Jan Pytel: Cholesky
     decomposition failed if the first element of the matrix was zero)
   
   - gmatvec 0.9.16 (added iterators to Mat<> / Vec<> classes)


1.3.31 2001-12-21

   - in scripts/make-linux2borland.sed added parameters needed by
     Rocinante and a minor change in scripts/Build_GaMa (Jan Pytel)

   - gmatvec 0.9.15: a bug in SymMat::invert() reported by Leos Mervart;
     in the case of dimension==1 inversion was computed twice
     and thus nothing happened (missing return statement)

   - a bug in computation of approximate coordinates reported by Jiri
     Vesely; if there were two neighbouring directions with identical
     targets, gama reported exception and stopped (the exception was
     thrown by the Angle constructor).

     From 1.3.31 we allow for angles left and right targets to be
     identical, surely this is not an realistic case but it's useful
     in g2d_point.h:67 and should not cause a trouble elsewhere


1.3.30  2001-12-07

   - a bug in ObservationData::deepCopy found by Jan Pytel (covariance
     matrices were not copied)

       diff -r1.1 gamadata.cpp
       59a60
       >       current->covariance_matrix = (*ci)->covariance_matrix;


1.3.29  2001-12-02

   - GNU GaMa sources installed at Savannah CVS repository

   - old change logs 

               from    version 1.3.28  2001-11-30
               back to         0.1.00  1998-01-19 

     are recorded in the file ChangeLog.1

*/












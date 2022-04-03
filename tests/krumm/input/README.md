# Geodetic Network Adjustment Examples

*Original example data files by Friedhelm Krumm, ```Geodetic Network
Adjustment Examples```, Geodätisches Institut Universität Stuttgart,
https://www.gis.uni-stuttgart.de, Rev. 3.5 January 20, 2020.*

All data files have extension ```*.dat``` and are stored
in subdirectories 1D, 2D and 3D. Files are coded in
the input format used in the textbook (*Krumm format*).
Each file defines one surveying network and is composed of a series of
data definition sections. A section starts with the sections header
followed by the data definition, for example

```
[Coordinates]
%    x    y         H
1    0 1000
2 1000 1000
3    0    0
4 1000    0
```

Network definition text files processed by lines, all text starting with
% is a comment and is ignored during tha data file processing.
Empty lines have no special meaning, but sections are typically ended with
one ore more empty lines for better reading.

The files in *Krumm format* can converted to the XML input data format of
```gama-local``` by the utility ```krumm2gama-local``` distributed within
the GNU Gama project.

Each ```*.dat``` file in this archive is accompanied by the ```*.adj``` file
containing adjusted coordinates, as published by Friedhelm Krumm.
Adjusted coordinates files were added to this archive to enable
unit testing of ```krum2gama-local``` conversion.

## Example

Input network definition in *Krum format*

```
%
%  Charles D. Ghilani (2010): Adjustment Computations.
%  Spatial Data Analysis. 5th Edition, Problem 21.10, S. 459
%
[Project]
Fix Distance-Angle network

[Quelle]
Ghilani Charles D. (2010): Adjustment Computations. Spatial Data
Analysis. Fifth Edition, John Wiley & Sons, Inc., ISBN
978-0-470-46491-5, Ex. 21.10, pp. 459

[Coordinates]
%    x    y         H
%  taken from book
A 5600.544 4966.236
B 6061.624 8043.173
C 9787.823 8038.529
D 9260.886 4843.911

%
% Graphics parameter
[Graphics]
ellpos:10000,4800,10        % xebar,yebar,lmstab
scale:2000                  % scale (for error ellipses)
axlims:5500,10500,4500,8500 % axlims for axis
legpos:Best

%
%  Datum specification (if applicable, with standard deviation [m]
%  or variance-covariance matrix [m^2] (in case of a dynamic network)
[Datum]
fix xA yA xB yB

%
%  Standard deviation of unit weight with unit (a priori standard deviation)
[Sigma0]
1

%
%  Angle observations [unit] with standard deviations [unit]
%
[Winkel,dms,s]
A B C 45°12'34" 2.1
A C D 38°10'54"
B C D 44°55'43"
B D A 53°31'23"
C D A 44°21'59"
C A B 36°20'26"
D A B 43°06'11"
D B C 54°22'00"

%
%  Distances [m] with constant standard deviation (sigma_c) [m],
%  distance dependent standard deviation (sigma_s) [m]
%  {sigma^2=sigma_c^2 + s[m]*sigma_s^2}
[Distances]
   A   B 3111.291 0.010
   B   C 3726.220 0.012
   C   D 3237.783 0.010
   D   A 3662.372 0.012
   A   C 5193.471 0.016
   B   D 4524.471 0.014
```

converted to XML by ```krum2gama-local```

```
<?xml version="1.0" ?>
<gama-local xmlns="http://www.gnu.org/software/gama/gama-local">
<network axes-xy="en" angles="left-handed">

<!-- This example is based on published material Geodetic Network Adjustment
     Examples by Friedhelm Krumm, Geodätisches Institut Universität Stuttgart,
     Rev. 3.5, January 20, 2020
-->

<description>
Fix Distance-Angle network

Ghilani Charles D. (2010): Adjustment Computations. Spatial Data
Analysis. Fifth Edition, John Wiley &amp; Sons, Inc., ISBN
978-0-470-46491-5, Ex. 21.10, pp. 459
</description>

<!-- sigma-apr/sigma0 gon to cc -->
<parameters
   sigma-apr = "1"
   conf-pr   = " 0.95 "
   tol-abs   = " 1000 "
   sigma-act = "aposteriori"
   algorithm = "gso"
   cov-band  = "-1"
/>

<points-observations>

<point id='A' x='5600.544' y='4966.236' fix='xy' />
<point id='B' x='6061.624' y='8043.173' fix='xy' />
<point id='C' x='9787.823' y='8038.529' adj='xy' />
<point id='D' x='9260.886' y='4843.911' adj='xy' />

<obs>
<distance from="A" to="B" val="3111.291" stdev="10.000000" />
<distance from="B" to="C" val="3726.220" stdev="12.000000" />
<distance from="C" to="D" val="3237.783" stdev="10.000000" />
<distance from="D" to="A" val="3662.372" stdev="12.000000" />
<distance from="A" to="C" val="5193.471" stdev="16.000000" />
<distance from="B" to="D" val="4524.471" stdev="14.000000" />
</obs>

<obs>
<angle from="A" bs="B" fs="C" val="45-12-34" stdev="2.1" />
<angle from="A" bs="C" fs="D" val="38-10-54" stdev="2.1" />
<angle from="B" bs="C" fs="D" val="44-55-43" stdev="2.1" />
<angle from="B" bs="D" fs="A" val="53-31-23" stdev="2.1" />
<angle from="C" bs="D" fs="A" val="44-21-59" stdev="2.1" />
<angle from="C" bs="A" fs="B" val="36-20-26" stdev="2.1" />
<angle from="D" bs="A" fs="B" val="43-06-11" stdev="2.1" />
<angle from="D" bs="B" fs="C" val="54-22-00" stdev="2.1" />
</obs>

</points-observations>

</network>
</gama-local>
```

## Changelog

Changes to the original files are following:

* Adjusted coordinates results were added to some example (files .adj)
  to enable check of gama-local adjustement results (in XML format).

* All example ```*.dat``` files were explicitly transcoded to UTF-8.
  German ISO-8859-2 accents may cause the error in processing of the
  resulting XML format.

  ```recode ISO-8859-2..UTF-8  filename```

* Long lines in sections ```[Project]``` and ```[Source]``` were reformated
  to fit in A4 raw print. (Sections [```Project]``` and ```[Source]```
  are used as the description text in gama-local --text output).

* Also some files contained strange characters (```#```) and were edited
  manually.

  ```emacs `rgrep \# | sed s/:.*// | sort` &```

* 1D/Niemeier_Height_free : fixed [Datum] fre --> free

* Fixed DMS angles format in 2D/Ghilani16_1_Traverse/Ghilani16_1_Traverse.dat

```
 %  Angle observations [unit] and standard deviations [unit]
 %
 [Angles,dms,s]
-R Q U 240°   30" % theta1=240°   +- 30"
-U R S 150°   30" % theta2=150°   +- 30"
-S U T 240°1' 30" % theta3=240°1' +- 30"
\ No newline at end of file
+R Q U 240°0'0" 30" % theta1=240°   +- 30"
+U R S 150°0'0" 30" % theta2=150°   +- 30"
+S U T 240°1'0" 30" % theta3=240°1' +- 30"
```

* Fix a bug in input data

```
-Campus    2416898.227 387602.294
+% Campus  2416898.227 387602.294  a bug in approximate coordinates
+Campus    2416892.670 387603.450
```
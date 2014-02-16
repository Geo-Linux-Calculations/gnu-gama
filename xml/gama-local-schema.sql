/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2010 Ales Cepek <cepek@gnu.org>, 2010 Jiri Novak
    <jiri.novak@petriny.net>, 2010 Vaclav Petras <vaclav.petras@fsv.cvut.cz>,
    2013 Ales Cepek <cepek@gnu.org>

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


create table gnu_gama_local_schema_version (
   major integer default 1 not null check (major = 1),
   minor integer default 1 not null check (minor = 1),
   primary key (major, minor)
);
insert into gnu_gama_local_schema_version (major, minor) values (1, 1);

create table gnu_gama_local_configurations (
   conf_id   integer primary key,
   conf_name varchar(60) not null unique,
   sigma_apr double precision default 10.0 not null check (sigma_apr > 0),
   conf_pr   double precision default 0.95 not null check (conf_pr > 0 and conf_pr <1),
   tol_abs   double precision default 1000 not null check (tol_abs > 0),
   sigma_act varchar(11) default 'aposteriori' not null check (sigma_act in ('apriori', 'aposteriori')),
   update_cc varchar(3) default 'no' not null check (update_cc in ('yes', 'no')),
   axes_xy   varchar(2) default 'ne' not null check (axes_xy in ('ne', 'sw', 'es', 'wn', 'en', 'nw', 'se', 'ws')),
   angles    varchar(12) default 'left-handed' not null check (angles in ('left-handed', 'right-handed')),
   ang_units integer default 400 not null check (ang_units in (400, 360)),
   cov_band  integer default -1 not null check (cov_band >= -1),
   algorithm varchar(12) check (algorithm in ('svd', 'gso', 'cholesky', 'envelope')),
   epoch     double precision,
   latitude  double precision,
   ellipsoid varchar(20)
);

create table gnu_gama_local_descriptions (
   conf_id   integer references gnu_gama_local_configurations,
   indx      integer check (indx >= 1),
   text      varchar(1000) not null,	     
   primary key (conf_id, indx)
);

create table gnu_gama_local_points (
   conf_id   integer references gnu_gama_local_configurations,
   id        varchar(80),
   x         double precision,   
   y         double precision,   
   z         double precision,
   txy       varchar(11) check (txy in ('fixed', 'adjusted', 'constrained')),   
   tz        varchar(11) check (tz  in ('fixed', 'adjusted', 'constrained')),
   primary key (conf_id, id)
);

create table gnu_gama_local_clusters (
   conf_id   integer references gnu_gama_local_configurations,
   ccluster  integer check (ccluster > 0),
   dim       integer not null check (dim > 0),
   band      integer not null,
   tag       varchar(18) not null check (tag in ('obs', 'coordinates', 'vectors', 'height-differences')),
   check (band between 0 and dim-1),
   primary key (conf_id, ccluster)
);
-- upper triangular variance-covariance band-matrix (0 <= bandwidth < dim)

create table gnu_gama_local_covmat (
   conf_id   integer,
   ccluster  integer,
   rind      integer check (rind > 0),
   cind      integer check (cind > 0),
   val       double precision not null,       
   foreign key (conf_id, ccluster) references gnu_gama_local_clusters,
   primary key (conf_id, ccluster, rind, cind)
);

create table gnu_gama_local_obs (
   conf_id   integer,
   ccluster  integer,
   indx      integer check (indx > 0),
   tag       varchar(10) check (tag in ('direction', 'distance', 'angle', 's-distance', 'z-angle', 'azimuth', 'dh')),
   from_id   varchar(80) not null,
   to_id     varchar(80) not null,
   to_id2    varchar(80),
   val       double precision not null,
   stdev     double precision,
   from_dh   double precision,
   to_dh     double precision,
   to_dh2    double precision,
   dist      double precision, -- dh dist 
   rejected  integer default 0 not null,
   primary key (conf_id, ccluster, indx),
   foreign key (conf_id, ccluster) references gnu_gama_local_clusters,
   check (tag <> 'angle' or to_id2 is not null),
   check (tag = 'dh' or (tag <> 'dh' and dist is null))
);

create table gnu_gama_local_coordinates (
   conf_id   integer,
   ccluster  integer check (ccluster > 0),
   indx      integer check (indx > 0),
   id        varchar(80),
   x         double precision,   
   y         double precision,   
   z         double precision,
   rejected  integer default 0 not null,
   foreign key (conf_id, ccluster) references gnu_gama_local_clusters,
   primary key (conf_id, ccluster, indx)
);

create table gnu_gama_local_vectors (
   conf_id   integer,
   ccluster  integer check (ccluster > 0),
   indx      integer check (indx > 0),
   from_id   varchar(80),
   to_id     varchar(80),
   dx        double precision,   
   dy        double precision,   
   dz        double precision,   
   from_dh   double precision,
   to_dh     double precision,
   rejected  integer default 0 not null,
   foreign key (conf_id, ccluster) references gnu_gama_local_clusters,
   primary key (conf_id, ccluster, indx)
);

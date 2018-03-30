#include <gnu_gama/local/test_linearization_visitor.h>
#include <cmath>

void GNU_gama::local::TestLinearizationVisitor::visit(Distance* obs)
{
  double ds, dd;
  computeBearingAndDistance(obs, ds, dd);
  mer  = obs->value() + v(i)/1000;
  mer -= dd;
  mer *= 1000;
  pol  = mer;
}

void GNU_gama::local::TestLinearizationVisitor::visit(Angle* obs)
{
  double ds, dd;
  computeBearingAndDistance(obs, ds, dd);
  
  double sx, sy, cx, cy;
  computeFromTo(obs, sx, sy, cx, cy);
  
  const LocalPoint& cil2 = IS->PD[obs->fs() ];
  double cy2 = cil2.y() + x(cil2.index_y())/1000;
  double cx2 = cil2.x() + x(cil2.index_x())/1000;
  double ds2, dd2;
  GNU_gama::local::bearing_distance(sy, sx, cy2, cx2, ds2, dd2);
  mer = obs->value() + v(i)*CC2R - ds2 + ds;
  while (mer >  M_PI) mer -= 2*M_PI;
  while (mer < -M_PI) mer += 2*M_PI;
  pol  = mer*std::max(dd,dd2)*1000;
  mer *= R2CC;
}

void GNU_gama::local::TestLinearizationVisitor::visit(S_Distance* obs)
{
  double dx {0}, dy {0}, dz {0};
  auto f = [&](const PointID& pid)
    {
      const LocalPoint& point = IS->PD[pid];
      dx += point.x();
      dy += point.y();
      dz += point.z();
      if (point.free_xy())
	{
	  dx += x(point.index_x())/1000;
	  dy += x(point.index_y())/1000;
	}
      if (point.free_z())
	{
	  dz += x(point.index_z())/1000;
	}
      dx *= -1;
      dy *= -1;
      dz *= -1;
    };
  
  f(obs->from());
  f(obs->to());
  double slope = std::sqrt(dx*dx + dy*dy + dz*dz);
  
  mer  = obs->value() + v(i)/1000;
  mer -= slope;
  mer *= 1000;
  pol  = mer;
}

void GNU_gama::local::TestLinearizationVisitor::visit(H_Diff*)     { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Z_Angle*)    { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(X*)          { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Y*)          { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Z*)          { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Xdiff*)      { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Ydiff*)      { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Zdiff*)      { mer = 0; pol = 0; }
void GNU_gama::local::TestLinearizationVisitor::visit(Azimuth*)    { mer = 0; pol = 0; }


void GNU_gama::local::TestLinearizationVisitor::visit(Direction* obs)
{
  double ds, dd;
  computeBearingAndDistance(obs, ds, dd);
  double orp = obs->orientation();
  mer = obs->value() + v(i)*CC2R + orp + x(obs->index_orientation())*CC2R;
  mer -= ds;
  while (mer >  M_PI) mer -= 2*M_PI;
  while (mer < -M_PI) mer += 2*M_PI;
  pol  = mer*dd*1000;
  mer *= R2CC;
}


bool GNU_gama::local::TestLinearization(GNU_gama::local::LocalNetwork* IS,
					double max_pyx,
					double max_dif)
{
  using namespace std;
  using namespace GNU_gama::local;
  
  bool test  = false;     // result of bad linearization test
  
  // difference in adjusted observations computed from residuals and
  // from adjusted coordinates
  // ===============================================================
  {
    const int M = IS->sum_observations();

    Vec dif_m(M);   // difference in computation of adjusted observation
    Vec dif_p(M);   //               corresponds to positional shift

    const Vec& v = IS->residuals();
    const Vec& x = IS->solve();

    TestLinearizationVisitor testVisitor(IS, v, x);

    for (int i=1; i<=M; i++)
      {
        dif_m(i) = 0;
        dif_p(i) = 0;
        // if (IS->obs_control(i) < 0.1) continue;  // uncontrolled observation
        // if (IS->obs_control(i) < 5.0) continue;  // weakly controlled obs.

        double  mer = 0, pol = 0;

        Observation* pm = IS->ptr_obs(i);

        // special case for coordinates
        if (dynamic_cast<const Coordinates*>(pm->ptr_cluster()))
          {
            dif_m(i) = dif_p(i) = 0;
            continue;
          }

        // other observations by visitor or by default values
        testVisitor.setObservationIndex(i);
        pm->accept(&testVisitor);

        mer = testVisitor.getMer();
        pol = testVisitor.getPol();

        dif_m(i) = mer;
        dif_p(i) = pol;
      }

    double max_pol = 0;
    {
      for (Vec::iterator i=dif_p.begin(); i != dif_p.end(); ++i)
        if (fabs(*i) > max_pol)
          max_pol = fabs(*i);
    }
    if (max_pol >= max_dif) test = true;

    if (test && !(IS->update_constrained_coordinates()))
      {
        // if all adjusted points are constrained, adjustment is never
        // repeated (unless explicitly asked for)
        // ------------------------------------------------------

        test = false;
        for (PointData::const_iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
          {
            const LocalPoint& b = (*i).second;
            if (b.free_xy() && !b.constrained_xy())
              {
                test = true;
                break;
              }
          }
      }
  }
  return test;
}

/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005  Ales Cepek <cepek@gnu.org>

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
 *  $Id: g3_model_write_xml_adjustment_results.cpp,v 1.2 2006/08/26 13:23:30 cepek Exp $
 */

#include <gnu_gama/g3/g3_model.h>
#include <gnu_gama/g3/g3_cluster.h>
#include <gnu_gama/g3/g3_write_observation_xml.h>
#include <gnu_gama/outstream.h>
#include <gnu_gama/adj/adj.h>
#include <iomanip>


using namespace std;
using namespace GNU_gama::g3;

using GNU_gama::Index;

namespace 
{
  class WriteAdjustedXML :  
    public GNU_gama::ObservationVisitor,
    public GNU_gama::Visitor<Angle>,
    public GNU_gama::Visitor<Azimuth>,
    public GNU_gama::Visitor<Distance>,
    public GNU_gama::Visitor<Height>,
    public GNU_gama::Visitor<HeightDiff>,
    public GNU_gama::Visitor<Vector>,
    public GNU_gama::Visitor<XYZ>,
    public GNU_gama::Visitor<ZenithAngle>
  {
  public:

    WriteAdjustedXML(GNU_gama::g3::Model* m, std::ostream& o) 
      : model(m), out(o) 
    {
    }

    void visit(Angle* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(Azimuth* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(Distance* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(Height* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(HeightDiff* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(Vector* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(XYZ* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }
    void visit(ZenithAngle* p)
    {
      model->write_xml_adjusted(out, p, index); 
    }

    Index index;

  private:

    GNU_gama::g3::Model* model;
    std::ostream&        out;

  };
}



void Model::write_xml_adjustment_results(std::ostream& out)
{
  if (!check_adjustment()) update_adjustment();

  write_xml_rejected_observations          (out);
  write_xml_adjustment_results_statistics  (out);
  write_xml_adjustment_results_points      (out);
  write_xml_adjustment_results_observations(out);
}



void Model::write_xml_rejected_observations(std::ostream& out)
{
  WriteObservationXML visitor(out);

  if (rejected_obs.empty()) return;

  out << "\n<rejected-observations>\n";

  for (RejectedObs::iterator 
         i=rejected_obs.begin(), e=rejected_obs.end(); i!=e; ++i)
    {
      Model::Rejected& robs = *i;

      string tag;
      switch (robs.criterion) 
        {
        case Model::Rejected::rhs: tag = "rhs";     break;
        default:                   tag = "unknown"; break;
        }

      out << "\n<" << tag << ">\n";
      
      robs.observation->accept(&visitor);
      out << "        ";
      for (int i=0; i<robs.observation->dimension(); i++)
        out << "<flt>" << robs.data[i] << "</flt> ";     

      out << "\n</"  << tag << ">\n";
    }

  out << "\n</rejected-observations>\n\n";
}



void Model::write_xml_adjustment_results_statistics(std::ostream& out)
{ 
  out << "\n<adjustment-statistics>\n\n";

  Adj::algorithm alg = adj->get_algorithm();
  out << "<algorithm> ";
  if      (alg == Adj::gso)      cout << "gso";
  else if (alg == Adj::svd)      cout << "svd";
  else if (alg == Adj::cholesky) cout << "cholesky";
  else                           cout << "unknown";
  out << " </algorithm>\n\n";

  {
    gama_ellipsoid id = gama_ellipsoid(ellipsoid.id);
    
    out.setf(ios_base::fixed, ios_base::floatfield);
    out << "<ellipsoid> "
        << "<caption> " << gama_ellipsoid_caption[id] << " </caption>\n";
    out << "            <id>      " << gama_ellipsoid_id[id] 
        << "         </id>\n";
    out.precision(5);
    out << "            <a>       " << ellipsoid.a() << " </a>\n"; 
    out.precision(5);
    out << "            <b>       " << ellipsoid.b() << " </b>\n"; 
    out << "            </ellipsoid>\n\n"; 
  }

  out << "<parameters>" << setw(5) << dm_cols       << " </parameters>\n";
  out << "<equations> " << setw(5) << dm_rows       << " </equations>\n";
  out << "<defect>    " << setw(5) << adj->defect() << " </defect>\n";

  out << "<redundancy>" << setw(5) << redundancy    << " </redundancy\n\n";

  out.setf(ios_base::scientific, ios_base::floatfield);
  out.precision(5);
  double rtr = adj->rtr();
  out << "<sum-of-squares>        " << rtr << " </sum-of-squares>\n";

  double sigma_apriori = apriori_sd*apriori_sd;
  out << "<apriori-variance>      " 
      << sigma_apriori << " </apriori-variance>\n";

  double sigma_aposteriori = aposteriori_sd*aposteriori_sd;
  out << "<aposteriori-variance>  " 
      << sigma_aposteriori <<" </aposteriori-variance>\n";
  
  out << "<variance-factor-used>  ";
  if (actual_sd == aposteriori)
    out << "aposteriori";
  else
    out << "    apriori";
  out <<" </variance-factor-used>\n";
  
  out << "<design-matrix-graph>  ";
  if (graph_is_connected())
    out << "   connected";
  else
    out << "disconnected";
  out <<" </design-matrix-graph>\n";
  


  out << "\n</adjustment-statistics>\n\n";
}



void Model::write_xml_adjustment_results_points      (std::ostream& out)
{
  out << "\n"
    "<!-- adjustment results    : dn / de / du  are in millimeters -->\n"
    "<!-- deflection of vertical: db / dl       are in arc seconds -->\n\n";
  
  out << "<adjustment-results>\n";

  for (ParameterList::iterator 
         i=par_list->begin(), e=par_list->end(); i!=e; ++i)
    {
      (*i)->write_xml_init();
    }
  for (ParameterList::iterator 
         i=par_list->begin(), e=par_list->end(); i!=e; ++i)
    {
      (*i)->write_xml(out);
    }
  
  out << "\n</adjustment-results>\n\n";
}



void Model::write_xml_adjustment_results_observations(std::ostream& out)
{
  out << "\n<adjusted-observations>\n";

  WriteAdjustedXML  write_xml(this, out);
  Index index = 1;
  for (ObservationList::iterator 
         i=active_obs->begin(), e=active_obs->end(); i!=e; ++i)
    {
      write_xml.index = index;
      (*i)->accept(&write_xml);
      index += (*i)->dimension();
    }

  out << "\n</adjusted-observations>\n";
}


// ==========================================================================


void Model::write_xml_adjusted_stdev(const char* prefix, 
                                     std::ostream& out, 
                                     const Observation* obs,
                                     Index obs_dim_index,
                                     Index index)
{
  const char* tab   = "        ";
  const char* ntab  = "\n        ";
  const int   prec  = out.precision(3);
  const int   width = 8;

  Index  cluster_index = obs->cluster_index + obs_dim_index;
  double obs_stdev     = obs->cluster->stdDev(cluster_index);

  // double res_stdev     = -1;

  index += obs_dim_index;
  double adj_stdev     = sqrt( cov_bb(index, index) );

  out << ntab << "<" << prefix << "stdev-obs>"
      << setw(width) << obs_stdev  
      << " </" << prefix << "stdev-obs>\n";
  //out << tab << "<" << prefix << "stdev-res>"
  //    << setw(width) << res_stdev  
  //    << " </" << prefix << "stdev-res>\n";
  out << tab << "<" << prefix << "stdev-adj>"
      << setw(width) << adj_stdev  
      << " </" << prefix << "stdev-adj>\n";

  out.precision(prec);
}


void Model::write_xml_adjusted_cov_xyz(std::ostream& out, 
                                       const Observation* obs,
                                       Index index)
{
  double cxx = cov_bb(index, index  );
  double cxy = cov_bb(index, index+1);
  double cxz = cov_bb(index, index+2);   index++;
  double cyy = cov_bb(index, index  );
  double cyz = cov_bb(index, index+1);   index++;
  double czz = cov_bb(index, index  );

  
  const std::ios_base::fmtflags format = out.setf(std::ios_base::scientific, 
                                                  std::ios_base::floatfield);
  const int prec = out.precision(7);
  const int width = 14;

  out << "\n";
  out << "        ";
  out << "<cxx> " << cxx << " </cxx> ";
  out << "<cxy> " << setw(width) << cxy << " </cxy> ";
  out << "\n                                   ";
  out << "<cxz> " << setw(width) << cxz << " </cxz>\n";
  out << "        ";
  out << "<cyy> " << cyy << " </cyy> ";
  out << "<cyz> " << setw(width) << cyz << " </cyz>\n";
  out << "        ";
  out << "<czz> " << czz << " </czz>\n";

  out.precision(prec);
  out.setf(format);
}


void Model::write_xml_adjusted(std::ostream& out, const Angle* a, Index index)
{
  out << "\n<angle> ";
  out << "        </angle>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const Azimuth* a, Index index)
{
  out << "\n<azimuth> ";
  out << "        </azimuth>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const Distance* d, Index index)
{
  out << "\n<distance> ";
  out << "        </distance>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const Vector* v, Index index)
{
  out << "\n<vector> "
      << "<from>"  << v->from << "</from> "
      << "<to>"    << v->to   << "</to> " 
      << "<index>" << index   << "</index>\n";

  double rdx = adj->r()(index)/Linear().scale();
  out << "\n        <dx-observed>" << setw(13) << v->dx()      
      << " </dx-observed>";
  out << "\n";
  out << "        <dx-residual>" << setw(13) << rdx          
      << " </dx-residual>";
  out << "\n";
  out << "        <dx-adjusted>" << setw(13) << v->dx()+rdx  
      << " </dx-adjusted>";
  out << "\n";

  double rdy = adj->r()(index+1)/Linear().scale();
  out << "\n        <dy-observed>" << setw(13) << v->dy()      
      << " </dy-observed>";
  out << "\n";
  out << "        <dy-residual>" << setw(13) << rdy          
      << " </dy-residual>";
  out << "\n";
  out << "        <dy-adjusted>" << setw(13) << v->dy()+rdy  
      << " </dy-adjusted>";
  out << "\n";

  double rdz = adj->r()(index+2)/Linear().scale();
  out << "\n        <dz-observed>" << setw(13) << v->dz()      
      << " </dz-observed>";
  out << "\n";
  out << "        <dz-residual>" << setw(13) << rdz          
      << " </dz-residual>";
  out << "\n";
  out << "        <dz-adjusted>" << setw(13) << v->dz()+rdz  
      << " </dz-adjusted>";
  out << "\n";

  write_xml_adjusted_stdev("dx-", out, v, 0, index);
  write_xml_adjusted_stdev("dy-", out, v, 1, index);
  write_xml_adjusted_stdev("dz-", out, v, 2, index);

  write_xml_adjusted_cov_xyz(out, v, index);
  
  out << "        </vector>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const Height* h, Index index)
{
  out << "\n<height> "
      << "<id>" << h->id << "</id> "
      << "<index>" << index   << "</index>\n";

  double rdh = adj->r()(index)/Linear().scale();
  out << "\n        <observed>" << setw(13) << h->obs()      
      << " </observed>";
  out << "\n";
  out << "        <residual>" << setw(13) << rdh          
      << " </residual>";
  out << "\n";
  out << "        <adjusted>" << setw(13) << h->obs()+rdh  
      << " </adjusted>";
  out << "\n";

  write_xml_adjusted_stdev("", out, h, 0, index);

  out << "        </height>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const HeightDiff* hd, Index index)
{
  out << "\n<height-diff> "
      << "<from>" << hd->from << "</from> <to>" << hd->to << "</to> "
      << "<index>" << index << "</index>\n";

  double rdhd = adj->r()(index)/Linear().scale();
  out << "\n        <observed>" << setw(13) << hd->obs()      
      << " </observed>";
  out << "\n";
  out << "        <residual>" << setw(13) << rdhd          
      << " </residual>";
  out << "\n";
  out << "        <adjusted>" << setw(13) << hd->obs()+rdhd  
      << " </adjusted>";
  out << "\n";

  write_xml_adjusted_stdev("", out, hd, 0, index);

  out << "        </height-diff>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const XYZ* xyz, Index index)
{
  out << "\n<xyz> "
      << "<id>" << xyz->id << "</id> "
      << "<index>" << index   << "</index>\n";

  double rx = adj->r()(index)/Linear().scale();
  out << "\n        <dx-observed>" << setw(13) << xyz->x()      
      << " </dx-observed>";
  out << "\n";
  out << "        <dx-residual>" << setw(13) << rx          
      << " </dx-residual>";
  out << "\n";
  out << "        <dx-adjusted>" << setw(13) << xyz->x()+rx  
      << " </dx-adjusted>";
  out << "\n";

  double ry = adj->r()(index+1)/Linear().scale();
  out << "\n        <dy-observed>" << setw(13) << xyz->y()      
      << " </dy-observed>";
  out << "\n";
  out << "        <dy-residual>" << setw(13) << ry          
      << " </dy-residual>";
  out << "\n";
  out << "        <dy-adjusted>" << setw(13) << xyz->y()+ry  
      << " </dy-adjusted>";
  out << "\n";

  double rz = adj->r()(index+2)/Linear().scale();
  out << "\n        <dz-observed>" << setw(13) << xyz->z()      
      << " </dz-observed>";
  out << "\n";
  out << "        <dz-residual>" << setw(13) << rz          
      << " </dz-residual>";
  out << "\n";
  out << "        <dz-adjusted>" << setw(13) << xyz->z()+rz  
      << " </dz-adjusted>";
  out << "\n";

  write_xml_adjusted_stdev("x-", out, xyz, 0, index);
  write_xml_adjusted_stdev("y-", out, xyz, 1, index);
  write_xml_adjusted_stdev("z-", out, xyz, 2, index);

  write_xml_adjusted_cov_xyz(out, xyz, index);
  
  out << "        </xyz>\n";
}



void Model::write_xml_adjusted(std::ostream& out, const ZenithAngle* za, Index index)
{
  out << "\n<zenith-angle> ";
  out << "        </zenith-angle>\n";
}




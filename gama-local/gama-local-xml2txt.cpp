#include <iostream>
#include <iomanip>
#include <sstream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>
#include <gnu_gama/version.h>
#include <gamalib/language.h>
#include <gamalib/local/results/text/underline.h>
#include <gnu_gama/statan.h>

using namespace GaMaLib;

typedef GNU_gama::LocalNetworkAdjustmentResults Adjustment;

void general_parameters   (std::ostream& cout,const Adjustment&);
void adjusted_parameters  (std::ostream& cout,const Adjustment&);
void adjusted_observations(std::ostream& cout,const Adjustment&);


int main()
{
  Adjustment adj;
  try
    {
      adj.read_xml(std::cin);

      general_parameters   (std::cout, adj);
      adjusted_parameters  (std::cout, adj);
      adjusted_observations(std::cout, adj);
    }
  catch (GNU_gama::Exception::parser perr)
    {
      std::cerr << "parser error : " << perr.error_code 
                << "  line : "       << perr.line
                << "  text : "       << perr.str
                << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << "unknown exception\n";
    }
}


void general_parameters(std::ostream& cout, const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

  set_gama_language(en);

  cout << T_GaMa_Adjustment_of_geodetic_network << "        "
       << T_GaMa_version 
       << adj.network_general_parameters.gama_local_version     << "-" 
       << adj.network_general_parameters.gama_local_algorithm   << " / " 
       << adj.network_general_parameters.gama_local_compiler    << "\n" 
       << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
       << "http://www.gnu.org/software/gama/\n\n\n";
  

  if (!adj.description.empty())
    {
      cout << T_GaMa_network_description << '\n'
           << underline(T_GaMa_network_description, '*') << '\n'
           << adj.description << "\n\n";
      
      if ((*adj.description.rbegin()) != '\n')
        cout << '\n';
    }

  cout << T_GaMa_General_solution_parameters << "\n"
       << underline(T_GaMa_General_solution_parameters, '*') << "\n\n";
  int w0 = 0, w_ = 8;
  {
    int n;
    n = strlen(T_GaMa_gpar1_coordinates);              if (n > w0) w0 = n;
    n = strlen(T_GaMa_gpar1_adjusted_coordinates);     if (n > w0) w0 = n;
    n = strlen(T_GaMa_gpar1_constrained_coordinates);  if (n > w0) w0 = n;
    n = strlen(T_GaMa_gpar1_fixed_coordinates);        if (n > w0) w0 = n;
    n = strlen(T_GaMa_gpar1_total);                    if (n > w0) w0 = n;
  }
  
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_coordinates << " ";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_+1) << "xyz"
       << setw(w_-1) << "xy"
       << setw(w_)   << "z"  << "\n\n";
  
  const int a_xyz = adj.coordinates_summary.adjusted.xyz;
  const int a_xy  = adj.coordinates_summary.adjusted.xy;
  const int a_z   = adj.coordinates_summary.adjusted.z;
  const int c_xyz = adj.coordinates_summary.constrained.xyz;
  const int c_xy  = adj.coordinates_summary.constrained.xy;
  const int c_z   = adj.coordinates_summary.constrained.z;
  const int f_xyz = adj.coordinates_summary.fixed.xyz;
  const int f_xy  = adj.coordinates_summary.fixed.xy;
  const int f_z   = adj.coordinates_summary.fixed.z;
  
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_adjusted_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << a_xyz
       << setw(w_)  << a_xy
       << setw(w_)  << a_z
       << '\n';
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_constrained_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << c_xyz
       << setw(w_)  << c_xy
       << setw(w_)  << c_z
       << '\n';
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_fixed_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << f_xyz
       << setw(w_)  << f_xy
       << setw(w_)  << f_z
       << '\n';
  
  for (int ii=0; ii<w0+1+3*w_+1; ii++) cout << '-'; cout << "\n";
  
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_total << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << (a_xyz + f_xyz)
       << setw(w_)  << (a_xy  + f_xy )
       << setw(w_)  << (a_z   + f_z  )
       << "\n\n";
  
  int w1 = 0;
  {
    int n;
    // n = strlen(T_GaMa_gpar1_computed_points);  if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_fixed_points);     if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_computed_heights); if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_fixed_heights);    if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_points_total);     if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_directions);       if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_angles);           if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_distances);        if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_observed_coords);  if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_leveling_diffs);   if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_z_angles);         if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_s_dists);          if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_obs_total);        if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_equations);        if (n > w1) w1 = n;
    n = strlen(T_GaMa_gpar1_redundancy);       if (n > w1) w1 = n;
  }
  int w2 = 0;
  {
    int n;
    // n = strlen(T_GaMa_gpar2_constrained_points);  if (n > w2) w2 = n;
    // n = strlen(T_GaMa_gpar2_constrained_heights); if (n > w2) w2 = n;
    n = strlen(T_GaMa_gpar2_bearings);            if (n > w2) w2 = n;
    n = strlen(T_GaMa_gpar2_number_of_unknowns);  if (n > w2) w2 = n;
    n = strlen(T_GaMa_gpar2_network_defect);      if (n > w2) w2 = n;
  }
  const char* tab_sep = "            ";
  
  const int pocdel  = adj.observations_summary.distances;
  const int pocsmer = adj.observations_summary.directions;
  const int pocuhl  = adj.observations_summary.angles;
  const int pocsour = adj.observations_summary.xyz_coords;
  const int pocnivp = adj.observations_summary.h_diffs;
  const int poczeni = adj.observations_summary.z_angles;
  const int pocsikm = adj.observations_summary.s_dists;
  // number of vectors is not printed in gama-local text output
  const int pocvekt = adj.observations_summary.vectors; 
  
  const int observations = pocdel + pocsmer + pocuhl + pocsour
    + pocnivp + poczeni + pocsikm + pocvekt;
  
  const int pocosn  = adj.orientations.size();
  
  if (pocosn)
    {
      cout << set_width(T_GaMa_gpar1_directions, w1) << ":"
           << setw(6) << pocsmer << tab_sep
           << set_width(T_GaMa_gpar2_bearings, w2) << ":"
           << setw(6) << pocosn << '\n';
    }
  if (pocuhl)
    {
      cout << set_width(T_GaMa_gpar1_angles, w1) << ":"
           << setw(6) << pocuhl << '\n';
    }
  if (pocdel)
    {
      cout << set_width(T_GaMa_gpar1_distances, w1) << ":"
           << setw(6) << pocdel << '\n';
    }
  if (pocsour)
    {
      cout << set_width(T_GaMa_gpar1_observed_coords, w1) << ":"
           << setw(6) << pocsour << '\n';
    }
  if (pocnivp && (pocnivp != observations))
    {
      cout << set_width(T_GaMa_gpar1_leveling_diffs, w1) << ":"
           << setw(6) << pocnivp << '\n';
    }
  if (poczeni)
    {
      cout << set_width(T_GaMa_gpar1_z_angles, w1) << ":"
           << setw(6) << poczeni << '\n';
    }
  if (pocsikm)
    {
      cout << set_width(T_GaMa_gpar1_s_dists, w1) << ":"
           << setw(6) << pocsikm << '\n';
    }
  int types = 0;
  if (pocsmer) types++;
  if (pocdel)  types++;
  if (pocuhl)  types++;
  if (pocsour) types++;
  if (pocnivp) types++;
  if (poczeni) types++;
  if (pocsikm) types++;
  if (types != 1)
    cout << set_width(T_GaMa_gpar1_obs_total, w1) << ":"
         << setw(6) << observations << "\n";
  cout << '\n';    

  
  cout << set_width(T_GaMa_gpar1_equations, w1) << ":"
       << setw(6) << observations     << tab_sep
       << set_width(T_GaMa_gpar2_number_of_unknowns, w2) << ":"
       << setw(6) << adj.project_equations.unknowns
       << '\n'
       << set_width(T_GaMa_gpar1_redundancy, w1) << ":"
       << setw(6) << adj.project_equations.degrees_of_freedom << tab_sep
       << set_width(T_GaMa_gpar2_network_defect, w2) << ":"
       << setw(6) << adj.project_equations.defect
       << '\n';
  cout.setf(ios_base::fixed, ios_base::floatfield);


  cout << "\n"
       << T_GaMa_m0_apriori << ":"
       << setprecision(2) << setw(9) << adj.standard_deviation.apriori << '\n';
  cout << T_GaMa_m0_empirical << ":"
       << setprecision(2) << setw(9)
       << (adj.project_equations.degrees_of_freedom > 0 ?
           sqrt(adj.project_equations.sum_of_squares /
                adj.project_equations.degrees_of_freedom) : 0);
  cout.setf(ios_base::scientific, ios_base::floatfield);
  cout << "         "
       << "[pvv] : "
       << setprecision(5)<< adj.project_equations.sum_of_squares
       << '\n';


  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "\n";
  cout << T_GaMa_During_statistical_analysis_we_work << "\n\n"
       << (adj.standard_deviation.using_aposteriori ?
           T_GaMa_statan_with_empirical_standard_deviation :
           T_GaMa_statan_with_apriori_standard_deviation);
  cout << setprecision(2) 
       << (adj.standard_deviation.using_aposteriori ?
           adj.standard_deviation.aposteriori : adj.standard_deviation.apriori)
       << "\n"
       <<  T_GaMa_statan_with_confidence_level
       << setprecision(0) << adj.standard_deviation.probability*100 
       << " %\n\n";

  if (const int nadb = adj.project_equations.degrees_of_freedom)
    {
      const double alfa_pul = (1 - adj.standard_deviation.probability)/2;
      if (adj.standard_deviation.using_aposteriori)
        {
          float testm0 = adj.standard_deviation.aposteriori 
            / adj.standard_deviation.apriori;
          float lower = sqrt(GNU_gama::Chi_square(1-alfa_pul,nadb)/nadb);
          float upper = sqrt(GNU_gama::Chi_square(  alfa_pul,nadb)/nadb);

          cout << T_GaMa_Ratio_empirical_to_apriori << setprecision(3)
               << testm0 << '\n'
               << setprecision(0) << adj.standard_deviation.probability*100
               << " % " << T_GaMa_interval << " ("
               << setprecision(3) << lower
               << ", " << upper
               << ") "
               << (lower<testm0 && upper>testm0 ?
                   T_GaMa_interval_contains :
                   T_GaMa_interval_doesnt_contain)
               << "\n";


          double m0d=0, m0s=0, m0u=0;   // m0' from dists. / dirs. / angles
          double sqd=0, sqs=0, squ=0;   // sum of weight coefficients
          int    itd=0, its=0, itu=0;
          for (int i=0; i<observations; i++)
            {
              const Adjustment::Observation& obs = adj.obslist[i];
              double v = obs.residual();
              double q = obs.qrr;
              if (obs.xml_tag == "distance")
                {
                  itd = 1;
                  m0d += v*v;
                  sqd += q;
                }
              else if (obs.xml_tag == "direction")
                {
                  its = 1;
                  m0s += v*v;
                  sqs += q;
                }
              else if (obs.xml_tag == "angle")
                {
                  itu = 2;
                  m0u += v*v;
                  squ += q;
                }
            }
          if (itd) m0d = sqrt(m0d/sqd);
          if (its + itu) m0s = sqrt((m0s+m0u)/(sqs+squ));
          if (itd+its+itu > 1)
            {
              double ma = adj.standard_deviation.apriori;
              if (itd)
                cout << T_GaMa_m0_distances << m0d/ma << "   ";
              if (its+itu)
                {
                  switch (its+itu) {
                  case 1: cout << T_GaMa_m0_directions; break;
                  case 2: cout << T_GaMa_m0_angles; break;
                  case 3: cout << T_GaMa_m0_dirs_angs; break;
                  }
                  cout << m0s/ma;
                }
              cout << '\n';
            }
        }


      double stud_opr;
      double max_stud = 0;
      int imax = -1;
      for (int i=0; i<observations; i++)
        {
          const Adjustment::Observation& obs = adj.obslist[i];
          if (obs.qrr > 1e-4)
            {
              stud_opr = std::abs(atof(obs.std_residual.c_str()));
              if (stud_opr > max_stud)
                {
                  max_stud = stud_opr;
                  imax = i;
                }
            }
        }
      bool aprm0 = !adj.standard_deviation.using_aposteriori;
      float krit_opr;
      if (aprm0)
        krit_opr = GNU_gama::Normal(alfa_pul);
      else
        {
          float s = GNU_gama::Student(alfa_pul, nadb-1);
          float t = s*s;
          krit_opr = sqrt(nadb*t/(nadb-1+t));
        }
      
      if (nadb > 1 && imax > -1 && adj.standard_deviation.using_aposteriori)
        {
          const Adjustment::Observation& obs = adj.obslist[imax];
          double v = obs.residual();
          double q = obs.qrr;
          double m_0_red = 
            sqrt(fabs(adj.project_equations.sum_of_squares-v*v/q)/(nadb-1));
          cout << "\n" << T_GaMa_Maximal_decrease_of_m0
              << setprecision(3) << m_0_red/adj.standard_deviation.apriori
              << "\n\n";
        }
      
      if (imax > -1)
        {
          cout.setf(ios_base::fixed, ios_base::floatfield);
          if (aprm0)
            cout << T_GaMa_Maximal_normalized_residual;
          else
            cout << T_GaMa_genpar_Maximal_studentized_residual;
          cout << setprecision(2) << max_stud;
          if (max_stud > krit_opr)
            cout << T_GaMa_genpar_exceeds;
          else
            cout << T_GaMa_genpar_doesnt_exceed;
          cout << T_GaMa_genpar_critical_value <<  krit_opr << "\n"
               << T_GaMa_genpar_on_significance_level
               << setprecision(0) 
               << (1 - adj.standard_deviation.probability)*100
               << T_GaMa_genpar_for_observation_ind
               << imax+1 << "\n";

          const Adjustment::Observation& obs = adj.obslist[imax];
          cout << "<" <<  obs.xml_tag;
          cout << " from=\"" << obs.from
               << "\" to=\"" << obs.to << "\"";

          cout.precision(3);
          cout << " val=\"" << obs.obs 
               << "\"";
          // cout << " stdev=\"" << obs.stdev 
          //      << "\"";
          cout << " />\n";
        }    
    }

  cout << "\n\n";
}


void adjusted_parameters(std::ostream& cout,const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

  const int MAXWID  = 12;     // IS->maxw_id();
  const int MAXWUNK =  3;     // IS->maxw_unk();
  const int MAXWOBS =  4;     // IS->maxw_obs();

  std::string prev_id;

  { /* fixed points */

    int pocpevb=0, pocpevv=0;
    {   // for ...
      for (int i=0, N=adj.fixed_points.size(); i<N; ++i)
        {
          const Adjustment::Point& point = adj.fixed_points[i];
          
          if (point.hxy) pocpevb++;
          if (point.hz ) pocpevv++;          
        }
    }   // for ...
    
    if (pocpevb != 0 || pocpevv != 0) 
      {
        cout << T_GaMa_Review_of_fixed_points << "\n"
             << underline(T_GaMa_Review_of_fixed_points, '*') << "\n\n";

        std::ostringstream ostr;
        ostr.width(MAXWID);
        ostr << T_GaMa_point;
        int table=0;
        if (pocpevb)
          {
            ostr.width(13);
            ostr << "x   ";
            ostr.width(13+2);
            ostr << "y   ";
            table += 2*13 + 2;
          }
        if (pocpevv)
          {
            if (pocpevb) 
              {
                ostr << "  ";
                table += 2;
              }
            ostr.width(13);
            ostr << "z   ";
            table += 13;
          }
        cout << ostr.str() << '\n';
        for (int i=0; i<ostr.str().size(); i++) cout << '=';
        cout << "\n\n";

        for (int i=0, N=adj.fixed_points.size(); i<N; ++i)
        {
          const Adjustment::Point& point = adj.fixed_points[i];
          cout.width(MAXWID); //IS->maxw_id());

          cout << point.id;
          if (point.hxy)
            {
              cout.precision(3);
              cout.width(13);
              cout << point.x;
              cout << "  ";
              cout.width(13);
              cout << point.y;
            }
          if (point.hz)
            {
              if (pocpevb && !point.hxy)
                {
                  cout.width(2*13+2);
                  cout << " ";
                }
              cout.precision(3);
              if (pocpevb)  cout << "  ";
              cout.width(13);
              cout << point.z;
          }

          cout << "\n";
        }
      }
    cout << "\n\n";
  }

  if (adj.adjusted_points.size())
    { /* Adjusted unknowns */
      
      cout << T_GaMa_adjunk_Review_of_unknowns_coordidantes << "\n"
           << underline(T_GaMa_adjunk_Review_of_unknowns_coordidantes, '*') 
           << "\n\n";
      cout.width(MAXWUNK);
      cout << "i" << " ";
      cout.width(MAXWID);
      cout << T_GaMa_point;
      cout << T_GaMa_adjunk_header1;
      for (int i=0; i<MAXWUNK+MAXWID+1; i++) cout << '=';
      cout << T_GaMa_adjunk_header2;
      cout.setf(ios_base::fixed, ios_base::floatfield);

      prev_id.clear();
      for (int ii=0; ii<adj.adjusted_points.size(); ii++)
        {
          const Adjustment::Point point = adj.adjusted_points[ii];
          
          if (point.hxy)
            {              
              cout.width(MAXWUNK);
              cout << " " << " ";
              cout.width(MAXWID);
              if (prev_id != point.id)
                cout << point.id.c_str();
              else
                cout << " ";
              prev_id = point.id;
            }

          cout << "\n";
        }
    }
}


void adjusted_observations(std::ostream& cout,const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

}


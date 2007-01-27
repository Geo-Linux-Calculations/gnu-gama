#include <iostream>
#include <iomanip>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>
#include <gnu_gama/version.h>
#include <gamalib/language.h>
#include <gamalib/local/results/text/underline.h>
#include <gnu_gama/statan.h>

using namespace GaMaLib;
using std::cout;
using std::setw;
using std::ios_base;
using std::setprecision;

typedef GNU_gama::LocalNetworkAdjustmentResults Adjustment;

void general_parameters(const Adjustment&);


int main()
{
  Adjustment adj;
  try
    {
      adj.read_xml(std::cin);

      general_parameters(adj);
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


void general_parameters(const Adjustment& adj)
{
  set_gama_language(en);

  cout << T_GaMa_Adjustment_of_geodetic_network << "        "
       << T_GaMa_version << GNU_gama::GNU_gama_version 
    /* << "-" << IS->algorithm() */
    /* << " / " << GNU_gama::GNU_gama_compiler */ << "\n" 
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

          /*
          float m0d=0, m0s=0, m0u=0;   // m0' from dists. / dirs. / angles
          float sqd=0, sqs=0, squ=0;   // sum of weight coefficients
          int   itd=0, its=0, itu=0;
          double m0used = (adj.standard_deviation.using_aposteriori ?
                           adj.standard_deviation.aposteriori :
                           adj.standard_deviation.apriori);
          for (int i=0; i<observations; i++)
            {
              const Adjustment::Observation& obs = adj.obslist[i];
              double v = obs.adj - obs.obs;
              double q = ... weight coeff. of residuals needs obs. weight
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
              float ma = adj.standard_deviation.apriori;
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
          */  
        }
    }
}

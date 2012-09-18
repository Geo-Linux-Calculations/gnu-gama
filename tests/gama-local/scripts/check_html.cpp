#include <iostream>
#include <fstream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

using GNU_gama::LocalNetworkAdjustmentResults;

int main(int argc, char* argv[])
{
  std::cout << "max.diff between XML and HTML rounding output for "
            << argv[1] << "\n";

  LocalNetworkAdjustmentResults* html = new LocalNetworkAdjustmentResults;
  {
    std::ifstream inp_html(argv[2]);
    if (!inp_html) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[2] << "\n";
      return 1;
    }
    html->read_html(inp_html);
  }

  LocalNetworkAdjustmentResults* xml = new LocalNetworkAdjustmentResults;
  {
    std::ifstream inp_xml(argv[3]);
    if (!inp_xml) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[3] << "\n";
      return 1;
    }
    xml->read_xml(inp_xml);
  }

  int result_gp = 0, rcoord = 0, robs = 0;

  // needs to fix trailing white spaces and HTML tags
  //if (html->description != xml->description) result_gp = 1;

  {
    bool test =
      html->coordinates_summary.adjusted.xyz    == xml->coordinates_summary.adjusted.xyz   &&
      html->coordinates_summary.adjusted.xy     == xml->coordinates_summary.adjusted.xy    &&
      html->coordinates_summary.adjusted.z      == xml->coordinates_summary.adjusted.z     &&
      html->coordinates_summary.constrained.xyz == xml->coordinates_summary.constrained.xyz&&
      html->coordinates_summary.constrained.xy  == xml->coordinates_summary.constrained.xy &&
      html->coordinates_summary.constrained.z   == xml->coordinates_summary.constrained.z  &&
      html->coordinates_summary.fixed.xyz       == xml->coordinates_summary.fixed.xyz      &&
      html->coordinates_summary.fixed.xy        == xml->coordinates_summary.fixed.xy       &&
      html->coordinates_summary.fixed.z         == xml->coordinates_summary.fixed.z;
    if (!test) {
      std::cout << "         coordinate summary failed\n";
      result_gp = 1;
    }
  }

  {
    bool test =
      html->observations_summary.distances  ==  xml->observations_summary.distances  &&
      html->observations_summary.directions ==  xml->observations_summary.directions &&
      html->observations_summary.angles     ==  xml->observations_summary.angles     &&
      html->observations_summary.xyz_coords ==  xml->observations_summary.xyz_coords &&
      html->observations_summary.h_diffs    ==  xml->observations_summary.h_diffs    &&
      html->observations_summary.z_angles   ==  xml->observations_summary.z_angles   &&
      html->observations_summary.s_dists    ==  xml->observations_summary.s_dists    &&
      html->observations_summary.vectors    ==  xml->observations_summary.vectors;
    if (!test) {
      std::cout << "         observation summary failed\n";
      result_gp = 1;
    }
  }

  {
    bool test =
      html->project_equations.equations          ==  xml->project_equations.equations           &&
      html->project_equations.unknowns           ==  xml->project_equations.unknowns            &&
      html->project_equations.degrees_of_freedom ==  xml->project_equations.degrees_of_freedom  &&
      html->project_equations.defect             ==  xml->project_equations.defect; 
    if (!test) {
      std::cout << "         project equations failed\n";
      result_gp = 1;
    }

    {
      double pvv = std::abs(html->project_equations.sum_of_squares
                            - xml->project_equations.sum_of_squares);
      double qvv = (html->project_equations.sum_of_squares
                    + xml->project_equations.sum_of_squares)/2;
      if (qvv) pvv /= qvv;
      if (pvv > 1e-5)
        {
          std::cout << "         sum of squares failed\n";
          result_gp = 1;
        }
    }
    {
      double mapr = std::abs(html->standard_deviation.apriori
                             - xml->standard_deviation.apriori);
      double qapr = (html->standard_deviation.apriori
                     + xml->standard_deviation.apriori)/2;
      if (qapr) mapr /= qapr;
      if (mapr > 1e-3)
        {
          std::cout << "         apriori standard deviation failed\n";
          result_gp = 1;
        }
    }
    {
      double mapo = std::abs(html->standard_deviation.aposteriori
                             - xml->standard_deviation.aposteriori);
      double qapo = (html->standard_deviation.aposteriori
                     + xml->standard_deviation.aposteriori)/2;
      if (qapo) mapo /= qapo;
      if (mapo > 1e-3)
        {
          std::cout << "         aposteriori standard deviation failed\n";
          result_gp = 1;
        }
    }

    if (html->project_equations.connected_network
        != xml->project_equations.connected_network)
      {
        std::cout << "         network connectivity test failed\n";
        result_gp = 1;
      }

    if (html->standard_deviation.using_aposteriori
        != xml->standard_deviation.using_aposteriori)
      {
        std::cout << "         a priori/a posteriori test failed\n";
        result_gp = 1;
      }

    {
      double mprob = std::abs(html->standard_deviation.probability
                             - xml->standard_deviation.probability);
      double qprob = (html->standard_deviation.probability
                     + xml->standard_deviation.probability)/2;
      if (qprob) mprob /= qprob;
      if (mprob > 1e-5)
        {
          std::cout << "         probability failed\n";
          result_gp = 1;
        }
    }

    if (html->standard_deviation.using_aposteriori)
      {
        if (html->standard_deviation.passed
            != xml->standard_deviation.passed)
          {
            std::cout << "         m0 in (lower, upper) test failed\n";
            result_gp = 1;
          }
        {
          double mratio = std::abs(html->standard_deviation.probability
                                  - xml->standard_deviation.probability);
          double qratio = (html->standard_deviation.probability
                          + xml->standard_deviation.probability)/2;
          if (qratio) mratio /= qratio;
          if (mratio > 1e-5)
            {
              std::cout << "         ratio m0'/m0 failed\n";
              result_gp = 1;
            }
        }
        {
          double mlower = std::abs(html->standard_deviation.lower
                                  - xml->standard_deviation.lower);
          double qlower = (html->standard_deviation.lower
                          + xml->standard_deviation.lower)/2;
          if (qlower) mlower /= qlower;
          if (mlower > 1e-5)
            {
              std::cout << "         lower limit failed\n";
              result_gp = 1;
            }
        }
        {
          double mupper = std::abs(html->standard_deviation.upper
                                  - xml->standard_deviation.upper);
          double qupper = (html->standard_deviation.upper
                          + xml->standard_deviation.upper)/2;
          if (qupper) mupper /= qupper;
          if (mupper > 1e-5)
            {
              std::cout << "         upper limit failed\n";
              result_gp = 1;
            }
        }
        {
          double mcscale = std::abs(html->standard_deviation.confidence_scale
                                  - xml->standard_deviation.confidence_scale);
          double qcscale = (html->standard_deviation.confidence_scale
                          + xml->standard_deviation.confidence_scale)/2;
          if (qcscale) mcscale /= qcscale;
          if (mcscale > 1e-5)
            {
              std::cout << "         confidence scale failed\n";
              result_gp = 1;
            }
        }
      } // if (html->standard_deviation.using_aposteriori)

    if (result_gp == 0) std::cout << "         general parameters passed\n";
  }

  return result_gp + rcoord + robs;
}

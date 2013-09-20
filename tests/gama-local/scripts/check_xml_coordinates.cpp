#include <iostream>
#include <iomanip>
#include <fstream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

std::string strip(const char* s)
{
  std::string t;
  while (*s)
    {
      if (*s == '.') break;
      t += *s;
      if (*s == '/') t.clear();
      s++;
    }

  return t;
}

int main(int argc, char* argv[])
{
  using namespace GNU_gama;

  if (argc != 3)
    {
      std::cout << "\nusage: " << argv[0] << "  fileA.xml  fileB.xml\n\n";
      return 1;
    }

  LocalNetworkAdjustmentResults* xml1 = new LocalNetworkAdjustmentResults;
  try {
    std::ifstream inp_xml1(argv[1]);
    if (!inp_xml1) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[1] << "\n";
      return 1;
    }
    xml1->read_xml(inp_xml1);
  }
  catch (GNU_gama::Exception::parser& e)
    {
      std::cout  << argv[1] << " "
                 << e.line << " " << e.error_code << " " << e.what() << "\n";
    }

  LocalNetworkAdjustmentResults* xml2 = new LocalNetworkAdjustmentResults;
  try {
    std::ifstream inp_xml2(argv[2]);
    if (!inp_xml2) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[2] << "\n";
      return 1;
    }
    xml2->read_xml(inp_xml2);
  }
  catch (GNU_gama::Exception::parser& e)
    {
      std::cout  << argv[2] << " "
                 << e.line << " " << e.error_code << " " << e.what() << "\n";
    }

  double diff = 0;
  const LocalNetworkAdjustmentResults::PointList& A = xml1->adjusted_points;
  const LocalNetworkAdjustmentResults::PointList& B = xml2->adjusted_points;
  for (int i=0; i<A.size(); i++)
    {
      for (int j=0; j<B.size(); j++)
        if (A[i].id == B[j].id)
          {
            if (A[i].hxy && B[j].hxy)
              {
                double dx = A[i].x - B[j].x;
                double dy = A[i].y - B[j].y;

                if (std::abs(dx) > std::abs(diff)) diff = dx;
                if (std::abs(dy) > std::abs(diff)) diff = dy;
              }

            if (A[i].z && B[j].z)
              {
                double dz = A[i].z - B[j].z;
                if (std::abs(dz) > std::abs(diff)) diff = dz;
              }

            break;
          }
    }

  std::cout.precision(4);
  std::cout << "         max.diff xyz" << std::setw(11) <<  diff << " [m] "
            << strip(argv[1]) << " " << strip(argv[2]) << "\n";

  if (std::abs(diff) >= 1e-4) return 1;
}

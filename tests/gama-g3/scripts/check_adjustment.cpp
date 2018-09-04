#include <iostream>
#include <fstream>
#include <streambuf>
#include <cmath>
#include <string>
#include <list>
#include <map>
#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/size_to.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::abs;
using GNU_gama::size_to;

namespace
{
  int errors = 0;

  string pad(string t)
  {
    string s( 7, ' ');
    string p(15, ' ');
    for (std::size_t i=0; i<p.length() && i<t.length(); i++) {
        p[i] = t[i];
      }
    s += p;
    s += ' ';
    return s;
  }

  void tst(string s, int x, int y)
  {
    cout << pad(s);
    bool ok = (x == y);
    if (ok) {
        cout << "passed";
      }
    else {
        cout << "failed";
        errors++;
      }
    cout << "\t" << x << "\t" << y << endl;
  };

  void tst(string s, double x, double y, double limit=1e-6)
  {
    cout << pad(s);
    bool ok = (abs(x - y) < limit);
    if (ok) {
        cout << "passed";
      }
    else {
        cout << "failed";
        errors++;
      }
    cout << "\t" << x << "\t" << y << endl;
  };

  struct Adj3res {
    double ax {0}, ay{0}, az{0};
  };

} /* unnamed namespace */

int main(int argc, char* argv[])
{
  if (argc != 3) {
      cerr << "wrong number of arguments, must be 2: input.xml output.xml\n";
      return 1;
    }

  std::list<GNU_gama::DataObject::Base*> objects;

  for (int i=1; i<=2; i++)
    {
      std::ifstream fb(argv[i]);
      if (!fb.is_open()){
          cerr << "file " << argv[i] << " is not open\n";
          return 1;
        }
      std::string text((std::istreambuf_iterator<char>(fb)),
                       std::istreambuf_iterator<char>());

      GNU_gama::DataParser parser(objects);
      try {
        parser.xml_parse(text.c_str(), size_to<int>(text.length()), 0);
        parser.xml_parse("", 0, 1);
      }
      catch(const GNU_gama::Exception::parser& p)
      {
        cerr << "\nXML parser error on line " << p.line
             << " of input data  "
             << "\t(error code " << p.error_code << ")\n"
             << p.str << "\n\n";
        return 1;
      }
      catch(...)
      {
        cerr << "catch ... \n";
        return 1;
      }
    }

  GNU_gama::DataObject::g3_adj_results* adjres1 {nullptr};
  GNU_gama::DataObject::g3_adj_results* adjres2 {nullptr};
  int n = 0;
  for (auto obj : objects) {
      if (auto t = dynamic_cast<GNU_gama::DataObject::g3_adj_results*>(obj)) {
          switch (++n) {
            case 1: adjres1 = t;
              break;
            case 2: adjres2 = t;
              break;
            }
        }
    }
  if (n != 2)
    {
      cerr << "wrong number of objects\n";
      return 1;
    }

  auto a = adjres1->adjres;
  auto b = adjres2->adjres;

  cout << "\ngama-g3 adjustment results diff" << endl;
  cout << "file 1 : " << argv[1] << "\t " << a->algorithm << endl;
  cout << "file 2 : " << argv[2] << "\t " << b->algorithm << endl;

  tst("parameters     ", stoi(a->parameters),     stoi(b->parameters));
  tst("equations      ", stoi(a->equations),      stoi(b->equations));
  tst("redundancy     ", stoi(a->redundancy),     stoi(b->redundancy));
  tst("defect         ", stoi(a->defect),         stoi(b->defect));
  tst("sum of sqrs    ", stod(a->sum_of_squares), stod(b->sum_of_squares));
  tst("apriori var    ", stod(a->apriori_var),    stod(b->apriori_var));
  tst("aposteriori var", stod(a->aposteriori_var),stod(b->aposteriori_var));

  using AdjMap = std::map<std::string, Adj3res>;
  AdjMap points;

  for (auto p : a->points) {
      Adj3res& point = points[p.id];
      if (p.x_adjusted.empty()) continue;
      point.ax = stod(p.x_adjusted);
      point.ay = stod(p.y_adjusted);
      point.az = stod(p.z_adjusted);
    }
  for (auto p : b->points) {
      if (p.x_adjusted.empty()) continue;
      Adj3res& point = points[p.id];
      point.ax -= stod(p.x_adjusted);
      point.ay -= stod(p.y_adjusted);
      point.az -= stod(p.z_adjusted);
    }
  double maxdiff = 0;
  string maxdiff_id = points.begin()->first;
  for (auto m : points) {

      if (abs(m.second.ax) > abs(maxdiff)) {
          maxdiff_id = m.first;
          maxdiff = m.second.ax;
        }
      if (abs(m.second.ay) > abs(maxdiff)) {
          maxdiff_id = m.first;
          maxdiff = m.second.ay;
        }
      if (abs(m.second.az) > abs(maxdiff)) {
          maxdiff_id = m.first;
          maxdiff = m.second.ay;
        }
    }

  if (abs(maxdiff) > 1e-4) {
      errors++;
      cout << "max diff in adjusted coordinates XYZ " << maxdiff << endl;
    }

  cout << pad("adjusted XYZ");
  if (abs(maxdiff) < 1e-4) {
      cout << "passed";
    }
  else {
      cout << "failed";
      errors++;
    }
  cout << "\t" << maxdiff << endl;

  return errors;
}

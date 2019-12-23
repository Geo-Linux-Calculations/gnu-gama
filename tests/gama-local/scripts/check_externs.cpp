#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <unordered_set>

#include <gnu_gama/simplified.h>
#include <gnu_gama/xml_expat.h>

using std::cout;
using std::endl;
using std::string;

int check(string, string, string);
string coordinates_extern;

int main(int argc, char** argv)
{
  int error = 0;

  cout << "Compares extern attributes from input and adjustment output XML\n\n";

  if (argc < 3)
    {
      cout << "Usage: " << argv[0] << " input_dir output_dir files...\n\n";
      return 1;
    }

  string argv1 = argv[1]; if (argv1.back() != '/') argv1 += '/';
  string argv2 = argv[2]; if (argv2.back() != '/') argv2 += '/';

  cout << "input  directory: " << argv1 << endl;
  cout << "output directory: " << argv2 << endl << endl;

  for (int i=3; i<argc; i++)
    {
      if (check(argv1, argv2, argv[i]))
        {
          cout << "  passed  ";
        }
      else
        {
          cout << "FAILED    ";
          error++;
        }

      cout << argv[i] << endl;
    }

  return error;
}


std::vector<string> gkf;
std::vector<string> xml;


int check(string inpdir, string outdir, string file)
{
  int input(std::string);

  gkf.clear();
  xml.clear();
  int result = 1;

  int error = input(inpdir + file + ".gkf");
  gkf = std::move(xml);
  error    += input(outdir + file + "-gso.xml");
  if (error) result = 0;

  if (gkf.size() != xml.size())
    {
      cout << " gkf size <> xml size "
	   << gkf.size() << " " << xml.size() << " ";
      result = 0;
    }

  if (result != 0)
    for (int i=0; i<gkf.size(); i++)
      {
        if (gkf[i] != xml[i])
          {
            cout << " gkf <> xml missmatch ";
            result = 0;
            break;
          }
      }

  return result;
}


void startElement(void *userData, const char *cname, const char **atts)
{
  while (*atts)
    {
      string name = cname;
      string attribute = *atts++;
      string value = GNU_gama::simplified(*atts++);

      if (!coordinates_extern.empty())
        {
          const std::unordered_set<string> tags
            {
             "x", "y", "z", "X", "Y", "Z"
            };

          if (tags.find(attribute) != tags.end())
            xml.push_back(coordinates_extern);
        }
      else
        {
          if (attribute == "extern")
            {
              if (name == "coordinates")
                {
                  // store coordinates' extern for all dx, dy, dz in the
                  // coordinates tag

                  coordinates_extern = value;
                }
              else
                {
                  xml.push_back(value);
                  if (string(name) == "vec")
                    {
                      xml.push_back(value);
                      xml.push_back(value);
                    }
                }
            }
        }
    }
}


void endElement(void *userData, const char *name)
{
  if (string(name) == "coordinates") coordinates_extern.clear();
}


int input(std::string filename)
{
  std::ifstream ifs(filename);
  std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

  XML_Parser parser = XML_ParserCreate(NULL);
  //XML_SetUserData(parser, &user_data);
  XML_SetElementHandler(parser, startElement, endElement);
  int done = 0;
  do {
    size_t len = content.length();
    done = 1;
    if (!XML_Parse(parser, content.c_str(), len, done)) {
      cout << "XML parser error " << XML_ErrorString(XML_GetErrorCode(parser))
           << " at line " <<  XML_GetCurrentLineNumber(parser)
           << endl;
      return 1;
    }
  } while (!done);
  XML_ParserFree(parser);

  return 0;
}

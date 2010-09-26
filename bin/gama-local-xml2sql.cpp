#include <gnu_gama/local/gkf2sql.h>
#include <gnu_gama/version.h>
#include <gnu_gama/exception.h>
#include <iostream>
#include <fstream>


int help()
{
  using std::cerr;
  
  cerr << "Usage: gama-local-xml2sql configuration (xml_input|-) [sql_output|-]\n\n" 
       << "Convert XML adjustment input of gama-local to SQL\n\n";
  
  return 1;
}



int parameters(int argc, char* argv[], std::istream*& xml, std::ostream*& sql)
{
  if (argc < 2 || argc > 4) return help();
  
  if (argv[1][0] == '-' ) return help();

  const char* inp = "-";
  const char* out = "-";

  switch (argc)
    {
    case 4 : out = argv[3];
    case 3 : inp = argv[2]; break;
    default: return help();
    }

  if (std::string(inp) == "-")
    xml = &std::cin;
  else
    xml = new std::ifstream(inp);

  if (std::string(out) == "-")
    sql = &std::cout;
  else
    sql = new std::ofstream(out);

  return 0;
}


int main(int argc, char* argv[])
{
  std::string   conf;
  std::istream* inp;
  std::ostream* out;

  try
    {
      if (const int k = parameters(argc, argv, inp, out)) return k;

      GNU_gama::local::Gkf2sql t(argv[1]);
      t.run(*inp, *out);
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



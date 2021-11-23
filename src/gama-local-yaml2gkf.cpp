#include <memory>
#include <iostream>
#include <fstream>
#include <gnu_gama/local/yaml2gkf.h>

using namespace GNU_gama::local;

int main(int argc, char* argv[])
{
  /* Using - as a filename to mean standard input/output */
  const std::string hyphen = "-";

  bool help = false;
  if (argc > 3) {
      help = true;
    }
  else if (argc == 2 || argc == 3) {
      for (int i = 1; i < argc; ++i) {
          std::string arg = argv[i];
          if (arg == "-h"  || arg == "-help" || arg == "--help") {
              help = true;
              break;
            }
        }
    }
  if (help) {
      std::cerr << "\nrun: " << argv[0] << " input.yaml  [ output.gkf ]\n\n";
      return 1;
    }

  std::string arg1, arg2;
  if (argc == 1) {
      arg1 = arg2 = hyphen;  // std input, outut
    }
  else if (argc == 2) {
      arg1 = argv[1];
      arg2 = hyphen;
    }
  else if (argc == 3) {
      arg1 = argv[1];
      arg2 = argv[2];
    }

  std::shared_ptr<std::ostream> output {};
  if (arg2 == hyphen) output.reset( &std::cout, [](std::ostream*){} );
  else                output.reset( new std::ofstream(arg2) );

  try
  {
    YAML::Node config;
    if (arg1 == hyphen) config = YAML::Load(std::cin);
    else                config = YAML::LoadFile(argv[1]);

    Yaml2gkf yaml2gkf(config, *output);
    // yaml2gkf.enable_warnings();

    return yaml2gkf.run();
  }
  catch(YAML::ParserException& exc)
  {
    std::cerr << "\n"
              << "Parser error: "  << exc.msg << "\n"
              << "Input data  : "  << argv[1]
              << " (line "         << exc.mark.line
              << ", column " << exc.mark.column << ")"
              << "\n\n";
    return 1;
  }
  catch(...)
  {
    std::cerr << "\n" << "Unknown exception on processing YAML input file "
              << argv[1] << "\n\n";
    return 1;
  }
}

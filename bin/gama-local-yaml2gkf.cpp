#include <memory>
#include <iostream>
#include <fstream>
#include <gnu_gama/local/yaml2gkf.h>

using namespace GNU_gama::local;

int main(int argc, char* argv[])
{
  std::shared_ptr<std::ostream> output {};

  switch (argc) {
  case 2:
    output.reset( &std::cout, [](std::ostream*){} );
    break;
  case 3:
    output.reset( new std::ofstream(argv[2]) );
    break;
  default:
    std::cerr << "\nrun: " << argv[0] << " input.yaml  [ output.gkf ]\n\n";
    return 1;
  };


  YAML::Node config = YAML::LoadFile(argv[1]);

  Yaml2gkf yaml2gkf(config, *output);
  // yaml2gkf.enable_warnings();

  return yaml2gkf.run();
}

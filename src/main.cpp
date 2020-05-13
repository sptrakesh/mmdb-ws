//
// Created by Rakesh on 2019-09-26.
//

#include <iostream>

#include "log/NanoLog.h"
#include "server/server.h"
#include "util/clara.h"
#include "util/config.h"

int main( int argc, char const * const * argv )
{
  using clara::Opt;

  auto config = std::make_shared<spt::util::Configuration>();
  bool console = false;
  std::string dir{"logs/"};
  bool help = false;

  auto options = clara::Help(help) |
      Opt(config->port, "port")["-p"]["--port"]("Port on which to listen (default 8010)") |
      Opt(config->threads, "threads")["-n"]["--threads"]("Number of server threads to spawn (default system)") |
      Opt(config->file, "file")["-f"]["--file"]("MMDB file") |
      Opt(console, "console")["-c"]["--console"]("Log to console (default false)") |
      Opt(dir, "dir")["-o"]["--dir"]("Log directory (default logs/)");

  auto result = options.parse(clara::Args(argc, argv));
  if (!result)
  {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  if (help)
  {
    options.writeToStream(std::cout);
    exit(0);
  }

  std::cout << "Starting daemon with options\n" <<
    "configuration: " << config->str() << '\n' <<
    "console: " << std::boolalpha << console << '\n' <<
    "dir: " << dir << '\n';

  nanolog::initialize( nanolog::GuaranteedLogger(), dir, "mmdb-ws", console );

  return spt::server::run( config );
}

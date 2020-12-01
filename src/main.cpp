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
      Opt(config->wsport, "8010")["-p"]["--websocket-port"]("WebSocket port on which to listen (default 8010)") |
      Opt(config->tcpport, "2010")["-t"]["--tcp-port"]("TCP port on which to listen (default 2010)") |
      Opt(config->threads, "2")["-n"]["--threads"]("Number of server threads to spawn (default system)") |
      Opt(config->file, "/tmp/dbip-city-lite-2020-11.mmdb.gz")["-f"]["--file"]("MMDB file") |
      Opt(console, "true")["-c"]["--console"]("Log to console (default false)") |
      Opt(config->logLevel, "info")["-l"]["--log-level"]("Log level to use [debug|info|warn|critical] (default info).") |
      Opt(dir, "logs/")["-o"]["--dir"]("Log directory (default logs/)");

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

  if ( config->logLevel == "debug" ) nanolog::set_log_level( nanolog::LogLevel::DEBUG );
  else if ( config->logLevel == "info" ) nanolog::set_log_level( nanolog::LogLevel::INFO );
  else if ( config->logLevel == "warn" ) nanolog::set_log_level( nanolog::LogLevel::WARN );
  else if ( config->logLevel == "critical" ) nanolog::set_log_level( nanolog::LogLevel::CRIT );
  nanolog::initialize( nanolog::GuaranteedLogger(), dir, "mmdb-ws", console );

  return spt::server::run( config );
}

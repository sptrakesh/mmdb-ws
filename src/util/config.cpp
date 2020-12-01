//
// Created by Rakesh on 2019-05-14.
//

#include "config.h"

#include <iomanip>
#include <sstream>

using spt::util::Configuration;

std::string Configuration::str() const
{
  std::ostringstream ss;
  ss << R"({"tcpPort": )" << tcpport <<
    R"(, "wsPort": )" << wsport <<
    R"(, "threads": )" << threads <<
    R"(, "logLevel": ")" << logLevel <<
    R"(", "file": )" << std::quoted( file ) <<
    '}';
  return ss.str();
}

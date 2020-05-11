//
// Created by Rakesh on 2019-05-14.
//

#include "config.h"

#include <sstream>

using spt::util::Configuration;

std::string Configuration::str() const
{
  std::ostringstream ss;
  ss << "{\"cacheDir\":" << cacheDir <<
    ", \"port\":" << port <<
    ", \"threads\":" << threads;
  return ss.str();
}

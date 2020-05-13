//
// Created by Rakesh on 2019-05-14.
//

#pragma once

#include <memory>
#include <string>
#include <thread>

namespace spt::util
{
  struct Configuration
  {
    using Ptr = std::shared_ptr<Configuration>;

    std::string file;
    int port = 8010;
    int threads = std::thread::hardware_concurrency();

    std::string str() const;
  };
}

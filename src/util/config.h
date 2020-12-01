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

    std::string logLevel{ "info" };
    std::string file;
    int wsport = 8010;
    int tcpport = 2010;
    uint16_t threads = std::thread::hardware_concurrency();

    [[nodiscard]] std::string str() const;
  };
}

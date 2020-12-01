//
// Created by Rakesh on 18/07/2020.
//

#pragma once

#include "util/config.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <optional>

namespace spt::server::tcp
{
  struct Server
  {
    explicit Server( util::Configuration::Ptr config, boost::asio::io_context& ioc );

  private:
    void doAccept();

    boost::asio::ip::tcp::acceptor acceptor;
  };
}

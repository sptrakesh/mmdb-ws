//
// Created by Rakesh on 28/11/2020.
//

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace spt::server::websocket
{
  void listen( boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint );
}

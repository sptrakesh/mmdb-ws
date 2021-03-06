//
// Created by Rakesh on 18/07/2020.
//

#pragma once

#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace spt::server::tcp
{
  struct Session : std::enable_shared_from_this<Session>
  {
    explicit Session( boost::asio::ip::tcp::socket socket );

    void start();

  private:
    void doRead();
    void doWrite( std::size_t length );

    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf buffer;
  };
}

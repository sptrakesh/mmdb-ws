//
// Created by Rakesh on 18/07/2020.
//

#include "server.h"
#include "session.h"
#include "log/NanoLog.h"
#include "util/config.h"

using spt::server::tcp::Server;
using btcp = boost::asio::ip::tcp;

// https://dens.website/tutorials/cpp-asio/async-tcp-server

Server::Server( util::Configuration::Ptr config, boost::asio::io_context& ioc ) :
  acceptor{ ioc, btcp::endpoint( btcp::v4(), static_cast<short>( config->tcpport ) ) }
{
  LOG_INFO << "Starting TCP service on port " << config->tcpport;
#ifdef __APPLE__
  acceptor.set_option( btcp::acceptor::reuse_address( true ) );
#endif
  doAccept();
}

void Server::doAccept()
{
  acceptor.async_accept([this] (boost::system::error_code ec, btcp::socket socket )
  {
    if ( !ec )
    {
      LOG_DEBUG << "Accepting connection from " << socket.remote_endpoint().address().to_string();
      std::make_shared<Session>( std::move(socket) )->start();
    }
    else
    {
      LOG_WARN << "Error accepting connection " << ec.message();
    }
    doAccept();
  });
}

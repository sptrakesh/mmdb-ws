//
// Created by Rakesh on 10/05/2020.
//

#include "server.h"
#include "decoder.h"
#include "log/NanoLog.h"
#include "tcp/server.h"
#include "websocket/listener.h"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>

#include <cstdlib>
#include <thread>
#include <vector>

int spt::server::run( util::Configuration::Ptr configuration )
{
  GeoDecoder::instance( configuration->file );
  auto const address = boost::asio::ip::make_address( "0.0.0.0" );
  auto const port = static_cast<unsigned short>( configuration->wsport );

  boost::asio::io_context ioc{ configuration->threads };

  boost::asio::signal_set signals( ioc, SIGINT, SIGTERM );
  signals.async_wait( [&]( boost::system::error_code const&, int ) { ioc.stop(); });

  websocket::listen( ioc, boost::asio::ip::tcp::endpoint{ address, port } );
  tcp::Server s{ configuration, ioc };

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve( configuration->threads - 1 );
  for( auto i = configuration->threads - 1; i > 0; --i )
  {
    v.emplace_back( [&ioc] { ioc.run(); });
  }

  LOG_INFO << "HTTP service started on port " << configuration->wsport;
  ioc.run();

  LOG_INFO << "TCP and HTTP service stopping";

  for ( auto& t : v ) t.join();

  LOG_INFO << "All I/O threads stopped";

  return EXIT_SUCCESS;
}

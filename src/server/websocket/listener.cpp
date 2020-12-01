//
// Created by Rakesh on 28/11/2020.
//

#include "listener.h"
#include "log/NanoLog.h"
#include "server/decoder.h"

#include <boost/asio/coroutine.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <functional>
#include <memory>

namespace spt::server::websocket::internal
{
  namespace beast = boost::beast;         // from <boost/beast.hpp>
  namespace http = beast::http;           // from <boost/beast/http.hpp>
  namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
  namespace net = boost::asio;            // from <boost/asio.hpp>
  using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

  // Report a failure
  void fail( beast::error_code ec, char const* what )
  {
    LOG_WARN << what << ": " << ec.message();
  }

  // Echoes back all received WebSocket messages
  class session :
      public boost::asio::coroutine, public std::enable_shared_from_this<session>
  {
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::string query;
    std::string response;

  public:
    // Take ownership of the socket
    explicit session(tcp::socket socket) : ws_(std::move(socket)) {}

    // Start the asynchronous operation
    void run()
    {
      // We need to be executing within a strand to perform async operations
      // on the I/O objects in this session. Although not strictly necessary
      // for single-threaded contexts, this example code is written to be
      // thread-safe by default.
      net::dispatch(ws_.get_executor(),
          beast::bind_front_handler(&session::loop,
              shared_from_this(),
              beast::error_code{},
              0));
    }

    #include <boost/asio/yield.hpp>

    void loop( beast::error_code ec, std::size_t bytes_transferred )
    {
      boost::ignore_unused(bytes_transferred);
      reenter(*this)
      {
        // Set suggested timeout settings for the websocket
        ws_.set_option( websocket::stream_base::timeout::suggested( beast::role_type::server ) );

        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
              res.set(http::field::server,
                  std::string(BOOST_BEAST_VERSION_STRING) + " MaxMind DB");
            }));

        // Accept the websocket handshake
        yield ws_.async_accept( [self=shared_from_this()]( beast::error_code ec ) { self->loop( ec, 0 ); } );

        if (ec) return fail(ec, "accept");

        for (;;)
        {
          // Read a message into our buffer
          yield ws_.async_read( buffer_,
              [self=shared_from_this()]( beast::error_code ec, std::size_t len ) { self->loop( ec, len ); } );

          if (ec == websocket::error::closed)
          {
            // This indicates that the session was closed
            return;
          }
          if (ec) fail(ec, "read");

          query = beast::buffers_to_string( buffer_.data() );
          if ( query.size() < 2 )
          {
            response = query;
          }
          else if ( query[0] == 'f' && query[1] == ':' )
          {
            response = GeoDecoder::instance().fields( query.substr( 2 ) );
          }
          else if ( query[0] == 'l' && query[1] == ':' )
          {
            response = GeoDecoder::instance().location( query.substr( 2 ) );
          }
          else if ( query[0] == 'j' && query[1] == ':' )
          {
            response = GeoDecoder::instance().lookup( query.substr( 2 ) );
          }
          else
          {
            response = GeoDecoder::instance().lookup( query );
          }
          ws_.text(ws_.got_text());

          yield ws_.async_write(
              boost::asio::buffer( response ),
              [self=shared_from_this()]( beast::error_code ec, std::size_t len )
              {
                self->loop( ec, len );
              } );

          if (ec) return fail(ec, "write");

          // Clear the buffer
          buffer_.consume(buffer_.size());
        }
      }
    }

    #include <boost/asio/unyield.hpp>
  };

//------------------------------------------------------------------------------

  // Accepts incoming connections and launches the sessions
  class listener
      : public boost::asio::coroutine
          , public std::enable_shared_from_this<listener>
  {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;

  public:
    listener( net::io_context& ioc, tcp::endpoint endpoint )
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
        , socket_(net::make_strand(ioc))
    {
      beast::error_code ec;

      // Open the acceptor
      acceptor_.open( endpoint.protocol(), ec );
      if (ec)
      {
        fail(ec, "open");
        return;
      }

      // Allow address reuse
      acceptor_.set_option( net::socket_base::reuse_address(true), ec );
      if (ec)
      {
        fail(ec, "set_option");
        return;
      }

      // Bind to the server address
      acceptor_.bind( endpoint, ec );
      if (ec)
      {
        fail(ec, "bind");
        return;
      }

      // Start listening for connections
      acceptor_.listen( net::socket_base::max_listen_connections, ec );
      if (ec)
      {
        fail(ec, "listen");
        return;
      }
    }

    // Start accepting incoming connections
    void run()
    {
      loop();
    }

  private:

  #include <boost/asio/yield.hpp>

    void
    loop(beast::error_code ec = {})
    {
      reenter(*this)
      {
        for(;;)
        {
          yield acceptor_.async_accept(
              socket_,
              std::bind( &listener::loop, shared_from_this(), std::placeholders::_1 )
          );
          if (ec)
          {
            fail(ec, "accept");
          }
          else
          {
            // Create the session and run it
            std::make_shared<session>(std::move(socket_))->run();
          }

          // Make sure each session gets its own strand
          socket_ = tcp::socket(net::make_strand(ioc_));
        }
      }
    }

  #include <boost/asio/unyield.hpp>
  };
}

void spt::server::websocket::listen( boost::asio::io_context& ioc,
    boost::asio::ip::tcp::endpoint endpoint )
{
  // Create and launch a listening port
  std::make_shared<internal::listener>( ioc, std::move( endpoint ) )->run();
}

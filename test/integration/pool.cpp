//
// Created by Rakesh on 30/11/2020.
//

#include "catch.hpp"
#include "pool.h"

#include <iostream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace spt::itest::pool
{
  struct Connection
  {
    Connection( boost::asio::io_context& ioc, std::string_view h, std::string_view p ) :
        s{ ioc }, resolver( ioc ),
        host{ h.data(), h.size() }, port{ p.data(), p.size() }
    {
      boost::asio::connect( s, resolver.resolve( host, port ) );
    }

    Connection( const Connection& ) = delete;
    Connection& operator=( const Connection& ) = delete;

    ~Connection()
    {
      s.close();
    }

    std::string execute( const std::string& query )
    {
      std::ostream os{ &buffer };
      os << query;
      os << '\n';

      const auto isize = socket().send( buffer.data() );
      buffer.consume( isize );

      boost::system::error_code ec;
      auto osize = boost::asio::read_until( socket(), buffer, "\n\n", ec );
      if ( ec )
      {
        std::cerr << "Error reading from socket " << ec.message() << '\n';
        return {};
      }

      buffer.commit( osize );

      auto resp = std::string{ reinterpret_cast<const char*>( buffer.data().data() ), osize };
      boost::algorithm::trim( resp );
      resp.erase( std::remove( std::begin( resp ), std::end( resp ), '\0' ), std::end( resp ) );

      buffer.consume( buffer.size() );
      return resp;
    }

    boost::asio::ip::tcp::socket& socket()
    {
      if ( ! s.is_open() ) boost::asio::connect( s, resolver.resolve( host, port ) );
      return s;
    }

    bool valid() const { return v; }
    void setValid( bool valid ) { this->v = valid; }

  private:
    boost::asio::ip::tcp::socket s;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::streambuf buffer;
    std::string host;
    std::string port;
    bool v{ true };
  };

  std::unique_ptr<Connection> create()
  {
    static boost::asio::io_context ioc{};
    return std::make_unique<Connection>( ioc, "localhost", "2010" );
  }

  void query( spt::pool::Pool<Connection>& pool )
  {
    using namespace std::string_literals;
    const auto query = "l:184.105.163.155"s;
    auto copt = pool.acquire();
    const auto response = (*copt)->execute( query );
    std::cout << response << '\n';
  }
}

SCENARIO( "TCP client pool test", "pool" )
{
  using namespace std::string_literals;

  GIVEN( "A connection pool to MMDB TCP Service" )
  {
    auto config = spt::pool::Configuration{};
    config.initialSize = 1;
    config.maxPoolSize = 3;
    config.maxConnections = 5;
    config.maxIdleTime = std::chrono::seconds{ 3 };
    spt::pool::Pool<spt::itest::pool::Connection> pool{ spt::itest::pool::create, std::move( config ) };

    WHEN( "Using initial connection to query details" )
    {
      const auto query = "184.105.163.155"s;

      REQUIRE( pool.active() == 0 );
      REQUIRE( pool.inactive() == 1 );
      REQUIRE( pool.totalCreated() == 1 );
      auto copt = pool.acquire();
      REQUIRE( copt );

      const auto response = (*copt)->execute( query );
      REQUIRE( query != response );

      REQUIRE( pool.inactive() == 0 );
      REQUIRE( pool.active() == 1 );
      REQUIRE( pool.totalCreated() == 1 );
    }

    AND_WHEN( "Pool is capped by maximum" )
    {
      REQUIRE( pool.active() == 0 );
      REQUIRE( pool.inactive() == 1 );

      {
        auto copt1 = pool.acquire();
        REQUIRE( copt1 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 1 );
        REQUIRE( pool.totalCreated() == 1 );

        auto copt2 = pool.acquire();
        REQUIRE( copt2 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 2 );
        REQUIRE( pool.totalCreated() == 2 );

        auto copt3 = pool.acquire();
        REQUIRE( copt3 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 3 );
        REQUIRE( pool.totalCreated() == 3 );

        auto copt4 = pool.acquire();
        REQUIRE( copt4 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 4 );
        REQUIRE( pool.totalCreated() == 4 );

        auto copt5 = pool.acquire();
        REQUIRE( copt5 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 5 );
        REQUIRE( pool.totalCreated() == 5 );

        auto copt6 = pool.acquire();
        REQUIRE_FALSE( copt6 );
        REQUIRE( pool.inactive() == 0 );
        REQUIRE( pool.active() == 5 );
        REQUIRE( pool.totalCreated() == 5 );
      }

      REQUIRE( pool.inactive() == 3 );
      REQUIRE( pool.active() == 0 );
      REQUIRE( pool.totalCreated() == 5 );
    }

    AND_WHEN( "Reusing connections from pool" )
    {
      {
        auto copt1 = pool.acquire();
        auto copt2 = pool.acquire();
      }

      REQUIRE( pool.totalCreated() == 2 );

      REQUIRE( pool.inactive() == 2 );
      REQUIRE( pool.active() == 0 );
      auto copt1 = pool.acquire();
      REQUIRE( copt1 );
      auto copt2 = pool.acquire();
      REQUIRE( copt2 );

      const auto query = "184.105.163.155"s;
      const auto response = (*copt2)->execute( query );
      REQUIRE( query != response );

      REQUIRE( pool.inactive() == 0 );
      REQUIRE( pool.active() == 2 );
      REQUIRE( pool.totalCreated() == 2 );
    }

    AND_WHEN( "Invalid connections are not added to pool" )
    {
      {
        auto copt1 = pool.acquire();
        (*copt1)->setValid( false );
        auto copt2 = pool.acquire();
        (*copt2)->setValid( false );
        REQUIRE( pool.active() == 2 );
      }
      REQUIRE( pool.inactive() == 0 );
    }

    AND_WHEN( "Connection TTL works" )
    {
      {
        auto copt1 = pool.acquire();
        auto copt2 = pool.acquire();
      }
      REQUIRE( pool.inactive() == 2 );
      std::cout << "Sleeping 6s to test TTL" << std::endl;
      std::this_thread::sleep_for( std::chrono::seconds{ 6 } );
      REQUIRE( pool.inactive() == 0 );
    }

    AND_WHEN( "Using pool from multiple threads" )
    {
      auto v = std::vector<std::thread>{};
      v.reserve( 5 );
      for ( int i = 0; i < 5; ++i )
      {
        v.emplace_back( spt::itest::pool::query, std::ref( pool ) );
      }

      for ( int i = 0; i < 5; ++i ) v[i].join();
      REQUIRE( pool.inactive() == 3 );
      REQUIRE( pool.active() == 0 );
    }
  }
}

//
// Created by Rakesh on 30/11/2020.
//

#include "catch.hpp"

#include <iostream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace spt::itest::tcp
{
  struct Connection
  {
    explicit Connection( boost::asio::io_context& ioc ) : s{ ioc }
    {
      auto resolver = boost::asio::ip::tcp::resolver{ ioc };
      boost::asio::connect( s, resolver.resolve( "localhost", "2010" ) );
    }

    ~Connection()
    {
      boost::system::error_code ec;
      s.close( ec );
      if ( ec ) std::cerr << "Error closing socket " << ec.message() << std::endl;
    }

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    std::string execute( const std::string& query )
    {
      std::ostream os{ &buffer };
      os << query;
      os << '\n';

      const auto isize = s.send( buffer.data() );
      buffer.consume( isize );

      boost::system::error_code ec;
      auto osize = boost::asio::read_until( s, buffer, "\n\n", ec );
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

  private:
    boost::asio::ip::tcp::socket s;
    boost::asio::streambuf buffer;
  };
}

SCENARIO( "TCP protocol test", "tcp" )
{
  boost::asio::io_context ioc;
  using namespace std::string_literals;

  GIVEN( "Connecting to MMDB TCP Service" )
  {
    auto c = spt::itest::tcp::Connection( ioc );

    WHEN( "Retrieving full JSON response using default query" )
    {
      const auto query = "184.105.163.155"s;
      const auto response = c.execute( query );
      std::cout << response << std::endl;
      REQUIRE( query != response );
    }

    AND_WHEN( "Retrieving simple field list" )
    {
      const auto query = "f:184.105.163.155"s;
      const auto response = c.execute( query );
      std::cout << response << std::endl;
      REQUIRE( query != response );
    }

    AND_WHEN( "Retrieving location data" )
    {
      const auto query = "l:184.105.163.155"s;
      const auto response = c.execute( query );
      std::cout << response << std::endl;
      REQUIRE( query != response );

      std::vector<std::string> parts;
      parts.reserve( 2 );
      boost::algorithm::split( parts, response, [](char c){return c == ',';} );
      REQUIRE( parts.size() == 2 );
    }

    AND_WHEN( "Retrieving full JSON response explicitly" )
    {
      const auto query = "j:184.105.163.155"s;
      const auto response = c.execute( query );
      std::cout << response << std::endl;
      REQUIRE( query != response );
    }

    AND_WHEN( "Retrieving invalid IP address" )
    {
      const auto query = "blah"s;
      const auto response = c.execute( query );
      REQUIRE( query == response );
    }

    AND_WHEN( "Retrieving IPv6 details" )
    {
      const auto query = "2607:f0d0:1002:51::4"s;
      const auto response = c.execute( query );
      std::cout << response << std::endl;
      REQUIRE( query != response );
    }

    AND_WHEN( "Executing queries in succession" )
    {
      auto query = "184.105.163.155"s;
      auto response = c.execute( query );
      REQUIRE( query != response );
      std::cout << response << std::endl;

      query = "f:184.105.163.155"s;
      response = c.execute( query );
      REQUIRE( query != response );
      std::cout << response << std::endl;

      query = "l:184.105.163.155"s;
      response = c.execute( query );
      REQUIRE( query != response );
      std::cout << response << std::endl;

      query = "j:184.105.163.155"s;
      response = c.execute( query );
      REQUIRE( query != response );
      std::cout << response << std::endl;

      query = "2607:f0d0:1002:51::4"s;
      response = c.execute( query );
      REQUIRE( query != response );
      std::cout << response << std::endl;
    }
  }
}

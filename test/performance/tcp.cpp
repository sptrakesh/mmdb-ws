//
// Created by Rakesh on 29/11/2020.
//
#include "pool.h"

#include <iostream>
#include <hayai.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;

namespace spt::ptest::pool
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

    tcp::socket& socket()
    {
      if ( ! s.is_open() ) boost::asio::connect( s, resolver.resolve( host, port ) );
      return s;
    }

    bool valid() const { return v; }
    void setValid( bool valid ) { this->v = valid; }

  private:
    tcp::socket s;
    tcp::resolver resolver;
    boost::asio::streambuf buffer;
    std::string host;
    std::string port;
    bool v{ true };
  };

  std::unique_ptr<Connection> create()
  {
    static boost::asio::io_context ioc{ 1000 };
    return std::make_unique<Connection>( ioc, "localhost", "2010" );
  }

  struct PoolHolder
  {
    static PoolHolder& instance()
    {
      static PoolHolder ph;
      return ph;
    }

    auto proxy() -> auto { return pool->acquire(); }

  private:
    PoolHolder()
    {
      auto config = spt::pool::Configuration{};
      config.initialSize = 0;
      config.maxPoolSize = 1000;
      config.maxConnections = 1000;
      config.maxIdleTime = std::chrono::seconds{ 300 };
      pool = std::make_unique<spt::pool::Pool<Connection>>( create, std::move( config ) );
    }

    std::unique_ptr<spt::pool::Pool<Connection>> pool = nullptr;
  };
}

struct TcpClient
{
  static int failures;

  TcpClient() : proxy{ spt::ptest::pool::PoolHolder::instance().proxy() }
  {
  }

  void json( const std::string& ip )
  {
    try
    {
      const auto response = (*proxy)->execute( ip );
      if ( ip == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void fields( const std::string& ip )
  {
    try
    {
      const std::string req = "f:" + ip;
      const auto response = (*proxy)->execute( req );
      if ( req == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void location( const std::string& ip )
  {
    try
    {
      const std::string req = "l:" + ip;
      const auto response = (*proxy)->execute( req );
      if ( req == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void error()
  {
    try
    {
      const std::string req = "blah";
      const auto response = (*proxy)->execute( req );
      if ( req != response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

private:
  std::optional<spt::pool::Pool<spt::ptest::pool::Connection>::Proxy> proxy;
};

int TcpClient::failures = 0;

struct TcpClientFixture : hayai::Fixture
{
  void SetUp() override
  {
    TcpClient::failures = 0;
    std::random_device rd;
    std::mt19937 g( rd() );
    std::shuffle( ips.begin(), ips.end(), g );
  }

  void TearDown() override
  {
    clients.clear();
    if ( TcpClient::failures > 0 )
    {
      std::cerr << "Total failures " << TcpClient::failures << '\n';
    }
  }

  void lookup( int concurrency )
  {
    bool create = false;
    if ( clients.empty() )
    {
      clients.reserve( concurrency );
      create = true;
    }

    std::vector<std::thread> threads;
    threads.reserve( concurrency );
    for ( auto i = 0; i < concurrency; ++i )
    {
      if (create) clients.emplace_back( std::make_unique<TcpClient>() );
      auto client = clients[i].get();
      threads.emplace_back( std::thread([=]
      {
        for ( const auto& ip : ips )
        {
          client->json( ip );
          client->fields( ip );
          client->location( ip );
          client->error();
        }
      }));
    }

    for (auto& t : threads) t.join();
  }

  std::vector<std::unique_ptr<TcpClient>> clients;
  std::vector<std::string> ips{
      "1.32.44.93",
      "102.164.199.78",
      "102.176.160.70",
      "102.176.160.82",
      "102.176.160.84",
      "103.101.17.171",
      "103.101.17.172",
      "103.104.119.193",
      "103.11.217.165",
      "103.110.110.226",
      "103.12.196.38",
      "103.204.220.24",
      "103.21.150.184",
      "103.28.121.58",
      "103.43.203.225",
      "103.46.210.34",
      "103.46.225.67",
      "103.60.137.2",
      "103.81.114.182",
      "104.244.75.26",
      "104.244.77.254",
      "105.179.10.210",
      "106.104.12.236",
      "106.104.151.142",
      "109.117.12.51",
      "109.167.40.129",
      "109.168.18.50",
      "110.36.239.234",
      "110.39.187.50",
      "110.74.195.215",
      "110.93.214.36",
      "111.220.90.41",
      "112.218.125.140",
      "113.255.210.168",
      "117.141.114.57",
      "117.206.150.8",
      "117.53.155.153",
      "118.163.13.200",
      "118.97.193.204",
      "119.110.209.94",
      "12.139.101.97",
      "12.218.209.130",
      "123.200.20.242",
      "124.106.57.182",
      "124.107.224.215",
      "124.41.211.180",
      "124.41.243.72",
      "125.59.223.27",
      "128.199.37.176"
  };
};

//#define TCP_TEST DISABLED_tcpReading
#define TCP_TEST tcpReading

BENCHMARK_P_F(TcpClientFixture, TCP_TEST, 2, 10, (int concurrency))
{
  lookup( concurrency );
}

BENCHMARK_P_INSTANCE(TcpClientFixture, TCP_TEST, (10));
BENCHMARK_P_INSTANCE(TcpClientFixture, TCP_TEST, (100));
BENCHMARK_P_INSTANCE(TcpClientFixture, TCP_TEST, (500));
BENCHMARK_P_INSTANCE(TcpClientFixture, TCP_TEST, (1000));

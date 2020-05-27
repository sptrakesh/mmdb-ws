//
// Created by Rakesh on 13/05/2020.
//

#include <hayai.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

struct WebsocketClient
{
  static int failures;

  WebsocketClient()
  {
    resolver = new tcp::resolver( ioc );
    ws = new websocket::stream<tcp::socket>( ioc );

    try
    {
      auto const results = resolver->resolve( "localhost", "8010" );
      net::connect( ws->next_layer(), results.begin(), results.end() );

      ws->set_option(websocket::stream_base::decorator(
          []( websocket::request_type& req )
          {
            req.set( http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket" );
          }));

      ws->handshake( "localhost", "/" );
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
    }
  }

  ~WebsocketClient()
  {
    try
    {
      ws->close(websocket::close_code::normal);
    }
    catch ( const std::exception& ex )
    {
      std::cerr << ex.what() << "\n";
    }

    delete resolver;
    delete ws;
  }

  void json( const std::string& ip )
  {
    try
    {
      ws->write( net::buffer( ip ) );
      beast::flat_buffer buffer;
      ws->read( buffer );

      std::ostringstream ss;
      ss << beast::make_printable( buffer.data() );
      const auto response = ss.str();
      if ( ip == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      //std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void fields( const std::string& ip )
  {
    try
    {
      const std::string req = "f:" + ip;
      ws->write( net::buffer( req ) );
      beast::flat_buffer buffer;
      ws->read( buffer );

      std::ostringstream ss;
      ss << beast::make_printable( buffer.data() );
      const auto response = ss.str();
      if ( req == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      //std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void location( const std::string& ip )
  {
    try
    {
      const std::string req = "l:" + ip;
      ws->write( net::buffer( req ) );
      beast::flat_buffer buffer;
      ws->read( buffer );

      std::ostringstream ss;
      ss << beast::make_printable( buffer.data() );
      const auto response = ss.str();
      if ( req == response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      //std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

  void error()
  {
    try
    {
      const std::string req = "blah";
      ws->write( net::buffer( req ) );
      beast::flat_buffer buffer;
      ws->read( buffer );

      std::ostringstream ss;
      ss << beast::make_printable( buffer.data() );
      const auto response = ss.str();
      if ( req != response ) ++failures;
    }
    catch ( const std::exception& ex )
    {
      //std::cerr << ex.what() << "\n";
      ++failures;
    }
  }

private:
  net::io_context ioc;
  tcp::resolver* resolver = nullptr;
  websocket::stream<tcp::socket>* ws = nullptr;
};

int WebsocketClient::failures = 0;

struct WebsocketClientFixture : hayai::Fixture
{
  void SetUp() override
  {
    WebsocketClient::failures = 0;
    std::random_device rd;
    std::mt19937 g( rd() );
    std::shuffle( ips.begin(), ips.end(), g );
  }

  void TearDown() override
  {
    clients.clear();
    if ( WebsocketClient::failures > 0 )
    {
      std::cerr << "Total failures " << WebsocketClient::failures << '\n';
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
      if (create) clients.emplace_back( std::make_unique<WebsocketClient>() );
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

  std::vector<std::unique_ptr<WebsocketClient>> clients;
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

//#define WEBSOCKET_TEST DISABLED_reading
#define WEBSOCKET_TEST reading

BENCHMARK_P_F(WebsocketClientFixture, WEBSOCKET_TEST, 2, 10, (int concurrency))
{
  lookup( concurrency );
}

BENCHMARK_P_INSTANCE(WebsocketClientFixture, WEBSOCKET_TEST, (10));
BENCHMARK_P_INSTANCE(WebsocketClientFixture, WEBSOCKET_TEST, (100));
BENCHMARK_P_INSTANCE(WebsocketClientFixture, WEBSOCKET_TEST, (500));
BENCHMARK_P_INSTANCE(WebsocketClientFixture, WEBSOCKET_TEST, (1000));

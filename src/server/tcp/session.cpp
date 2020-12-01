//
// Created by Rakesh on 18/07/2020.
//

#include "session.h"
#include "log/NanoLog.h"
#include "server/decoder.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <algorithm>

using spt::server::tcp::Session;
using btcp = boost::asio::ip::tcp;

Session::Session( btcp::socket socket ) : socket{ std::move( socket ) }
{
  LOG_INFO << "Created TCP session for " << this->socket.remote_endpoint().address().to_string();
}

void Session::start()
{
  doRead();
}

void Session::doRead()
{
  auto self{ shared_from_this() };
  buffer.consume( buffer.size() );
  buffer.prepare( 128 );
  boost::asio::async_read_until( socket, buffer, '\n', [this, self] (boost::system::error_code ec, std::size_t length )
  {
    if ( !ec )
    {
      doWrite( length );
    }
  });
}

void Session::doWrite( std::size_t length )
{
  auto self{ shared_from_this() };

  if (!length)
  {
    LOG_DEBUG << "Empty read";
    return doRead();
  }

  buffer.commit( length );

  auto query = std::string{ reinterpret_cast<const char*>( buffer.data().data() ), length };
  boost::algorithm::trim( query );
  query.erase( std::remove( std::begin( query ), std::end( query ), '\n'), std::end( query ) );
  LOG_DEBUG << "Query: " << query;
  std::string response;

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

  buffer.consume( buffer.size() );
  std::ostream os{ &buffer };
  os.write( response.data(), response.size() );
  os << "\n\n";

  boost::asio::async_write( socket, buffer,
      [this, self](boost::system::error_code ec, std::size_t length)
      {
        LOG_DEBUG << "Wrote bytes: " << int(length) << " of: " << int(buffer.data().size());
        if (!ec)
        {
          doRead();
        }
        else
        {
          LOG_DEBUG << "Error writing data " << ec.message();
        }
      });
}

//
// Created by Rakesh on 10/05/2020.
//

#include "decoder.h"
#include "log/NanoLog.h"

#if !defined(__APPLE__)
#include <sstream>
#endif

using spt::server::GeoDecoder;

GeoDecoder::GeoDecoder( const std::string& file )
#if !defined(__APPLE__)
: db( file )
#endif
{
  LOG_INFO << "Opened database file " << file;
}

std::string GeoDecoder::lookup( const std::string& ip )
{
#if !defined(__APPLE__)
  try
  {
    return db.lookup( ip );
  }
  catch ( const std::exception& ex )
  {
    LOG_WARN << "Error looking up address: " << ip << ". " << ex.what();
  }
#endif
  return ip;
}

std::string GeoDecoder::fields( const std::string& ip )
{
#if !defined(__APPLE__)
  try
  {
    const auto map = db.get_all_fields( ip );
    std::ostringstream oss;
    for ( const auto& [key, value] : map )
    {
      oss << key << " : " << value << '\n';
    }
    return oss.str();
  }
  catch ( const std::exception& ex )
  {
    LOG_WARN << "Error looking up address: " << ip << ". " << ex.what();
  }
#endif
  return ip;
}

std::string GeoDecoder::location( const std::string& ip )
{
#if !defined(__APPLE__)
  try
  {
    auto map = db.get_all_fields( ip );
    std::ostringstream oss;
    oss << map["latitude"] << ',' << map["longitude"];
    return oss.str();
  }
  catch ( const std::exception& ex )
  {
    LOG_WARN << "Error looking up address: " << ip << ". " << ex.what();
  }
#endif
  return ip;
}

//
// Created by Rakesh on 10/05/2020.
//

#include "decoder.h"
#include "log/NanoLog.h"

#include <sstream>

using spt::server::GeoDecoder;

GeoDecoder::GeoDecoder()
#if !defined(__APPLE__)
: db( "/opt/spt/data/dbip.mmdb" )
#endif
{
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

std::string spt::server::GeoDecoder::fields( const std::string& ip )
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

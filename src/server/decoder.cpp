//
// Created by Rakesh on 10/05/2020.
//

#include "decoder.h"
#include "log/NanoLog.h"

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

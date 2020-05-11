//
// Created by Rakesh on 10/05/2020.
//

#include "decoder.h"

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
  return db.lookup( ip );
#endif
  return ip;
}

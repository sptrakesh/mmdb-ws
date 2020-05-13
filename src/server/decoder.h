//
// Created by Rakesh on 10/05/2020.
//

#pragma once

#if !defined(__APPLE__)
#include <GeoLite2PP.hpp>
#endif

#include <string>

namespace spt::server
{
  class GeoDecoder
  {
  public:
    static GeoDecoder& instance( const std::string& file = {} )
    {
      static GeoDecoder obj( file );
      return obj;
    }

    std::string lookup( const std::string& ip );
    std::string fields( const std::string& ip );
    std::string location( const std::string& ip );

    ~GeoDecoder() = default;
    GeoDecoder( const GeoDecoder& ) = delete;
    GeoDecoder& operator=( const GeoDecoder& ) = delete;

  private:
    GeoDecoder( const std::string& file );

#if !defined(__APPLE__)
    GeoLite2PP::DB db;
#endif
  };
}

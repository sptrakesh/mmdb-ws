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
    static GeoDecoder& instance()
    {
      static GeoDecoder obj;
      return obj;
    }

    std::string lookup( const std::string& ip );

    ~GeoDecoder() = default;
    GeoDecoder( const GeoDecoder& ) = delete;
    GeoDecoder& operator=( const GeoDecoder& ) = delete;

  private:
    GeoDecoder();

#if !defined(__APPLE__)
    GeoLite2PP::DB db;
#endif
  };
}

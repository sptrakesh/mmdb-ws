# DB IP Service
Websocket service for the [dbip](https://www.db-ip.com/db/download/ip-to-city-lite)
MMDB Lite database.

## Protocol
Simplest use is to send the IP address for which you want details to the service,
and if it is a valid IP address, you will receive a JSON response with all the
details.

The following example shows a request IP address and the response:
```shell script
184.105.163.155
```

```json

{
    "city" : 
    {
        "names" : 
        {
            "en" : "Oklahoma City (Central Oklahoma City)"
        }
    },
    "continent" : 
    {
        "code" : "NA",
        "geoname_id" : 6255149,
        "names" : 
        {
            "de" : "Nordamerika",
            "en" : "North America",
            "es" : "Norteamérica",
            "fa" : " امریکای شمالی",
            "fr" : "Amérique Du Nord",
            "ja" : "北アメリカ大陸",
            "ko" : "북아메리카",
            "pt-BR" : "América Do Norte",
            "ru" : "Северная Америка",
            "zh-CN" : "北美洲"
        }
    },
    "country" : 
    {
        "geoname_id" : 6252001,
        "is_in_european_union" : false,
        "iso_code" : "US",
        "names" : 
        {
            "de" : "Vereinigte Staaten von Amerika",
            "en" : "United States",
            "es" : "Estados Unidos de América (los)",
            "fa" : "ایالات متحدهٔ امریکا",
            "fr" : "États-Unis",
            "ja" : "アメリカ合衆国",
            "ko" : "미국",
            "pt-BR" : "Estados Unidos",
            "ru" : "США",
            "zh-CN" : "美国"
        }
    },
    "location" : 
    {
        "latitude" : 35.5248,
        "longitude" : -97.5971
    },
    "subdivisions" : 
    [
        
        {
            "names" : 
            {
                "en" : "Oklahoma"
            }
        }
    ]
}
```

### Fields
If you prefer a simpler listing of *field : value* pairs, prefix the IP address
with `f:`.  The following example shows the same request as above:

```shell script
f:184.105.163.155

city : Oklahoma City (Central Oklahoma City)
continent : North America
country : United States
country_iso_code : US
latitude : 35.524800
longitude : -97.597100
query_ip_address : 184.105.163.155
query_language : en
subdivision : Oklahoma
```

## Warning
There is no authentication/authorisation implemented for this service.  The
assumption is that users will be running this service internally with no public
exposure.

## Pre-requisites
Download the MMDB lite database and place under the `docker` directory.  The
`Dockerfile` will include the file when building the image.  The docker image
will contain only one file, so make sure to remove any older files if you have
them to ensure that the latest file is included in the image.

## Mac OS X
I have not been able to build or test using `libmaxminddb` on Mac OS X.  There seems
to be some incompatibility (probably some conflicting preprocessor define) with
using `libmaxminddb` with `boost`.  All testing has been via building and running
the `docker` image.

No such issues on Linux obviously, since the docker container works without issues.

## Acknowledgements
This software has been developed mainly using work other people have contributed.
The following are the components used to build this software:
* **[GeoLite2++](https://www.ccoderun.ca/GeoLite2++/api/usage.html) - C++
wrapper library to read `mmdb` file via [libmaxminddb](https://github.com/maxmind/libmaxminddb).
* **[Boost:Beast](https://github.com/boostorg/beast)** - We use *Beast* for the
`websocket` server implementation.  The implementation is a modified version of the
[stackless](https://github.com/boostorg/beast/tree/develop/example/websocket/server/stackless)
example.
* **[Clara](https://github.com/catchorg/Clara)** - Command line options parser.
* **[NanoLog](https://github.com/Iyengar111/NanoLog)** - Logging framework used
for the server.  I modified the implementation for daily rolling log files.

In future we will look at [ftrie](https://github.com/trisulnsm/ftrie).  The
main downside to it is that we need to package the much (`5x`) bigger `csv` file in
the image.

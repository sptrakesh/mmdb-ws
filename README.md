# DB IP Service

* [Protocol](#protocol)
* [Warning](#warning)
* [Pre-requisites](#pre-requisites)
* [Testing](#testing)
    * [Performance](#performance)
* [Mac OS X](#mac-os-x)
* [Acknowledgements](#acknowledgements)

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

### Location
You can also retrieve the geo-coordinates (latitude and longitude) for an IP
address by prefixing the address with `l:`.  The following examples illustrates:

```shell script
l:184.105.163.155

35.524800,-97.597100
```

## Warning
There is no authentication/authorisation implemented for this service.  The
assumption is that users will be running this service internally with no public
exposure.

## Pre-requisites
Download the MMDB lite database and place under an appropriate directory.  The
`docker/run.sh` script assumes the `docker` directory.  Mount this file as
`/opt/spt/data/dbip.mmdb.gz` when running the `docker` container.  The
`entrypoint` script will decompress the file into the running container and
use it when running.

## Testing
[Integration](test/lua/README.md) test suite in Lua.  Tests basic requests
and responses.

### Performance
[Performance](test/performance/websocket.cpp) test suite using
[hayai](https://github.com/nickbruun/hayai) and Boost:Beast WebSocket client is
also available.
  
Performance test function sends 4 requests for each IP Address from
a pre-defined `vector` of 50 addresses.  Thus each test involves 200 websocket
requests internally.  

Each test runs the test function 10 times (iterations).  Each set of 10 iterations
is repeated 2 times to get better average numbers (higher repeats like 5 would
be much better). We then run the performance test with different concurrency values.
At present the tests are set up to run with 10, 100, 500 and 1000 concurrent
threads connecting to the service.

Even with 1000 concurrent requests, the docker container rarely consumes above
25Mb RAM.

| Concurrency | Avg Time per run (us) | Avg Timer per iteration (us) | Avg Iterations per second |
| :-- | :-: | :-: | :-: |
| 10 | 8245983.332 | 824598.333 | 1.21271 |
| 100 | 48174280.958 | 4817428.096 | 0.20758 |
| 500 | 246399960.709 | 24639996.071 | 0.04058 |
| 1000 | 435108315.251 | 43510831.525 | 0.02298 |

All these numbers were against a docker container deployed on laptop.  Tests
were run from IDE.  From previous experience deploying the test as another container
could yield better results (depending on how much CPU is allocated to the Docker
daemon).  This is by no means definitive, since I was doing a few other things
(watching a movie, another script running etc.) while the test was in progress.

## Mac OS X
I have not been able to build or test using `libmaxminddb` on Mac OS X.  There seems
to be some incompatibility (probably some conflicting preprocessor define) with
using `libmaxminddb` with `boost`.  All testing has been via building and running
the `docker` image.

No such issues on Linux obviously, since the docker container works without issues.

## Acknowledgements
This software has been developed mainly using work other people have contributed.
The following are the components used to build this software:
* **[GeoLite2++](https://www.ccoderun.ca/GeoLite2++/api/usage.html)** - C++
wrapper library to read `mmdb` file via [libmaxminddb](https://github.com/maxmind/libmaxminddb).
* **[Boost:Beast](https://github.com/boostorg/beast)** - We use *Beast* for the
`websocket` server implementation.  The implementation is a modified version of the
[stackless](https://github.com/boostorg/beast/tree/develop/example/websocket/server/stackless)
example.
* **[Clara](https://github.com/catchorg/Clara)** - Command line options parser.
* **[NanoLog](https://github.com/Iyengar111/NanoLog)** - Logging framework used
for the server.  I modified the implementation for daily rolling log files.
* **[hayai](https://github.com/nickbruun/hayai)** - Performance testing framework.

In future we will look at [ftrie](https://github.com/trisulnsm/ftrie).  The
main downside to it is that we need to package the much (`5x`) bigger `csv` file in
the image.

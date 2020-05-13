# DB IP Service
Websocket service for the [dbip](https://www.db-ip.com/db/download/ip-to-city-lite)
MMDB Lite database.

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

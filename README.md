# DB IP Service
Websocket service for the [dbip](https://www.db-ip.com/db/download/ip-to-city-lite)
MMDB Lite database.

## Acknowledgements
This software has been developed mainly using work other people have contributed.
The following are the components used to build this software:
* **[GeoLite2++](https://www.ccoderun.ca/GeoLite2++/api/usage.html) - C++
wrapper library to read `mmdb` file.
* **[Boost:Beast](https://github.com/boostorg/beast)** - We use *Beast* for the
`http` server implementation.  The implementation is a modified version of the
[stackless](https://github.com/boostorg/beast/tree/develop/example/websocket/server/stackless)
example.
* **[Clara](https://github.com/catchorg/Clara)** - Command line options parser.
* **[NanoLog](https://github.com/Iyengar111/NanoLog)** - Logging framework used
for the server.  I modified the implementation for daily rolling log files.

For future we will also look at [ftrie](https://github.com/trisulnsm/ftrie).

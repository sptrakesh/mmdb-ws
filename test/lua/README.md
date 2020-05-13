# Integration Tests
Integration tests written as Lua scripts using [busted](http://olivinelabs.com/busted/)
and [lua-http](https://github.com/daurnimator/lua-http)

## Pre-requisites
Install necessary Lua modules via `luarocks`

```shell script
luarocks install busted --local
luarocks install http --local
```

## Running tests
Start local **mmdb-ws** instance before running the tests.

```shell script
<path to>/mmdb-ws -c true -o /tmp/ 
```

Run tests by pointing `busted` at the test directory root.

```shell script
~/.luarocks/bin/busted -v test/lua
```

## References
* [busted](http://olivinelabs.com/busted/)
* [lua-http](https://github.com/daurnimator/lua-http)
* [json](https://github.com/rxi/json.lua)

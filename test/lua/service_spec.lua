---
--- Created by rakesh.
--- DateTime: 12/05/2020 20:30
---

local json = require "test.lua.json"
local log = require "test.lua.log"
local websocket = require "http.websocket"

describe("Basic tests for mmdb server", function()
  local uri = "ws://localhost:8010"

  it("Retrieve full JSON response for IP address", function()
    local query = "184.105.163.155"
    local ws = websocket.new_from_uri(uri)
    assert(ws:connect())
    assert(ws:send(query))
    local data = assert(ws:receive())
    assert.are_not.equal(query, data)

    local js = json.decode(data)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(ws:close())
  end)

  it("Retrieve simple field list", function()
    local query = "f:184.105.163.155"
    local ws = websocket.new_from_uri(uri)
    assert(ws:connect())
    assert(ws:send(query))

    local data = assert(ws:receive())
    assert.are_not.equal(query, data)
    assert.is.truthy(data)
    assert.is.truthy(string.find(data, "continent"))
    assert.is.truthy(string.find(data, "country"))
    assert.is.truthy(string.find(data, "latitude"))
    assert.is.truthy(string.find(data, "longitude"))
    log.info(data)

    assert(ws:close())
  end)

  it("Retrieve full JSON response for IP address using explicit query", function()
    local query = "j:184.105.163.155"
    local ws = websocket.new_from_uri(uri)
    assert(ws:connect())
    assert(ws:send(query))
    local data = assert(ws:receive())
    assert.are_not.equal(query, data)

    local js = json.decode(data)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(ws:close())
  end)

  it("Invalid IP address echoes back query", function()
    local query = "abc123"
    local ws = websocket.new_from_uri(uri)
    assert(ws:connect())
    assert(ws:send(query))
    local data = assert(ws:receive())
    assert.are.equal(query, data)
    assert(ws:close())
  end)

  it("IPv6 address returns full details", function()
    local query = "2607:f0d0:1002:51::4"
    local ws = websocket.new_from_uri(uri)
    assert(ws:connect())
    assert(ws:send(query))

    local data = assert(ws:receive())
    assert.are_not.equal(query, data)

    local js = json.decode(data)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(ws:close())
  end)
end)
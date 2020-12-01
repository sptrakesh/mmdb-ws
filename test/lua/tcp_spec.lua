---
--- Created by rakesh.
--- DateTime: 28/11/2020 17:01
---
local json = require "test.lua.json"
local log = require "test.lua.log"
local socket = require("socket")

describe("Basic tests for MMDB TCP server", function()
  local host, port = "127.0.0.1", 2010

  it("Retrieve full JSON response for IP address", function()
    local query = "184.105.163.155"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. s
      if status == "closed" then break end
    end

    assert.are_not.equal(query, buffer)

    local js = json.decode(buffer)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(tcp:close())
  end)

  it("Retrieve simple field list", function()
    local query = "f:184.105.163.155"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. '\n' .. s
      if status == "closed" then break end
    end

    assert.are_not.equal(query, buffer)
    assert.is.truthy(buffer)
    assert.is.truthy(string.find(buffer, "continent"))
    assert.is.truthy(string.find(buffer, "country"))
    assert.is.truthy(string.find(buffer, "latitude"))
    assert.is.truthy(string.find(buffer, "longitude"))
    log.info(buffer)

    assert(tcp:close())
  end)

  it("Retrieve location data", function()
    local query = "l:184.105.163.155"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. '\n' .. s
      if status == "closed" then break end
    end

    assert.are_not.equal(query, buffer)
    assert.is.truthy(buffer)
    assert.is.falsy(string.find(buffer, "continent"))
    assert.is.falsy(string.find(buffer, "country"))
    assert.is.falsy(string.find(buffer, "latitude"))
    assert.is.falsy(string.find(buffer, "longitude"))
    assert.is.truthy(string.find(buffer, ","))
    log.info(buffer)

    local function split(s, delimiter)
      result = {};
      for match in (s..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match);
      end
      return result;
    end
    local function len(t)
      local count = 0
      for _ in pairs(t) do count = count + 1 end
      return count
    end
    assert.are.equal(len(split(buffer, ",")), 2)

    assert(tcp:close())
  end)

  it("Retrieve full JSON response for IP address using explicit query", function()
    local query = "j:184.105.163.155"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. '\n' .. s
      if status == "closed" then break end
    end

    assert.are_not.equal(query, buffer)

    local js = json.decode(buffer)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(tcp:close())
  end)

  it("Invalid IP address echoes back query", function()
    local query = "abc123"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. s
      if status == "closed" then break end
    end

    buffer = buffer:gsub("%c+", "")
    assert.are.equal(query, buffer)
    assert(tcp:close())
  end)

  it("IPv6 address returns full details", function()
    local query = "2607:f0d0:1002:51::4"
    local tcp = assert(socket.tcp())
    assert(tcp:connect(host, port))
    assert(tcp:send(query .. "\n"))

    local buffer = ""
    tcp:settimeout(0.01)
    while true do
      local s, status = tcp:receive()
      if s == nil then break end
      buffer = buffer .. s
      if status == "closed" then break end
    end

    assert.are_not.equal(query, buffer)

    local js = json.decode(buffer)
    assert.is.truthy(js["city"])
    assert.is.truthy(js["continent"])
    assert.is.truthy(js["continent"]["code"])

    assert(tcp:close())
  end)
end)

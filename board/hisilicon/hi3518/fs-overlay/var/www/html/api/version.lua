#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"

local verfile = "/etc/version"

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "GET" then
    local status, errmsg = pcall(function()
      for line in io.lines(verfile) do
        local k,v
        _, _, k, v = line:find("(%a+)%s*=%s*(.*)")
        if k and v then
          resp[k] = v
        end
      end
    end)
  else
    resp.status = "FAIL"
    resp.message = "method not supported"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

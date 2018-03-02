#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)
    if ri.system then
      local sys = ri.system
      if sys.deviceid then
        local ret = os.execute("fw_setenv deviceid " .. sys.deviceid)
        if ret ~= 0 then
          resp.status = "FAIL"
        end
      end
      if sys.securecode then
        local ret = os.execute("fw_setenv securecode " .. sys.securecode)
        if ret ~= 0 then
          resp.status = "FAIL"
        end
      end
    end
    resp.status = resp.status or "OK"
  elseif req.method == "GET" then
    resp.system = { deviceid = '', securecode='' }
    local f, errmsg
    f, errmsg = io.popen("fw_printenv -n deviceid", "r")
    if f then resp.system.deviceid = f:read("*line") end
    f, errmsg = io.popen("fw_printenv -n securecode", "r")
    if f then resp.system.securecode = f:read("*line") end
  else
    resp.status = "FAIL"
    resp.message = "method not supported"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

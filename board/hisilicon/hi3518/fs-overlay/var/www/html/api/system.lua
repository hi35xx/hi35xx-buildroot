#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  local actions = {
    ['REBOOT'] = function()
      os.execute("reboot -d 2 &")
    end,
    ['RESET-WIFI'] = function()
      os.execute("rm -f /overlay/etc/wpa_supplicant.conf")
      os.execute("killall -USR1 wifi-config")
    end,
    ['RESET-SYS'] = function()
      os.execute("rm -rf /overlay/*")
      os.execute("reboot -d 2 &")
    end
  }

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)
    if ri.action then
      local act = actions[ri.action]
      if act then
        act()
        resp.status = "OK"
      else
        resp.status = "FAIL"
        resp.message = "action not supported"
      end
    else
      resp.status = "FAIL"
      resp.message = "action not specified"
    end
  else
    resp.status = "FAIL"
    resp.message = "method not supported"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

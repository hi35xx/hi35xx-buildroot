#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"
local sdbus = require 'simpledbus'
local bus = sdbus.SystemBus()
local vs0 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoSource0')
local vs0antiflicker = vs0['ipcam.Media.VideoSource.Imaging.AntiFlicker']

local afmode_int2str = { [0] = "OFF", [1] = "INDOOR", [2] = "OUTDOOR" }
local afmode_str2int = { OFF = 0, INDOOR = 1, OUTDOOR = 2 }

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local imaging = 'ipcam.Media.VideoSource.Imaging'
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)
    if ri.mode then
      vs0antiflicker.Mode = afmode_str2int[string.upper(ri.mode)]
    end
    if ri.frequency then
      vs0antiflicker.Frequency = ri.frequency
    end
    resp.status = "OK"
  elseif req.method == "GET" then
    resp.mode = afmode_int2str[vs0antiflicker.Mode]
    resp.frequency = vs0antiflicker.Frequency
  else
    resp.status = "FAIL"
    resp.message = "Unsupported method"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

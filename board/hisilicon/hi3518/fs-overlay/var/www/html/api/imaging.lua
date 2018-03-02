#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"
local sdbus = require 'simpledbus'
local bus = sdbus.SystemBus()
local vs0 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoSource0')
local vs0img = vs0['ipcam.Media.VideoSource.Imaging']

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local imaging = 'ipcam.Media.VideoSource.Imaging'
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)
    if ri.mirror ~= nil then
      vs0img.Mirror = ri.mirror
    end
    if ri.flip ~= nil then
      vs0img.Flip = ri.flip
    end
    if ri.brightness ~= nil then
      vs0img.Brightness = tonumber(ri.brightness)
    end
    if ri.contrast ~= nil then
      vs0img.Contrast = tonumber(ri.contrast)
    end
    if ri.chroma ~= nil then
      vs0img.Chroma = tonumber(ri.chroma)
    end
    if ri.saturation ~= nil then
      vs0img.Saturation = tonumber(ri.saturation)
    end
    resp.status = "OK"
  elseif req.method == "GET" then
    resp.mirror = vs0img.Mirror
    resp.flip = vs0img.Flip
    resp.brightness = vs0img.Brightness
    resp.contrast = vs0img.Contrast
    resp.chroma = vs0img.Chroma
    resp.saturation = vs0img.Saturation
  else
    resp.status = "FAIL"
    resp.message = "Unsupported method"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

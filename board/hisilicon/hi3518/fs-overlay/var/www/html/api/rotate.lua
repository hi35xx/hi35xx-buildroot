#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"
local sdbus = require 'simpledbus'
local bus = sdbus.SystemBus()
local vs0 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoSource0')
local vs0rot = vs0['ipcam.Media.VideoSource.Imaging.Rotation']

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local imaging = 'ipcam.Media.VideoSource.Imaging'
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)
    if ri.mode ~= nil then vs0rot.Mode = ri.mode end
    if ri.degree ~= nil then vs0rot.Degree = ri.degree end
    resp.status = "OK"
  elseif req.method == "GET" then
    resp.mode = vs0rot.Mode
    resp.degree = vs0rot.Degree
  else
    resp.status = "FAIL"
    resp.message = "Unsupported method"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

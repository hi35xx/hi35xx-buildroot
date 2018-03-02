#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"
local sdbus = require 'simpledbus'
local bus = sdbus.SystemBus()
local vs0 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoSource0')
local vs0gamma = vs0['ipcam.Media.VideoSource.Imaging.Gamma']

local GAMMA_DIR = "/etc/config/gamma/"

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local imaging = 'ipcam.Media.VideoSource.Imaging'
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  if req.method == "POST" then
    local ri = json.decode(req.POST.post_data)

    if ri.preset then
      local path = GAMMA_DIR .. ri.preset .. ".txt"
      local status, errmsg = pcall(function()
        local gamma_data = {}
        for line in io.lines(path) do
          table.insert(gamma_data, tonumber(line))
        end
        vs0gamma.CurveData = gamma_data
      end)
      if not status then
        resp.status = "FAIL"
        resp.message = errmsg
      end
    end
    if ri.gamma_data then
      local status, errmsg = pcall(function()
        vs0gamma.CurveData = ri.gamma_data
      end)
      if not status then
        resp.status = "FAIL"
        resp.message = errmsg
      end
    end
    resp.status = resp.status or "OK"
  elseif req.method == "GET" then
    resp.gamma_data = vs0gamma.CurveData
  else
    resp.status = "FAIL"
    resp.message = "Unsupported method"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

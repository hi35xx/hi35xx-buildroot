#!/usr/bin/env wsapi.fcgi

module(..., package.seeall)

local json = require "json"
local wsapi = require "wsapi"
wsapi.request = require "wsapi.request"
local sdbus = require "simpledbus"
local bus = sdbus.SystemBus()
local ve0 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoEncoder0')
local ve1 = bus:auto_proxy('ipcam.Media', '/ipcam/Media/VideoEncoder1')

local h264p_int2str = { [0] = "baseline", [1] = "main", [2] = "high" }
local h264p_str2int = { baseline = 0, main = 1, high = 2 }
local vmode_int2str = { [0] = "CBR", [1] = "VBR" }
local vmode_str2int = { CBR = 0, VBR = 1 }

function run(wsapi_env)
  local headers = { ["Content-type"] = "application/json" }
  local req = wsapi.request.new(wsapi_env)
  local resp = {}

  local ve0venc = ve0['ipcam.Media.VideoEncoder']
  local ve0rc = ve0['ipcam.Media.VideoEncoder.RateControl']
  local ve0h264 = ve0['ipcam.Media.VideoEncoder.H264']
  local ve1venc = ve1['ipcam.Media.VideoEncoder']
  local ve1rc = ve1['ipcam.Media.VideoEncoder.RateControl']
  local ve1h264 = ve1['ipcam.Media.VideoEncoder.H264']

  if req.method == "POST" then
    local r = json.decode(req.POST.post_data)

    if r.main_stream then
      if r.main_stream.profile then
        ve0h264.H264Profile = h264p_str2int[r.main_stream.profile]
      end
      if r.main_stream.resolution then
        ve0venc.Resolution = r.main_stream.resolution
      end
      if r.main_stream.framerate then
        ve0rc.FrameRate = r.main_stream.framerate
      end
      if r.main_stream.mode then
        ve0rc.RateControlMode = vmode_str2int[r.main_stream.mode]
      end
      if r.main_stream.bitrate then
        ve0rc.Bitrate = r.main_stream.bitrate
      end
    end
    if r.sub_stream then
      if r.sub_stream.profile then
        ve1h264.H264Profile = h264p_str2int[r.sub_stream.profile]
      end
      if r.sub_stream.resolution then
        ve1venc.Resolution = r.sub_stream.resolution
      end
      if r.sub_stream.framerate then
        ve1rc.FrameRate = r.sub_stream.framerate
      end
      if r.sub_stream.mode then
        ve1rc.RateControlMode = vmode_str2int[r.sub_stream.mode]
      end
      if r.sub_stream.bitrate then
        ve1rc.Bitrate = r.sub_stream.bitrate
      end
    end
    resp.status = "OK"
  elseif req.method == "GET" then
    resp = { main_stream = {}, sub_stream = {} }
    -- main stream
    resp.main_stream.profile = h264p_int2str[ve0h264.H264Profile]
    resp.main_stream.mode = vmode_int2str[ve0rc.RateControlMode]
    resp.main_stream.bitrate = ve0rc.Bitrate
    resp.main_stream.framerate = ve0rc.FrameRate
    resp.main_stream.resolution = ve0venc.Resolution
    -- sub stream
    resp.sub_stream.profile = h264p_int2str[ve0h264.H264Profile]
    resp.sub_stream.mode = vmode_int2str[ve1rc.RateControlMode]
    resp.sub_stream.bitrate = ve1rc.Bitrate
    resp.sub_stream.framerate = ve1rc.FrameRate
    resp.sub_stream.resolution = ve1venc.Resolution
  else
    resp.status = "FAIL"
    resp.message = "Unsupported method"
  end

  local function html_text()
    coroutine.yield(json.encode(resp))
  end

  return 200, headers, coroutine.wrap(html_text)
end

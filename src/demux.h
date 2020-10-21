/*
  Aerostat Beam Coder - Node.js native bindings for FFmpeg.
  Copyright (C) 2019  Streampunk Media Ltd.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  https://www.streampunk.media/ mailto:furnace@streampunk.media
  14 Ormiscaig, Aultbea, Achnasheen, IV22 2JJ  U.K.
*/

#ifndef DEMUX_H
#define DEMUX_H

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavutil/opt.h>
  #include <libavutil/dict.h>
  #include <libavutil/pixfmt.h>
  #include <libavutil/attributes.h>
  #include <libavutil/pixdesc.h>
}

#include "beamcoder_util.h"
#include "packet.h"
#include "format.h"
#include "node_api.h"
#include "adaptor.h"

void demuxerExecute(napi_env env, void* data);
void demuxerComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value demuxer(napi_env env, napi_callback_info info);

void readFrameExecute(napi_env env, void* data);
void readFrameComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value readFrame(napi_env env, napi_callback_info info);

void seekFrameExecute(napi_env env, void *data);
void seekFrameComplete(napi_env env, napi_status asyncStatus, void *data);
napi_value seekFrame(napi_env env, napi_callback_info info);

void demuxerFinalizer(napi_env env, void* data, void* hint);
void readBufferFinalizer(napi_env env, void* data, void* hint);

napi_value forceCloseInput(napi_env env, napi_callback_info info);

struct demuxerCarrier : carrier {
  const char* filename = nullptr;
  Adaptor *adaptor = nullptr;
  AVFormatContext* format = nullptr;
  AVInputFormat* iformat = nullptr;
  AVDictionary* options = nullptr;
  ~demuxerCarrier() {
    if (format != nullptr) { avformat_close_input(&format); }
    if (options != nullptr) { av_dict_free(&options); }
  }
};

struct readFrameCarrier : carrier {
  fmtCtxRef* formatRef = nullptr;
  Adaptor *adaptor = nullptr;
  AVPacket* packet = av_packet_alloc();
  ~readFrameCarrier() {
    if (packet != nullptr) av_packet_free(&packet);
  }
};

struct seekFrameCarrier : carrier {
  fmtCtxRef* formatRef = nullptr;
  int streamIndex = -1;
  int64_t timestamp = 0;
  // weird semantic - backward actually means 'find nearest key frame before timestamp'
  // setting as default as more natural
  int flags = AVSEEK_FLAG_BACKWARD;
  ~seekFrameCarrier() { }
};

#endif // DEMUX_H

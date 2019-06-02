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

#ifndef MUX_H
#define MUX_H

#include "node_api.h"
#include "beamcoder_util.h"
#include "format.h"
#include "frame.h"
#include "adaptor.h"

extern "C" {
  #include <libavformat/avformat.h>
}

napi_value muxer(napi_env env, napi_callback_info info); // Set to interleaving once

void openIOExecute(napi_env env, void* data);
void openIOComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value openIO(napi_env env, napi_callback_info info);

void writeHeaderExecute(napi_env env, void* data);
void writeHeaderComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value writeHeader(napi_env env, napi_callback_info info);

void initOutputExecute(napi_env env, void* data);
void initOutputComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value initOutput(napi_env env, napi_callback_info info);

void writeFrameExecute(napi_env env, void* data);
void writeFrameComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value writeFrame(napi_env env, napi_callback_info info); // IF AVFrame, must include stream_index

void writeTrailerExecute(napi_env env, void* data);
void writeTrailerComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value writeTrailer(napi_env env, napi_callback_info info);

napi_value forceClose(napi_env env, napi_callback_info info);

struct openIOCarrier : carrier {
  AVFormatContext* format;
  int flags = AVIO_FLAG_WRITE;
  AVDictionary* options = nullptr;
  ~openIOCarrier() {
    if (options != nullptr) av_dict_free(&options);
  }
};

struct writeHeaderCarrier : carrier {
  AVFormatContext* format;
  AVDictionary* options = nullptr;
  int result = -1;
  ~writeHeaderCarrier() {
    if (options != nullptr) av_dict_free(&options);
  }
};

struct initOutputCarrier : carrier {
  AVFormatContext* format;
  AVDictionary* options = nullptr;
  int result = -1;
  ~initOutputCarrier() {
    if (options != nullptr) av_dict_free(&options);
  }
};

struct writeFrameCarrier : carrier {
  AVFormatContext* format;
  Adaptor *adaptor = nullptr;
  AVPacket* packet = nullptr;
  AVFrame* frame = nullptr;
  int streamIndex = 0;
  bool interleaved = true;
  ~writeFrameCarrier() {
    if (packet != nullptr) { av_packet_free(&packet); }
    if (frame != nullptr) { av_frame_free(&frame); }
  }
};

struct writeTrailerCarrier : carrier {
  AVFormatContext* format;
  Adaptor *adaptor = nullptr;
  ~writeTrailerCarrier() {
  }
};

#endif // MUX_H

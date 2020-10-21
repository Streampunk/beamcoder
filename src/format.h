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

#ifndef FORMAT_H
#define FORMAT_H

#include "node_api.h"
#include "beamcoder_util.h"
#include "codec_par.h"
#include "packet.h"
#include "adaptor.h"

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavutil/avstring.h>
}

// Indirection required to avoid double delete after demuxer forceClose 
struct fmtCtxRef {
  AVFormatContext* fmtCtx = nullptr;
};

napi_value muxers(napi_env env, napi_callback_info info);
napi_value demuxers(napi_env env, napi_callback_info info);
napi_value guessFormat(napi_env env, napi_callback_info info);
napi_value newStream(napi_env env, napi_callback_info info);

napi_status fromAVInputFormat(napi_env env,
  const AVInputFormat* iformat, napi_value* result);
napi_status fromAVOutputFormat(napi_env env,
  const AVOutputFormat* iformat, napi_value* result);
napi_status fromAVFormatContext(napi_env env,
  AVFormatContext* fmtCtx, Adaptor *adaptor, napi_value* result);
napi_status fromAVStream(napi_env env, AVStream* stream, napi_value* result);

void formatContextFinalizer(napi_env env, void* data, void* hint);

napi_value makeFormat(napi_env env, napi_callback_info info);
napi_value streamToJSON(napi_env env, napi_callback_info info);
napi_value formatToJSON(napi_env env, napi_callback_info info);

/* Notes

AVInputFormat and AVOutputFormats
  - iterate over to produce lists of available
  - JS objects that reflect the internal values only - lazy
  - Ability to "guess" an output formats

AVFormatContext
  - full getters and setters pattern
  - demuxer to offer add new streams
  - uses setters and getters

AVStream
  - only public constructor is muxer.newStream()
  - uses setters and getters
*/

#endif // FORMAT_H

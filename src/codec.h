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

#ifndef BEAM_CODEC_H
#define BEAM_CODEC_H

#include "node_api.h"
#include "beamcoder_util.h"
#include "decode.h"
#include "encode.h"
#include "codec_par.h"

extern "C" {
  #include <libavcodec/avcodec.h>
}

void codecContextFinalizer(napi_env env, void* data, void* hint);
napi_status fromAVCodecContext(napi_env env, AVCodecContext* codec,
    napi_value* result, bool encoding);
napi_status fromAVCodecDescriptor(napi_env env, const AVCodecDescriptor* codecDesc,
    napi_value *result);

napi_value extractParams(napi_env env, napi_callback_info info);
napi_value useParams(napi_env env, napi_callback_info info);

#endif // BEAM_CODEC_H

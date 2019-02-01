/*
  Aerostat Beam Coder - Node.JS native mappings for FFmpeg.
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

#ifndef FRAME_H
#define FRAME_H

#include "node_api.h"
#include "beamcoder_util.h"
#include <vector>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
}

void frameFinalizer(napi_env env, void* data, void* hint);
void frameDataFinalizer(napi_env env, void* data, void* hint);
void frameBufferFinalizer(napi_env env, void* data, void* hint);
void frameBufferFree(void* opaque, uint8_t* data);

struct frameData {
  AVFrame* frame = nullptr;
  std::vector<napi_ref> dataRefs;
  int32_t extSize = 0;
  ~frameData() {
    // printf("Freeing frame with pts = %i\n", frame->pts);
    av_frame_free(&frame);
  }
};

napi_value makeFrame(napi_env env, napi_callback_info info);
napi_status fromAVFrame(napi_env env, frameData* frame, napi_value* result);

#endif // FRAME_H

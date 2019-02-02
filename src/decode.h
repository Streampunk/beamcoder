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

#ifndef DECODE_H
#define DECODE_H

#include "beamcoder_util.h"
#include "packet.h"
#include "frame.h"
#include "codec.h"
#include <vector>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavutil/pixdesc.h>
  #include <libavutil/opt.h>
  #include <libavformat/avformat.h>
}

napi_value decoder(napi_env env, napi_callback_info info);

void decodeExecute(napi_env env, void* data);
void decodeComplete(napi_env env, napi_status asyncStatus, void* data);
napi_value decode(napi_env env, napi_callback_info info);
napi_value flushDec(napi_env env, napi_callback_info info);

void decoderFinalizer(napi_env env, void* data, void* hint);

/* struct decoderCarrier : carrier {
  AVCodecContext* decoder = nullptr;
  AVCodecParameters* params = nullptr;
  int streamIdx = -1;
  char* codecName;
  size_t codecNameLen = 0;
  int32_t codecID = -1;
  ~decoderCarrier() {
    if (decoder != nullptr) {
      avcodec_close(decoder);
      avcodec_free_context(&decoder);
    }
  }
}; */

struct decodeCarrier : carrier {
  AVCodecContext* decoder;
  std::vector<AVPacket*> packets;
  std::vector<AVFrame*> frames;
  std::vector<napi_ref> packetRefs;
  ~decodeCarrier() {
    // printf("Decode carrier destructor.\n");
  }
};

napi_status isPacket(napi_env env, napi_value packet);
AVPacket* getPacket(napi_env env, napi_value packet);

#endif // DECODE_H

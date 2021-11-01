/*
  Aerostat Beam Coder - Node.js native bindings for FFmpeg.
  Copyright (C) 2018  Streampunk Media Ltd.

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

#include "decode.h"

char* req_hw_type = nullptr;
AVPixelFormat req_hw_pix_fmt = AV_PIX_FMT_NONE;

AVPixelFormat get_format(AVCodecContext *s, const AVPixelFormat *pix_fmts)
{
  AVPixelFormat result = AV_PIX_FMT_NONE;
  const AVPixelFormat *p;
  int i, err;

  if (0 == strcmp("auto", req_hw_type)) {
    for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
      const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);
      const AVCodecHWConfig  *config = NULL;

      if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
        break;

      for (i = 0;; i++) {
        config = avcodec_get_hw_config(s->codec, i);
        if (!config)
          break;
        if (!(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
          continue;
        if (config->pix_fmt == *p)
          break;
      }

      if (config) {
        err = av_hwdevice_ctx_create(&s->hw_device_ctx, config->device_type, NULL, NULL, 0);
        if (err < 0) {
          char errstr[128];
          av_make_error_string(errstr, 128, err);
          printf("Error in get_format \'auto\' av_hwdevice_ctx_create: %s\n", errstr);
        }
        break;
      }
    }
    result = *p;
  } else {
    err = av_hwdevice_ctx_create(&s->hw_device_ctx, av_hwdevice_find_type_by_name(req_hw_type), NULL, NULL, 0);
    if (err < 0) {
      char errstr[128];
      av_make_error_string(errstr, 128, err);
      printf("Error in get_format \'%s\' av_hwdevice_ctx_create: %s\n", req_hw_type, errstr);
    }
    result = req_hw_pix_fmt;
  }

  return result;
}

napi_value decoder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value, formatJS, formatExt, global, jsObject, assign, jsParams;
  napi_valuetype type;
  bool isArray, hasName, hasID, hasFormat, hasStream, hasParams, hasHWaccel;
  AVCodecContext* decoder = nullptr;
  AVFormatContext* format = nullptr;
  const AVCodec* codec = nullptr;
  int ret = 0, streamIdx = -1;
  const AVCodecDescriptor* codecDesc = nullptr;
  AVCodecParameters* params = nullptr;
  char* codecName = nullptr;
  size_t codecNameLen = 0;
  size_t hwTypeLen = 0;
  int32_t codecID = -1;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;

  if (argc != 1) {
    NAPI_THROW_ERROR("Decoder requires a single options object.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if ((type != napi_object) || (isArray == true)) {
    NAPI_THROW_ERROR("Decoder must be configured with a single parameter, an options object.");
  }

  status = napi_has_named_property(env, args[0], "name", &hasName);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "codec_id", &hasID);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "demuxer", &hasFormat);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "stream_index", &hasStream);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "params", &hasParams);
  CHECK_STATUS;

  if (hasFormat && hasStream) {
    status = napi_get_named_property(env, args[0], "demuxer", &formatJS);
    CHECK_STATUS;
    status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
    CHECK_STATUS;
    status = napi_get_value_external(env, formatExt, (void**) &format);
    CHECK_STATUS;

    status = napi_get_named_property(env, args[0], "stream_index", &value);
    CHECK_STATUS;
    status = napi_get_value_int32(env, value, &streamIdx);
    CHECK_STATUS;
    if (streamIdx < 0 || streamIdx >= (int) format->nb_streams) {
      NAPI_THROW_ERROR("Stream index is out of bounds for the given format.");
    }
    params = format->streams[streamIdx]->codecpar;
    codecID = params->codec_id;
    codecName = (char*) avcodec_get_name(params->codec_id);
    codecNameLen = strlen(codecName);
    goto create;
  }

  if (hasParams) {
    status = napi_get_named_property(env, args[0], "params", &value);
    CHECK_STATUS;
    status = napi_get_named_property(env, value, "_codecPar", &jsParams);
    CHECK_STATUS;
    status = napi_typeof(env, jsParams, &type);
    CHECK_STATUS;
    if (type != napi_external) {
      NAPI_THROW_ERROR("The provided parameters do not appear to be a valid codec parameters object.");
    }
    status = napi_get_value_external(env, jsParams, (void**) &params);
    CHECK_STATUS;
    codecID = params->codec_id;
    codecName = (char*) avcodec_get_name(params->codec_id);
    codecNameLen = strlen(codecName);
    goto create;
  }

  if (!(hasName || hasID)) {
    NAPI_THROW_ERROR("Decoder must be identified with a 'codec_id' or a 'name'.");
  }

  if (hasName) {
    status = napi_get_named_property(env, args[0], "name", &value);
    CHECK_STATUS;
    codecNameLen = 64;
    codecName = (char*) malloc(sizeof(char) * (codecNameLen + 1));
    status = napi_get_value_string_utf8(env, value, codecName,
      64, &codecNameLen);
    CHECK_STATUS;
  }
  else {
    status = napi_get_named_property(env, args[0], "codec_id", &value);
    CHECK_STATUS;
    status = napi_get_value_int32(env, value, (int32_t*) &codecID);
    CHECK_STATUS;
  }

create:
  codec = ((codecID == -1) && (codecName != nullptr)) ?
    avcodec_find_decoder_by_name(codecName) :
    avcodec_find_decoder((AVCodecID) codecID);
  if ((codec == nullptr) && (codecID == -1)) { // one more go via codec descriptor
    codecDesc = avcodec_descriptor_get_by_name(codecName);
    if (codecDesc != nullptr) {
      codec = avcodec_find_decoder(codecDesc->id);
    }
  }
  if (codec == nullptr) {
    NAPI_THROW_ERROR("Failed to find a decoder from it's name.");
  }
  decoder = avcodec_alloc_context3(codec);
  if (decoder == nullptr) {
    NAPI_THROW_ERROR("Problem allocating decoder context.");
  }
  if (params != nullptr) {
    if ((ret = avcodec_parameters_to_context(decoder, params))) {
      printf("DEBUG: Failed to set context parameters from those provided.");
    }
  }

  status = napi_has_named_property(env, args[0], "hwaccel", &hasHWaccel);
  CHECK_STATUS;
  if (hasHWaccel) {
    status = napi_get_named_property(env, args[0], "hwaccel", &value);
    CHECK_STATUS;
    hwTypeLen = 64;
    req_hw_type = (char*) malloc(sizeof(char) * (hwTypeLen + 1));
    status = napi_get_value_string_utf8(env, value, req_hw_type,
      64, &hwTypeLen);
    CHECK_STATUS;
    req_hw_pix_fmt = av_get_pix_fmt(req_hw_type);

    if (0 != strcmp("auto", req_hw_type) && req_hw_pix_fmt == AV_PIX_FMT_NONE)
      printf("Decoder hwaccel name \'%s\' not recognised\n", req_hw_type);
    else
      decoder->get_format = get_format;
  }

  status = fromAVCodecContext(env, decoder, &result, false);
  const napi_value fargs[2] = { result, args[0] };
  CHECK_BAIL;

  status = napi_get_global(env, &global);
  CHECK_BAIL;
  status = napi_get_named_property(env, global, "Object", &jsObject);
  CHECK_BAIL;
  status = napi_get_named_property(env, jsObject, "assign", &assign);
  CHECK_BAIL;

  status = napi_call_function(env, result, assign, 2, fargs, &result);
  CHECK_BAIL;

  if (decoder != nullptr) return result;

bail:
  if (decoder != nullptr) {
    avcodec_close(decoder);
    avcodec_free_context(&decoder);
  }
  return nullptr;
}

void decoderFinalizer(napi_env env, void* data, void* hint) {
  AVCodecContext* decoder = (AVCodecContext*) data;
  avcodec_close(decoder);
  avcodec_free_context(&decoder);
}

void decodeExecute(napi_env env, void* data) {
  decodeCarrier* c = (decodeCarrier*) data;
  int ret = 0;
  AVFrame* frame = nullptr;
  AVFrame *sw_frame = nullptr;
  HR_TIME_POINT decodeStart = NOW;

  for ( auto it = c->packets.cbegin() ; it != c->packets.cend() ; it++ ) {
  bump:
    ret = avcodec_send_packet(c->decoder, *it);
    switch (ret) {
      case AVERROR(EAGAIN):
        // printf("Input is not accepted in the current state - user must read output with avcodec_receive_frame().\n");
        frame = av_frame_alloc();
        avcodec_receive_frame(c->decoder, frame);
        c->frames.push_back(frame);
        goto bump;
      case AVERROR_EOF:
        c->status = BEAMCODER_ERROR_EOF;
        c->errorMsg = "The decoder has been flushed, and no new packets can be sent to it.";
        return;
      case AVERROR(EINVAL):
        if ((ret = avcodec_open2(c->decoder, c->decoder->codec, nullptr))) {
          c->status = BEAMCODER_ERROR_ALLOC_DECODER;
          c->errorMsg = avErrorMsg("Problem opening decoder: ", ret);
          return;
        }
        goto bump;
      case AVERROR(ENOMEM):
        c->status = BEAMCODER_ERROR_ENOMEM;
        c->errorMsg = "Failed to add packet to internal queue.";
        return;
      case 0:
        // printf("Successfully sent packet to codec.\n");
        break;
      default:
        c->status = BEAMCODER_ERROR_DECODE;
        c->errorMsg = avErrorMsg("Error sending packet: ", ret);
        return;
    }
  } // loop through input packets

  AVPixelFormat frame_hw_pix_fmt = AV_PIX_FMT_NONE;
  if (c->decoder->hw_frames_ctx)
    frame_hw_pix_fmt = ((AVHWFramesContext*)c->decoder->hw_frames_ctx->data)->format;

  frame = av_frame_alloc();
  sw_frame = av_frame_alloc();
  do {
    ret = avcodec_receive_frame(c->decoder, frame);
    if (ret == 0) {
      if (frame->format == frame_hw_pix_fmt) {
        if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) {
          printf("Error transferring hw data to system memory\n");
        }
        c->frames.push_back(sw_frame);
        av_frame_free(&frame);
      } else
        c->frames.push_back(frame);

      frame = av_frame_alloc();
      sw_frame = av_frame_alloc();
    }
  } while (ret == 0);
  av_frame_free(&frame);
  av_frame_free(&sw_frame);

  c->totalTime = microTime(decodeStart);
};

void decodeComplete(napi_env env, napi_status asyncStatus, void* data) {
  decodeCarrier* c = (decodeCarrier*) data;
  napi_value result, frames, frame, prop;

  for ( auto it = c->packetRefs.cbegin() ; it != c->packetRefs.cend() ; it++ ) {
    c->status = napi_delete_reference(env, *it);
    REJECT_STATUS;
  }

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Decoder allocator failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_object(env, &result);
  REJECT_STATUS;
  c->status = beam_set_string_utf8(env, result, "type", "frames");
  REJECT_STATUS;

  c->status = napi_create_array(env, &frames);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "frames", frames);
  REJECT_STATUS;

  uint32_t frameCount = 0;
  for ( auto it = c->frames.begin() ; it != c->frames.end() ; it++ ) {
    frameData* f = new frameData;
    f->frame = *it;

    c->status = fromAVFrame(env, f, &frame);
    REJECT_STATUS;

    c->status = napi_set_element(env, frames, frameCount++, frame);
    REJECT_STATUS;
  }

  c->status = napi_create_int64(env, c->totalTime, &prop);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "total_time", prop);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
};

napi_value decode(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, decoderJS, decoderExt, value;
  decodeCarrier* c = new decodeCarrier;
  bool isArray;
  uint32_t packetsLength;
  napi_ref packetRef;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  napi_value* args = nullptr;

  c->status = napi_get_cb_info(env, info, &argc, args, &decoderJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, decoderJS, "_CodecContext", &decoderExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, decoderExt, (void**) &c->decoder);
  REJECT_RETURN;

  if (argc == 0) {
    REJECT_ERROR_RETURN("Decode call requires one or more packets.",
      BEAMCODER_INVALID_ARGS);
  }

  args = (napi_value*) malloc(sizeof(napi_value) * argc);
  c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  REJECT_RETURN;

  c->status = napi_is_array(env, args[0], &isArray);
  REJECT_RETURN;
  if (isArray) {
    c->status = napi_get_array_length(env, args[0], &packetsLength);
    REJECT_RETURN;
    for ( uint32_t x = 0 ; x < packetsLength ; x++ ) {
      c->status = napi_get_element(env, args[0], x, &value);
      REJECT_RETURN;
      c->status = isPacket(env, value);
      if (c->status != napi_ok) {
        REJECT_ERROR_RETURN("All passed values in an array must be of type packet.",
          BEAMCODER_INVALID_ARGS);
      }
    }
    for ( uint32_t x = 0 ; x < packetsLength ; x++ ) {
      c->status = napi_get_element(env, args[0], x, &value);
      REJECT_RETURN;
      c->status = napi_create_reference(env, value, 1, &packetRef);
      REJECT_RETURN;
      c->packetRefs.push_back(packetRef);
      c->packets.push_back(getPacket(env, value));
    }
  }
  else {
    for ( uint32_t x = 0 ; x < argc ; x++ ) {
      c->status = isPacket(env, args[x]);
      if (c->status != napi_ok) {
        REJECT_ERROR_RETURN("All passed packets as arguments must be of type packet.",
          BEAMCODER_INVALID_ARGS);
      }
    }
    for ( uint32_t x = 0 ; x < argc ; x++ ) {
      c->status = napi_create_reference(env, args[x], 1, &packetRef);
      REJECT_RETURN;
      c->packetRefs.push_back(packetRef);
      c->packets.push_back(getPacket(env, args[x]));
    }
  }

  c->status = napi_create_string_utf8(env, "Decode", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, decodeExecute,
    decodeComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  free(args);

  return promise;
};

napi_status isPacket(napi_env env, napi_value packet) {
  napi_status status;
  napi_value value;
  bool result;
  char objType[10];
  size_t typeLen;
  int cmp;
  napi_valuetype type;

  status = napi_typeof(env, packet, &type);
  if ((status != napi_ok) || (type != napi_object)) return napi_invalid_arg;
  status = napi_is_array(env, packet, &result);
  if ((status != napi_ok) || (result == true)) return napi_invalid_arg;

  status = napi_has_named_property(env, packet, "type", &result);
  if ((status != napi_ok) || (result == false)) return napi_invalid_arg;

  status = napi_has_named_property(env, packet, "_packet", &result);
  if ((status != napi_ok) || (result == false)) return napi_invalid_arg;

  status = napi_get_named_property(env, packet, "type", &value);
  if (status != napi_ok) return status;
  status = napi_get_value_string_utf8(env, value, objType, 10, &typeLen);
  if (status != napi_ok) return status;
  cmp = strcmp("Packet", objType);
  if (cmp != 0) return napi_invalid_arg;

  status = napi_get_named_property(env, packet, "_packet", &value);
  if (status != napi_ok) return status;
  status = napi_typeof(env, value, &type);
  if (status != napi_ok) return status;
  if (type != napi_external) return napi_invalid_arg;

  return napi_ok;
}

AVPacket* getPacket(napi_env env, napi_value packet) {
  napi_status status;
  napi_value value;
  packetData* result = nullptr;
  status = napi_get_named_property(env, packet, "_packet", &value);
  if (status != napi_ok) return nullptr;
  status = napi_get_value_external(env, value, (void**) &result);
  if (status != napi_ok) return nullptr;

  return result->packet;
}

napi_value flushDec(napi_env env, napi_callback_info info) {
  decodeCarrier* c = new decodeCarrier;
  napi_value decoderJS, decoderExt, promise, resourceName;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  napi_value* args = nullptr;

  c->status = napi_get_cb_info(env, info, &argc, args, &decoderJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, decoderJS, "_CodecContext", &decoderExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, decoderExt, (void**) &c->decoder);
  REJECT_RETURN;

  if (argc != 0) {
    REJECT_ERROR_RETURN("Decode flush takes no arguments.",
      BEAMCODER_INVALID_ARGS);
  }

  c->packets.push_back(nullptr);

  c->status = napi_create_string_utf8(env, "DecodeFlush", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, decodeExecute,
    decodeComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

/* napi_value getDecProperties(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, decoderJS, decoderExt;
  AVCodecContext* decoder;

  size_t argc = 0;
  napi_value* args = nullptr;

  status = napi_get_cb_info(env, info, &argc, args, &decoderJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, decoderJS, "_CodecContext", &decoderExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, decoderExt, (void**) &decoder);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_string_utf8(env, result, "type", "CodecContext");
  CHECK_STATUS;
  status = beam_set_bool(env, result, "encoding", false);
  CHECK_STATUS;
  status = getPropsFromCodec(env, result, decoder, false);
  CHECK_STATUS;

  return result;
} */

/* napi_value setDecProperties(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, decoderJS, decoderExt;
  napi_valuetype type;
  AVCodecContext* decoder;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, &decoderJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, decoderJS, "_CodecContext", &decoderExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, decoderExt, (void**) &decoder);
  CHECK_STATUS;

  if (argc < 1) {
    NAPI_THROW_ERROR("Cannot set decoder properties with no values.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Set properties must be provided as a single object.");
  }
  setCodecFromProps(env, decoder, args[0], false);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}; */

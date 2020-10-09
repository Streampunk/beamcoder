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

#include "encode.h"

napi_value encoder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value value, result, global, jsObject, assign, jsParams;
  napi_valuetype type;
  bool isArray, hasName, hasID, hasParams;
  char* codecName = nullptr;
  size_t codecNameLen = 0;
  int32_t codecID = -1;
  const AVCodec* codec = nullptr;
  const AVCodecDescriptor* codecDesc = nullptr;
  AVCodecContext* encoder;
  AVCodecParameters* codecParams = nullptr;
  int ret;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;

  if (argc != 1) {
    NAPI_THROW_ERROR("Encoder requires a single options object.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if ((type != napi_object) || (isArray == true)) {
    NAPI_THROW_ERROR("Encoder must be configured with a single parameter, an options object.");
  }

  status = napi_has_named_property(env, args[0], "name", &hasName);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "codec_id", &hasID);
  CHECK_STATUS;
  status = napi_has_named_property(env, args[0], "params", &hasParams);
  CHECK_STATUS;

  if (!(hasName || hasID || hasParams)) {
    NAPI_THROW_ERROR("Decoder must be identified with a 'codec_id', a 'name' or 'params'.");
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
    status = napi_get_value_external(env, jsParams, (void**) &codecParams);
    CHECK_STATUS;
    codecName = (char*) avcodec_get_name(codecParams->codec_id);
    codecNameLen = strlen(codecName);
    goto create;
  }
  if (hasName) {
    status = napi_get_named_property(env, args[0], "name", &value);
    CHECK_STATUS;
    codecNameLen = 64;
    codecName = (char*) malloc(sizeof(char) * (codecNameLen + 1));
    status = napi_get_value_string_utf8(env, value, codecName,
      codecNameLen, &codecNameLen);
    CHECK_STATUS;
  }
  else {
    status = napi_get_named_property(env, args[0], "codec_id", &value);
    CHECK_STATUS;
    status = napi_get_value_int32(env, value, &codecID);
    CHECK_STATUS;
  }

create:
  codec = ((codecID == -1) && (codecName != nullptr)) ?
    avcodec_find_encoder_by_name(codecName) :
    avcodec_find_encoder((AVCodecID) codecID);
  if ((codec == nullptr) && (codecID == -1)) { // one more go via codec descriptor
    codecDesc = avcodec_descriptor_get_by_name(codecName);
    if (codecDesc != nullptr) {
      codec = avcodec_find_encoder(codecDesc->id);
    }
  }
  if (codec == nullptr) {
    NAPI_THROW_ERROR("Failed to find an encoder from it's name or ID.");
  }
  encoder = avcodec_alloc_context3(codec);
  if (encoder == nullptr) {
    NAPI_THROW_ERROR("Problem allocating encoder context.");
  }

  if (codecParams != nullptr) {
    if ((ret = avcodec_parameters_to_context(encoder, (const AVCodecParameters*) codecParams))) {
      NAPI_THROW_ERROR(avErrorMsg("Failed to set encoder parameters from provided params: ", ret));
    }
    // printf("Params to context result: %i\n", ret);
  }

  status = fromAVCodecContext(env, encoder, &result, true);
  const napi_value fargs[] = { result, args[0] };
  CHECK_BAIL;

  status = napi_get_global(env, &global);
  CHECK_BAIL;
  status = napi_get_named_property(env, global, "Object", &jsObject);
  CHECK_BAIL;
  status = napi_get_named_property(env, jsObject, "assign", &assign);
  CHECK_BAIL;
  status = napi_call_function(env, result, assign, 2, fargs, &result);
  CHECK_BAIL;

  if ((encoder->sample_fmt != AV_SAMPLE_FMT_NONE) && 
      (encoder->sample_rate > 0) && (encoder->channel_layout != 0)) {
    // For audio encodes open the encoder if sufficient parameters have been provided
    // Encoder specific parameters will then be set up and available before the first encode
    ret = avcodec_open2(encoder, encoder->codec, nullptr);
    if (ret) {
      NAPI_THROW_ERROR(avErrorMsg("Failed to open audio encoder: ", ret));
    }
  }

  if (encoder != nullptr) return result;

bail:
  if (encoder != nullptr) {
    avcodec_close(encoder);
    avcodec_free_context(&encoder);
  }
  return nullptr;
};

void encoderFinalizer(napi_env env, void* data, void* hint) {
  AVCodecContext* encoder = (AVCodecContext*) data;
  avcodec_close(encoder);
  avcodec_free_context(&encoder);
};

void encodeExecute(napi_env env, void* data) {
  encodeCarrier* c = (encodeCarrier*) data;
  int ret = 0;
  AVPacket* packet = nullptr;
  HR_TIME_POINT encodeStart = NOW;

  for ( auto it = c->frames.cbegin() ; it != c->frames.cend() ; it++ ) {
  bump:
   ret = avcodec_send_frame(c->encoder, *it);
   switch (ret) {
     case AVERROR(EAGAIN):
       //printf("Input is not accepted in the current state - user must read output with avcodec_receive_frame().\n");
       packet = av_packet_alloc();
       avcodec_receive_packet(c->encoder, packet);
       c->packets.push_back(packet);
       goto bump;
     case AVERROR_EOF:
       c->status = BEAMCODER_ERROR_EOF;
       c->errorMsg = "The encoder has been flushed, and no new frames can be sent to it.";
       return;
     case AVERROR(EINVAL):
       if ((ret = avcodec_open2(c->encoder, c->encoder->codec, nullptr))) {
         c->status = BEAMCODER_ERROR_ALLOC_ENCODER;
         c->errorMsg = avErrorMsg("Problem opening encoder: ", ret);
         return;
       }
       goto bump;
     case AVERROR(ENOMEM):
       c->status = BEAMCODER_ERROR_ENOMEM;
       c->errorMsg = "Failed to add frame to internal queue.";
       return;
     case 0:
       //printf("Successfully sent frame to codec.\n");
       break;
     default:
       c->status = BEAMCODER_ERROR_ENCODE;
       c->errorMsg = avErrorMsg("Error sending frame: ", ret);
       return;
    }
  } // loop through input frames

  do {
    packet = av_packet_alloc();
    ret = avcodec_receive_packet(c->encoder, packet);
    if (ret == 0) {
      c->packets.push_back(packet);
      packet = av_packet_alloc();
    } else {
      //printf("Receive packet got status %i\n", ret);
    }
  } while (ret == 0);
  av_packet_free(&packet);

  c->totalTime = microTime(encodeStart);
  /* if (!c->frames.empty()) {
    printf("Finished encoding frame. First pts = %i\n", c->frames.front()->pts);
  } else {
    printf("Flushing complete.\n");
  } */
};

void encodeComplete(napi_env env, napi_status asyncStatus, void* data) {
  encodeCarrier* c = (encodeCarrier*) data;
  napi_value result, packets, packet, value;

  for ( auto it = c->frameRefs.cbegin() ; it != c->frameRefs.cend() ; it++ ) {
    // printf("Deleting frame reference. First pts = %i\n", c->frames.front()->pts);
    c->status = napi_delete_reference(env, *it);
    REJECT_STATUS;
  }

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Encode operation failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_object(env, &result);
  REJECT_STATUS;
  c->status = beam_set_string_utf8(env, result, "type", "packets");
  REJECT_STATUS;

  c->status = napi_create_array(env, &packets);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "packets", packets);
  REJECT_STATUS;

  uint32_t packetCount = 0;
  for ( auto it = c->packets.begin(); it != c->packets.end() ; it++ ) {
    packetData* p = new packetData;
    p->packet = *it;

    c->status = fromAVPacket(env, p, &packet);
    REJECT_STATUS;

    c->status = napi_set_element(env, packets, packetCount++, packet);
    REJECT_STATUS;
  }

  c->status = napi_create_int64(env, c->totalTime, &value);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "total_time", value);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
};

napi_value encode(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, encoderJS, encoderExt, value;
  encodeCarrier* c = new encodeCarrier;
  bool isArray;
  uint32_t framesLength;
  napi_ref frameRef;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  napi_value* args = nullptr;

  c->status = napi_get_cb_info(env, info, &argc, args, &encoderJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, encoderJS, "_CodecContext", &encoderExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, encoderExt, (void**) &c->encoder);
  REJECT_RETURN;

  if (argc == 0) {
    REJECT_ERROR_RETURN("Encode call requires one or more frames.",
      BEAMCODER_INVALID_ARGS);
  }

  args = (napi_value*) malloc(sizeof(napi_value) * argc);
  c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  REJECT_RETURN;

  c->status = napi_is_array(env, args[0], &isArray);
  REJECT_RETURN;
  if (isArray) {
    c->status = napi_get_array_length(env, args[0], &framesLength);
    REJECT_RETURN;
    for ( uint32_t x = 0 ; x < framesLength ; x++ ) {
      c->status = napi_get_element(env, args[0], x, &value);
      REJECT_RETURN;
      c->status = isFrame(env,value);
      if (c->status != napi_ok) {
        REJECT_ERROR_RETURN("Add passed values is an array whose elements must be of type frame.",
          BEAMCODER_INVALID_ARGS);
      }
    }
    for ( uint32_t x = 0 ; x < framesLength ; x++ ) {
      c->status = napi_get_element(env, args[0], x, &value);
      REJECT_RETURN;
      // printf("Creating a reference to a frame.\n");
      c->status = napi_create_reference(env, value, 1, &frameRef);
      REJECT_RETURN;
      c->frameRefs.push_back(frameRef);
      c->frames.push_back(getFrame(env, value));
    }
  } else {
    for ( uint32_t x = 0 ; x < argc ; x++ ) {
      c->status = isFrame(env, args[x]);
      if (c->status != napi_ok) {
        REJECT_ERROR_RETURN("All passed values as arguments must be of type frame.",
          BEAMCODER_INVALID_ARGS);
      }
    }
    for ( uint32_t x = 0 ; x < argc ; x++ ) {
      c->status = napi_create_reference(env, args[x], 1, &frameRef);
      REJECT_RETURN;
      c->frameRefs.push_back(frameRef);
      c->frames.push_back(getFrame(env, args[x]));
      // printf("Creating a reference to a frame. First pts = %i\n", c->frames.front()->pts);
    }
  }

  c->status = napi_create_string_utf8(env, "Encode", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, encodeExecute,
    encodeComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  free(args);

  return promise;
};

/* napi_value getEncProperties(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, encoderJS, encoderExt;
  AVCodecContext* encoder;

  size_t argc = 0;
  napi_value* args = nullptr;

  status = napi_get_cb_info(env, info, &argc, args, &encoderJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, encoderJS, "_encoder", &encoderExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, encoderExt, (void**) &encoder);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_string_utf8(env, result, "type", "CodecContext");
  CHECK_STATUS;
  status = beam_set_bool(env, result, "encoding", true);
  CHECK_STATUS;
  status = getPropsFromCodec(env, result, encoder, true);
  CHECK_STATUS;

  return result;
}; */

/* napi_value setEncProperties(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, encoderJS, encoderExt;
  napi_valuetype type;
  AVCodecContext* encoder;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, &encoderJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, encoderJS, "_encoder", &encoderExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, encoderExt, (void**) &encoder);
  CHECK_STATUS;

  if (argc < 1) {
    NAPI_THROW_ERROR("Cannot set encoder properties with no values.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Set properties must be provided as a single object.");
  }
  setCodecFromProps(env, encoder, args[0], false);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}; */

napi_status isFrame(napi_env env, napi_value packet) {
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

  status = napi_has_named_property(env, packet, "_frame", &result);
  if ((status != napi_ok) || (result == false)) return napi_invalid_arg;

  status = napi_get_named_property(env, packet, "type", &value);
  if (status != napi_ok) return status;
  status = napi_get_value_string_utf8(env, value, objType, 10, &typeLen);
  if (status != napi_ok) return status;
  cmp = strcmp("Frame", objType);
  if (cmp != 0) return napi_invalid_arg;

  status = napi_get_named_property(env, packet, "_frame", &value);
  if (status != napi_ok) return status;
  status = napi_typeof(env, value, &type);
  if (status != napi_ok) return status;
  if (type != napi_external) return napi_invalid_arg;

  return napi_ok;
}

AVFrame* getFrame(napi_env env, napi_value packet) {
  napi_status status;
  napi_value value;
  frameData* result = nullptr;
  status = napi_get_named_property(env, packet, "_frame", &value);
  if (status != napi_ok) return nullptr;
  status = napi_get_value_external(env, value, (void**) &result);
  if (status != napi_ok) return nullptr;

  return result->frame;
};

napi_value flushEnc(napi_env env, napi_callback_info info) {
  encodeCarrier* c = new encodeCarrier;
  napi_value encoderJS, encoderExt, promise, resourceName;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  napi_value* args = nullptr;

  c->status = napi_get_cb_info(env, info, &argc, args, &encoderJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, encoderJS, "_CodecContext", &encoderExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, encoderExt, (void**) &c->encoder);
  REJECT_RETURN;

  if (argc != 0) {
    REJECT_ERROR_RETURN("Encode flush takes no arguments.",
      BEAMCODER_INVALID_ARGS);
  }

  c->frames.push_back(nullptr);

  c->status = napi_create_string_utf8(env, "EncodeFlush", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, encodeExecute,
    encodeComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

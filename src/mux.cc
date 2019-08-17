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

#include "mux.h"

int write_packet(void *opaque, uint8_t *buf, int buf_size)
{
  Adaptor *adaptor = (Adaptor *)opaque;
  return adaptor->write(buf, buf_size);
}

napi_value muxer(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, prop, subprop;
  int ret;
  AVOutputFormat* oformat = nullptr;
  char* formatName = nullptr;
  char* filename = nullptr;
  size_t strLen;
  napi_valuetype type;
  bool isArray;
  AVFormatContext* fmtCtx = nullptr;
  Adaptor *adaptor = nullptr;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Muxer can only be allocated with a single options object.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Muxer must be allocated be a single options object.");
  }
  status = napi_get_named_property(env, args[0], "format_name", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  if (type == napi_undefined) {
    status = napi_get_named_property(env, args[0], "name", &prop);
    CHECK_STATUS;
    status = napi_typeof(env, prop, &type);
    CHECK_STATUS;
  }
  if (type == napi_string) { // Found a name property
    status = napi_get_value_string_utf8(env, prop, nullptr, 0, &strLen);
    CHECK_STATUS;
    formatName = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, prop, formatName, strLen + 1, &strLen);
    CHECK_STATUS;
  }

  status = napi_get_named_property(env, args[0], "filename", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  if (type == napi_string) {
    status = napi_get_value_string_utf8(env, prop, nullptr, 0, &strLen);
    CHECK_STATUS;
    filename = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, prop, filename, strLen + 1, &strLen);
    CHECK_STATUS;
  }

  status = napi_get_named_property(env, args[0], "oformat", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  status = napi_is_array(env, prop, &isArray);
  CHECK_STATUS;
  if (!isArray && (type == napi_object)) {
    status = napi_get_named_property(env, prop, "_oformat", &subprop);
    CHECK_STATUS;
    status = napi_typeof(env, subprop, &type);
    CHECK_STATUS;
    if (type == napi_external) {
      status = napi_get_value_external(env, subprop, (void**) &oformat);
      CHECK_STATUS;
    }
  }

  status = napi_get_named_property(env, args[0], "governor", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  if (type == napi_object) {
    napi_value adaptorValue;
    status = napi_get_named_property(env, prop, "_adaptor", &adaptorValue);
    CHECK_STATUS;
    status = napi_typeof(env, adaptorValue, &type);
    CHECK_STATUS;
    if (type == napi_external) {
      status = napi_get_value_external(env, adaptorValue, (void**)&adaptor);
      CHECK_STATUS;
    } else if (type != napi_undefined) {
      NAPI_THROW_ERROR("Adaptor must be of external type when specified.");
    }
  }

  AVIOContext* avio_ctx = nullptr;
  if (adaptor) {
    avio_ctx = avio_alloc_context(adaptor->buf(), adaptor->bufLen(), 1, adaptor, nullptr, &write_packet, nullptr);
    if (!avio_ctx) {
      NAPI_THROW_ERROR("Problem allocating muxer stream output context.");
    }
  }
  ret = avformat_alloc_output_context2(&fmtCtx, oformat, formatName, filename);

  free(formatName);
  free(filename);

  if (ret < 0) {
    NAPI_THROW_ERROR(avErrorMsg("Error allocating muxer context: ", ret));
  }

  fmtCtx->pb = avio_ctx;

  status = fromAVFormatContext(env, fmtCtx, adaptor, &result);
  CHECK_STATUS;

  status = napi_create_function(env, "openIO", NAPI_AUTO_LENGTH,
    openIO, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "openIO", prop);
  CHECK_STATUS;

  status = napi_create_function(env, "writeHeader", NAPI_AUTO_LENGTH,
    writeHeader, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "writeHeader", prop);
  CHECK_STATUS;

  status = napi_create_function(env, "initOutput", NAPI_AUTO_LENGTH,
    initOutput, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "initOutput", prop);
  CHECK_STATUS;

  status = napi_create_function(env, "writeFrame", NAPI_AUTO_LENGTH,
    writeFrame, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "writeFrame", prop);
  CHECK_STATUS;

  status = napi_create_function(env, "writeTrailer", NAPI_AUTO_LENGTH,
    writeTrailer, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "writeTrailer", prop);
  CHECK_STATUS;

  status = napi_create_function(env, "forceClose", NAPI_AUTO_LENGTH,
    forceClose, nullptr, &prop);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "forceClose", prop);
  CHECK_STATUS;

  return result;
}

void openIOExecute(napi_env env, void* data) {
  openIOCarrier* c = (openIOCarrier*) data;
  int ret;
  if (c->format->pb == nullptr) {
    ret = avio_open2(&c->format->pb, c->format->url, c->flags, nullptr, &c->options);
    if (ret < 0) {
      c->status = BEAMCODER_ERROR_OPENIO;
      c->errorMsg = avErrorMsg("Problem opening IO context: ", ret);
    }
  }
}

void openIOComplete(napi_env env, napi_status asyncStatus, void* data) {
  napi_value result, unset;
  openIOCarrier* c = (openIOCarrier*) data;
  AVDictionaryEntry* tag = nullptr;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Open IO failed to complete.";
  }
  REJECT_STATUS;

  if (c->options != nullptr) {
    c->status = napi_create_object(env, &result);
    REJECT_STATUS;
    c->status = napi_create_object(env, &unset);
    REJECT_STATUS;
    c->status = napi_set_named_property(env, result, "unset", unset);
    REJECT_STATUS;
    while ((tag = av_dict_get(c->options, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      c->status = beam_set_string_utf8(env, unset, tag->key, tag->value);
      REJECT_STATUS;
    }
  } else {
    c->status = napi_get_undefined(env, &result);
    REJECT_STATUS;
  }

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value openIO(napi_env env, napi_callback_info info) {
  napi_value promise, formatJS, formatExt, resourceName, prop;
  napi_valuetype type;
  bool isArray, present, flag;
  size_t strLen;
  openIOCarrier* c = new openIOCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  c->status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatExt, (void**) &c->format);
  REJECT_RETURN;

  if (argc > 0) { // Possible options, url and flags
    napi_value args[1];
    argc = 1;
    c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    REJECT_RETURN;
    c->status = napi_typeof(env, args[0], &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, args[0], &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) {
      REJECT_ERROR_RETURN("OpenIO can only be provided with a single options object.",
        BEAMCODER_INVALID_ARGS);
    }

    c->status = napi_get_named_property(env, args[0], "url", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    if (type == napi_string) {
      c->status = napi_get_value_string_utf8(env, prop, nullptr, 0, &strLen);
      REJECT_RETURN;
      c->format->url = (char*) av_malloc(sizeof(char) * (strLen + 1));
      c->status = napi_get_value_string_utf8(env, prop, c->format->url, strLen + 1, &strLen);
      REJECT_RETURN;
    }

    c->status = napi_get_named_property(env, args[0], "filename", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    if (type == napi_string) {
      c->status = napi_get_value_string_utf8(env, prop, nullptr, 0, &strLen);
      REJECT_RETURN;
      c->format->url = (char*) av_malloc(sizeof(char) * (strLen + 1));
      c->status = napi_get_value_string_utf8(env, prop, c->format->url, strLen + 1, &strLen);
      REJECT_RETURN;
    }

    c->status = napi_get_named_property(env, args[0], "options", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, prop, &isArray);
    REJECT_RETURN;
    if (!isArray && (type == napi_object)) {
      c->status = makeAVDictionary(env, prop, &c->options);
      REJECT_RETURN;
    }

    c->status = napi_get_named_property(env, args[0], "flags", &prop);
    REJECT_RETURN
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, prop, &isArray);
    REJECT_RETURN;
    if (!isArray && (type == napi_object)) {
      c->status = beam_get_bool(env, prop, "READ", &present, &flag);
      REJECT_RETURN;
      if (present) { c->flags = (flag) ?
        c->flags | AVIO_FLAG_READ :
        c->flags & ~AVIO_FLAG_READ; }
      c->status = beam_get_bool(env, prop, "WRITE", &present, &flag);
      REJECT_RETURN;
      if (present) { c->flags = (flag) ?
        c->flags | AVIO_FLAG_WRITE :
        c->flags & ~AVIO_FLAG_WRITE; }
      c->status = beam_get_bool(env, prop, "NONBLOCK", &present, &flag);
      REJECT_RETURN;
      if (present) { c->flags = (flag) ?
        c->flags | AVIO_FLAG_NONBLOCK :
        c->flags & ~AVIO_FLAG_NONBLOCK; }
      c->status = beam_get_bool(env, prop, "DIRECT", &present, &flag);
      REJECT_RETURN;
      if (present) { c->flags = (flag) ?
        c->flags | AVIO_FLAG_DIRECT :
        c->flags & ~AVIO_FLAG_DIRECT; }
    }
  }

  if ((c->format->url == nullptr) && (c->format->pb == nullptr)) {
    REJECT_ERROR_RETURN("Cannot open muxer IO without a URL or filename.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_create_string_utf8(env, "OpenIO", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, openIOExecute,
    openIOComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void writeHeaderExecute(napi_env env, void* data) {
  writeHeaderCarrier* c = (writeHeaderCarrier*) data;

  c->result = avformat_write_header(c->format, &c->options);
  if (c->result < 0) {
    c->status = BEAMCODER_ERROR_WRITE_HEADER;
    c->errorMsg = avErrorMsg("Failed to write header: ", c->result);
    return;
  }
}

void writeHeaderComplete(napi_env env, napi_status asyncStatus, void* data) {
  napi_value result, unset, initIn;
  writeHeaderCarrier* c = (writeHeaderCarrier*) data;
  AVDictionaryEntry* tag = nullptr;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Write header failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_object(env, &result);
  REJECT_STATUS;

  c->status = napi_create_string_utf8(env,
    (c->result == AVSTREAM_INIT_IN_WRITE_HEADER) ? "WRITE_HEADER" : "INIT_OUTPUT",
    NAPI_AUTO_LENGTH, &initIn);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "INIT_IN", initIn);
  REJECT_STATUS;

  if (c->options != nullptr) {
    c->status = napi_create_object(env, &unset);
    REJECT_STATUS;
    c->status = napi_set_named_property(env, result, "unset", unset);
    REJECT_STATUS;
    while ((tag = av_dict_get(c->options, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      c->status = beam_set_string_utf8(env, unset, tag->key, tag->value);
      REJECT_STATUS;
    }
  }

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value writeHeader(napi_env env, napi_callback_info info) {
  napi_value promise, formatJS, formatExt, resourceName, prop;
  napi_valuetype type;
  bool isArray;
  writeHeaderCarrier* c = new writeHeaderCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  c->status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatExt, (void**) &c->format);
  REJECT_RETURN;

  if (argc > 0) { // Possible options
    napi_value args[1];
    argc = 1;
    c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    REJECT_RETURN;
    c->status = napi_typeof(env, args[0], &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, args[0], &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) {
      REJECT_ERROR_RETURN("Write header can only be provided with a single options object.",
        BEAMCODER_INVALID_ARGS);
    }

    c->status = napi_get_named_property(env, args[0], "options", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, prop, &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) { // Allow flat or nested options
      prop = args[0];
    }
    c->status = makeAVDictionary(env, prop, &c->options);
    REJECT_RETURN;
  }

  c->status = napi_create_string_utf8(env, "WriteHeader", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, writeHeaderExecute,
    writeHeaderComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void initOutputExecute(napi_env env, void* data) {
  initOutputCarrier* c = (initOutputCarrier*) data;

  c->result = avformat_init_output(c->format, &c->options);
  if (c->result < 0) {
    c->status = BEAMCODER_ERROR_INIT_OUTPUT;
    c->errorMsg = avErrorMsg("Failed to initialize output: ", c->result);
    return;
  }
}

void initOutputComplete(napi_env env, napi_status asyncStatus, void* data) {
  napi_value result, initIn, unset;
  AVDictionaryEntry* tag = nullptr;
  initOutputCarrier* c = (initOutputCarrier*) data;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Initialize output failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_object(env, &result);
  REJECT_STATUS;

  c->status = napi_create_string_utf8(env,
    (c->result == AVSTREAM_INIT_IN_WRITE_HEADER) ? "WRITE_HEADER" : "INIT_OUTPUT",
    NAPI_AUTO_LENGTH, &initIn);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "INIT_IN", initIn);
  REJECT_STATUS;

  if (c->options != nullptr) {
    c->status = napi_create_object(env, &unset);
    REJECT_STATUS;
    c->status = napi_set_named_property(env, result, "unset", unset);
    REJECT_STATUS;
    while ((tag = av_dict_get(c->options, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      c->status = beam_set_string_utf8(env, unset, tag->key, tag->value);
      REJECT_STATUS;
    }
  }

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value initOutput(napi_env env, napi_callback_info info) {
  napi_value promise, formatJS, formatExt, resourceName, prop;
  napi_valuetype type;
  bool isArray;
  initOutputCarrier* c = new initOutputCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  c->status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatExt, (void**) &c->format);
  REJECT_RETURN;

  if (argc > 0) { // Possible options
    napi_value args[1];
    argc = 1;
    c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    REJECT_RETURN;
    c->status = napi_typeof(env, args[0], &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, args[0], &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) {
      REJECT_ERROR_RETURN("Initialize output can only be provided with a single options object.",
        BEAMCODER_INVALID_ARGS);
    }

    c->status = napi_get_named_property(env, args[0], "options", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, prop, &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) { // Allow flat or nested options
      prop = args[0];
    }
    c->status = makeAVDictionary(env, prop, &c->options);
    REJECT_RETURN;
  }

  c->status = napi_create_string_utf8(env, "InitOutput", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, initOutputExecute,
    initOutputComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void writeFrameExecute(napi_env env, void* data) {
  writeFrameCarrier* c = (writeFrameCarrier*) data;
  int ret;

  if (c->interleaved) {
    if (c->packet != nullptr) {
      ret = av_interleaved_write_frame(c->format, c->packet);
    } else if (c->frame != nullptr) {
      ret = av_interleaved_write_uncoded_frame(c->format, c->streamIndex, c->frame);
    } else {
      ret = av_interleaved_write_frame(c->format, nullptr); // flush
    }
  } else {
    if (c->packet != nullptr) {
      ret = av_write_frame(c->format, c->packet);
    } else if (c->frame != nullptr) {
      ret = av_write_uncoded_frame(c->format, c->streamIndex, c->frame);
    } else {
      ret = av_write_frame(c->format, nullptr); // flush
    }
  }

  if (ret < 0) {
    c->status = BEAMCODER_ERROR_WRITE_FRAME;
    c->errorMsg = avErrorMsg("Error writing frame: ", ret);
    return;
  }
}

void writeFrameComplete(napi_env env, napi_status asyncStatus, void* data) {
  napi_value result;
  writeFrameCarrier* c = (writeFrameCarrier*) data;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Write frame failed to complete.";
  }
  REJECT_STATUS;

  // tidy up adaptor chunks if required
  if (c->adaptor) {
    c->status = c->adaptor->finaliseBufs(env);
    REJECT_STATUS;
  }

  c->status = napi_get_undefined(env, &result);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value writeFrame(napi_env env, napi_callback_info info) {
  napi_value promise, formatJS, formatExt, adaptorExt, interleavedJS, resourceName, options, prop;
  napi_valuetype type;
  bool isArray;
  bool hasOptions = false;
  writeFrameCarrier* c = new writeFrameCarrier;
  packetData* packetData;
  frameData* frameData;
  int ret;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];
  c->status = napi_get_cb_info(env, info, &argc, args, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatExt, (void**) &c->format);
  REJECT_RETURN;

  c->status = napi_get_named_property(env, formatJS, "_adaptor", &adaptorExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, adaptorExt, (void**)&c->adaptor);
  REJECT_RETURN;

  c->status = napi_get_named_property(env, formatJS, "interleaved", &interleavedJS);
  REJECT_RETURN;
  c->status = napi_get_value_bool(env, interleavedJS, &c->interleaved);
  REJECT_RETURN;

  if (argc > 0) { // is it a packet, a frame, an options object
    c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    REJECT_RETURN;
    c->status = napi_typeof(env, args[0], &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, args[0], &isArray);
    REJECT_RETURN;
    if (isArray || (type != napi_object)) {
      REJECT_ERROR_RETURN("Write frame requires a frame, a packet or an options object.",
        BEAMCODER_INVALID_ARGS);
    }

    c->status = napi_get_named_property(env, args[0], "packet", &options);
    REJECT_RETURN;
    c->status = napi_typeof(env, options, &type);
    REJECT_RETURN;
    hasOptions = (type == napi_object);
    c->status = napi_get_named_property(env,
      hasOptions ? options : args[0], "_packet", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    if (type == napi_external) {
      c->status = napi_get_value_external(env, prop, (void**) &packetData);
      REJECT_RETURN;
      c->packet = av_packet_alloc();
      if ((ret = av_packet_ref(c->packet, const_cast<AVPacket*>(packetData->packet)))) {
        REJECT_ERROR_RETURN(avErrorMsg("Failed to reference packet: ", ret),
          BEAMCODER_ERROR_ENOMEM);
      };
      goto work;
    }

    c->status = napi_get_named_property(env, args[0], "frame", &options);
    REJECT_RETURN;
    c->status = napi_typeof(env, options, &type);
    REJECT_RETURN;
    hasOptions = (type == napi_object);
    c->status = napi_get_named_property(env,
      hasOptions ? options : args[0], "_frame", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    if (type == napi_external) {
      c->status = napi_get_value_external(env, prop, (void**) &frameData);
      REJECT_RETURN;
      c->frame = av_frame_alloc();
      if ((ret = av_frame_ref(c->frame, const_cast<AVFrame*>(frameData->frame)))) {
        REJECT_ERROR_RETURN(avErrorMsg("Failed to reference frame: ", ret),
          BEAMCODER_ERROR_ENOMEM);
      };
    }

    c->status = napi_get_named_property(env,
      hasOptions ? options : args[0], "stream_index", &prop);
    REJECT_RETURN;
    c->status = napi_typeof(env, prop, &type);
    REJECT_RETURN;
    if (type == napi_number) {
      c->status = napi_get_value_int32(env, prop, &c->streamIndex);
      REJECT_RETURN;
    }
    if (c->streamIndex < 0) {
      REJECT_ERROR_RETURN("Frame for writing must be provided with a stream_index, either as an internal property or in an options object.",
        BEAMCODER_INVALID_ARGS);
    }
  } else {
    REJECT_ERROR_RETURN("A packet or frame must be provided.",
      BEAMCODER_INVALID_ARGS);
  }

work:
  if ((c->packet != nullptr) || (c->frame != nullptr)) {
    c->status = napi_create_reference(env, hasOptions ? options : args[0], 1,
      &c->passthru);
    REJECT_RETURN;
  } // Don't garbage collect during write

  c->status = napi_create_string_utf8(env, "WriteFrame", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, writeFrameExecute,
    writeFrameComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void writeTrailerExecute(napi_env env, void* data) {
  writeTrailerCarrier* c = (writeTrailerCarrier*) data;
  int retWrite = 0, retClose = 0;

  retWrite = av_write_trailer(c->format);
  if (c->format->pb != nullptr) {
    if (c->adaptor) {
      c->adaptor->finish();
      avio_context_free(&c->format->pb);
    }
    else
      retClose = avio_closep(&c->format->pb);
  }
  if ((retWrite < 0) && (retClose < 0)) {
    c->status = BEAMCODER_ERROR_WRITE_TRAILER;
    c->errorMsg = "Errors writing trailer and closing IO.";
    return;
  }
  if (retWrite < 0) {
    c->status = BEAMCODER_ERROR_WRITE_TRAILER;
    c->errorMsg = avErrorMsg("Error writing trailer: ", retWrite);
    return;
  }
  if (retClose < 0) {
    c->status = BEAMCODER_ERROR_WRITE_TRAILER;
    c->errorMsg = avErrorMsg("Error closing IO: ", retClose);
    return;
  }
}

void writeTrailerComplete(napi_env env, napi_status asyncStatus, void* data) {
  napi_value result;
  writeTrailerCarrier* c = (writeTrailerCarrier*) data;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Write trailer failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_get_undefined(env, &result);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value writeTrailer(napi_env env, napi_callback_info info) {
  napi_value promise, formatJS, formatExt, adaptorExt, resourceName;
  writeTrailerCarrier* c = new writeTrailerCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  c->status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatExt, (void**) &c->format);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_adaptor", &adaptorExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, adaptorExt, (void**) &c->adaptor);
  REJECT_RETURN;

  c->status = napi_create_string_utf8(env, "WriteTrailer", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, writeTrailerExecute,
    writeTrailerComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

napi_value forceClose(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, formatJS, formatExt;
  AVFormatContext* format;
  int ret;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, formatJS, "_formatContext", &formatExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, formatExt, (void**) &format);
  CHECK_STATUS;

  if (format->pb != nullptr) {
    ret = avio_closep(&format->pb);
    if (ret < 0) {
      NAPI_THROW_ERROR(avErrorMsg("Failed to force close muxer resource: ", ret));
    }
  } else {
    printf("DEBUG: Muxer IO resource '%s' already closed or not managed by AVIO.\n",
      (format->url != nullptr) ? format->url : "unknown");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

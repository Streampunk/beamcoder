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

#include "demux.h"

int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
  Adaptor *adaptor = (Adaptor *)opaque;
  int numBytes = adaptor->read(buf, buf_size);
  if (0 == numBytes)
    return AVERROR_EOF;

  return numBytes;
}

void demuxerExecute(napi_env env, void* data) {
  demuxerCarrier* c = (demuxerCarrier*) data;

  int ret;

  if (!(c->format = avformat_alloc_context())) {
    c->status = BEAMCODER_ERROR_START;
    c->errorMsg = avErrorMsg("Problem allocating demuxer: ", AVERROR(ENOMEM));
    return;
  }

  if (c->adaptor) {
    AVIOContext* avio_ctx = avio_alloc_context(nullptr, 0, 0, c->adaptor, &read_packet, nullptr, nullptr);
    if (!avio_ctx) {
      c->status = BEAMCODER_ERROR_START;
      c->errorMsg = avErrorMsg("Problem allocating demuxer context: ", AVERROR(ENOMEM));
      return;
    }
    c->format->pb = avio_ctx;
  }

  if ((ret = avformat_open_input(&c->format, c->filename, c->iformat, &c->options))) {
    c->status = BEAMCODER_ERROR_START;
    c->errorMsg = avErrorMsg("Problem opening input format: ", ret);
    return;
  }

  if ((ret = avformat_find_stream_info(c->format, nullptr))) {
    printf("DEBUG: Could not find stream info for file %s, return value %i.",
      c->filename, ret);
  }
}

void demuxerComplete(napi_env env,  napi_status asyncStatus, void* data) {
  demuxerCarrier* c = (demuxerCarrier*) data;
  napi_value result, prop;
  AVDictionaryEntry* tag = nullptr;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Demuxer creator failed to complete.";
  }
  REJECT_STATUS;

  // tidy up adaptor chunks if required
  if (c->adaptor) {
    c->status = c->adaptor->finaliseBufs(env);
    REJECT_STATUS;
  }

  while ((tag = av_dict_get(c->options, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    printf("DEBUG: On creating demuxer '%s', failed to set option %s.\n",
      c->format->iformat->name, tag->key);
  }

  c->status = fromAVFormatContext(env, c->format, c->adaptor, &result);
  c->format = nullptr;
  REJECT_STATUS;

  c->status = napi_create_function(env, "readFrame", NAPI_AUTO_LENGTH, readFrame,
    nullptr, &prop);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "read", prop);
  REJECT_STATUS;

  c->status = napi_create_function(env, "seekFrame", NAPI_AUTO_LENGTH, seekFrame,
    nullptr, &prop);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "seek", prop);
  REJECT_STATUS;

  c->status = napi_create_function(env, "forceClose", NAPI_AUTO_LENGTH, forceCloseInput,
    nullptr, &prop);
  REJECT_STATUS;
  c->status = napi_set_named_property(env, result, "forceClose", prop);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value demuxer(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, value, subValue;
  napi_valuetype type;
  size_t strLen;
  bool isArray;
  demuxerCarrier* c = new demuxerCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];

  c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  REJECT_RETURN;

  if (argc < 1) {
    REJECT_ERROR_RETURN("Format requires a filename, URL or buffer.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_typeof(env, args[0], &type);
  REJECT_RETURN;
  c->status = napi_is_array(env, args[0], &isArray);
  REJECT_RETURN;

  if (type == napi_string) {
    c->status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
    REJECT_RETURN;
    c->filename = (const char *) malloc((strLen + 1) * sizeof(char));
    c->status = napi_get_value_string_utf8(env, args[0], (char *) c->filename, strLen + 1, &strLen);
    REJECT_RETURN;
  } else if ((isArray == false) && (type == napi_object)) {
    napi_value governorValue;
    c->status = napi_get_named_property(env, args[0], "governor", &governorValue);
    REJECT_RETURN;
    c->status = napi_typeof(env, governorValue, &type);
    REJECT_RETURN;
    if (type == napi_object) {
      napi_value adaptorValue;
      c->status = napi_get_named_property(env, governorValue, "_adaptor", &adaptorValue);
      REJECT_RETURN;
      c->status = napi_typeof(env, adaptorValue, &type);
      REJECT_RETURN;
      if (type == napi_external) {
        c->status = napi_get_value_external(env, adaptorValue, (void**)&c->adaptor);
        REJECT_RETURN;
      } else if (type != napi_undefined) {
        REJECT_ERROR_RETURN("Adaptor must be of external type when specified.",
          BEAMCODER_INVALID_ARGS);
      }
    }

    c->status = napi_get_named_property(env, args[0], "url", &value);
    REJECT_RETURN;
    c->status = napi_typeof(env, value, &type);
    REJECT_RETURN;
    if (type == napi_string) {
      c->status = napi_get_value_string_utf8(env, value, nullptr, 0, &strLen);
      REJECT_RETURN;
      c->filename = (const char *) malloc((strLen + 1) * sizeof(char));
      c->status = napi_get_value_string_utf8(env, value, (char *) c->filename, strLen + 1, &strLen);
      REJECT_RETURN;
    }

    c->status = napi_get_named_property(env, args[0], "iformat", &value);
    REJECT_RETURN;
    c->status = napi_typeof(env, value, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, value, &isArray);
    REJECT_RETURN;
    if (!isArray && (type == napi_object)) {
      c->status = napi_get_named_property(env, value, "_iformat", &subValue);
      REJECT_RETURN;
      c->status = napi_typeof(env, subValue, &type);
      REJECT_RETURN;
      if (type == napi_external) {
        c->status = napi_get_value_external(env, subValue, (void**) &c->iformat);
        REJECT_RETURN;
      }
    }

    c->status = napi_get_named_property(env, args[0], "options", &value);
    REJECT_RETURN;
    c->status = napi_typeof(env, value, &type);
    REJECT_RETURN;
    c->status = napi_is_array(env, value, &isArray);
    REJECT_RETURN;
    if ((isArray == false) && (type == napi_object)) {
      c->status = makeAVDictionary(env, value, &c->options);
      REJECT_RETURN;
    }
  }

  if ((c->filename == nullptr) && (c->adaptor == nullptr)) {
    REJECT_ERROR_RETURN("Neither a filename nor an adaptor have been provided.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_create_string_utf8(env, "Format", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, demuxerExecute,
    demuxerComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void demuxerFinalizer(napi_env env, void* data, void* hint) {
  AVFormatContext *fmtCtx = (AVFormatContext*) data;
  avformat_close_input(&fmtCtx);
}

void readFrameExecute(napi_env env, void* data) {
  readFrameCarrier* c = (readFrameCarrier*) data;
  int ret;

  AVFormatContext* fmtCtx = c->formatRef->fmtCtx;
  if (fmtCtx == nullptr) {
    c->status = BEAMCODER_ERROR_READ_FRAME;
    c->errorMsg = "Format context has been deleted.";
    return;
  }

  ret = av_read_frame(fmtCtx, c->packet);
  if (ret == AVERROR_EOF) {
    av_packet_free(&c->packet);
  } else if (ret < 0) {
    c->status = BEAMCODER_ERROR_READ_FRAME;
    c->errorMsg = avErrorMsg("Problem reading frame: ", ret);
    return;
  }
}

void readFrameComplete(napi_env env, napi_status asyncStatus, void* data) {
  readFrameCarrier* c = (readFrameCarrier*) data;
  napi_value result;
  packetData* p;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Read frame failed to complete.";
  }
  REJECT_STATUS;

  // tidy up adaptor chunks if required
  if (c->adaptor) {
    c->status = c->adaptor->finaliseBufs(env);
    REJECT_STATUS;
  }

  if (c->packet != nullptr) {
    p = new packetData;
    p->packet = c->packet;
    c->status = fromAVPacket(env, p, &result);
    c->packet = nullptr;
    REJECT_STATUS;
  } else {
    c->status = napi_get_null(env, &result);
    REJECT_STATUS;
  }

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value readFrame(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, formatJS, formatRefExt, adaptorExt;
  readFrameCarrier* c = new readFrameCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 0;
  c->status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContextRef", &formatRefExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatRefExt, (void**) &c->formatRef);
  REJECT_RETURN;

  c->status = napi_get_named_property(env, formatJS, "_adaptor", &adaptorExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, adaptorExt, (void**)&c->adaptor);
  REJECT_RETURN;

  c->status = napi_create_reference(env, formatJS, 1, &c->passthru);
  REJECT_RETURN;

  c->status = napi_create_string_utf8(env, "ReadFrame", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, readFrameExecute,
    readFrameComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

void readBufferFinalizer(napi_env env, void* data, void* hint) {
  AVBufferRef* hintRef = (AVBufferRef*) hint;
  napi_status status;
  int64_t externalMemory;
  status = napi_adjust_external_memory(env, -hintRef->size, &externalMemory);
  if (status != napi_ok) {
    printf("DEBUG: Napi failure to adjust external memory. In beamcoder format.cc readBufferFinalizer.");
  }
  av_buffer_unref(&hintRef);
}

void seekFrameExecute(napi_env env, void *data) {
  seekFrameCarrier* c = (seekFrameCarrier*) data;
  int ret;

  AVFormatContext* fmtCtx = c->formatRef->fmtCtx;
  if (fmtCtx == nullptr) {
    c->status = BEAMCODER_ERROR_READ_FRAME;
    c->errorMsg = "Format context has been deleted.";
    return;
  }

  ret = av_seek_frame(fmtCtx, c->streamIndex, c->timestamp, c->flags);
  // printf("Seek and ye shall %i, streamIndex = %i, timestamp = %i, flags = %i\n",
  //   ret, c->streamIndex, c->timestamp, c->flags );
  if (ret < 0) {
    c->status = BEAMCODER_ERROR_SEEK_FRAME;
    c->errorMsg = avErrorMsg("Problem seeking frame: ", ret);
    return;
  }
};

void seekFrameComplete(napi_env env, napi_status asyncStatus, void *data) {
  seekFrameCarrier* c = (seekFrameCarrier*) data;
  napi_value result;
  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Seek frame failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_get_null(env, &result);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
};

/*
  let frame = await format.seek({
    streamIndex: 0, // Default is -1 - use primary stream, seek in seconds
    timestamp: 12345, // Timestamp - default units are stream timeBase
    backward: false, // Seek backwards
    byte: false, // Timestamp is a byte position
    any: false, // Select any frame, not just key frames
    frame: false // Timestamp is frame number
  });
*/

napi_value seekFrame(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, formatJS, formatRefExt, value;
  napi_valuetype type;
  seekFrameCarrier* c = new seekFrameCarrier;
  bool isArray, bValue;
  double fracTime = 0.0;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value argv[1];

  c->status = napi_get_cb_info(env, info, &argc, argv, &formatJS, nullptr);
  REJECT_RETURN;
  c->status = napi_get_named_property(env, formatJS, "_formatContextRef", &formatRefExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, formatRefExt, (void**) &c->formatRef);
  REJECT_RETURN;

  c->status = napi_create_reference(env, formatJS, 1, &c->passthru);
  REJECT_RETURN;

  if ((argc < 1) || (argc > 1)) {
    REJECT_ERROR_RETURN("Seek must have exactly one options object argument.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_typeof(env, argv[0], &type);
  REJECT_RETURN;
  c->status = napi_is_array(env, argv[0], &isArray);
  REJECT_RETURN;
  if ((type != napi_object) || (isArray == true)) {
    REJECT_ERROR_RETURN("Single argument options object must be an object and not an array.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_get_named_property(env, argv[0], "stream_index", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_number) {
    c->status = napi_get_value_int32(env, value, &c->streamIndex);
    REJECT_RETURN;
  }

  c->status = napi_get_named_property(env, argv[0], "timestamp", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_number) {
    if (c->streamIndex < 0) {
      REJECT_ERROR_RETURN("Stream index must be provided when seeking by timestamp.",
        BEAMCODER_INVALID_ARGS);
    }
    c->status = napi_get_value_int64(env, value, &c->timestamp);
    REJECT_RETURN;
    goto flags;
  } else {
    if (type != napi_undefined) {
      REJECT_ERROR_RETURN("Timestamp must by specified with a number.",
        BEAMCODER_INVALID_ARGS);
    }
  }

  c->status = napi_get_named_property(env, argv[0], "time", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_number) {
    if (c->streamIndex >= 0) {
      REJECT_ERROR_RETURN("Cannot seek by time value on a specific stream. Try to unset 'stream_index'?",
        BEAMCODER_INVALID_ARGS);
    }
    c->status = napi_get_value_double(env, value, &fracTime);
    REJECT_RETURN;
    c->timestamp = (int64_t) (fracTime * AV_TIME_BASE);
    c->streamIndex = -1;
    goto flags;
  } else {
    if (type != napi_undefined) {
      REJECT_ERROR_RETURN("Time value must by specified with a number.",
        BEAMCODER_INVALID_ARGS);
    }
  }

  c->status = napi_get_named_property(env, argv[0], "pos", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_number) {
    if (c->streamIndex >= 0) {
      REJECT_ERROR_RETURN("Cannot seek by byte position on a specific stream. Try to unset 'stream_index'?",
        BEAMCODER_INVALID_ARGS);
    }
    c->status = napi_get_value_int64(env, value, &c->timestamp);
    REJECT_RETURN;
    c->flags = c->flags | AVSEEK_FLAG_BYTE;
    goto flags;
  } else {
    if (type != napi_undefined) {
      REJECT_ERROR_RETURN("Position value must by specified with a number.",
        BEAMCODER_INVALID_ARGS);
    }
  }

  c->status = napi_get_named_property(env, argv[0], "frame", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_number) {
    if (c->streamIndex < 0) {
      REJECT_ERROR_RETURN("Cannot seek by frame number unless a 'stream_index' is provided.",
        BEAMCODER_INVALID_ARGS);
    }
    c->status = napi_get_value_int64(env, value, &c->timestamp);
    REJECT_RETURN;
    c->flags = c->flags | AVSEEK_FLAG_FRAME;
    goto flags;
  } else {
    if (type != napi_undefined) {
      REJECT_ERROR_RETURN("Frame number must by specified with a number.",
        BEAMCODER_INVALID_ARGS);
    }
  }

flags:
  c->status = napi_get_named_property(env, argv[0], "any", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_boolean) {
    c->status = napi_get_value_bool(env, value, &bValue);
    REJECT_RETURN;
    c->flags = (bValue) ? c->flags | AVSEEK_FLAG_ANY : c->flags;
  }

  c->status = napi_get_named_property(env, argv[0], "backward", &value);
  REJECT_RETURN;
  c->status = napi_typeof(env, value, &type);
  REJECT_RETURN;
  if (type == napi_boolean) {
    c->status = napi_get_value_bool(env, value, &bValue);
    REJECT_RETURN;
    c->flags = (bValue) ?
      c->flags | AVSEEK_FLAG_BACKWARD : c->flags & ~AVSEEK_FLAG_BACKWARD;
  }

  c->status = napi_create_string_utf8(env, "SeekFrame", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, seekFrameExecute,
    seekFrameComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
};

napi_value forceCloseInput(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, formatJS, formatRefExt, adaptorExt;
  fmtCtxRef* fmtRef;
  AVFormatContext* fc;
  Adaptor *adaptor;
  int ret;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &formatJS, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, formatJS, "_formatContextRef", &formatRefExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, formatRefExt, (void**) &fmtRef);
  CHECK_STATUS;
  status = napi_get_named_property(env, formatJS, "_adaptor", &adaptorExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, adaptorExt, (void**) &adaptor);
  CHECK_STATUS;

  if (fmtRef->fmtCtx != nullptr) {
    fc = fmtRef->fmtCtx;
    if (fc->pb != nullptr) {
      if (adaptor)
        avio_context_free(&fc->pb);
      else {
        ret = avio_closep(&fc->pb);
        if (ret < 0) {
          printf("DEBUG: For url '%s', %s", (fc->url != nullptr) ? fc->url : "unknown",
            avErrorMsg("error closing IO: ", ret));
        }
      }
    }

    avformat_close_input(&fmtRef->fmtCtx);
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

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

#include "format.h"

napi_value getIFormatName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, iformat->name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, oformat->name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatLongName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, iformat->long_name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatLongName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, oformat->long_name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatMimeType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (iformat->mime_type != nullptr) ? iformat->mime_type : "", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatMimeType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (oformat->mime_type != nullptr) ? oformat->mime_type : "", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatExtensions(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (iformat->extensions != nullptr) ? iformat->extensions : "", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatExtensions(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (oformat->extensions != nullptr) ? oformat->extensions : "", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_status getIOFormatCodecTag(napi_env env, AVInputFormat* format, napi_value* result) {
  napi_status status;
  napi_value value;
  char fourcc[AV_FOURCC_MAX_STRING_SIZE];

  // from libavformat/internal.h:
  // typedef struct AVCodecTag {
  //   enum AVCodecID id;
  //   unsigned int tag;
  // } AVCodecTag;
  status = napi_create_array(env, &value);
  PASS_STATUS;

  if (0 != format->codec_tag) {
    uint32_t *tag = (uint32_t *)(*format->codec_tag);
    uint32_t i = 0;
    while (true) {
      if (0 == tag[0])
        break;
      napi_value tagObj;
      status = napi_create_object(env, &tagObj);
      PASS_STATUS;
      status = beam_set_uint32(env, tagObj, "id", tag[0]);
      PASS_STATUS;
      av_fourcc_make_string(fourcc, tag[1]);
      if (strchr(fourcc, '[')) { // not a recognised tag
        status = beam_set_uint32(env, tagObj, "tag", tag[1]);
        PASS_STATUS;
      } else {
        status = beam_set_string_utf8(env, tagObj, "tag", fourcc);
        PASS_STATUS;
      }
      status = napi_set_element(env, value, i++, tagObj);
      PASS_STATUS;
      tag += 2;      
    }
  }

  *result = value;
  return napi_ok;
}

napi_value getOFormatCodecTag(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  // oformat codec_tag doesn't seem to correspond to the documentation - it isn't a valid pointer
  // status = getIOFormatCodecTag(env, oformat, &result);
  status = napi_create_array(env, &result);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatCodecTag(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = getIOFormatCodecTag(env, iformat, &result);
  CHECK_STATUS;

  return result;
}

napi_status getIOFormatFlags(napi_env env, int flags, napi_value* result, bool isInput) {
  napi_status status;
  napi_value value;

  status = napi_create_object(env, &value);
  PASS_STATUS;
  status = beam_set_bool(env, value, "NOFILE", flags & AVFMT_NOFILE); // O I
  PASS_STATUS;
  status = beam_set_bool(env, value, "NEEDNUMBER", flags & AVFMT_NEEDNUMBER); // O I
  PASS_STATUS;
  if (isInput) {
    status = beam_set_bool(env, value, "SHOW_IDS", flags & AVFMT_SHOW_IDS); // I
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "GLOBALHEADER", flags & AVFMT_GLOBALHEADER); // O
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "NOTIMESTAMPS", flags & AVFMT_NOTIMESTAMPS); // O
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "GENERIC_INDEX", flags & AVFMT_GENERIC_INDEX); // I
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "TS_DISCONT", flags & AVFMT_TS_DISCONT); // I
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "VARIABLE_FPS", flags & AVFMT_VARIABLE_FPS); // O
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "NODIMENSIONS", flags & AVFMT_NODIMENSIONS); // O
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "NOSTREAMS", flags & AVFMT_NOSTREAMS); // O
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "NOBINSEARCH", flags & AVFMT_NOBINSEARCH); // I
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "NODIMENSIONS", flags & AVFMT_NODIMENSIONS); // O
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "NOGENSEARCH", flags & AVFMT_NOGENSEARCH); // I
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "NO_BYTE_SEEK", flags & AVFMT_NO_BYTE_SEEK); // I
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "ALLOW_FLUSH", flags & AVFMT_ALLOW_FLUSH); // O
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "TS_NONSTRICT", flags & AVFMT_TS_NONSTRICT); // O
    PASS_STATUS;
  }
  if (!isInput) {
    status = beam_set_bool(env, value, "TS_NEGATIVE", flags & AVFMT_TS_NEGATIVE); // O
    PASS_STATUS;
  }
  if (isInput) {
    status = beam_set_bool(env, value, "SEEK_TO_PTS", flags & AVFMT_SEEK_TO_PTS); // I
    PASS_STATUS;
  }

  *result = value;
  return napi_ok;
}

napi_value getOFormatFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = getIOFormatFlags(env, oformat->flags, &result, false);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = getIOFormatFlags(env, iformat->flags, &result, true);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatRawCodecID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_int32(env, iformat->raw_codec_id, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatPrivDataSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_int32(env, oformat->priv_data_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value getIFormatPrivDataSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  status = napi_create_int32(env, iformat->priv_data_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatPrivClass(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  if (oformat->priv_class != nullptr) {
    status = fromAVClass(env, (const AVClass*) oformat->priv_class, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value getIFormatPrivClass(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVInputFormat* iformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &iformat);
  CHECK_STATUS;

  if (iformat->priv_class != nullptr) {
    status = fromAVClass(env, (const AVClass*) iformat->priv_class, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value getOFormatAudioCodec(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (char*) avcodec_get_name(oformat->audio_codec),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatVideoCodec(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (char*) avcodec_get_name(oformat->video_codec),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getOFormatSubtitleCodec(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVOutputFormat* oformat;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &oformat);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (char*) avcodec_get_name(oformat->subtitle_codec),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value muxers(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, muxer;
  void* opaque = nullptr;
  const AVOutputFormat* oformat = nullptr;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  oformat = av_muxer_iterate(&opaque);
  while ( oformat != nullptr ) {
    status = fromAVOutputFormat(env, oformat, &muxer);
    CHECK_STATUS;
    status = napi_set_named_property(env, result, oformat->name, muxer);
    CHECK_STATUS;

    oformat = av_muxer_iterate(&opaque);
  }

  return result;
}

napi_value demuxers(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, demuxer;
  void* opaque = nullptr;
  const AVInputFormat* iformat = nullptr;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  iformat = av_demuxer_iterate(&opaque);
  while ( iformat != nullptr ) {
    status = fromAVInputFormat(env, iformat, &demuxer);
    CHECK_STATUS;
    status = napi_set_named_property(env, result, iformat->name, demuxer);
    CHECK_STATUS;

    iformat = av_demuxer_iterate(&opaque);
  }

  return result;
}

napi_value guessFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  char* testName;
  size_t strLen;
  AVOutputFormat* oformat;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Unable to guess an output format without a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("Cannot guess an output format without a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  testName = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], testName, strLen + 1, &strLen);
  CHECK_STATUS;

  oformat = av_guess_format((const char*) testName, nullptr, nullptr);
  if (oformat == nullptr) {
    oformat = av_guess_format(nullptr, (const char*) testName, nullptr);
  }
  if (oformat == nullptr) {
    oformat = av_guess_format(nullptr, nullptr, (const char*) testName);
  }
  if (oformat == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = fromAVOutputFormat(env, oformat, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_status fromAVOutputFormat(napi_env env,
    const AVOutputFormat* oformat, napi_value* result) {
  napi_status status;
  napi_value jsOFormat, extOFormat, typeName;

  status = napi_create_object(env, &jsOFormat);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "OutputFormat", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_create_external(env, (void*) oformat, nullptr, nullptr, &extOFormat);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "name", nullptr, nullptr, getOFormatName, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "long_name", nullptr, nullptr, getOFormatLongName, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "mime_type", nullptr, nullptr, getOFormatMimeType, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "extensions", nullptr, nullptr, getOFormatExtensions, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "audio_codec", nullptr, nullptr, getOFormatAudioCodec, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "video_codec", nullptr, nullptr, getOFormatVideoCodec, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "subtitle_codec", nullptr, nullptr, getOFormatSubtitleCodec, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "flags", nullptr, nullptr, getOFormatFlags, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "codec_tag", nullptr, nullptr, getOFormatCodecTag, nullptr,
      nullptr, napi_enumerable, (void*) oformat }, // 10
    { "priv_class", nullptr, nullptr, getOFormatPrivClass, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "priv_data_size", nullptr, nullptr, getOFormatPrivDataSize, nullptr,
      nullptr, napi_enumerable, (void*) oformat },
    { "_oformat", nullptr, nullptr, nullptr, nullptr, extOFormat, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsOFormat, 13, desc);
  PASS_STATUS;

  *result = jsOFormat;
  return napi_ok;
}

napi_status fromAVInputFormat(napi_env env,
    const AVInputFormat* iformat, napi_value* result) {
  napi_status status;
  napi_value jsIFormat, extIFormat, typeName;

  status = napi_create_object(env, &jsIFormat);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "InputFormat", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_create_external(env, (void*) iformat, nullptr, nullptr, &extIFormat);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "name", nullptr, nullptr, getIFormatName, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "long_name", nullptr, nullptr, getIFormatLongName, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "flags", nullptr, nullptr, getIFormatFlags, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "extensions", nullptr, nullptr, getIFormatExtensions, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "codec_tag", nullptr, nullptr, getIFormatCodecTag, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "priv_class", nullptr, nullptr, getIFormatPrivClass, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "mime_type", nullptr, nullptr, getIFormatMimeType, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "raw_codec_id", nullptr, nullptr, getIFormatRawCodecID, nullptr,
      nullptr, napi_enumerable, (void*) iformat },
    { "priv_data_size", nullptr, nullptr, getIFormatPrivDataSize, nullptr,
      nullptr, napi_enumerable, (void*) iformat }, // 10
    { "_iformat", nullptr, nullptr, nullptr, nullptr, extIFormat, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsIFormat, 11, desc);
  PASS_STATUS;

  *result = jsIFormat;
  return napi_ok;
}

napi_value getFmtCtxOFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->oformat != nullptr) {
    status = fromAVOutputFormat(env, fmtCtx->oformat, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value setFmtCtxOFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, prop;
  napi_valuetype type;
  bool isArray;
  AVFormatContext* fmtCtx;
  char* name;
  size_t strLen;
  const AVOutputFormat* fmt = nullptr;
  void* i = nullptr;
  bool found = false;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Setting a muxer output format requires a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    fmtCtx->oformat = nullptr;
    if (fmtCtx->iformat == nullptr) {
      av_freep(&fmtCtx->priv_data);
    }
    goto over;
  }
  if (type == napi_string) {
    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
    CHECK_STATUS;
    name = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
    CHECK_STATUS;
    while ((fmt = av_muxer_iterate(&i))) {
      if (av_match_name(name, fmt->name)) {
        fmtCtx->oformat = (AVOutputFormat*) fmt;
        found = true;
        break;
      }
    }
    free(name);
    goto done;
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Setting a muxer output requires an object representing an output format.");
  }

  status = napi_get_named_property(env, args[0], "_oformat", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  if (type != napi_external) {
    NAPI_THROW_ERROR("Object provided to set muxer output does not embed an AVOutputFormat.");
  }
  status = napi_get_value_external(env, prop, (void**) &fmtCtx->oformat);
  CHECK_STATUS;
  found = true;

done:
  if ((fmtCtx->oformat == nullptr) || (found == false)) {
    NAPI_THROW_ERROR("Unable to find and/or set output format.");
  }
  if (fmtCtx->oformat->priv_data_size > 0) {
    av_freep(&fmtCtx->priv_data);
    if (!(fmtCtx->priv_data = av_mallocz(fmtCtx->oformat->priv_data_size))) {
      NAPI_THROW_ERROR("Failed to allocate memory for private data.");
    }
    if (fmtCtx->oformat->priv_class) {
      *(const AVClass **) fmtCtx->priv_data = fmtCtx->oformat->priv_class;
      av_opt_set_defaults(fmtCtx->priv_data);
    }
  }

over:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxIFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->iformat != nullptr) {
    status = fromAVInputFormat(env, fmtCtx->iformat, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value setFmtCtxIFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, prop;
  napi_valuetype type;
  bool isArray;
  AVFormatContext* fmtCtx;
  char* name;
  size_t strLen;
  const AVInputFormat* fmt = nullptr;
  void* i = nullptr;
  bool found = false;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Setting a format's input type requires a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    fmtCtx->iformat = nullptr;
    if (fmtCtx->oformat == nullptr) {
      av_freep(&fmtCtx->priv_data);
    }
    goto over;
  }
  if (type == napi_string) {
    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
    CHECK_STATUS;
    name = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
    CHECK_STATUS;
    while ((fmt = av_demuxer_iterate(&i))) {
      if (av_match_name(name, fmt->name)) {
        fmtCtx->iformat = (AVInputFormat*) fmt;
        found = true;
        break;
      }
    }
    free(name);
    goto done;
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Setting a format's input type requires an object representing an input format.");
  }

  status = napi_get_named_property(env, args[0], "_iformat", &prop);
  CHECK_STATUS;
  status = napi_typeof(env, prop, &type);
  CHECK_STATUS;
  if (type != napi_external) {
    NAPI_THROW_ERROR("Object provided to set demuxer output does not embed an AVInputFormat.");
  }
  status = napi_get_value_external(env, prop, (void**) &fmtCtx->iformat);
  CHECK_STATUS;
  found = true;

done:
  if ((fmtCtx->iformat == nullptr) || (found == false)) {
    NAPI_THROW_ERROR("Unable to find and/or set input format.");
  }
  if (fmtCtx->iformat->priv_data_size > 0) {
    av_freep(&fmtCtx->priv_data);
    if (!(fmtCtx->priv_data = av_mallocz(fmtCtx->iformat->priv_data_size))) {
      NAPI_THROW_ERROR("Failed to allocate memory for private data.");
    }
    if (fmtCtx->iformat->priv_class) {
      *(const AVClass **) fmtCtx->priv_data = fmtCtx->iformat->priv_class;
      av_opt_set_defaults(fmtCtx->priv_data);
    }
  }

over:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxCtxFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "NOHEADER", fmtCtx->ctx_flags & AVFMTCTX_NOHEADER);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "UNSEEKABLE", fmtCtx->ctx_flags & AVFMTCTX_UNSEEKABLE);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxStreams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element, jsStreams, jsContext;
  bool isArray;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsContext, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_named_property(env, jsContext, "__streams", &jsStreams);
  CHECK_STATUS;
  status = napi_is_array(env, jsStreams, &isArray);
  CHECK_STATUS;
  if (isArray) {
    return jsStreams;
  }

  status = napi_create_array(env, &result);
  CHECK_STATUS;

  for ( uint32_t x = 0 ; x < fmtCtx->nb_streams ; x++ ) {
    if (fmtCtx->streams[x] == nullptr) continue;
    status = fromAVStream(env, fmtCtx->streams[x], &element);
    CHECK_STATUS;
    status = napi_set_element(env, result, x, element);
    CHECK_STATUS;
  }

  // Ensure streams have same lifecycle as owning context
  status = napi_set_named_property(env, jsContext, "__streams", result);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxStreamsJSON(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, streams, strToJSON, element, outEl;
  uint32_t nbStreams;
  bool pending;

  streams = getFmtCtxStreams(env, info);
  status = napi_is_exception_pending(env, &pending);
  CHECK_STATUS;

  if (pending) {
    status = napi_get_undefined(env, &result);
    CHECK_STATUS;
    return result;
  }

  status = napi_create_array(env, &result);
  CHECK_STATUS;

  status = napi_get_array_length(env, streams, &nbStreams);
  CHECK_STATUS;
  status = napi_create_function(env, "streamToJSON", NAPI_AUTO_LENGTH, streamToJSON,
    nullptr, &strToJSON);
  CHECK_STATUS;
  for ( uint32_t x = 0 ; x < nbStreams ; x++ ) {
    status = napi_get_element(env, streams, x, &element);
    CHECK_STATUS;
    status = napi_call_function(env, element, strToJSON, 0, nullptr, &outEl);
    CHECK_STATUS;
    status = napi_set_element(env, result, x, outEl);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxStreams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, constructStream, dud, jsFmtCtx, element;
  bool isArray;
  AVFormatContext* fmtCtx;
  uint32_t nbStreams;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, &jsFmtCtx, (void**) &fmtCtx);
  CHECK_STATUS;

  if (argc < 1) {
    NAPI_THROW_ERROR("The streams property can only be created with a value.");
  }

  // Only allow to run if nb_streams = 0, e.g. when initialising
  if (fmtCtx->nb_streams != 0) {
    NAPI_THROW_ERROR("Streams can only be created on context construction. Otherwise, use newStream().")
  }

  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (!isArray) {
    NAPI_THROW_ERROR("The streams property must be set with an array of streams.");
  }

  status = napi_create_function(env, "constructStream", NAPI_AUTO_LENGTH, newStream,
    fmtCtx, &constructStream);
  CHECK_STATUS;

  status = napi_get_array_length(env, args[0], &nbStreams);
  CHECK_STATUS;
  for ( uint32_t x = 0 ; x < nbStreams ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    CHECK_STATUS;
    const napi_value fargs[] = { element };
    status = napi_call_function(env, jsFmtCtx, constructStream, 1, fargs, &dud);
    CHECK_STATUS;
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxURL(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (fmtCtx->url != nullptr) ? fmtCtx->url : "", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxURL(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* url;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Muxer URL must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("Muxer URL must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  url = (char*) av_malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], url, strLen + 1, &strLen);
  CHECK_STATUS;

  fmtCtx->url = url;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxStartTime(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->start_time, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->duration, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Muxer duration must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Muxer duration must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->duration);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->bit_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context bit_rate must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context bit_rate must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->bit_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxPacketSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_uint32(env, fmtCtx->packet_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxPacketSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context packet_size must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context packet_size must be set with a number.");
  }
  status = napi_get_value_uint32(env, args[0], &fmtCtx->packet_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxDelay(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->max_delay, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxDelay(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_delay must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_delay must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->max_delay);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  // Generate missing pts even if it requires parsing future frames.
  status = beam_set_bool(env, result, "GENPTS", fmtCtx->flags & AVFMT_FLAG_GENPTS);
  CHECK_STATUS;
  // Ignore index.
  status = beam_set_bool(env, result, "IGNIDX", fmtCtx->flags & AVFMT_FLAG_IGNIDX);
  CHECK_STATUS;
  // Do not block when reading packets from input.
  status = beam_set_bool(env, result, "NONBLOCK", fmtCtx->flags & AVFMT_FLAG_NONBLOCK);
  CHECK_STATUS;
  // Ignore DTS on frames that contain both DTS & PTS
  status = beam_set_bool(env, result, "IGNDTS", fmtCtx->flags & AVFMT_FLAG_IGNDTS);
  CHECK_STATUS;
  // Do not infer any values from other values, just return what is stored in the container
  status = beam_set_bool(env, result, "NOFILLIN", fmtCtx->flags & AVFMT_FLAG_NOFILLIN);
  CHECK_STATUS;
  // Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
  status = beam_set_bool(env, result, "NOPARSE", fmtCtx->flags & AVFMT_FLAG_NOPARSE);
  CHECK_STATUS;
  // Do not buffer frames when possible
  status = beam_set_bool(env, result, "NOBUFFER", fmtCtx->flags & AVFMT_FLAG_NOBUFFER);
  CHECK_STATUS;
  // The caller has supplied a custom AVIOContext, don't avio_close() it.
  status = beam_set_bool(env, result, "CUSTOM_IO", fmtCtx->flags & AVFMT_FLAG_CUSTOM_IO);
  CHECK_STATUS;
  // Discard frames marked corrupted
  status = beam_set_bool(env, result, "DISCARD_CORRUPT", fmtCtx->flags & AVFMT_FLAG_DISCARD_CORRUPT);
  CHECK_STATUS;
  // Flush the AVIOContext every packet.
  status = beam_set_bool(env, result, "FLUSH_PACKETS", fmtCtx->flags & AVFMT_FLAG_FLUSH_PACKETS);
  CHECK_STATUS;
  // For testing
  status = beam_set_bool(env, result, "BITEXACT", fmtCtx->flags & AVFMT_FLAG_BITEXACT);
  CHECK_STATUS;
  // try to interleave outputted packets by dts (using this flag can slow demuxing down)
  status = beam_set_bool(env, result, "SORT_DTS", fmtCtx->flags & AVFMT_FLAG_SORT_DTS);
  CHECK_STATUS;
  // Enable use of private options by delaying codec open (this could be made default once all code is converted)
  status = beam_set_bool(env, result, "PRIV_OPT", fmtCtx->flags & AVFMT_FLAG_PRIV_OPT);
  CHECK_STATUS;
  // Enable fast, but inaccurate seeks for some formats
  status = beam_set_bool(env, result, "FAST_SEEK", fmtCtx->flags & AVFMT_FLAG_FAST_SEEK);
  CHECK_STATUS;
  // Stop muxing when the shortest stream stops.
  status = beam_set_bool(env, result, "SHORTEST", fmtCtx->flags & AVFMT_FLAG_SHORTEST);
  CHECK_STATUS;
  // Add bitstream filters as requested by the muxer
  status = beam_set_bool(env, result, "AUTO_BSF", fmtCtx->flags & AVFMT_FLAG_AUTO_BSF);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context flags must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Format context flags must be set with an object of Boolean values.");
  }
  // Generate missing pts even if it requires parsing future frames.
  status = beam_get_bool(env, args[0], "GENPTS", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_GENPTS :
    fmtCtx->flags & ~AVFMT_FLAG_GENPTS; }
  // Ignore index.
  status = beam_get_bool(env, args[0], "IGNIDX", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_IGNIDX :
    fmtCtx->flags & ~AVFMT_FLAG_IGNIDX; }
  // Do not block when reading packets from input.
  status = beam_get_bool(env, args[0], "NONBLOCK", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_NONBLOCK :
    fmtCtx->flags & ~AVFMT_FLAG_NONBLOCK; }
  // Ignore DTS on frames that contain both DTS & PTS
  status = beam_get_bool(env, args[0], "IGNDTS", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_IGNDTS :
    fmtCtx->flags & ~AVFMT_FLAG_IGNDTS; }
  // Do not infer any values from other values, just return what is stored in the container
  status = beam_get_bool(env, args[0], "NOFILLIN", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_NOFILLIN :
    fmtCtx->flags & ~AVFMT_FLAG_NOFILLIN; }
  // Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
  status = beam_get_bool(env, args[0], "NOPARSE", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_NOPARSE :
    fmtCtx->flags & ~AVFMT_FLAG_NOPARSE; }
  // Do not buffer frames when possible
  status = beam_get_bool(env, args[0], "NOBUFFER", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_NOBUFFER :
    fmtCtx->flags & ~AVFMT_FLAG_NOBUFFER; }
  // The caller has supplied a custom AVIOContext, don't avio_close() it.
  status = beam_get_bool(env, args[0], "CUSTOM_IO", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_CUSTOM_IO :
    fmtCtx->flags & ~AVFMT_FLAG_CUSTOM_IO; }
  // Discard frames marked corrupted
  status = beam_get_bool(env, args[0], "DISCARD_CORRUPT", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_DISCARD_CORRUPT :
    fmtCtx->flags & ~AVFMT_FLAG_DISCARD_CORRUPT; }
  // Flush the AVIOContext every packet.
  status = beam_get_bool(env, args[0], "FLUSH_PACKETS", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_FLUSH_PACKETS :
    fmtCtx->flags & ~AVFMT_FLAG_FLUSH_PACKETS; }
  // Testing only
  status = beam_get_bool(env, args[0], "BITEXACT", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_BITEXACT :
    fmtCtx->flags & ~AVFMT_FLAG_BITEXACT; }
  // try to interleave outputted packets by dts (using this flag can slow demuxing down)
  status = beam_get_bool(env, args[0], "SORT_DTS", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_SORT_DTS :
    fmtCtx->flags & ~AVFMT_FLAG_SORT_DTS; }
  // Enable use of private options by delaying codec open (this could be made default once all code is converted)
  status = beam_get_bool(env, args[0], "PRIV_OPT", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_PRIV_OPT :
    fmtCtx->flags & ~AVFMT_FLAG_PRIV_OPT; }
  // Enable fast, but inaccurate seeks for some formats
  status = beam_get_bool(env, args[0], "FAST_SEEK", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_FAST_SEEK :
    fmtCtx->flags & ~AVFMT_FLAG_FAST_SEEK; }
  // Stop muxing when the shortest stream stops.
  status = beam_get_bool(env, args[0], "SHORTEST", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_SHORTEST :
    fmtCtx->flags & ~AVFMT_FLAG_SHORTEST; }
  // Add bitstream filters as requested by the muxer
  status = beam_get_bool(env, args[0], "AUTO_BSF", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->flags = (flag) ?
    fmtCtx->flags | AVFMT_FLAG_AUTO_BSF :
    fmtCtx->flags & ~AVFMT_FLAG_AUTO_BSF; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxKey(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* ctx;
  void* resultData;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &ctx);
  CHECK_STATUS;
  if (ctx->keylen > 0) {
    status = napi_create_buffer_copy(env, ctx->keylen, ctx->key,
      &resultData, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxKey(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsBuffer, jsBufferFrom, arrayData;
  napi_valuetype type;
  bool isBuffer, isArray;
  AVFormatContext* fmtCtx;
  uint8_t* data;
  size_t dataLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the key property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type == napi_null) {
    if (fmtCtx->keylen > 0) { // Tidy up old buffers
      av_freep(&fmtCtx->key);
    }
    fmtCtx->keylen = 0;
    goto done;
  }
  status = napi_is_buffer(env, args[0], &isBuffer);
  CHECK_STATUS;
  if (!isBuffer) {
    status = napi_get_named_property(env, args[0], "data", &arrayData);
    CHECK_STATUS;
    // TODO more checks that this is a buffer from JSON?
    status = napi_is_array(env, arrayData, &isArray);
    CHECK_STATUS;
    if (isArray) {
      status = napi_get_global(env, &global);
      CHECK_STATUS;
      status = napi_get_named_property(env, global, "Buffer", &jsBuffer);
      CHECK_STATUS;
      status = napi_get_named_property(env, jsBuffer, "from", &jsBufferFrom);
      CHECK_STATUS;
      const napi_value fargs[] = { arrayData };
      status = napi_call_function(env, args[0], jsBufferFrom, 1, fargs, &args[0]);
      CHECK_STATUS;
    } else {
      NAPI_THROW_ERROR("A buffer is required to set the key propeprty.");
    }
  }

  status = napi_get_buffer_info(env, args[0], (void**) &data, &dataLen);
  CHECK_STATUS;
  if (fmtCtx->keylen > 0) { // Tidy up old buffers
    av_freep(&fmtCtx->key);
    fmtCtx->keylen = 0;
  }
  fmtCtx->key = (uint8_t*) av_mallocz(dataLen + AV_INPUT_BUFFER_PADDING_SIZE);
  fmtCtx->keylen = dataLen;
  memcpy((void*) fmtCtx->key, data, dataLen);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxPrograms(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element, metadata;
  AVFormatContext* ctx;
  AVProgram* program;
  AVDictionaryEntry* tag = nullptr;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &ctx);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  for ( uint32_t x = 0 ; x < ctx->nb_programs ; x++ ) {
    program = ctx->programs[x];
    status = napi_create_object(env, &element);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, element, "type", "Program");
    CHECK_STATUS;
    status = beam_set_int32(env, element, "id", program->id);
    CHECK_STATUS;
    status = beam_set_bool(env, element, "flags_running",
      program->flags & AV_PROGRAM_RUNNING);
    CHECK_STATUS;
    status = beam_set_uint32(env, element, "stream_index", *program->stream_index);
    CHECK_STATUS;
    status = beam_set_uint32(env, element, "nb_stream_indexes", program->nb_stream_indexes);
    CHECK_STATUS;
    status = napi_create_object(env, &metadata);
    CHECK_STATUS;
    while ((tag = av_dict_get(program->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      status = beam_set_string_utf8(env, metadata, tag->key, tag->value);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, element, "metadata", metadata);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "program_num", program->program_num);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "pmt_pid", program->pmt_pid);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "pcr_pid", program->pcr_pid);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "pmt_version", program->pmt_version);
    CHECK_STATUS;

    status = napi_set_element(env, result, x, element);
    CHECK_STATUS;
  }

  return result;
}

// TODO parking for now - quite complex interactions
/* napi_value setFmtCtxPrograms(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element, metadata;
  napi_valuetype type;
  bool isArray;
  AVFormatContext* ctx;
  AVProgram* program;
  AVProgram** programs;
  AVDictionary* dict = nullptr;
  uint32_t progCount;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &ctx);
  CHECK_STATUS;

  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the programs property.");
    CHECK_STATUS;
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (!isArray) {
    NAPI_THROW_ERROR("The value of the programs property requires an array of program objects.");
  }
  status = napi_get_array_length(env, args[0], &progCount);
  CHECK_STATUS;
  for ( uint32_t x = 0 ; x < progCount ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    CHECK_STATUS;
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_object) {
      NAPI_THROW_ERROR("Cannot set the value of programs unless every array element is an object.");
    }
  }
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  programs = (AVProgram**) av_mallocz(sizeof(AVProgram*) * progCount);
  for ( uint32_t x = 0 ; x < progCount ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    CHECK_BAIL;
    program = (AVProgram*) av_mallocz(sizeof(AVProgram));
    status = beam_get_int32(env, element, "id", &program->id);
    CHECK_BAIL;
    status = napi_typeof(env, element, &type);
    CHECK_BAIL;
    if ()
    status = beam_get_bool(env, element, "flags_running", &present, &flag);
    CHECK_BAIL;
    if (present) { program->flags = (flag) ?
      program->flags | AV_PROGRAM_RUNNING :
      program->flags & ~AV_PROGRAM_RUNNING; }
    programs[x] = program;
  }

  for ( int i = ctx->nb_programs - 1; i >= 0; i--) {
    av_dict_free(&ctx->programs[i]->metadata);
    av_freep(&ctx->programs[i]->stream_index);
    av_freep(&ctx->programs[i]);
  }
  av_freep(&ctx->programs);

  ctx->programs = programs;
  ctx->nb_programs = progCount;

  return result;
bail:
  for ( int i = progCount - 1; i >= 0; i--) {
    av_dict_free(&programs[i]->metadata);
    av_freep(&programs->stream_index);
    av_freep(&programs[i]);
  }
  av_freep(&ctx->programs);
  return result;
} */

napi_value getFmtCtxProbeSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->probesize, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxProbeSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context probesize must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context probesize must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->probesize);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxAnDur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->max_analyze_duration, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxAnDur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_analyze_duration must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_analyze_duration must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->max_analyze_duration);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxIndexSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_uint32(env, fmtCtx->max_index_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxIndexSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_index_size must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_index_size must be set with a number.");
  }
  status = napi_get_value_uint32(env, args[0], &fmtCtx->max_index_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxPictBuf(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_uint32(env, fmtCtx->max_picture_buffer, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxPictBuf(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_picture_buffer must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_picture_buffer must be set with a number.");
  }
  status = napi_get_value_uint32(env, args[0], &fmtCtx->max_picture_buffer);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMetadata(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;
  AVDictionaryEntry* tag = nullptr;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  while ((tag = av_dict_get(fmtCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    status = beam_set_string_utf8(env, result, tag->key, tag->value);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxMetadata(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool isArray;
  AVDictionary* dict = nullptr;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the format context's metadata property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Format context metadata can only be set with an object of tag names and values.");
  }

  status = makeAVDictionary(env, args[0], &dict);
  CHECK_STATUS;
  fmtCtx->metadata = dict;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

// TODO use Javascript Date?
napi_value getFmtCtxStartTRealT(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->start_time_realtime == AV_NOPTS_VALUE) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_int64(env, fmtCtx->start_time_realtime, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxStartTRealT(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context start_time_realtime must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if ((type == napi_undefined) || (type == napi_null)) {
    fmtCtx->start_time_realtime = AV_NOPTS_VALUE;
    goto done;
  }

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context start_time_realtime must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->start_time_realtime);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxFpsProbeSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->fps_probe_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxFpsProbeSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context fps_probe_size must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context fps_probe_size must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->fps_probe_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxErrRecog(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->error_recognition, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxErrRecog(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context error_recognition must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context error_recognition must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->error_recognition);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxDebug(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "TS", fmtCtx->debug & FF_FDEBUG_TS);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxDebug(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  bool isArray, present, flag;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context debug must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Format context debug must be set with an object with Boolean-valued flags.");
  }
  status = beam_get_bool(env, args[0], "TS", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->debug = (flag) ?
    fmtCtx->debug | FF_FDEBUG_TS :
    fmtCtx->debug & ~FF_FDEBUG_TS; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxInterleaveD(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->max_interleave_delta, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxInterleaveD(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_interleave_delta must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_interleave_delta must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->max_interleave_delta);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxStrictStdComp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_compliance->forward, fmtCtx->strict_std_compliance),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxStrictStdComp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the strict_std_compliance property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the strict_std_compliance property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_compliance->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    fmtCtx->strict_std_compliance = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for strict_std_compliance.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxEventFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "METADATA_UPDATED",
    fmtCtx->event_flags & AVFMT_EVENT_FLAG_METADATA_UPDATED);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxEventFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  bool isArray, present, flag;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context event_flags must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Format context event_flags must be set with an object with Boolean-valued flags.");
  }
  status = beam_get_bool(env, args[0], "METADATA_UPDATED", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->event_flags = (flag) ?
    fmtCtx->event_flags | AVFMT_EVENT_FLAG_METADATA_UPDATED :
    fmtCtx->event_flags & ~AVFMT_EVENT_FLAG_METADATA_UPDATED; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxTsProbe(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->max_ts_probe, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxTsProbe(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_ts_probe must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_ts_probe must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->max_ts_probe);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxAvoidNegTs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avoid_neg_ts->forward, fmtCtx->avoid_negative_ts),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxAvoidNegTs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* enumName;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_ts_probe must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    fmtCtx->avoid_negative_ts = AVFMT_AVOID_NEG_TS_AUTO;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Format context avoid_negative_ts must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumName = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], enumName, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = beam_lookup_enum(beam_avoid_neg_ts->inverse, enumName);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    fmtCtx->avoid_negative_ts = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for avoid_negative_ts. One of 'auto', 'make_non_negative' or 'make_zero'.")
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxAudioPreload(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->audio_preload, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxAudioPreload(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context audio_preload must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context audio_preload must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->audio_preload);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxChunkDur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->max_chunk_duration, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxChunkDur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_chunk_duration must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_chunk_duration must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->max_chunk_duration);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxChunkSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->max_chunk_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxChunkSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_chunk_size must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_chunk_size must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->max_chunk_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxUseWallclock(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_boolean(env, fmtCtx->use_wallclock_as_timestamps, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxUseWallclock(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context use_wallclock_as_timestamps must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_boolean) {
    NAPI_THROW_ERROR("Format context use_wallclock_as_timestamps must be set with a Boolean.");
  }
  status = napi_get_value_bool(env, args[0], &flag);
  CHECK_STATUS;
  fmtCtx->use_wallclock_as_timestamps = flag ? 1 : 0;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxAvioFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "READ", fmtCtx->avio_flags & AVIO_FLAG_READ);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "WRITE", fmtCtx->avio_flags & AVIO_FLAG_WRITE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "NONBLOCK", fmtCtx->avio_flags & AVIO_FLAG_NONBLOCK);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DIRECT", fmtCtx->avio_flags & AVIO_FLAG_DIRECT);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxAvioFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context avio_flags must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Format context avio_flags must be set with an object of Boolean values.");
  }
  status = beam_get_bool(env, args[0], "READ", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->avio_flags = (flag) ?
    fmtCtx->avio_flags | AVIO_FLAG_READ :
    fmtCtx->avio_flags & ~AVIO_FLAG_READ; }
  status = beam_get_bool(env, args[0], "WRITE", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->avio_flags = (flag) ?
    fmtCtx->avio_flags | AVIO_FLAG_WRITE :
    fmtCtx->avio_flags & ~AVIO_FLAG_WRITE; }
  status = beam_get_bool(env, args[0], "NONBLOCK", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->avio_flags = (flag) ?
    fmtCtx->avio_flags | AVIO_FLAG_NONBLOCK :
    fmtCtx->avio_flags & ~AVIO_FLAG_NONBLOCK; }
  status = beam_get_bool(env, args[0], "DIRECT", &present, &flag);
  CHECK_STATUS;
  if (present) { fmtCtx->avio_flags = (flag) ?
    fmtCtx->avio_flags | AVIO_FLAG_DIRECT :
    fmtCtx->avio_flags & ~AVIO_FLAG_DIRECT; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxDurEstMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avfmt_duration2->forward, fmtCtx->duration_estimation_method),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxSkipInitBytes(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->skip_initial_bytes, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxSkipInitBytes(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context skip_initial_bytes must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context skip_initial_bytes must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->skip_initial_bytes);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxCorrectTsOf(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_boolean(env, fmtCtx->correct_ts_overflow, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxCorrectTsOf(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context correct_ts_overflow must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_boolean) {
    NAPI_THROW_ERROR("Format context correct_ts_overflow must be set with a Boolean.");
  }
  status = napi_get_value_bool(env, args[0], &flag);
  CHECK_STATUS;
  fmtCtx->correct_ts_overflow = flag ? 1 : 0;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxSeek2Any(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_boolean(env, fmtCtx->seek2any, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxSeek2Any(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context seek2any must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_boolean) {
    NAPI_THROW_ERROR("Format context seek2any must be set with a Boolean.");
  }
  status = napi_get_value_bool(env, args[0], &flag);
  CHECK_STATUS;
  fmtCtx->seek2any = flag ? 1 : 0;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxFlushPackets(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->flush_packets, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxFlushPackets(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context flush_packets must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context flush_packets must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->flush_packets);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxFmtProbesize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->format_probesize, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxFmtProbesize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context format_probesize must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context format_probesize must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->format_probesize);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxProbeScore(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->probe_score, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxCodecWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->codec_whitelist != nullptr) {
    status = napi_create_string_utf8(env, fmtCtx->codec_whitelist,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxCodecWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* list;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context codec_whitelist must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if ((type == napi_null) || (type == napi_undefined)) {
    if (fmtCtx->codec_whitelist != nullptr) {
      av_freep(&fmtCtx->codec_whitelist);
    }
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Format context codec_whitelist must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  list = (char*) av_mallocz(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], list, strLen + 1, &strLen);
  CHECK_STATUS;

  if (fmtCtx->codec_whitelist != nullptr) {
    av_freep(&fmtCtx->codec_whitelist);
  }
  fmtCtx->codec_whitelist = list;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxFmtWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->format_whitelist != nullptr) {
    status = napi_create_string_utf8(env, fmtCtx->format_whitelist,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxFmtWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* list;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context format_whitelist must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if ((type == napi_null) || (type == napi_undefined)) {
    if (fmtCtx->format_whitelist != nullptr) {
      av_freep(&fmtCtx->format_whitelist);
    }
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Format context format_whitelist must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  list = (char*) av_mallocz(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], list, strLen + 1, &strLen);
  CHECK_STATUS;

  if (fmtCtx->format_whitelist != nullptr) {
    av_freep(&fmtCtx->format_whitelist);
  }
  fmtCtx->format_whitelist = list;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxProtWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->protocol_whitelist != nullptr) {
    status = napi_create_string_utf8(env, fmtCtx->protocol_whitelist,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxProtWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* list;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context protocol_whitelist must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if ((type == napi_null) || (type == napi_undefined)) {
    if (fmtCtx->protocol_whitelist != nullptr) {
      av_freep(&fmtCtx->protocol_whitelist);
    }
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Format context protocol_whitelist must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  list = (char*) av_mallocz(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], list, strLen + 1, &strLen);
  CHECK_STATUS;

  if (fmtCtx->protocol_whitelist != nullptr) {
    av_freep(&fmtCtx->protocol_whitelist);
  }
  fmtCtx->protocol_whitelist = list;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxProtBlacklist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->protocol_blacklist != nullptr) {
    status = napi_create_string_utf8(env, fmtCtx->protocol_blacklist,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxProtBlacklist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  char* list;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context protocol_blacklist must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if ((type == napi_null) || (type == napi_undefined)) {
    if (fmtCtx->protocol_blacklist != nullptr) {
      av_freep(&fmtCtx->protocol_blacklist);
    }
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Format context protocol_blacklist must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  list = (char*) av_mallocz(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], list, strLen + 1, &strLen);
  CHECK_STATUS;

  if (fmtCtx->protocol_blacklist != nullptr) {
    av_freep(&fmtCtx->protocol_blacklist);
  }
  fmtCtx->protocol_blacklist = list;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxIORepo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_boolean(env, fmtCtx->io_repositioned, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFmtCtxMetadataHdrPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->metadata_header_padding, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMetadataHdrPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context metadata_header_padding must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context metadata_header_padding must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->metadata_header_padding);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxOutputTSOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int64(env, fmtCtx->output_ts_offset, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxOutputTSOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context output_ts_offset must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context output_ts_offset must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &fmtCtx->output_ts_offset);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxDumpSep(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (fmtCtx->dump_separator != nullptr) {
    status = napi_create_string_utf8(env, (char*) fmtCtx->dump_separator,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setFmtCtxDumpSep(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  uint8_t* dumpy;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the dump_separator property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (fmtCtx->dump_separator != nullptr) {
      av_freep(&fmtCtx->dump_separator);
    }
    fmtCtx->dump_separator = nullptr;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the dump_separator property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  dumpy = (uint8_t*) av_malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], (char*) dumpy, strLen + 1, &strLen);
  CHECK_STATUS;

  if (fmtCtx->dump_separator != nullptr) {
    av_freep(&fmtCtx->dump_separator);
  }
  fmtCtx->dump_separator = dumpy;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxMaxStreams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_int32(env, fmtCtx->max_streams, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxMaxStreams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context max_streams must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Format context max_streams must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &fmtCtx->max_streams);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxSkipEstDurFromPTS(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_get_boolean(env, fmtCtx->skip_estimate_duration_from_pts, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxSkipEstDurFromPTS(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Format context skip_estimate_duration_from_pts must be set with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_boolean) {
    NAPI_THROW_ERROR("Format context skip_estimate_duration_from_pts must be set with a Boolean.");
  }
  status = napi_get_value_bool(env, args[0], &flag);
  CHECK_STATUS;
  fmtCtx->skip_estimate_duration_from_pts = flag ? 1 : 0;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->priv_data == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
    return result;
  }

  status = fromContextPrivData(env, fmtCtx->priv_data, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFmtCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVFormatContext* fmtCtx;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if (fmtCtx->priv_data == nullptr) {
    goto done;
  }

  if (argc == 0) {
    NAPI_THROW_ERROR("A value must be provided to set priv_data.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("An object of private property values must be provided to set priv_data.");
  }

  status = toContextPrivData(env, args[0], fmtCtx->priv_data);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFmtCtxTypeName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  bool isFormat, isMuxer;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  isFormat = !((fmtCtx->oformat == nullptr) ^ (fmtCtx->iformat == nullptr));
  if (isFormat) {
    status = napi_create_string_utf8(env, "format", NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    isMuxer = fmtCtx->oformat != nullptr;
    status = napi_create_string_utf8(env, isMuxer ? "muxer" : "demuxer", NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value failSetter(napi_env env, napi_callback_info info) {
  NAPI_THROW_ERROR("Cannot set this read-only property value.");
}

// Makes a muxer as a generic holder of logical cable metadata
napi_value makeFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsObject, assign, jsJSON, jsParse;
  napi_valuetype type;
  bool isArray, deleted;
  AVFormatContext* fmtCtx = avformat_alloc_context();

  status = napi_get_global(env, &global);
  CHECK_STATUS;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;
  if (argc > 1) {
    NAPI_THROW_ERROR("Format may be created with zero or one options object argument.");
  }
  if (argc == 1) {
    status = napi_typeof(env, args[0], &type);
    CHECK_STATUS;
    if (type == napi_string) {
      status = napi_get_named_property(env, global, "JSON", &jsJSON);
      CHECK_STATUS;
      status =  napi_get_named_property(env, jsJSON, "parse", &jsParse);
      CHECK_STATUS;
      const napi_value pargs[] = { args[0] };
      status = napi_call_function(env, args[0], jsParse, 1, pargs, &args[0]);
      CHECK_STATUS;
      status = napi_typeof(env, args[0], &type);
      CHECK_STATUS;
      if (type == napi_object) {
        status = beam_delete_named_property(env, args[0], "type", &deleted);
        CHECK_STATUS;
        // status = beam_delete_named_property(env, args[0], "size", &deleted);
        // CHECK_STATUS;
      }
    }
    status = napi_is_array(env, args[0], &isArray);
    CHECK_STATUS;
    if (isArray || (type != napi_object)) {
      NAPI_THROW_ERROR("Cannot create a packet unless argument is an object.");
    }
  }

  status = fromAVFormatContext(env, fmtCtx, nullptr, &result);
  CHECK_STATUS;

  if (argc == 1) {
    status = napi_get_named_property(env, global, "Object", &jsObject);
    CHECK_STATUS;
    status = napi_get_named_property(env, jsObject, "assign", &assign);
    CHECK_STATUS;
    const napi_value fargs[] = { result, args[0] };
    status = napi_call_function(env, result, assign, 2, fargs, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getFmtCtxIFormatName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if ((fmtCtx->iformat != nullptr) && (fmtCtx->iformat->name != nullptr)) {
    status = napi_create_string_utf8(env, fmtCtx->iformat->name, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, "unknown", NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getFmtCtxOFormatName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFormatContext* fmtCtx;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &fmtCtx);
  CHECK_STATUS;

  if ((fmtCtx->oformat != nullptr) && (fmtCtx->oformat->name != nullptr)) {
    status = napi_create_string_utf8(env, fmtCtx->oformat->name, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, "unknown", NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value formatToJSON(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, jsObject, jsInter;
  AVFormatContext* fmtCtx;
  int count = 0;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &jsObject, (void**) &fmtCtx);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = napi_get_named_property(env, jsObject, "interleaved", &jsInter);
  CHECK_STATUS;

  napi_property_descriptor desc[55];

  DECLARE_GETTER3("type", true, getFmtCtxTypeName, fmtCtx);
  DECLARE_GETTER3("iformat", fmtCtx->iformat != nullptr, getFmtCtxIFormatName, fmtCtx);
  DECLARE_GETTER3("oformat", fmtCtx->oformat != nullptr, getFmtCtxOFormatName, fmtCtx);
  DECLARE_GETTER3("priv_data", fmtCtx->priv_data != nullptr, getFmtCtxPrivData, fmtCtx);
  DECLARE_GETTER3("ctx_flags", fmtCtx->ctx_flags > 0, getFmtCtxCtxFlags, fmtCtx);
  DECLARE_GETTER3("streams", true, getFmtCtxStreamsJSON, fmtCtx);
  DECLARE_GETTER3("url", (fmtCtx->url != nullptr) && (strlen(fmtCtx->url) > 0), getFmtCtxURL, fmtCtx);
  DECLARE_GETTER3("start_time", fmtCtx->start_time > 0, getFmtCtxStartTime, fmtCtx);
  DECLARE_GETTER3("duration", fmtCtx->duration > 0, getFmtCtxDuration, fmtCtx);
    // 10
  DECLARE_GETTER3("bit_rate", fmtCtx->bit_rate > 0, getFmtCtxBitRate, fmtCtx);
  DECLARE_GETTER3("packet_size", fmtCtx->packet_size > 0, getFmtCtxPacketSize, fmtCtx);
  DECLARE_GETTER3("max_delay", fmtCtx->max_delay >= 0, getFmtCtxMaxDelay, fmtCtx);
  DECLARE_GETTER3("flags", fmtCtx->flags != AVFMT_FLAG_AUTO_BSF, getFmtCtxFlags, fmtCtx);
  DECLARE_GETTER3("probesize", fmtCtx->probesize != 5000000, getFmtCtxProbeSize, fmtCtx);
  DECLARE_GETTER3("max_analyze_duration", fmtCtx->max_analyze_duration != 0, getFmtCtxMaxAnDur, fmtCtx);
  DECLARE_GETTER3("key", fmtCtx->keylen > 0, getFmtCtxKey, fmtCtx);
  DECLARE_GETTER3("programs", fmtCtx->nb_programs > 0, getFmtCtxPrograms, fmtCtx);
  DECLARE_GETTER3("max_index_size", fmtCtx->max_index_size != 1<<20, getFmtCtxMaxIndexSize, fmtCtx);
  DECLARE_GETTER3("max_picture_buffer", fmtCtx->max_picture_buffer != 3041280, getFmtCtxMaxPictBuf, fmtCtx);
    // 20
  DECLARE_GETTER3("metadata", fmtCtx->metadata != nullptr, getFmtCtxMetadata, fmtCtx);
  DECLARE_GETTER3("start_time_realtime", fmtCtx->start_time_realtime != AV_NOPTS_VALUE, getFmtCtxStartTRealT, fmtCtx);
  DECLARE_GETTER3("fps_probe_size", fmtCtx->fps_probe_size >= 0, getFmtCtxFpsProbeSize, fmtCtx);
  DECLARE_GETTER3("error_recognition", fmtCtx->error_recognition != 1, getFmtCtxErrRecog, fmtCtx);
  DECLARE_GETTER3("debug", fmtCtx->debug > 0, getFmtCtxDebug, fmtCtx);
  DECLARE_GETTER3("max_interleave_delta", fmtCtx->max_interleave_delta != 10000000, getFmtCtxMaxInterleaveD, fmtCtx);
  DECLARE_GETTER3("strict_std_compliance", fmtCtx->strict_std_compliance != FF_COMPLIANCE_NORMAL, getFmtCtxStrictStdComp, fmtCtx);
  DECLARE_GETTER3("event_flags", fmtCtx->event_flags > 0, getFmtCtxEventFlags, fmtCtx);
  DECLARE_GETTER3("max_ts_probe", fmtCtx->max_ts_probe != 50, getFmtCtxMaxTsProbe, fmtCtx);
  DECLARE_GETTER3("avoid_negative_ts", fmtCtx->avoid_negative_ts != AVFMT_AVOID_NEG_TS_AUTO, getFmtCtxAvoidNegTs, fmtCtx);
    // 30
  DECLARE_GETTER3("audio_preload", fmtCtx->audio_preload > 0, getFmtCtxAudioPreload, fmtCtx);
  DECLARE_GETTER3("max_chunk_duration", fmtCtx->max_chunk_duration > 0, getFmtCtxMaxChunkDur, fmtCtx);
  DECLARE_GETTER3("max_chunk_size", fmtCtx->max_chunk_size > 0, getFmtCtxMaxChunkSize, fmtCtx);
  DECLARE_GETTER3("use_wallclock_as_timestamps", fmtCtx->use_wallclock_as_timestamps != 0, getFmtCtxUseWallclock, fmtCtx);
  DECLARE_GETTER3("avio_flags", fmtCtx->avio_flags > 0, getFmtCtxAvioFlags, fmtCtx);
  DECLARE_GETTER3("duration_estimation_method", fmtCtx->duration_estimation_method != AVFMT_DURATION_FROM_PTS, getFmtCtxDurEstMethod, fmtCtx);
  DECLARE_GETTER3("skip_initial_bytes", fmtCtx->skip_initial_bytes > 0, getFmtCtxSkipInitBytes, fmtCtx);
  DECLARE_GETTER3("correct_ts_overflow", fmtCtx->correct_ts_overflow != 1, getFmtCtxCorrectTsOf, fmtCtx);
  DECLARE_GETTER3("seek2any", fmtCtx->seek2any != 0, getFmtCtxSeek2Any, fmtCtx);
  DECLARE_GETTER3("flush_packets", fmtCtx->flush_packets != -1, getFmtCtxFlushPackets, fmtCtx);
    // 40
  DECLARE_GETTER3("probe_score", fmtCtx->probe_score != 0, getFmtCtxProbeScore, fmtCtx);
  DECLARE_GETTER3("format_probesize", fmtCtx->format_probesize != 1<<20, getFmtCtxFmtProbesize, fmtCtx);
  DECLARE_GETTER3("codec_whitelist", fmtCtx->codec_whitelist != nullptr, getFmtCtxCodecWhitelist, fmtCtx);
  DECLARE_GETTER3("format_whitelist", fmtCtx->codec_whitelist != nullptr, getFmtCtxFmtWhitelist, fmtCtx);
  DECLARE_GETTER3("io_repositioned", fmtCtx->io_repositioned != 0, getFmtCtxIORepo, fmtCtx);
  DECLARE_GETTER3("metadata_header_padding", fmtCtx->metadata_header_padding >= 0, getFmtCtxMetadataHdrPad, fmtCtx);
  DECLARE_GETTER3("output_ts_offset", fmtCtx->output_ts_offset != 0, getFmtCtxOutputTSOffset, fmtCtx);
  DECLARE_GETTER3("dump_separator", strcmp(const_cast<char*>((char*) fmtCtx->dump_separator), ", ") != 0, getFmtCtxDumpSep, fmtCtx);
  DECLARE_GETTER3("protocol_whitelist", fmtCtx->protocol_whitelist != nullptr, getFmtCtxProtWhitelist, fmtCtx);
  DECLARE_GETTER3("protocol_blacklist", fmtCtx->protocol_blacklist != nullptr, getFmtCtxProtBlacklist, fmtCtx);
    // 50
  DECLARE_GETTER3("max_streams", fmtCtx->max_streams != 1000, getFmtCtxMaxStreams, fmtCtx);
  DECLARE_GETTER3("skip_estimate_duration_from_pts", fmtCtx->skip_estimate_duration_from_pts != 0, getFmtCtxSkipEstDurFromPTS, fmtCtx);
  desc[count++] = { "interleaved", nullptr, nullptr, nullptr, nullptr, jsInter, napi_enumerable, nullptr };

  status = napi_define_properties(env, result, count, desc);
  CHECK_STATUS;

  return result;
}

napi_status fromAVFormatContext(napi_env env, AVFormatContext* fmtCtx,
    Adaptor *adaptor, napi_value* result) {
  napi_status status;
  napi_value jsFmtCtx, extFmtCtxRef, extFmtCtx, extAdaptor, truth, undef;
  fmtCtxRef* fmtRef = new fmtCtxRef;
  fmtRef->fmtCtx = fmtCtx;

  bool isMuxer = fmtCtx->oformat != nullptr;
  bool isFormat = !((fmtCtx->oformat == nullptr) ^ (fmtCtx->iformat == nullptr));

  status = napi_create_object(env, &jsFmtCtx);
  PASS_STATUS;
  status = napi_get_boolean(env, true, &truth);
  PASS_STATUS;
  status = napi_get_undefined(env, &undef);
  PASS_STATUS;
  status = napi_create_external(env, fmtRef, formatContextFinalizer, adaptor, &extFmtCtxRef);
  PASS_STATUS;
  status = napi_create_external(env, fmtCtx, nullptr, adaptor, &extFmtCtx);
  PASS_STATUS;
  status = napi_create_external(env, adaptor, nullptr, nullptr, &extAdaptor);
  PASS_STATUS;

  if (!isFormat) {
    napi_property_descriptor desc[] = {
      { "type", nullptr, nullptr, getFmtCtxTypeName, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { isMuxer ? "oformat" : "iformat", nullptr, nullptr,
        isMuxer ? getFmtCtxOFormat : getFmtCtxIFormat,
        isMuxer ? setFmtCtxOFormat : nullptr, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, fmtCtx },
      { "priv_data", nullptr, nullptr, getFmtCtxPrivData, setFmtCtxPrivData, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "ctx_flags", nullptr, nullptr, getFmtCtxCtxFlags, nullptr, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "streams", nullptr, nullptr, getFmtCtxStreams, setFmtCtxStreams, nullptr,
         (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "url", nullptr, nullptr, getFmtCtxURL,
        isMuxer ? setFmtCtxURL : nullptr, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "start_time", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxStartTime, nullptr, nullptr,
        isMuxer ? napi_default : napi_enumerable, fmtCtx },
      { "duration", nullptr, nullptr, getFmtCtxDuration,
        isMuxer ? setFmtCtxDuration : nullptr, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "bit_rate", nullptr, nullptr, getFmtCtxBitRate, setFmtCtxBitRate, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 10
      { "packet_size", nullptr, nullptr, getFmtCtxPacketSize, setFmtCtxPacketSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_delay", nullptr, nullptr, getFmtCtxMaxDelay, setFmtCtxMaxDelay, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "flags", nullptr, nullptr, getFmtCtxFlags, setFmtCtxFlags, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "probesize", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxProbeSize,
        isMuxer ? nullptr : setFmtCtxProbeSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_analyze_duration", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxMaxAnDur,
        isMuxer ? nullptr : setFmtCtxMaxAnDur, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "key", nullptr, nullptr, getFmtCtxKey, setFmtCtxKey, nullptr,
        napi_enumerable, fmtCtx }, // As const uint8_t value, assume not settable
      { "programs", nullptr, nullptr, getFmtCtxPrograms, failSetter /* setFmtCtxPrograms */, nullptr,
        napi_enumerable, fmtCtx },
        // video_codec_id / audio_codec_id / subtitle_codec_id
      { "max_index_size", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxMaxIndexSize,
        isMuxer ? failSetter : setFmtCtxMaxIndexSize, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_picture_buffer", nullptr, nullptr, getFmtCtxMaxPictBuf, setFmtCtxMaxPictBuf, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
        // chapters?
      { "metadata", nullptr, nullptr, getFmtCtxMetadata, setFmtCtxMetadata, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 20
      { "start_time_realtime", nullptr, nullptr, getFmtCtxStartTRealT, setFmtCtxStartTRealT, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "fps_probe_size", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxFpsProbeSize,
        isMuxer ? failSetter : setFmtCtxFpsProbeSize, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "error_recognition", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxErrRecog,
        isMuxer ? failSetter : setFmtCtxErrRecog, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "debug", nullptr, nullptr, getFmtCtxDebug, setFmtCtxDebug, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_interleave_delta", nullptr, nullptr,
        isMuxer ? getFmtCtxMaxInterleaveD : nullptr,
        isMuxer ? setFmtCtxMaxInterleaveD : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "strict_std_compliance", nullptr, nullptr, getFmtCtxStrictStdComp, setFmtCtxStrictStdComp, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "event_flags", nullptr, nullptr, getFmtCtxEventFlags, setFmtCtxEventFlags, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_ts_probe", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxMaxTsProbe,
        isMuxer ? failSetter : setFmtCtxMaxTsProbe, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // ts_id - says it's going private
      { "avoid_negative_ts", nullptr, nullptr,
        isMuxer ? getFmtCtxAvoidNegTs : nullptr,
        isMuxer ? setFmtCtxAvoidNegTs : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "audio_preload", nullptr, nullptr,
        isMuxer ? getFmtCtxAudioPreload : nullptr,
        isMuxer ? setFmtCtxAudioPreload : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      // 30
      { "max_chunk_duration", nullptr, nullptr,
        isMuxer ? getFmtCtxMaxChunkDur : nullptr,
        isMuxer ? setFmtCtxMaxChunkDur : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "max_chunk_size", nullptr, nullptr,
        isMuxer ? getFmtCtxMaxChunkSize : nullptr,
        isMuxer ? setFmtCtxMaxChunkSize : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "use_wallclock_as_timestamps", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxUseWallclock,
        isMuxer ? failSetter : setFmtCtxUseWallclock, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "avio_flags", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxAvioFlags,
        isMuxer ? failSetter : setFmtCtxAvioFlags, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "duration_estimation_method", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxDurEstMethod, failSetter, nullptr,
        isMuxer ? napi_default : napi_enumerable, fmtCtx },
      { "skip_initial_bytes", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxSkipInitBytes,
        isMuxer ? failSetter : setFmtCtxSkipInitBytes, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "correct_ts_overflow", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxCorrectTsOf,
        isMuxer ? failSetter : setFmtCtxCorrectTsOf, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "seek2any", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxSeek2Any,
        isMuxer ? failSetter : setFmtCtxSeek2Any, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "flush_packets", nullptr, nullptr,
        isMuxer ? getFmtCtxFlushPackets : nullptr,
        isMuxer ? setFmtCtxFlushPackets : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "probe_score", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxProbeScore, failSetter, nullptr,
        isMuxer ? napi_default : napi_enumerable, fmtCtx },
      // 40
      { "format_probesize", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxFmtProbesize,
        isMuxer ? failSetter : setFmtCtxFmtProbesize, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "codec_whitelist", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxCodecWhitelist,
        isMuxer ? failSetter : setFmtCtxCodecWhitelist, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "format_whitelist", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxFmtWhitelist,
        isMuxer ? failSetter : setFmtCtxFmtWhitelist, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "io_repositioned", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxIORepo, failSetter, nullptr,
        isMuxer ? napi_default : napi_enumerable, fmtCtx },
      { "metadata_header_padding", nullptr, nullptr,
        isMuxer ? getFmtCtxMetadataHdrPad : nullptr,
        isMuxer ? setFmtCtxMetadataHdrPad : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
        // not exposing opaque
      { "output_ts_offset", nullptr, nullptr,
        isMuxer ? getFmtCtxOutputTSOffset : nullptr,
        isMuxer ? setFmtCtxOutputTSOffset : failSetter, nullptr,
        isMuxer ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, fmtCtx },
      { "dump_separator", nullptr, nullptr, getFmtCtxDumpSep, setFmtCtxDumpSep, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "protocol_whitelist", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxProtWhitelist,
        isMuxer ? failSetter : setFmtCtxProtWhitelist, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "protocol_blacklist", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxProtBlacklist,
        isMuxer ? failSetter : setFmtCtxProtBlacklist, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_streams", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxMaxStreams,
        isMuxer ? failSetter : setFmtCtxMaxStreams, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 50
      { "skip_estimate_duration_from_pts", nullptr, nullptr,
        isMuxer ? nullptr : getFmtCtxSkipEstDurFromPTS,
        isMuxer ? failSetter : setFmtCtxSkipEstDurFromPTS, nullptr,
        isMuxer ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "interleaved", nullptr, nullptr, nullptr, nullptr, truth,
        (napi_property_attributes) (napi_writable | napi_enumerable), nullptr }, // format is interleaved?
      { "newStream", nullptr, newStream, nullptr, nullptr, nullptr,
        napi_enumerable, fmtCtx },
      { "toJSON", nullptr, formatToJSON, nullptr, nullptr, nullptr, napi_default, fmtCtx },
      { "_formatContextRef", nullptr, nullptr, nullptr, nullptr, extFmtCtxRef, napi_default, nullptr },
      { "_formatContext", nullptr, nullptr, nullptr, nullptr, extFmtCtx, napi_default, nullptr },
      { "_adaptor", nullptr, nullptr, nullptr, nullptr, extAdaptor, napi_default, nullptr },
      { "__streams", nullptr, nullptr, nullptr, nullptr, undef, napi_writable, nullptr }
    };
    status = napi_define_properties(env, jsFmtCtx, 57, desc);
    PASS_STATUS;
  } else {
    napi_property_descriptor desc[] = {
      { "type", nullptr, nullptr, getFmtCtxTypeName, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "oformat", nullptr, nullptr, getFmtCtxOFormat, setFmtCtxOFormat, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "iformat", nullptr, nullptr, getFmtCtxIFormat, setFmtCtxIFormat, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "priv_data", nullptr, nullptr, getFmtCtxPrivData, setFmtCtxPrivData, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "ctx_flags", nullptr, nullptr, getFmtCtxCtxFlags, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "streams", nullptr, nullptr, getFmtCtxStreams, setFmtCtxStreams, nullptr,
         (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "url", nullptr, nullptr, getFmtCtxURL, setFmtCtxURL, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "start_time", nullptr, nullptr, getFmtCtxStartTime, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "duration", nullptr, nullptr, getFmtCtxDuration, setFmtCtxDuration, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 10
      { "bit_rate", nullptr, nullptr, getFmtCtxBitRate, setFmtCtxBitRate, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "packet_size", nullptr, nullptr, getFmtCtxPacketSize, setFmtCtxPacketSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_delay", nullptr, nullptr, getFmtCtxMaxDelay, setFmtCtxMaxDelay, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "flags", nullptr, nullptr, getFmtCtxFlags, setFmtCtxFlags, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "probesize", nullptr, nullptr, getFmtCtxProbeSize, setFmtCtxProbeSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_analyze_duration", nullptr, nullptr, getFmtCtxMaxAnDur, setFmtCtxMaxAnDur, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "key", nullptr, nullptr, getFmtCtxKey, setFmtCtxKey, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "programs", nullptr, nullptr, getFmtCtxPrograms, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
        // video_codec_id / audio_codec_id / subtitle_codec_id
      { "max_index_size", nullptr, nullptr, getFmtCtxMaxIndexSize, setFmtCtxMaxIndexSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_picture_buffer", nullptr, nullptr, getFmtCtxMaxPictBuf, setFmtCtxMaxPictBuf, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
        // chapters?
      // 20
      { "metadata", nullptr, nullptr, getFmtCtxMetadata, setFmtCtxMetadata, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "start_time_realtime", nullptr, nullptr, getFmtCtxStartTRealT, setFmtCtxStartTRealT, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "fps_probe_size", nullptr, nullptr, getFmtCtxFpsProbeSize, setFmtCtxFpsProbeSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "error_recognition", nullptr, nullptr, getFmtCtxErrRecog, setFmtCtxErrRecog, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "debug", nullptr, nullptr, getFmtCtxDebug, setFmtCtxDebug, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_interleave_delta", nullptr, nullptr, getFmtCtxMaxInterleaveD, setFmtCtxMaxInterleaveD, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "strict_std_compliance", nullptr, nullptr, getFmtCtxStrictStdComp, setFmtCtxStrictStdComp, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "event_flags", nullptr, nullptr, getFmtCtxEventFlags, setFmtCtxEventFlags, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_ts_probe", nullptr, nullptr, getFmtCtxMaxTsProbe, setFmtCtxMaxTsProbe, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // ts_id - says it's going private
      { "avoid_negative_ts", nullptr, nullptr, getFmtCtxAvoidNegTs, setFmtCtxAvoidNegTs, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 30
      { "audio_preload", nullptr, nullptr, getFmtCtxAudioPreload, setFmtCtxAudioPreload, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_chunk_duration", nullptr, nullptr, getFmtCtxMaxChunkDur, setFmtCtxMaxChunkDur, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "max_chunk_size", nullptr, nullptr, getFmtCtxMaxChunkSize, setFmtCtxMaxChunkSize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "use_wallclock_as_timestamps", nullptr, nullptr, getFmtCtxUseWallclock, setFmtCtxUseWallclock, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "avio_flags", nullptr, nullptr, getFmtCtxAvioFlags, setFmtCtxAvioFlags, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "duration_estimation_method", nullptr, nullptr, getFmtCtxDurEstMethod, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "skip_initial_bytes", nullptr, nullptr, getFmtCtxSkipInitBytes, setFmtCtxSkipInitBytes, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "correct_ts_overflow", nullptr, nullptr, getFmtCtxCorrectTsOf, setFmtCtxCorrectTsOf, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "seek2any", nullptr, nullptr, getFmtCtxSeek2Any, setFmtCtxSeek2Any, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "flush_packets", nullptr, nullptr, getFmtCtxFlushPackets, setFmtCtxFlushPackets, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 40
      { "probe_score", nullptr, nullptr, getFmtCtxProbeScore, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "format_probesize", nullptr, nullptr, getFmtCtxFmtProbesize, setFmtCtxFmtProbesize, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "codec_whitelist", nullptr, nullptr, getFmtCtxCodecWhitelist, setFmtCtxCodecWhitelist, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "format_whitelist", nullptr, nullptr, getFmtCtxFmtWhitelist, setFmtCtxFmtWhitelist, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "io_repositioned", nullptr, nullptr, getFmtCtxIORepo, nop, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "metadata_header_padding", nullptr, nullptr, getFmtCtxMetadataHdrPad, setFmtCtxMetadataHdrPad, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
        // not exposing opaque
      { "output_ts_offset", nullptr, nullptr, getFmtCtxOutputTSOffset, setFmtCtxOutputTSOffset, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "dump_separator", nullptr, nullptr, getFmtCtxDumpSep, setFmtCtxDumpSep, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "protocol_whitelist", nullptr, nullptr, getFmtCtxProtWhitelist, setFmtCtxProtWhitelist, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "protocol_blacklist", nullptr, nullptr, getFmtCtxProtBlacklist, setFmtCtxProtBlacklist, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      // 50
      { "max_streams", nullptr, nullptr, getFmtCtxMaxStreams, setFmtCtxMaxStreams, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "skip_estimate_duration_from_pts", nullptr, nullptr, getFmtCtxSkipEstDurFromPTS, setFmtCtxSkipEstDurFromPTS, nullptr,
        (napi_property_attributes) (napi_writable | napi_enumerable), fmtCtx },
      { "interleaved", nullptr, nullptr, nullptr, nullptr, truth,
        (napi_property_attributes) (napi_writable | napi_enumerable), nullptr }, // format is interleaved?
      { "newStream", nullptr, newStream, nullptr, nullptr, nullptr,
        napi_enumerable, fmtCtx },
      { "toJSON", nullptr, formatToJSON, nullptr, nullptr, nullptr, napi_default, fmtCtx },
      { "_formatContextRef", nullptr, nullptr, nullptr, nullptr, extFmtCtxRef, napi_default, nullptr },
      { "_formatContext", nullptr, nullptr, nullptr, nullptr, extFmtCtx, napi_default, nullptr },
      { "_adaptor", nullptr, nullptr, nullptr, nullptr, extAdaptor, napi_default, nullptr },
      { "__streams", nullptr, nullptr, nullptr, nullptr, undef, napi_writable, nullptr }
    };
    status = napi_define_properties(env, jsFmtCtx, 58, desc);
    PASS_STATUS;
  }

  *result = jsFmtCtx;
  return napi_ok;
}

void formatContextFinalizer(napi_env env, void* data, void* hint) {
  fmtCtxRef* fmtRef = (fmtCtxRef*) data;
  AVFormatContext* fc;
  Adaptor *adaptor = (Adaptor *)hint;
  int ret;

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

    if (fc->iformat != nullptr) {
      avformat_close_input(&fc);
    } else {
      // FIXME this is segfaulting ... why
      /* if (fc->codec_whitelist != nullptr) {
        av_freep(fc->codec_whitelist);
      }
      if (fc->format_whitelist != nullptr) {
        av_freep(fc->format_whitelist);
      }
      if (fc->protocol_whitelist != nullptr) {
        av_freep(fc->protocol_whitelist);
      }
      if (fc->protocol_blacklist != nullptr) {
        av_freep(fc->protocol_blacklist);
      } */
    }

    if (adaptor != nullptr) // crashes otherwise...
      avformat_free_context(fc);
  }

  delete fmtRef;
}

napi_value newStream(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, name, assign, global, jsObject, jsContext, jsStreams,
    extInput, jsJSON, jsParse, cpValue, cidValue;
  napi_valuetype type;
  bool isArray, deleted;
  AVFormatContext* fmtCtx;
  AVStream* stream;
  const AVStream* inputStream;
  char* codecName = nullptr;
  size_t strLen;
  AVCodec* codec = nullptr;
  const AVCodecDescriptor* codecDesc = nullptr;
  uint32_t streamCount;
  int ret;
  AVCodecID codecID = AV_CODEC_ID_NONE;

  status = napi_get_global(env, &global);
  CHECK_STATUS;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, &jsContext, (void**) &fmtCtx);
  CHECK_STATUS;

  if (argc >= 1) {
    status = napi_typeof(env, args[0], &type);
    CHECK_STATUS;
    if ((type != napi_string) && (type != napi_object)) {
      NAPI_THROW_ERROR("New stream for a format context requires a string value to specify a codec, where provided.");
    }
    if (type == napi_object) {
      status = napi_get_named_property(env, args[0], "name", &name);
      CHECK_STATUS;
    } else {
      name = args[0];
    }
    status = napi_typeof(env, name, &type);
    CHECK_STATUS;
    if (type == napi_string) {
      status = napi_get_value_string_utf8(env, name, nullptr, 0, &strLen);
      CHECK_STATUS;
      codecName = (char*) malloc(sizeof(char) * (strLen + 1));
      status = napi_get_value_string_utf8(env, name, codecName, strLen + 1, &strLen);
      CHECK_STATUS;
    }

    if ((codecName != nullptr) && (strlen(codecName) == 0)) {
      // printf("Searching for input stream codec name.\n");
      status = napi_get_named_property(env, args[0], "_stream", &extInput);
      CHECK_STATUS;
      status = napi_typeof(env, extInput, &type);
      CHECK_STATUS;
      if (type != napi_external) {
        NAPI_THROW_ERROR("Unable to determine the codec name for the new stream.");
      }
      status = napi_get_value_external(env, extInput, (void**) &inputStream);
      CHECK_STATUS;
      free(codecName);
      codecName = strdup(avcodec_get_name(inputStream->codecpar->codec_id));
    }

    if ((codecName != nullptr) && (codecName[0] == '{')) { // Assume that some JSON has been passed
      status = napi_get_named_property(env, global, "JSON", &jsJSON);
      CHECK_STATUS;
      status =  napi_get_named_property(env, jsJSON, "parse", &jsParse);
      CHECK_STATUS;
      const napi_value pargs[] = { args[0] };
      status = napi_call_function(env, args[0], jsParse, 1, pargs, &args[0]);
      CHECK_STATUS;
      status = napi_typeof(env, args[0], &type);
      CHECK_STATUS;
      if (type == napi_object) {
        status = beam_delete_named_property(env, args[0], "type", &deleted);
        CHECK_STATUS;
      }
    }

    status = napi_get_named_property(env, args[0], "codecpar", &cpValue);
    CHECK_STATUS;
    status = napi_typeof(env, cpValue, &type);
    CHECK_STATUS;
    if (type == napi_object) {
      status = napi_get_named_property(env, cpValue, "codec_id", &cidValue);
      CHECK_STATUS;
      status = napi_get_value_int32(env, cidValue, (int32_t*) &codecID);
      if (status == napi_ok) {
        if ((fmtCtx->iformat == nullptr) && (fmtCtx->oformat == nullptr)) {
          codec = avcodec_find_encoder(codecID);
        } else {
          if (fmtCtx->oformat) { codec = avcodec_find_encoder(codecID); }
        }
        if (codec == nullptr) {
          codec = avcodec_find_decoder(codecID);
        }
      }
    } else {
      if (fmtCtx->oformat) { // Look for encoders
        codec = avcodec_find_encoder_by_name(codecName);
        if (codec == nullptr) {
          codecDesc = avcodec_descriptor_get_by_name(codecName);
          if (codecDesc != nullptr) {
            codec = avcodec_find_encoder(codecDesc->id);
          }
        }
      }
      if (codec == nullptr) {
        codec = avcodec_find_decoder_by_name(codecName);
        if (codec == nullptr) {
          codecDesc = codecName != nullptr ?
            avcodec_descriptor_get_by_name(codecName) : nullptr;
          if (codecDesc != nullptr) {
            codec = avcodec_find_decoder(codecDesc->id);
          }
        }
      }
    }
    // printf("From name %s, selected AVCodec %s\n", codecName,
    //   (codec != nullptr) ? codec->name : "");
    if (codecName != nullptr) { free(codecName); }
  } else {
    NAPI_THROW_ERROR("Stream requires an options object with a codec name.");
  }
  stream = avformat_new_stream(fmtCtx, codec);
  // printf("Stupidist codec timebase is %i/%i\n", stream->codec->time_base.num,
  //   stream->codec->time_base.den);

  if (stream == nullptr) {
    NAPI_THROW_ERROR("Unable to create a stream for this format context.");
  }

  if (codec != nullptr) {
    stream->codecpar->codec_type = codec->type;
    stream->codecpar->codec_id = codec->id;
    // TODO set codec_tag here
  }

  status = napi_get_named_property(env, args[0], "_stream", &extInput);
  CHECK_STATUS;
  status = napi_typeof(env, extInput, &type);
  CHECK_STATUS;
  // printf("External input type is %i\n", type);
  if (type == napi_external) { // Output streams set with input stream?
    // printf("Attempting to set transfer internal stream timing.\n");
    status = napi_get_value_external(env, extInput, (void**) &inputStream);
    CHECK_STATUS;
    ret = avformat_transfer_internal_stream_timing_info(fmtCtx->oformat,
      stream, inputStream, AVFMT_TBCF_AUTO);
    if (ret < 0) {
      printf("%s", avErrorMsg("DEBUG: Failed to transfer timebase: ", ret));
    }
  }

  status = fromAVStream(env, stream, &result);
  CHECK_STATUS;

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((argc >=1) && (type == napi_object)) {
    status = napi_get_named_property(env, global, "Object", &jsObject);
    CHECK_STATUS;
    status = napi_get_named_property(env, jsObject, "assign", &assign);
    CHECK_STATUS;
    const napi_value fargs[] = { result, args[0] };
    status = napi_call_function(env, result, assign, 2, fargs, &result);
    CHECK_STATUS;
  }

  status = napi_get_named_property(env, jsContext, "__streams", &jsStreams); 
  CHECK_STATUS;
  status = napi_typeof(env, jsStreams, &type);
  CHECK_STATUS;
  if (type != napi_object) {
    return result;
  }

  status = napi_is_array(env, jsStreams, &isArray);
  CHECK_STATUS;
  if (isArray) {
    status = napi_get_array_length(env, jsStreams, &streamCount);
    CHECK_STATUS;
  } else {
    napi_value propNames;
    status = napi_get_property_names(env, jsStreams, &propNames);
    CHECK_STATUS;
    status = napi_get_array_length(env, propNames, &streamCount);
    CHECK_STATUS;
  }
  status = napi_set_element(env, jsStreams, streamCount, result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamIndex(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_int32(env, stream->index, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_int32(env, stream->id, &result);
  CHECK_STATUS;
  return result;
}

napi_value setStreamID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream id property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("The stream's id property must be set with a number.");
  }

  status = napi_get_value_int32(env, args[0], &stream->id);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  if (stream->priv_data == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = fromContextPrivData(env, stream->priv_data, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setStreamPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;

  if (stream->priv_data == nullptr) {
    goto done;
  }

  if (argc == 0) {
    NAPI_THROW_ERROR("A value must be provided to set priv_data.");
  }

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("An object of private property values must be provided to set priv_data.");
  }

  status = toContextPrivData(env, args[0], stream->priv_data);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamTimeBase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->time_base.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->time_base.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setStreamTimeBase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream time_base property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("The stream's time_base property must be set with an array of two numbers.");
  // }
  for ( uint32_t x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("The stream's time_base property must be set with an array of two numbers.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("The stream's time_base property array elements must be numbers.");
    }
  }
  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->time_base.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->time_base.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamStartTime(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  if (stream->start_time == AV_NOPTS_VALUE) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_int64(env, stream->start_time, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value setStreamStartTime(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set a stream's start_time property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    stream->start_time = AV_NOPTS_VALUE;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("The stream's start_time property must be set with a number.");
  }

  status = napi_get_value_int64(env, args[0], &stream->start_time);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  if (stream->duration == AV_NOPTS_VALUE) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_int64(env, stream->duration, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value setStreamDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set a stream's duration property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    stream->start_time = AV_NOPTS_VALUE;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("The stream's duration property must be set with a number.");
  }

  status = napi_get_value_int64(env, args[0], &stream->duration);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamNbFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_int64(env, stream->nb_frames, &result);
  CHECK_STATUS;
  return result;
}

napi_value setStreamNbFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set a stream's nb_frames property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    stream->start_time = 0;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("The stream's nb_frames property must be set with a number.");
  }

  status = napi_get_value_int64(env, args[0], &stream->nb_frames);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamDisposition(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = beam_set_bool(env, result, "DEFAULT", stream->disposition & AV_DISPOSITION_DEFAULT);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DUB", stream->disposition & AV_DISPOSITION_DUB);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "ORIGINAL", stream->disposition & AV_DISPOSITION_ORIGINAL);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "COMMENT", stream->disposition & AV_DISPOSITION_COMMENT);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "LYRICS", stream->disposition & AV_DISPOSITION_LYRICS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "KARAOKE", stream->disposition & AV_DISPOSITION_KARAOKE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "FORCED", stream->disposition & AV_DISPOSITION_FORCED);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "HEARING_IMPAIRED", stream->disposition & AV_DISPOSITION_HEARING_IMPAIRED);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "VISUAL_IMPAIRED", stream->disposition & AV_DISPOSITION_VISUAL_IMPAIRED);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CLEAN_EFFECTS", stream->disposition & AV_DISPOSITION_CLEAN_EFFECTS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "ATTACHED_PIC", stream->disposition & AV_DISPOSITION_ATTACHED_PIC);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "TIMED_THUMBNAILS", stream->disposition & AV_DISPOSITION_TIMED_THUMBNAILS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CAPTIONS", stream->disposition & AV_DISPOSITION_CAPTIONS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DESCRIPTIONS", stream->disposition & AV_DISPOSITION_DESCRIPTIONS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "METADATA", stream->disposition & AV_DISPOSITION_METADATA);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DEPENDENT", stream->disposition & AV_DISPOSITION_DEPENDENT);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "STILL_IMAGE", stream->disposition & AV_DISPOSITION_STILL_IMAGE);
  CHECK_STATUS;

  return result;
}

napi_value setStreamDisposition(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;
  bool isArray, present, flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set a stream's disposition flags.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("The stream's disposition flags must be set with an object with Boolean-valued properties.");
  }

  status = beam_get_bool(env, args[0], "DEFAULT", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_DEFAULT :
    stream->disposition & ~AV_DISPOSITION_DEFAULT; }
  status = beam_get_bool(env, args[0], "DUB", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_DUB :
    stream->disposition & ~AV_DISPOSITION_DUB; }
  status = beam_get_bool(env, args[0], "ORIGINAL", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_ORIGINAL :
    stream->disposition & ~AV_DISPOSITION_ORIGINAL; }
  status = beam_get_bool(env, args[0], "COMMENT", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_COMMENT :
    stream->disposition & ~AV_DISPOSITION_COMMENT; }
  status = beam_get_bool(env, args[0], "LYRICS", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_LYRICS :
    stream->disposition & ~AV_DISPOSITION_LYRICS; }
  status = beam_get_bool(env, args[0], "KARAOKE", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_KARAOKE :
    stream->disposition & ~AV_DISPOSITION_KARAOKE; }
  status = beam_get_bool(env, args[0], "FORCED", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_FORCED :
    stream->disposition & ~AV_DISPOSITION_FORCED; }
  status = beam_get_bool(env, args[0], "HEARING_IMPAIRED", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_HEARING_IMPAIRED :
    stream->disposition & ~AV_DISPOSITION_HEARING_IMPAIRED; }
  status = beam_get_bool(env, args[0], "VISUAL_IMPAIRED", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_VISUAL_IMPAIRED :
    stream->disposition & ~AV_DISPOSITION_VISUAL_IMPAIRED; }
  status = beam_get_bool(env, args[0], "CLEAN_EFFECTS", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_CLEAN_EFFECTS :
    stream->disposition & ~AV_DISPOSITION_CLEAN_EFFECTS; }
  status = beam_get_bool(env, args[0], "ATTACHED_PIC", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_ATTACHED_PIC :
    stream->disposition & ~AV_DISPOSITION_ATTACHED_PIC; }
  status = beam_get_bool(env, args[0], "TIMED_THUMBNAILS", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_TIMED_THUMBNAILS :
    stream->disposition & ~AV_DISPOSITION_TIMED_THUMBNAILS; }
  status = beam_get_bool(env, args[0], "CAPTIONS", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_CAPTIONS :
    stream->disposition & ~AV_DISPOSITION_CAPTIONS; }
  status = beam_get_bool(env, args[0], "DESCRIPTIONS", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_DESCRIPTIONS :
    stream->disposition & ~AV_DISPOSITION_DESCRIPTIONS; }
  status = beam_get_bool(env, args[0], "METADATA", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_METADATA :
    stream->disposition & ~AV_DISPOSITION_METADATA; }
  status = beam_get_bool(env, args[0], "DEPENDENT", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_DEPENDENT :
    stream->disposition & ~AV_DISPOSITION_DEPENDENT; }
  status = beam_get_bool(env, args[0], "STILL_IMAGE", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->disposition = (flag) ?
    stream->disposition | AV_DISPOSITION_STILL_IMAGE :
    stream->disposition & ~AV_DISPOSITION_STILL_IMAGE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamDiscard(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avdiscard->forward, stream->discard),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;
  return result;
}

napi_value setStreamDiscard(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;
  char* enumName;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set a stream's discard property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("The stream's discard property must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumName = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], enumName, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = beam_lookup_enum(beam_avdiscard->inverse, enumName);
  stream->discard = (enumValue != BEAM_ENUM_UNKNOWN) ? (AVDiscard) enumValue : AVDISCARD_DEFAULT;
  free(enumName);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamSmpAspectRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->sample_aspect_ratio.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->sample_aspect_ratio.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setStreamSmpAspectRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream sample_aspect_ratio property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("The stream's sample_aspect_ratio property must be set with an array of two numbers.");
  // }
  for ( uint32_t x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("The stream's sample_aspect_ratio property must be set with an array of two numbers.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("The stream's sample_aspect_ratio property array elements must be numbers.");
    }
  }
  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->sample_aspect_ratio.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->sample_aspect_ratio.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamEventFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = beam_set_bool(env, result, "METADATA_UPDATED",
    stream->event_flags & AVSTREAM_EVENT_FLAG_METADATA_UPDATED);
  CHECK_STATUS;

  return result;
}

napi_value setStreamEventFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;
  bool isArray, present, flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream event_flags property.")
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Stream property event_flags is set with an object of Boolean values.");
  }
  status = beam_get_bool(env, args[0], "METADATA_UPDATED", &present, &flag);
  CHECK_STATUS;
  if (present) { stream->event_flags = (flag) ?
    stream->event_flags | AVSTREAM_EVENT_FLAG_METADATA_UPDATED :
    stream->event_flags & ~AVSTREAM_EVENT_FLAG_METADATA_UPDATED; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamMetadata(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;
  AVDictionaryEntry* tag = nullptr;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  while ((tag = av_dict_get(stream->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    status = beam_set_string_utf8(env, result, tag->key, tag->value);
    CHECK_STATUS;
  }

  return result;
}

napi_value setStreamMetadata(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;
  bool isArray;
  AVDictionary* dict = nullptr;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream's metadata property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Stream metadata can only be set with an object of tag names and values.");
  }

  status = makeAVDictionary(env, args[0], &dict);
  CHECK_STATUS;
  stream->metadata = dict;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamAvgFrameRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->avg_frame_rate.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->avg_frame_rate.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setStreamAvgFrameRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream avg_frame_rate property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("The stream's avg_frame_rate property must be set with an array of two numbers.");
  // }
  for ( uint32_t x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("The stream's avg_frame_rate property must be set with an array of two numbers.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("The stream's avg_frame_rate property array elements must be numbers.");
    }
  }
  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->avg_frame_rate.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->avg_frame_rate.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamRFrameRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->r_frame_rate.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, stream->r_frame_rate.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setStreamRFrameRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stream r_frame_rate property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("The stream's r_frame_rate property must be set with an array of two numbers.");
  // }
  for ( uint32_t x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("The stream's r_frame_rate property must be set with an array of two numbers.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("The stream's r_frame_rate property array elements must be numbers.");
    }
  }
  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->r_frame_rate.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &stream->r_frame_rate.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamCodecPar(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, jsStream, jsPars;
  napi_valuetype type;
  AVStream* stream;

  status = napi_get_cb_info(env, info, 0, nullptr, &jsStream, (void**) &stream);
  CHECK_STATUS;

  status = napi_get_named_property(env, jsStream, "__codecPar", &jsPars);
  CHECK_STATUS;
  status = napi_typeof(env, jsPars, &type);
  CHECK_STATUS;
  if (type == napi_object) {
    return jsPars;
  }

  status = fromAVCodecParameters(env, stream->codecpar, false, &result);
  CHECK_STATUS;
  status = napi_set_named_property(env, jsStream, "__codecPar", result);
  CHECK_STATUS;

  return result;
}

napi_value setStreamCodecPar(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, jsCodecPar, extCodecPar, jsStream;
  napi_valuetype type;
  AVStream* stream;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, &jsStream, (void**) &stream);
  CHECK_STATUS;
  if (argc != 1) {
    NAPI_THROW_ERROR("Cannot set stream codec parameters without a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("Setting stream codec parameters requires an object.");
  }
  status = napi_get_named_property(env, args[0], "_codecPar", &extCodecPar);
  CHECK_STATUS;
  status = napi_typeof(env, extCodecPar, &type);
  CHECK_STATUS;
  if (type != napi_external) {
    jsCodecPar = makeCodecParamsInternal(env, info, false); // Try and make some
    status = napi_get_named_property(env, jsCodecPar, "_codecPar", &extCodecPar);
    CHECK_STATUS;
  }
  status = napi_get_value_external(env, extCodecPar, (void**) &stream->codecpar);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS; // Reset external codec par reference
  status = napi_set_named_property(env, jsStream, "__codecPar", result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamAttachedPic(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVStream* stream;
  packetData* pd = new packetData;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  if (stream->attached_pic.data == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    pd->packet = &stream->attached_pic;
    status = fromAVPacket(env, pd, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value getStreamSideData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVStream* stream;
  void* resultData;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &stream);
  CHECK_STATUS;

  status = napi_get_null(env, &result);
  CHECK_STATUS;

  if (stream->nb_side_data <= 0) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_object(env, &result);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, result, "type", "PacketSideData");
    for ( int x = 0 ; x < stream->nb_side_data ; x++ ) {
      status = napi_create_buffer_copy(env, stream->side_data[x].size,
        stream->side_data[x].data, &resultData, &element);
      CHECK_STATUS;
      status = napi_set_named_property(env, result,
        beam_lookup_name(beam_packet_side_data_type->forward,
          stream->side_data[x].type), element);
      CHECK_STATUS;
    }
  }


  return result;
}

napi_value setStreamSideData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsBuffer, jsBufferFrom, name, names, element, arrayData;
  napi_valuetype type;
  bool isArray, isBuffer;
  uint32_t sdCount;
  AVStream* stream;
  AVPacketSideData* sd;
  char* typeName;
  size_t strLen;
  int psdt;
  void* rawdata;
  size_t rawdataSize;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set side_data property.");
  }

  status = napi_get_global(env, &global);
  CHECK_STATUS;
  status = napi_get_named_property(env, global, "Buffer", &jsBuffer);
  CHECK_STATUS;
  status = napi_get_named_property(env, jsBuffer, "from", &jsBufferFrom);
  CHECK_STATUS;

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;

  switch (type) {
    case napi_object:
    case napi_null:
    case napi_undefined:
      for ( int x = 0 ; x < stream->nb_side_data ; x++ ) {
        sd = &stream->side_data[x];
        av_freep(&sd->data);
      }
      av_freep(&stream->side_data);
      stream->nb_side_data = 0;
      if (type != napi_object) { goto done; };
      break;
    default:
      NAPI_THROW_ERROR("Stream side_data property requires an object with Buffer-valued properties.");
  }

  status = napi_get_property_names(env, args[0], &names);
  CHECK_STATUS;
  status = napi_get_array_length(env, names, &sdCount);
  CHECK_STATUS;

  for ( uint32_t x = 0 ; x < sdCount ; x++ ) {
    status = napi_get_element(env, names, x, &name);
    CHECK_STATUS;
    status = napi_get_property(env, args[0], name, &element);
    CHECK_STATUS;
    status = napi_is_buffer(env, element, &isBuffer);
    CHECK_STATUS;
    if (!isBuffer) {
      status = napi_get_named_property(env, element, "data", &arrayData);
      CHECK_STATUS;
      // TODO more checks that this is a buffer from JSON?
      status = napi_is_array(env, arrayData, &isArray);
      CHECK_STATUS;
      if (isArray) {
        const napi_value fargs[] = { arrayData };
        status = napi_call_function(env, element, jsBufferFrom, 1, fargs, &element);
        CHECK_STATUS;
      } else {
        continue;
      }
    }
    status = napi_get_value_string_utf8(env, name, nullptr, 0, &strLen);
    CHECK_STATUS;
    typeName = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, name, typeName, strLen + 1, &strLen);
    CHECK_STATUS;

    psdt = beam_lookup_enum(beam_packet_side_data_type->inverse, typeName);
    free(typeName);
    if (psdt == BEAM_ENUM_UNKNOWN) {
      continue;
    } else {
      status = napi_get_buffer_info(env, element, &rawdata, &rawdataSize);
      CHECK_STATUS;
      uint8_t* pktdata = av_stream_new_side_data(stream,
        (AVPacketSideDataType) psdt, rawdataSize);
      if (pktdata != nullptr) {
        memcpy(pktdata, rawdata, rawdataSize);
      }
    }
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getStreamTypeName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value typeName;

  status = napi_create_string_utf8(env, "Stream", NAPI_AUTO_LENGTH, &typeName);
  CHECK_STATUS;

  return typeName;
}

// Used in restful GET of single streams
napi_value streamSecretSetIndex(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVStream* stream;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &stream);
  CHECK_STATUS;
  if (argc < 1) {
    goto done; // fail quietly
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    goto done; // shh
  }
  status = napi_get_value_int32(env, args[0], &stream->index);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value streamToJSON(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, thisStream, jsStream;
  AVStream* s;
  int count = 0;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &thisStream, (void**) &s);
  CHECK_STATUS;

  if (s == nullptr) {
    status = napi_get_named_property(env, thisStream, "_stream", &jsStream);
    CHECK_STATUS;
    status = napi_get_value_external(env, jsStream, (void**) &s);
    CHECK_STATUS;
  }

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  napi_property_descriptor desc[20];

  DECLARE_GETTER3("type", true, getStreamTypeName, s);
  DECLARE_GETTER3("index", true, getStreamIndex, s);
  DECLARE_GETTER3("id", true, getStreamID, s);
  DECLARE_GETTER3("time_base", true, getStreamTimeBase, s);
  DECLARE_GETTER3("start_time", s->start_time != AV_NOPTS_VALUE, getStreamStartTime, s);
  DECLARE_GETTER3("duration", s->start_time != AV_NOPTS_VALUE, getStreamDuration, s);
  DECLARE_GETTER3("nb_frames", s->nb_frames > 0, getStreamNbFrames, s);
  DECLARE_GETTER3("disposition", s->disposition != 0, getStreamDisposition, s);
  DECLARE_GETTER3("discard", s->discard != AVDISCARD_DEFAULT, getStreamDiscard, s);
  // 10
  DECLARE_GETTER3("sample_aspect_ratio",
    (s->sample_aspect_ratio.num != 0) || (s->sample_aspect_ratio.den != 1),
      getStreamSmpAspectRt, s);
  DECLARE_GETTER3("metadata", s->metadata != nullptr, getStreamMetadata, s);
  DECLARE_GETTER3("avg_frame_rate", s->avg_frame_rate.num != 0, getStreamAvgFrameRate, s);
  // TODO attached_pic
  DECLARE_GETTER3("side_data", s->nb_side_data > 0, getStreamSideData, s);
  DECLARE_GETTER3("event_flags", s->event_flags > 0, getStreamEventFlags, s);
  DECLARE_GETTER3("r_frame_rate", s->r_frame_rate.num != 0, getStreamRFrameRate, s);
  DECLARE_GETTER3("codecpar", true, codecParToJSON, s->codecpar);

  status = napi_define_properties(env, result, count, desc);
  CHECK_STATUS;

  return result;
}

napi_status fromAVStream(napi_env env, AVStream* stream, napi_value* result) {
  napi_status status;
  napi_value jsStream, extStream, undef, nameValue;

  status = napi_create_object(env, &jsStream);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &nameValue);
  PASS_STATUS;
  status = napi_get_undefined(env, &undef);
  PASS_STATUS;
  // Note - streams are cleaned by avcodec_close() and avcodec_free_context()
  status = napi_create_external(env, stream, nullptr, nullptr, &extStream);
  PASS_STATUS;

  // Note - name is a dummy property used to avoid it being accidentally set
  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, getStreamTypeName, nop, nullptr, napi_enumerable, nullptr },
    { "index", nullptr, nullptr, getStreamIndex, nop, nullptr, napi_enumerable, stream },
    { "id", nullptr, nullptr, getStreamID, setStreamID, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "time_base", nullptr, nullptr, getStreamTimeBase, setStreamTimeBase, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "start_time", nullptr, nullptr, getStreamStartTime, setStreamStartTime, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "duration", nullptr, nullptr, getStreamDuration, setStreamDuration, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "nb_frames", nullptr, nullptr, getStreamNbFrames, setStreamNbFrames, nullptr,
       (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "disposition", nullptr, nullptr, getStreamDisposition, setStreamDisposition, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "discard", nullptr, nullptr, getStreamDiscard, setStreamDiscard, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    // 10
    { "sample_aspect_ratio", nullptr, nullptr, getStreamSmpAspectRt, setStreamSmpAspectRt, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "metadata", nullptr, nullptr, getStreamMetadata, setStreamMetadata, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "avg_frame_rate", nullptr, nullptr, getStreamAvgFrameRate, setStreamAvgFrameRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "attached_pic", nullptr, nullptr, getStreamAttachedPic, nop, nullptr,
       napi_enumerable, stream },
    { "side_data", nullptr, nullptr, getStreamSideData, setStreamSideData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "event_flags", nullptr, nullptr, getStreamEventFlags, setStreamEventFlags, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "r_frame_rate", nullptr, nullptr, getStreamRFrameRate, setStreamRFrameRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "codecpar", nullptr, nullptr, getStreamCodecPar, setStreamCodecPar, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), stream },
    { "name", nullptr, nullptr, nullptr, nullptr, nameValue, napi_writable, nullptr },
    { "_stream", nullptr, nullptr, nullptr, nullptr, extStream, napi_default, nullptr },
    // 20
    { "toJSON", nullptr, streamToJSON, nullptr, nullptr, nullptr, napi_default, stream },
    { "__codecPar", nullptr, nullptr, nullptr, nullptr, undef, napi_writable, nullptr },
    { "__index", nullptr, streamSecretSetIndex, nullptr, nullptr, nullptr, napi_default, stream }
  };
  status = napi_define_properties(env, jsStream, 22, desc);
  PASS_STATUS;

  *result = jsStream;
  return napi_ok;
}

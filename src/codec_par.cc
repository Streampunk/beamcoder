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

#include "codec_par.h"

napi_value getCodecParCodecType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_get_media_type_string(c->codec_type);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "data",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParCodecType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  int mtype;
  char * typeName;
  size_t typeNameLen;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters codec_type must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameter codec_type must be set using a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &typeNameLen);
  CHECK_STATUS;
  typeName = (char*) malloc(sizeof(char) * (typeNameLen + 1));
  status = napi_get_value_string_utf8(env, args[0], typeName, typeNameLen + 1, &typeNameLen);
  CHECK_STATUS;

  mtype = beam_lookup_enum(beam_avmedia_type->inverse, typeName);
  c->codec_type = (mtype == BEAM_ENUM_UNKNOWN) ?
    AVMEDIA_TYPE_UNKNOWN : (AVMediaType) mtype;

//SET_BODY codec_type AVMediaType CodecType
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParCodecID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->codec_id, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParCodecID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters codec_id must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameters codec_id must be set by a number.");
  }

  status = napi_get_value_int32(env, args[0], (int32_t*) &c->codec_id);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, (char*) avcodec_get_name(c->codec_id),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* codecName;
  size_t nameLen;
  const AVCodecDescriptor* codecDesc;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters name must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters name must be provided with a value.");
  }

  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &nameLen);
  CHECK_STATUS;
  codecName = (char*) malloc(sizeof(char) * (nameLen + 1));
  status = napi_get_value_string_utf8(env, args[0], codecName, nameLen + 1, &nameLen);
  CHECK_STATUS;
  codecDesc = avcodec_descriptor_get_by_name((const char *) codecName);
  CHECK_STATUS;
  if (codecDesc == nullptr) {
    c->codec_id = AV_CODEC_ID_NONE;
    c->codec_type = AVMEDIA_TYPE_DATA;
  } else {
    c->codec_id = codecDesc->id;
    c->codec_type = codecDesc->type;
  }

  free(codecName);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParCodecTag(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  char fourcc[AV_FOURCC_MAX_STRING_SIZE];

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  av_fourcc_make_string(fourcc, c->codec_tag);
  if (strchr(fourcc, '[')) { // not a recognised tag
    status = napi_create_uint32(env, c->codec_tag, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, fourcc, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecParCodecTag(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters codec_tag must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;

  if (type == napi_string) {
    char* tag;
    size_t tagLen;
    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &tagLen);
    CHECK_STATUS;
    tag = (char*) malloc(sizeof(char) * (tagLen + 1));
    status = napi_get_value_string_utf8(env, args[0], tag, tagLen + 1, &tagLen);
    CHECK_STATUS;
    c->codec_tag = MKTAG(tag[0], tag[1], tag[2], tag[3]);
  } else if (type == napi_number) {
    status = napi_get_value_uint32(env, args[0], &c->codec_tag);
    CHECK_STATUS;
  } else {
    printf("Setting codec parameters codec_tag expects a string or a number - attempting to set from codec_id\n");
    if ((AVMEDIA_TYPE_VIDEO == c->codec_type) || (AVMEDIA_TYPE_AUDIO == c->codec_type)) {
      const struct AVCodecTag *table[] = { 
        (AVMEDIA_TYPE_VIDEO == c->codec_type) ? avformat_get_riff_video_tags() : avformat_get_riff_audio_tags(), 0
      };
      if (0 == av_codec_get_tag2(table, c->codec_id, &c->codec_tag))
        NAPI_THROW_ERROR("Codec parameters codec_tag could not be set.");
      printf("Setting codec parameters codec_tag to 0x%08x\n", c->codec_tag);
    } else
      NAPI_THROW_ERROR("Codec parameters codec_tag expects a string or number value.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParExtraData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* codecPar;
  void* resultData;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codecPar);
  CHECK_STATUS;
  if (codecPar->extradata_size > 0) {
    status = napi_create_buffer_copy(env, codecPar->extradata_size,
      codecPar->extradata, &resultData, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecParExtraData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsBuffer, jsBufferFrom, arrayData;
  napi_valuetype type;
  bool isBuffer, isArray;
  AVCodecParameters* codecPar;
  uint8_t* data;
  size_t dataLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codecPar);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the extradata property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type == napi_null) {
    if (codecPar->extradata_size > 0) { // Tidy up old buffers
      av_freep(&codecPar->extradata);
    }
    codecPar->extradata_size = 0;
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
      NAPI_THROW_ERROR("A buffer is required to set the extradata propeprty.");
    }
  }

  status = napi_get_buffer_info(env, args[0], (void**) &data, &dataLen);
  CHECK_STATUS;
  if (codecPar->extradata_size > 0) { // Tidy up old buffers
    av_freep(&codecPar->extradata);
    codecPar->extradata_size = 0;
  }
  codecPar->extradata = (uint8_t*) av_mallocz(dataLen + AV_INPUT_BUFFER_PADDING_SIZE);
  codecPar->extradata_size = dataLen;
  memcpy(codecPar->extradata, data, dataLen);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* fmtName = nullptr;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  switch (c->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
      fmtName = av_get_pix_fmt_name((AVPixelFormat) c->format);
      break;
    case AVMEDIA_TYPE_AUDIO:
      fmtName = av_get_sample_fmt_name((AVSampleFormat) c->format);
      break;
    default: // Might not have media type set
      fmtName = av_get_pix_fmt_name((AVPixelFormat) c->format);
      if (fmtName == nullptr) {
        fmtName = av_get_sample_fmt_name((AVSampleFormat) c->format);
      }
      break;
  }
  if (fmtName == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, (char*) fmtName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecParFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* formatName;
  size_t nameLen;
  int format;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters format must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_undefined) || (type == napi_null)) {
    c->format = AV_PIX_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters format must be set with a string value");
  }

  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &nameLen);
  CHECK_STATUS;
  formatName = (char*) malloc(sizeof(char) * (nameLen + 1));
  status = napi_get_value_string_utf8(env, args[0], formatName, nameLen + 1, &nameLen);
  CHECK_STATUS;

  switch (c->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
      format = (int) av_get_pix_fmt((const char *) formatName);
      break;
    case AVMEDIA_TYPE_AUDIO:
      format = (int) av_get_sample_fmt((const char *) formatName);
      break;
    default: // codec_type may not have been set yet ... guess mode
      format = (int) av_get_pix_fmt((const char *) formatName);
      if (format == AV_PIX_FMT_NONE) {
        format = (int) av_get_sample_fmt((const char *) formatName);
      }
      break;
  }
  c->format = format;

  free(formatName);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int64(env, c->bit_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters bit_rate must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameters bit_rate must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &c->bit_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParSmpAspectRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, c->sample_aspect_ratio.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, c->sample_aspect_ratio.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParSmpAspectRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, num, den;
  napi_valuetype type;
  AVCodecParameters* c;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters sample_aspect_ratio must be provided with a value.");
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (!isArray) {
    NAPI_THROW_ERROR("Codec parameters sample_aspect_ratio must be set with an array of numbers.");
  }

  status = napi_get_element(env, args[0], 0, &num);
  CHECK_STATUS;
  status = napi_typeof(env, num, &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameters sample_aspect_ratio numerator must be a number.");
  }
  CHECK_STATUS;

  status = napi_get_element(env, args[0], 1, &den);
  CHECK_STATUS;
  status = napi_typeof(env, den, &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameters sample_aspect_ratio denominator must be a number.");
  }
  CHECK_STATUS;

  status = napi_get_value_int32(env, num, &c->sample_aspect_ratio.num);
  CHECK_STATUS;
  status = napi_get_value_int32(env, den, &c->sample_aspect_ratio.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParFieldOrder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    (char*) beam_lookup_name(beam_field_order->forward, c->field_order),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParFieldOrder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters field_order must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters field_order must be set with a string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = beam_lookup_enum(beam_field_order->inverse, enumString);
  if (enumValue == BEAM_ENUM_UNKNOWN) {
    NAPI_THROW_ERROR("Codec parameters field_order value unrecognised. Did you mean e.g. 'progressive'?")
  }
  c->field_order = (AVFieldOrder) enumValue;

  free(enumString);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParColorRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_color_range_name(c->color_range);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "unknown", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParColorRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters color_range must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->color_range = AVCOL_RANGE_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters color_range must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = av_color_range_from_name((const char *) enumString);
  free(enumString);
  if (enumValue < 0) {
    NAPI_THROW_ERROR("Codec parameter color_range is not recognised. One of 'tv' or 'pc'?");
  }
  c->color_range = (AVColorRange) enumValue;


done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParColorPrims(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_color_primaries_name(c->color_primaries);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParColorPrims(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters color_primaries must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->color_primaries = AVCOL_PRI_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters color_primaries must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = av_color_primaries_from_name((const char *) enumString);
  free(enumString);
  if (enumValue < 0) {
    NAPI_THROW_ERROR("Codec parameter color_primaries is not recognised. Did you mean e.g. 'bt709'?");
  }
  c->color_primaries = (AVColorPrimaries) enumValue;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParColorTrc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_color_transfer_name(c->color_trc);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParColorTrc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters color_trc must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->color_trc = AVCOL_TRC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters color_trc must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = av_color_transfer_from_name((const char *) enumString);
  free(enumString);
  if (enumValue < 0) {
    NAPI_THROW_ERROR("Codec parameter color_trc is not recognised. Did you mean e.g. 'bt709'?");
  }
  c->color_trc = (AVColorTransferCharacteristic) enumValue;


done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParColorSpace(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_color_space_name(c->color_space);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParColorSpace(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters color_space must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->color_space = AVCOL_SPC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters color_space must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = av_color_space_from_name((const char *) enumString);
  free(enumString);
  if (enumValue < 0) {
    NAPI_THROW_ERROR("Codec parameter color_space is not recognised. Did you mean e.g. 'bt709'?");
  }
  c->color_space = (AVColorSpace) enumValue;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParChromaLoc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* enumName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  enumName = av_chroma_location_name(c->chroma_location);
  status = napi_create_string_utf8(env,
    (enumName != nullptr) ? (char*) enumName : "unspecified",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParChromaLoc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameters chroma_location must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->chroma_location = AVCHROMA_LOC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameters chroma_location must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  enumValue = av_chroma_location_from_name((const char *) enumString);
  free(enumString);
  if (enumValue < 0) {
    NAPI_THROW_ERROR("Codec parameter chroma_location is not recognised. Did you mean e.g. 'left'?");
  }
  c->chroma_location = (AVChromaLocation) enumValue;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  char enumName[64];

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  av_get_channel_layout_string(enumName, 64, 0,
    c->channel_layout ? c->channel_layout : av_get_default_channel_layout(c->channels));
  status = napi_create_string_utf8(env, enumName, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecParChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  uint64_t chanLay;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {  NAPI_THROW_ERROR("Codec parameters channel_layout must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->channel_layout = 0;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameter channel_layout must be set with a string value.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  chanLay = av_get_channel_layout((const char *) enumString);
  free(enumString);
  if (chanLay != 0) {
    c->channel_layout = chanLay;
  } else {
    NAPI_THROW_ERROR("Channel layout name is not recognized. Set 'null' for '0 channels'.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParBitsPerCodedSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->bits_per_coded_sample, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParBitsPerCodedSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter bits_per_coded_sample must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter bits_per_coded_sample must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->bits_per_coded_sample);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParBitsPerRawSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->bits_per_raw_sample, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParBitsPerRawSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter bits_per_raw_sample must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter bits_per_raw_sample must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->bits_per_raw_sample);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParLevel(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->level, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParLevel(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter level must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->level = FF_LEVEL_UNKNOWN;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter level must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->level);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->width, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter width must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter width must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->width);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->height, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter height must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter height must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->height);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParVideoDelay(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->video_delay, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParVideoDelay(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter video_delay must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter video_delay must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->video_delay);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}


napi_value getCodecParChannels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->channels, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParChannels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter channels must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter channels must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->channels);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParSampleRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->sample_rate, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParSampleRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter sample_rate must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter sample_rate must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->sample_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParBlockAlign(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->block_align, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParBlockAlign(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter block_align must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter block_align must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->block_align);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}


napi_value getCodecParFrameSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->frame_size, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParFrameSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter frame_size must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter frame_size must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->frame_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParInitialPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->initial_padding, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParInitialPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter initial_padding must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter initial_padding must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->initial_padding);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParTrailingPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->trailing_padding, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParTrailingPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter trailing_padding must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter trailing_padding must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->trailing_padding);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParSeekPreroll(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_int32(env, c->seek_preroll, &result);
  CHECK_STATUS;
  return result;
}

napi_value setCodecParSeekPreroll(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter seek_preroll must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Codec parameter seek_preroll must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &c->seek_preroll);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParProfile(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  const char* profName;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  profName = avcodec_profile_name(c->codec_id, c->profile);
  if (profName != nullptr) {
    status = napi_create_string_utf8(env, (char*) profName,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_int32(env, c->profile, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecParProfile(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecParameters* c;
  char* enumString;
  size_t strLen;
  const AVProfile* profile;
  const AVCodecDescriptor* codecDesc;
  bool foundProfile = false;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &c);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Codec parameter profile must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    c->profile = FF_PROFILE_UNKNOWN;
    goto done;
  }
  if (type == napi_number) {
    status = napi_get_value_int32(env, args[0], &c->profile);
    CHECK_STATUS;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("Codec parameter profile must be set by integer or string.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  enumString = (char*) malloc(sizeof(char) * (strLen + 1));
  CHECK_STATUS;
  status = napi_get_value_string_utf8(env, args[0], enumString, strLen + 1, &strLen);
  CHECK_STATUS;

  codecDesc = avcodec_descriptor_get(c->codec_id);
  if (codecDesc == nullptr) goto done;
  profile = codecDesc->profiles;
  if (!profile) {
    printf("Failed to set codec profile \'%s\' - recognised profiles not available for codec \'%s\'.\n", enumString, codecDesc->name);
    printf("Set profile as a numeric value to work around this problem.\n");
    c->profile = FF_PROFILE_UNKNOWN;
  } else {
    while (profile->profile != FF_PROFILE_UNKNOWN) {
      if (strcmp(enumString, profile->name) == 0) {
        c->profile = profile->profile;
        foundProfile = true;
        break;
      }
      profile = profile + 1;
    }
    if (!foundProfile) {
      c->profile = FF_PROFILE_UNKNOWN;
      printf("Failed to find codec profile \'%s\' in recognised profiles.\n", enumString);
    }
  }
  free(enumString);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecParTypeName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;

  status = napi_create_string_utf8(env, "CodecParameters", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value makeCodecParameters(napi_env env, napi_callback_info info) {
  return makeCodecParamsInternal(env, info, true);
}

napi_value makeCodecParamsInternal(napi_env env, napi_callback_info info, bool ownAlloc) {
  napi_status status;
  napi_value result, global, jsObject, assign, jsJSON, jsParse;
  napi_valuetype type;
  bool isArray, deleted;
  AVCodecParameters* c = avcodec_parameters_alloc();

  status = napi_get_global(env, &global);
  CHECK_STATUS;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;
  if (argc > 1) {
    NAPI_THROW_ERROR("Parameters may be created with zero or one options object argument.");
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
        status = beam_delete_named_property(env, args[0], "name", &deleted);
        CHECK_STATUS;
      }
    }
    status = napi_is_array(env, args[0], &isArray);
    CHECK_STATUS;
    if (isArray || (type != napi_object)) {
      NAPI_THROW_ERROR("Cannot create codec parameters unless argument is an object.");
    }
  }

  status = fromAVCodecParameters(env, c, ownAlloc, &result);
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

  if ((c->codec_type == AVMEDIA_TYPE_UNKNOWN) && (c->codec_id != AV_CODEC_ID_NONE)) {
    c->codec_type = avcodec_get_type(c->codec_id);
  }

  return result;
}

napi_value codecParToJSON(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecParameters* c;
  int count = 0;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &c);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  napi_property_descriptor desc[35];
  DECLARE_GETTER3("type", true, getCodecParTypeName, c);
  DECLARE_GETTER3("codec_type", true, getCodecParCodecType, c);
  DECLARE_GETTER3("codec_id", true, getCodecParCodecID, c);
  DECLARE_GETTER3("name", true, getCodecParName, c);
  DECLARE_GETTER3("codec_tag", c->codec_tag > 0, getCodecParCodecTag, c);
  DECLARE_GETTER3("extradata", c->extradata != nullptr, getCodecParExtraData, c);
  DECLARE_GETTER3("format", c->format >= 0, getCodecParFormat, c);
  DECLARE_GETTER3("bit_rate", c->bit_rate > 0, getCodecParBitRate, c);
  DECLARE_GETTER3("bits_per_coded_sample", c->bits_per_coded_sample > 0, getCodecParBitsPerCodedSmp, c);
    // 10
  DECLARE_GETTER3("bits_per_raw_sample", c->bits_per_raw_sample > 0, getCodecParBitsPerRawSmp, c);
  DECLARE_GETTER3("profile", c->profile != FF_PROFILE_UNKNOWN, getCodecParProfile, c);
  DECLARE_GETTER3("level", c->level != FF_LEVEL_UNKNOWN, getCodecParLevel, c);
  DECLARE_GETTER3("width", c->width != 0, getCodecParWidth, c);
  DECLARE_GETTER3("height", c->height != 0, getCodecParHeight, c);
  DECLARE_GETTER3("sample_aspect_ratio",
      (c->sample_aspect_ratio.num != 0) || (c->sample_aspect_ratio.den != 1),
        getCodecParSmpAspectRt, c);
  DECLARE_GETTER3("field_order", c->field_order != AV_FIELD_UNKNOWN, getCodecParFieldOrder, c);
  DECLARE_GETTER3("color_range", c->color_range != AVCOL_RANGE_UNSPECIFIED, getCodecParColorRange, c);
  DECLARE_GETTER3("color_primaries", c->color_primaries != AVCOL_PRI_UNSPECIFIED, getCodecParColorPrims, c);
  DECLARE_GETTER3("color_trc", c->color_trc != AVCOL_TRC_UNSPECIFIED, getCodecParColorTrc, c);
    // 20
  DECLARE_GETTER3("color_space", c->color_space != AVCOL_SPC_UNSPECIFIED, getCodecParColorSpace, c);
  DECLARE_GETTER3("chroma_location", c->chroma_location != AVCHROMA_LOC_UNSPECIFIED, getCodecParChromaLoc, c);
  DECLARE_GETTER3("video_delay", c->video_delay != 0, getCodecParVideoDelay, c);
  DECLARE_GETTER3("channel_layout", c->channel_layout != 0, getCodecParChanLayout, c);
  DECLARE_GETTER3("channels", c->channels > 0, getCodecParChannels, c);
  DECLARE_GETTER3("sample_rate", c->sample_rate > 0, getCodecParSampleRate, c);
  DECLARE_GETTER3("block_align", c->block_align > 0, getCodecParBlockAlign, c);
  DECLARE_GETTER3("frame_size", c->frame_size > 0, getCodecParFrameSize, c);
  DECLARE_GETTER3("initial_padding", c->initial_padding > 0, getCodecParInitialPad, c);
  DECLARE_GETTER3("trailing_padding", c->trailing_padding > 0, getCodecParTrailingPad, c);
    // 30
  DECLARE_GETTER3("seek_preroll", c->seek_preroll > 0, getCodecParSeekPreroll, c);

  status = napi_define_properties(env, result, count, desc);
  CHECK_STATUS;

  return result;
}

napi_status fromAVCodecParameters(napi_env env, AVCodecParameters* c, bool ownAlloc, napi_value* result) {
  napi_status status;
  napi_value jsCodecPar, extCodecPar;

  status = napi_create_object(env, &jsCodecPar);
  PASS_STATUS;
  status = napi_create_external(env, c, ownAlloc?codecParamsFinalizer:nullptr, nullptr, &extCodecPar);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, getCodecParTypeName, nop, nullptr, napi_enumerable, nullptr },
    { "codec_type", nullptr, nullptr, getCodecParCodecType, setCodecParCodecType, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "codec_id", nullptr, nullptr, getCodecParCodecID, setCodecParCodecID, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "name", nullptr, nullptr, getCodecParName, setCodecParName, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "codec_tag", nullptr, nullptr, getCodecParCodecTag, setCodecParCodecTag, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "extradata", nullptr, nullptr, getCodecParExtraData, setCodecParExtraData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "format", nullptr, nullptr, getCodecParFormat, setCodecParFormat, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "bit_rate", nullptr, nullptr, getCodecParBitRate, setCodecParBitRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "bits_per_coded_sample", nullptr, nullptr, getCodecParBitsPerCodedSmp, setCodecParBitsPerCodedSmp, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "bits_per_raw_sample", nullptr, nullptr, getCodecParBitsPerRawSmp, setCodecParBitsPerRawSmp, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c }, // 10
    { "profile", nullptr, nullptr, getCodecParProfile, setCodecParProfile, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "level", nullptr, nullptr, getCodecParLevel, setCodecParLevel, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "width", nullptr, nullptr, getCodecParWidth, setCodecParWidth, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "height", nullptr, nullptr, getCodecParHeight, setCodecParHeight, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "sample_aspect_ratio", nullptr, nullptr, getCodecParSmpAspectRt, setCodecParSmpAspectRt, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "field_order", nullptr, nullptr, getCodecParFieldOrder, setCodecParFieldOrder, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "color_range", nullptr, nullptr, getCodecParColorRange, setCodecParColorRange, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "color_primaries", nullptr, nullptr, getCodecParColorPrims, setCodecParColorPrims, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "color_trc", nullptr, nullptr, getCodecParColorTrc, setCodecParColorTrc, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "color_space", nullptr, nullptr, getCodecParColorSpace, setCodecParColorSpace, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c }, // 20
    { "chroma_location", nullptr, nullptr, getCodecParChromaLoc, setCodecParChromaLoc, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "video_delay", nullptr, nullptr, getCodecParVideoDelay, setCodecParVideoDelay, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "channel_layout", nullptr, nullptr, getCodecParChanLayout, setCodecParChanLayout, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "channels", nullptr, nullptr, getCodecParChannels, setCodecParChannels, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "sample_rate", nullptr, nullptr, getCodecParSampleRate, setCodecParSampleRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "block_align", nullptr, nullptr, getCodecParBlockAlign, setCodecParBlockAlign, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "frame_size", nullptr, nullptr, getCodecParFrameSize, setCodecParFrameSize, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "initial_padding", nullptr, nullptr, getCodecParInitialPad, setCodecParInitialPad, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "trailing_padding", nullptr, nullptr, getCodecParTrailingPad, setCodecParTrailingPad, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c },
    { "seek_preroll", nullptr, nullptr, getCodecParSeekPreroll, setCodecParSeekPreroll, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), c }, // 30
    { "toJSON", nullptr, codecParToJSON, nullptr, nullptr, nullptr, napi_default, c},
    { "_codecPar", nullptr, nullptr, nullptr, nullptr, extCodecPar, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsCodecPar, 32, desc);
  PASS_STATUS;

  *result = jsCodecPar;
  return napi_ok;
};

void codecParamsFinalizer(napi_env env, void* data, void* hint) {
  AVCodecParameters* c = (AVCodecParameters*) data;
  // if ((c->extradata != nullptr) && (c->extradata_size > 0)) {
  //   av_freep(&c->extradata);
  //   c->extradata_size = 0;
  // }
  avcodec_parameters_free(&c);
}

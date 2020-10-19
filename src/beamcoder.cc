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

#include "node_api.h"
#include "beamcoder_util.h"
#include "governor.h"
#include "format.h"
#include "demux.h"
#include "decode.h"
#include "filter.h"
#include "encode.h"
#include "mux.h"
#include "packet.h"
#include "codec_par.h"
#include <stdio.h>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavdevice/avdevice.h>
  #include <libavfilter/avfilter.h>
  #include <libavformat/avformat.h>
  #include <libavutil/avutil.h>
  #include <libavutil/pixdesc.h>
  #include <libpostproc/postprocess.h>
  #include <libswresample/swresample.h>
  #include <libswscale/swscale.h>
}

napi_value versions(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = napi_create_uint32(env, avcodec_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avcodec", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, avdevice_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avdevice", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, avfilter_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avfilter", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, avformat_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avformat", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, avutil_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avutil", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, postproc_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "postproc", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, swresample_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swresample", value);
  CHECK_STATUS;
  status = napi_create_uint32(env, swscale_version(), &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swscale", value);
  CHECK_STATUS;

  return result;
}

napi_value avVersionInfo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;

  const char* verInfo = av_version_info();

  status = napi_create_string_utf8(env, verInfo, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value versionStrings(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  char vstr[16];

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBAVCODEC_VERSION_MAJOR, LIBAVCODEC_VERSION_MINOR,
    LIBAVCODEC_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avcodec", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBAVDEVICE_VERSION_MAJOR, LIBAVDEVICE_VERSION_MINOR,
    LIBAVDEVICE_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avdevice", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBAVFILTER_VERSION_MAJOR, LIBAVFILTER_VERSION_MINOR,
    LIBAVFILTER_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avfilter", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR,
    LIBAVFORMAT_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avformat", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBAVUTIL_VERSION_MAJOR, LIBAVUTIL_VERSION_MINOR,
    LIBAVUTIL_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avutil", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBPOSTPROC_VERSION_MAJOR, LIBPOSTPROC_VERSION_MINOR,
    LIBPOSTPROC_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "postproc", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBSWRESAMPLE_VERSION_MAJOR, LIBSWRESAMPLE_VERSION_MINOR,
    LIBSWRESAMPLE_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swresample", value);
  CHECK_STATUS;

  sprintf(vstr, "%i.%i.%i", LIBSWSCALE_VERSION_MAJOR, LIBSWSCALE_VERSION_MINOR,
    LIBSWSCALE_VERSION_MICRO);
  status = napi_create_string_utf8(env, vstr, NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swscale", value);
  CHECK_STATUS;

  return result;
}

napi_value configurations(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, avcodec_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avcodec", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avdevice_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avdevice", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avfilter_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avfilter", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avformat_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avformat", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avutil_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avutil", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, postproc_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "postproc", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, swresample_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swresample", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, swscale_configuration(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swscale", value);
  CHECK_STATUS;

  return result;
}

napi_value licenses(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, avcodec_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avcodec", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avdevice_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avdevice", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avfilter_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avfilter", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avformat_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avformat", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, avutil_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "avutil", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, postproc_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "postproc", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, swresample_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swresample", value);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, swscale_license(), NAPI_AUTO_LENGTH, &value);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "swscale", value);
  CHECK_STATUS;

  return result;
}

napi_value logging(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  int logLevel;
  char* logLevelStr;
  size_t strLen;

  napi_value args[1];
  size_t argc = 1;
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;

  if (argc == 0) {
    logLevel = av_log_get_level();
    status = napi_create_string_utf8(env,
      (char*) beam_lookup_name(beam_logging_level->forward, logLevel),
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    if (argc != 1) {
      status = napi_throw_error(env, nullptr, "Wrong number of arguments to set log level.");
      return nullptr;
    }

    napi_value params = args[0];
    napi_valuetype t;
    status = napi_typeof(env, params, &t);
    CHECK_STATUS;
    if (t != napi_string) {
      status = napi_throw_type_error(env, nullptr, "Logging level parameter must be a string.");
      return nullptr;
    }

    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
    CHECK_STATUS;
    logLevelStr = (char*) malloc(sizeof(char) * (strLen + 1));
    CHECK_STATUS;
    status = napi_get_value_string_utf8(env, args[0], logLevelStr, strLen + 1, &strLen);
    CHECK_STATUS;

    logLevel = beam_lookup_enum(beam_logging_level->inverse, logLevelStr);
    if (logLevel == BEAM_ENUM_UNKNOWN) {
      NAPI_THROW_ERROR("Logging level string unrecognised");
    }
    av_log_set_level(logLevel);

    status = napi_get_undefined(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_status fromAVCodec(napi_env env, const AVCodec* codec, napi_value *result) {
  napi_status status;
  napi_value array, element, subel, value, props, nullval;
  const AVProfile* profile;
  const AVRational* framerate;
  const enum AVPixelFormat* pixfmt;
  const int* samplerate;
  const AVCodecDescriptor* codecDesc;
  const enum AVSampleFormat* samplefmt;
  const uint64_t* chanlay;
  int32_t index = 0;

  status = napi_get_null(env, &nullval);
  PASS_STATUS;

  status = napi_create_object(env, &value);
  PASS_STATUS;
  status = beam_set_string_utf8(env, value, "type", "Codec");
  PASS_STATUS;
  status = beam_set_string_utf8(env, value, "name", (char*) codec->name);
  PASS_STATUS;
  status = beam_set_string_utf8(env, value, "long_name", (char*) codec->long_name);
  PASS_STATUS;
  status = beam_set_string_utf8(env, value, "codec_type",
    (char*) av_get_media_type_string(codec->type));
  PASS_STATUS;
  status = beam_set_int32(env, value, "id", (int32_t) codec->id);
  PASS_STATUS;
  status = beam_set_bool(env, value, "decoder", av_codec_is_decoder(codec));
  PASS_STATUS;
  status = beam_set_bool(env, value, "encoder", av_codec_is_encoder(codec));
  PASS_STATUS;

  status = napi_create_object(env, &props);
  PASS_STATUS;
  status = beam_set_bool(env, props, "DRAW_HORIZ_BAND", codec->capabilities & AV_CODEC_CAP_DRAW_HORIZ_BAND);
  PASS_STATUS;
  status = beam_set_bool(env, props, "DR1", codec->capabilities & AV_CODEC_CAP_DR1);
  PASS_STATUS;
  status = beam_set_bool(env, props, "TRUNCATED", codec->capabilities & AV_CODEC_CAP_TRUNCATED);
  PASS_STATUS;
  status = beam_set_bool(env, props, "DELAY", codec->capabilities & AV_CODEC_CAP_DELAY);
  PASS_STATUS;
  status = beam_set_bool(env, props, "SMALL_LAST_FRAME", codec->capabilities & AV_CODEC_CAP_SMALL_LAST_FRAME);
  PASS_STATUS;
  status = beam_set_bool(env, props, "SUBFRAMES", codec->capabilities & AV_CODEC_CAP_SUBFRAMES);
  PASS_STATUS;
  status = beam_set_bool(env, props, "EXPERIMENTAL", codec->capabilities & AV_CODEC_CAP_EXPERIMENTAL);
  PASS_STATUS;
  status = beam_set_bool(env, props, "CHANNEL_CONF", codec->capabilities & AV_CODEC_CAP_CHANNEL_CONF);
  PASS_STATUS;
  status = beam_set_bool(env, props, "FRAME_THREADS", codec->capabilities & AV_CODEC_CAP_FRAME_THREADS);
  PASS_STATUS;
  status = beam_set_bool(env, props, "SLICE_THREADS", codec->capabilities & AV_CODEC_CAP_SLICE_THREADS);
  PASS_STATUS;
  status = beam_set_bool(env, props, "PARAM_CHANGE", codec->capabilities & AV_CODEC_CAP_PARAM_CHANGE);
  PASS_STATUS;
  status = beam_set_bool(env, props, "AUTO_THREADS", codec->capabilities & AV_CODEC_CAP_AUTO_THREADS);
  PASS_STATUS;
  status = beam_set_bool(env, props, "VARIABLE_FRAME_SIZE", codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE);
  PASS_STATUS;
  status = beam_set_bool(env, props, "AVOID_PROBING", codec->capabilities & AV_CODEC_CAP_AVOID_PROBING);
  PASS_STATUS;
  status = beam_set_bool(env, props, "INTRA_ONLY", codec->capabilities & AV_CODEC_CAP_INTRA_ONLY);
  PASS_STATUS;
  status = beam_set_bool(env, props, "LOSSLESS", codec->capabilities & AV_CODEC_CAP_LOSSLESS);
  PASS_STATUS;
  status = beam_set_bool(env, props, "HARDWARE", codec->capabilities & AV_CODEC_CAP_HARDWARE);
  PASS_STATUS;
  status = beam_set_bool(env, props, "HYBRID", codec->capabilities & AV_CODEC_CAP_HYBRID);
  PASS_STATUS;
  status = napi_set_named_property(env, value, "capabilities", props);
  PASS_STATUS;

  if (codec->supported_framerates != nullptr) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    framerate = codec->supported_framerates;
    index = 0;
    while ((framerate->num != 0) && (framerate->den != 0)) {
      status = napi_create_array(env, &element);
      PASS_STATUS;
      status = napi_create_int32(env, framerate->num, &subel);
      PASS_STATUS;
      status = napi_set_element(env, element, 0, subel);
      PASS_STATUS;
      status = napi_create_int32(env, framerate->den, &subel);
      PASS_STATUS;
      status = napi_set_element(env, element, 1, subel);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      framerate = framerate + 1;
    }
    status = napi_set_named_property(env, value, "supported_framerates", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "supported_framerates", nullval);
    PASS_STATUS;
  }

  if (codec->pix_fmts != nullptr) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    pixfmt = codec->pix_fmts;
    index = 0;
    while (*pixfmt != -1) {
      status = napi_create_string_utf8(env,
        (char*) av_get_pix_fmt_name(*pixfmt), NAPI_AUTO_LENGTH, &element);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      pixfmt = pixfmt + 1;
    }
    status = napi_set_named_property(env, value, "pix_fmts", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "pix_fmts", nullval);
    PASS_STATUS;
  }

  if (codec->supported_samplerates != nullptr) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    samplerate = codec->supported_samplerates;
    index = 0;
    while (*samplerate != 0) {
      status = napi_create_int32(env, *samplerate, &element);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      samplerate = samplerate + 1;
    }
    status = napi_set_named_property(env, value, "supported_samplerates", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "supported_samplerates", nullval);
    PASS_STATUS;
  }

  if (codec->sample_fmts) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    samplefmt = codec->sample_fmts;
    index = 0;
    while (*samplefmt != -1) {
      status = napi_create_string_utf8(env,
        (char*) av_get_sample_fmt_name(*samplefmt), NAPI_AUTO_LENGTH, &element);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      samplefmt = samplefmt + 1;
    }
    status = napi_set_named_property(env, value, "sample_fmts", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "sample_fmts", nullval);
    PASS_STATUS;
  }

  if (codec->channel_layouts != nullptr) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    chanlay = codec->channel_layouts;
    index = 0;
    while (*chanlay != 0) {
      char chanLayStr[64];
      av_get_channel_layout_string(chanLayStr, 64, 0, *chanlay);
      status = napi_create_string_utf8(env, chanLayStr, NAPI_AUTO_LENGTH, &element);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      chanlay = chanlay + 1;
    }
    status = napi_set_named_property(env, value, "channel_layouts", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "channel_layouts", nullval);
    PASS_STATUS;
  }

  status = beam_set_uint32(env, value, "max_lowres", codec->max_lowres);
  PASS_STATUS;

  if (codec->priv_class != nullptr) {
    status = fromAVClass(env, codec->priv_class, &element);
    PASS_STATUS;
    status = napi_set_named_property(env, value, "priv_class", element);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "priv_class", nullval);
    PASS_STATUS;
  }

  if (codec->profiles != nullptr) {
    status = napi_create_array(env, &array);
    PASS_STATUS;
    profile = codec->profiles;
    index = 0;
    // printf("Profiles for %s %s\n", codec->name, profile->name);
    while (profile->profile != FF_PROFILE_UNKNOWN) {
      status = napi_create_string_utf8(env, profile->name, NAPI_AUTO_LENGTH, &element);
      PASS_STATUS;
      status = napi_set_element(env, array, index++, element);
      PASS_STATUS;
      profile = profile + 1;
    }
    status = napi_set_named_property(env, value, "profiles", array);
    PASS_STATUS;
  } else {
    status = napi_set_named_property(env, value, "profiles", nullval);
    PASS_STATUS;
  }

  if (codec->wrapper_name != nullptr) {
    status = beam_set_string_utf8(env, value, "wrapper_name", (char*) codec->wrapper_name);
    PASS_STATUS;
  }

  codecDesc = avcodec_descriptor_get(codec->id);
  if (codecDesc != nullptr) {
    status = fromAVCodecDescriptor(env, codecDesc, &element);
    PASS_STATUS;
    status = napi_set_named_property(env, value, "descriptor", element);
    PASS_STATUS;
  }

  *result = value;
  return napi_ok;
}

napi_value codecs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value, decEnc;
  bool hasProp;
  void* opaque = nullptr;
  const AVCodec* codec;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  codec = av_codec_iterate(&opaque);
  while (codec != nullptr) {
    status = fromAVCodec(env, codec, &value);
    CHECK_STATUS;
    status = napi_has_named_property(env, result, codec->name, &hasProp);
    CHECK_STATUS;
    if (hasProp) {
      status = napi_get_named_property(env, result, codec->name, &decEnc);
      CHECK_STATUS;
    } else {
      status = napi_create_object(env, &decEnc);
      CHECK_STATUS;
      status = napi_set_named_property(env, result, codec->name, decEnc);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, decEnc,
      av_codec_is_decoder(codec) ? "decoder" : "encoder", value);
    CHECK_STATUS;

    codec = av_codec_iterate(&opaque);
  }

  return result;
}

napi_value decoders(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  void* opaque = nullptr;
  const AVCodec* codec;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  codec = av_codec_iterate(&opaque);
  while (codec != nullptr) {
    if (av_codec_is_decoder(codec)) {
      status = fromAVCodec(env, codec, &value);
      CHECK_STATUS;
      status = napi_set_named_property(env, result, codec->name, value);
      CHECK_STATUS;
    }

    codec = av_codec_iterate(&opaque);
  }

  return result;
}

napi_value encoders(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  void* opaque = nullptr;
  const AVCodec* codec;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  codec = av_codec_iterate(&opaque);
  while (codec != nullptr) {
    if (av_codec_is_encoder(codec)) {
      status = fromAVCodec(env, codec, &value);
      CHECK_STATUS;
      status = napi_set_named_property(env, result, codec->name, value);
      CHECK_STATUS;
    }

    codec = av_codec_iterate(&opaque);
  }

  return result;
}

napi_value pix_fmts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, pixfmt, flags, comps, comp;
  const AVPixFmtDescriptor* desc = nullptr;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  while ((desc = av_pix_fmt_desc_next(desc))) {
    status = napi_create_object(env, &pixfmt);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, pixfmt, "name", (char*) desc->name);
    CHECK_STATUS;
    status = beam_set_int32(env, pixfmt, "nb_components", desc->nb_components);
    CHECK_STATUS;
    status = beam_set_uint32(env, pixfmt, "log2_chroma_h", desc->log2_chroma_h);
    CHECK_STATUS;
    status = beam_set_uint32(env, pixfmt, "log2_chroma_w", desc->log2_chroma_w);
    CHECK_STATUS;
    status = napi_create_object(env, &flags);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "BE", desc->flags & AV_PIX_FMT_FLAG_BE);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "PAL", desc->flags & AV_PIX_FMT_FLAG_PAL);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "BITSTREAM", desc->flags & AV_PIX_FMT_FLAG_BITSTREAM);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "HWACCEL", desc->flags & AV_PIX_FMT_FLAG_HWACCEL);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "PLANAR", desc->flags & AV_PIX_FMT_FLAG_PLANAR);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "RGB", desc->flags & AV_PIX_FMT_FLAG_RGB);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "PSEUDOPAL", desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "ALPHA", desc->flags & AV_PIX_FMT_FLAG_ALPHA);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "BAYER", desc->flags & AV_PIX_FMT_FLAG_BAYER);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "FLOAT", desc->flags & AV_PIX_FMT_FLAG_FLOAT);
    CHECK_STATUS;
    status = napi_set_named_property(env, pixfmt, "flags", flags);
    CHECK_STATUS;
    status = napi_create_array(env, &comps);
    CHECK_STATUS;
    for ( int x = 0 ; x < desc->nb_components ; x++ ) {
      status = napi_create_object(env, &comp);
      CHECK_STATUS;
      char letter[] = { 'X', '\0' };
      switch(x) {
        case 0:
          if (desc->nb_components < 3) { letter[0] = 'Y'; break; }
          letter[0] = (desc->flags & AV_PIX_FMT_FLAG_RGB) ? 'R' : 'Y';
          break;
        case 1:
          if (desc->nb_components < 3) { letter[0] = 'A'; break; }
          letter[0] = (desc->flags & AV_PIX_FMT_FLAG_RGB) ? 'G' : 'U';
          break;
        case 2:
          letter[0] = (desc->flags & AV_PIX_FMT_FLAG_RGB) ? 'B' : 'V';
          break;
        case 3:
          letter[0] = 'A';
          break;
        default:
          break;
      }
      status = beam_set_string_utf8(env, comp, "code", letter);
      CHECK_STATUS;

      status = beam_set_int32(env, comp, "plane", desc->comp[x].plane);
      CHECK_STATUS;
      status = beam_set_int32(env, comp, "step", desc->comp[x].step);
      CHECK_STATUS;
      status = beam_set_int32(env, comp, "offset", desc->comp[x].offset);
      CHECK_STATUS;
      status = beam_set_int32(env, comp, "shift", desc->comp[x].shift);
      CHECK_STATUS;
      status = beam_set_int32(env, comp, "depth", desc->comp[x].depth);
      CHECK_STATUS;

      status = napi_set_element(env, comps, x, comp);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, pixfmt, "comp", comps);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, pixfmt, "alias",
      const_cast<char*>((desc->alias != nullptr) ? desc->alias : ""));
    CHECK_STATUS;
    status = napi_set_named_property(env, result, desc->name, pixfmt);
    CHECK_STATUS;
  }

  return result;
}

napi_value protocols(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, inputs, outputs, element;
  void* opaque = nullptr;
  int pos = 0;
  const char* protocol;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = napi_create_array(env, &inputs);
  CHECK_STATUS;
  status = napi_create_array(env, &outputs);
  CHECK_STATUS;

  while ((protocol = avio_enum_protocols(&opaque, 0))) {
    status = napi_create_string_utf8(env, (char*) protocol, NAPI_AUTO_LENGTH, &element);
    CHECK_STATUS;
    status = napi_set_element(env, inputs, pos++, element);
    CHECK_STATUS;
  }

  opaque = nullptr;
  pos = 0;
  while ((protocol = avio_enum_protocols(&opaque, 1))) {
    status = napi_create_string_utf8(env, (char*) protocol, NAPI_AUTO_LENGTH, &element);
    CHECK_STATUS;
    status = napi_set_element(env, outputs, pos++, element);
    CHECK_STATUS;
  }

  status = napi_set_named_property(env, result, "inputs", inputs);
  CHECK_STATUS;
  status = napi_set_named_property(env, result, "outputs", outputs);
  CHECK_STATUS;

  return result;
}

napi_value filters(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, desc, flags, pads, pad;
  void* opaque = nullptr;
  const AVFilter* filter;
  int padCount;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  while ((filter = av_filter_iterate(&opaque))) {
    status = napi_create_object(env, &desc);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, desc, "name", (char*) filter->name);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, desc, "description", (char*) filter->description);
    CHECK_STATUS;

    status = napi_create_array(env, &pads);
    CHECK_STATUS;
    padCount = avfilter_pad_count(filter->inputs);
    for ( int x = 0 ; x < padCount ; x++ ) {
      status = napi_create_object(env, &pad);
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "name",
        (char*) avfilter_pad_get_name(filter->inputs, x));
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "media_type",
        (char*) av_get_media_type_string(avfilter_pad_get_type(filter->inputs, x)));
      CHECK_STATUS;
      status = napi_set_element(env, pads, x, pad);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, desc, "inputs", pads);
    CHECK_STATUS;

    status = napi_create_array(env, &pads);
    CHECK_STATUS;
    padCount = avfilter_pad_count(filter->outputs);
    for ( int x = 0 ; x < padCount ; x++ ) {
      status = napi_create_object(env, &pad);
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "name",
        (char*) avfilter_pad_get_name(filter->outputs, x));
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "media_type",
        (char*) av_get_media_type_string(avfilter_pad_get_type(filter->outputs, x)));
      CHECK_STATUS;
      status = napi_set_element(env, pads, x, pad);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, desc, "outputs", pads);
    CHECK_STATUS;

    // TODO expand class details
    if (filter->priv_class != nullptr) {
      status = fromAVClass(env, filter->priv_class, &pad);
      CHECK_STATUS;
    } else {
      status = napi_get_null(env, &pad);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, desc, "priv_class", pad);
    CHECK_STATUS;

    status = napi_create_object(env, &flags);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "DYNAMIC_INPUTS",
      filter->flags & AVFILTER_FLAG_DYNAMIC_INPUTS);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "DYNAMIC_OUTPUTS",
      filter->flags & AVFILTER_FLAG_DYNAMIC_OUTPUTS);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "SLICE_THREADS",
      filter->flags & AVFILTER_FLAG_SLICE_THREADS);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "SUPPORT_TIMELINE_GENERIC",
      filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC);
    CHECK_STATUS;
    status = beam_set_bool(env, flags, "SUPPORT_TIMELINE_INTERNAL",
      filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL);
    CHECK_STATUS;
    status = napi_set_named_property(env, desc, "flags", flags);
    CHECK_STATUS;

    status = napi_set_named_property(env, result, filter->name, desc);
    CHECK_STATUS;
  }

  return result;
}

napi_value bsfs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, desc, codecs, codec;
  void* opaque = nullptr;
  const AVBitStreamFilter* bsf;
  const AVCodecID* codecID;
  int pos = 0;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  while ((bsf = av_bsf_iterate(&opaque))) {
    status = napi_create_object(env, &desc);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, desc, "name", (char*) bsf->name);
    CHECK_STATUS;

    status = napi_create_array(env, &codecs);
    CHECK_STATUS;
    codecID = bsf->codec_ids;
    if (codecID != nullptr) {
      pos = 0;
      while (*codecID != AV_CODEC_ID_NONE) {
        status = napi_create_string_utf8(env,
          (char*) avcodec_get_name(*codecID), NAPI_AUTO_LENGTH, &codec);
        CHECK_STATUS;
        status = napi_set_element(env, codecs, pos++, codec);
        CHECK_STATUS;
        codecID = codecID + 1;
      }
    }
    status = napi_set_named_property(env, desc, "codec_ids", codecs);
    CHECK_STATUS;

    // TODO expand class details
    if (bsf->priv_class != nullptr) {
      status = fromAVClass(env, bsf->priv_class, &codec);
      CHECK_STATUS;
    } else {
      status = napi_get_null(env, &codec);
      CHECK_STATUS;
    }
    status = napi_set_named_property(env, desc, "priv_class", codec);
    CHECK_STATUS;

    status = napi_set_named_property(env, result, bsf->name, desc);
    CHECK_STATUS;
  }

  return result;
}

napi_value sampleFormats(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  int count = 0;
  const char* name;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  while ((name = av_get_sample_fmt_name((AVSampleFormat) count))) {
    status = napi_create_object(env, &value);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, value, "type", "SampleFormat");
    CHECK_STATUS;
    status = beam_set_string_utf8(env, value, "name", (char*) name);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, value, "packed",
      (char*) av_get_sample_fmt_name(av_get_packed_sample_fmt((AVSampleFormat) count)));
    CHECK_STATUS;
    status = beam_set_string_utf8(env, value, "planar",
      (char*) av_get_sample_fmt_name(av_get_planar_sample_fmt((AVSampleFormat) count)));
    CHECK_STATUS;
    status = beam_set_int32(env, value, "bytes_per_sample",
      av_get_bytes_per_sample((AVSampleFormat) count));
    CHECK_STATUS;
    status = beam_set_bool(env, value, "is_planar",
      av_sample_fmt_is_planar((AVSampleFormat) count));
    CHECK_STATUS;
    status = napi_set_named_property(env, result, name, value);
    CHECK_STATUS;
    count++;
  }

  return result;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value padSize, noopts;
  status = napi_create_int32(env, AV_INPUT_BUFFER_PADDING_SIZE, &padSize);
  CHECK_STATUS;
  status = napi_create_int64(env, AV_NOPTS_VALUE, &noopts);
  CHECK_STATUS;

  napi_property_descriptor desc[] = {
    DECLARE_NAPI_METHOD("versions", versions),
    DECLARE_NAPI_METHOD("avVersionInfo", avVersionInfo),
    DECLARE_NAPI_METHOD("versionStrings", versionStrings),
    DECLARE_NAPI_METHOD("configurations", configurations),
    DECLARE_NAPI_METHOD("licenses", licenses),
    DECLARE_NAPI_METHOD("logging", logging),
    DECLARE_NAPI_METHOD("governor", governor),
    DECLARE_NAPI_METHOD("format", makeFormat),
    DECLARE_NAPI_METHOD("decoder", decoder),
    DECLARE_NAPI_METHOD("filterer", filterer), // 10
    DECLARE_NAPI_METHOD("encoder", encoder),
    DECLARE_NAPI_METHOD("codecs", codecs),
    DECLARE_NAPI_METHOD("decoders", decoders),
    DECLARE_NAPI_METHOD("encoders", encoders),
    DECLARE_NAPI_METHOD("muxers", muxers),
    DECLARE_NAPI_METHOD("demuxers", demuxers),
    DECLARE_NAPI_METHOD("pix_fmts", pix_fmts),
    DECLARE_NAPI_METHOD("sample_fmts", sampleFormats),
    DECLARE_NAPI_METHOD("protocols", protocols),
    DECLARE_NAPI_METHOD("filters", filters), // 20
    DECLARE_NAPI_METHOD("bsfs", bsfs),
    DECLARE_NAPI_METHOD("packet", makePacket),
    DECLARE_NAPI_METHOD("frame", makeFrame),
    DECLARE_NAPI_METHOD("codecParameters", makeCodecParameters),
    DECLARE_NAPI_METHOD("demuxer", demuxer),
    DECLARE_NAPI_METHOD("muxer", muxer),
    DECLARE_NAPI_METHOD("guessFormat", guessFormat),
    { "AV_INPUT_BUFFER_PADDING_SIZE", nullptr, nullptr, nullptr, nullptr,
      padSize, napi_enumerable, nullptr },
    { "AV_NOPTS_VALUE", nullptr, nullptr, nullptr, nullptr,
      noopts, napi_enumerable, nullptr }
  };
  status = napi_define_properties(env, exports, 29, desc);
  CHECK_STATUS;

  avdevice_register_all();
  avformat_network_init();

  // Iterate over all codecs to makes sure they are registered
  void* opaque = nullptr;
  const AVCodec* codec;
  do {
    codec = av_codec_iterate(&opaque);
    // printf("Registered '%s'\n", (codec != nullptr) ? codec->name : "(null)");
  } while (codec != nullptr);
  return exports;
}

NAPI_MODULE(beamcoder, Init)

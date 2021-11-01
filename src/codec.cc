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

#include "codec.h"
#include "hwcontext.h"

napi_value getCodecCtxCodecID(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->codec_id, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, codec->codec->name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxLongName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, codec->codec->long_name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxCodecTag(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  char codecTag[AV_FOURCC_MAX_STRING_SIZE];

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  av_fourcc_make_string(codecTag, codec->codec_tag);
  if (strchr(codecTag, '[')) { // not a recognised tag
    status = napi_create_uint32(env, codec->codec_tag, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, codecTag, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if ((codec->codec->priv_class == nullptr) || (codec->priv_data == nullptr)) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
    return result;
  }

  status = fromContextPrivData(env, codec->priv_data, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;

  if ((codec->codec->priv_class == nullptr) || (codec->priv_data == nullptr)) {
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

  status = toContextPrivData(env, args[0], codec->priv_data);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int64(env, codec->bit_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBitRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the bit_rate property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the bit_rate property.");
  }

  status = napi_get_value_int64(env, args[0], &codec->bit_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBitRateTol(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->bit_rate_tolerance, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBitRateTol(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the bit_rate_tolerance property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the bit_rate_tolerance property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->bit_rate_tolerance);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxGlobalQ(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->global_quality, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxGlobalQ(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the global_quality property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the global_quality property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->global_quality);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxCompLvl(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->compression_level, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxCompLvl(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the compression_level property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the compression_level property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->compression_level);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  /**
   * Allow decoders to produce frames with data planes that are not aligned
   * to CPU requirements (e.g. due to cropping).
   */
  status = beam_set_bool(env, result, "UNALIGNED", (codec->flags & AV_CODEC_FLAG_UNALIGNED) != 0);
  CHECK_STATUS;
  /**
   * Use fixed qscale.
   */
  status = beam_set_bool(env, result, "QSCALE", (codec->flags & AV_CODEC_FLAG_QSCALE) != 0);
  CHECK_STATUS;
  /**
   * 4 MV per MB allowed / advanced prediction for H.263.
   */
  status = beam_set_bool(env, result, "4MV", (codec->flags & AV_CODEC_FLAG_4MV) != 0);
  CHECK_STATUS;
  /**
   * Output even those frames that might be corrupted.
   */
  status = beam_set_bool(env, result, "OUTPUT_CORRUPT", (codec->flags & AV_CODEC_FLAG_OUTPUT_CORRUPT) != 0);
  CHECK_STATUS;
  /**
   * Use qpel MC.
   */
  status = beam_set_bool(env, result, "QPEL", (codec->flags & AV_CODEC_FLAG_QPEL) != 0);
  CHECK_STATUS;
  /**
   * Use internal 2pass ratecontrol in first pass mode.
   */
  status = beam_set_bool(env, result, "PASS1", (codec->flags & AV_CODEC_FLAG_PASS1) != 0);
  CHECK_STATUS;
  /**
   * Use internal 2pass ratecontrol in second pass mode.
   */
  status = beam_set_bool(env, result, "PASS2", (codec->flags & AV_CODEC_FLAG_PASS2) != 0);
  CHECK_STATUS;
  /**
   * loop filter.
   */
  status = beam_set_bool(env, result, "LOOP_FILTER", (codec->flags & AV_CODEC_FLAG_LOOP_FILTER) != 0);
  CHECK_STATUS;
  /**
   * Only decode/encode grayscale.
   */
  status = beam_set_bool(env, result, "GRAY", (codec->flags & AV_CODEC_FLAG_GRAY) != 0);
  CHECK_STATUS;
  /**
   * error[?] variables will be set during encoding.
   */
  status = beam_set_bool(env, result, "PSNR", (codec->flags & AV_CODEC_FLAG_PSNR) != 0);
  CHECK_STATUS;
  /**
   * Input bitstream might be truncated at a random location
   * instead of only at frame boundaries.
   */
  status = beam_set_bool(env, result, "TRUNCATED", (codec->flags & AV_CODEC_FLAG_TRUNCATED) != 0);
  CHECK_STATUS;
  /**
   * Use interlaced DCT.
   */
  status = beam_set_bool(env, result, "INTERLACED_DCT", (codec->flags & AV_CODEC_FLAG_INTERLACED_DCT) != 0);
  CHECK_STATUS;
  /**
   * Force low delay.
   */
  status = beam_set_bool(env, result, "LOW_DELAY", (codec->flags & AV_CODEC_FLAG_LOW_DELAY) != 0);
  CHECK_STATUS;
  /**
   * Place global headers in extradata instead of every keyframe.
   */
  status = beam_set_bool(env, result, "GLOBAL_HEADER", (codec->flags & AV_CODEC_FLAG_GLOBAL_HEADER) != 0);
  CHECK_STATUS;
  /**
   * Use only bitexact stuff (except (I)DCT).
   */
  status = beam_set_bool(env, result, "BITEXACT", (codec->flags & AV_CODEC_FLAG_BITEXACT) != 0);
  CHECK_STATUS;
  /* Fx : Flag for H.263+ extra options */
  /**
   * H.263 advanced intra coding / MPEG-4 AC prediction
   */
  status = beam_set_bool(env, result, "AC_PRED", (codec->flags & AV_CODEC_FLAG_AC_PRED) != 0);
  CHECK_STATUS;
  /**
   * interlaced motion estimation
   */
  status = beam_set_bool(env, result, "INTERLACED_ME", (codec->flags & AV_CODEC_FLAG_INTERLACED_ME) != 0);
  CHECK_STATUS;

  status = beam_set_bool(env, result, "CLOSED_GOP", (codec->flags & AV_CODEC_FLAG_CLOSED_GOP) != 0);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  napi_valuetype type;
  AVCodecContext* codec;
  bool isArray, present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the flags property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the flags property.");
  }

  value = args[0];
  /**
   * Allow decoders to produce frames with data planes that are not aligned
   * to CPU requirements (e.g. due to cropping).
   */
  status = beam_get_bool(env, value, "UNALIGNED", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_UNALIGNED :
    codec->flags & ~AV_CODEC_FLAG_UNALIGNED; }
  /**
   * Use fixed qscale.
   */
  status = beam_get_bool(env, value, "QSCALE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_QSCALE :
    codec->flags & ~AV_CODEC_FLAG_QSCALE; }
  /**
   * 4 MV per MB allowed / advanced prediction for H.263.
   */
  status = beam_get_bool(env, value, "4MV", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_4MV :
    codec->flags & ~AV_CODEC_FLAG_4MV; }
  /**
   * Output even those frames that might be corrupted.
   */
  status = beam_get_bool(env, value, "OUTPUT_CORRUPT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_OUTPUT_CORRUPT :
    codec->flags & ~AV_CODEC_FLAG_OUTPUT_CORRUPT; }
  /**
   * Use qpel MC.
   */
  status = beam_get_bool(env, value, "QPEL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_QPEL :
    codec->flags & ~AV_CODEC_FLAG_QPEL; }
  /**
   * Use internal 2pass ratecontrol in first pass mode.
   */
  status = beam_get_bool(env, value, "PASS1", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_PASS1 :
    codec->flags & ~AV_CODEC_FLAG_PASS1; }
  /**
   * Use internal 2pass ratecontrol in second pass mode.
   */
  status = beam_get_bool(env, value, "PASS2", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_PASS2 :
    codec->flags & ~AV_CODEC_FLAG_PASS2; }
  /**
   * loop filter.
   */
  status = beam_get_bool(env, value, "LOOP_FILTER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_LOOP_FILTER :
    codec->flags & ~AV_CODEC_FLAG_LOOP_FILTER; }
  /**
   * Only decode/encode grayscale.
   */
  status = beam_get_bool(env, value, "GRAY", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_GRAY :
    codec->flags & ~AV_CODEC_FLAG_GRAY; }
  /**
   * error[?] variables will be set during encoding.
   */
  status = beam_get_bool(env, value, "PSNR", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_PSNR:
    codec->flags & ~AV_CODEC_FLAG_PSNR; }
  /**
   * Input bitstream might be truncated at a random location
   * instead of only at frame boundaries.
   */
  status = beam_get_bool(env, value, "TRUNCATED", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_TRUNCATED :
    codec->flags & ~AV_CODEC_FLAG_TRUNCATED; }
  /**
   * Use interlaced DCT.
   */
  status = beam_get_bool(env, value, "INTERLACED_DCT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_INTERLACED_DCT :
    codec->flags & ~AV_CODEC_FLAG_INTERLACED_DCT; }
  /**
   * Force low delay.
   */
  status = beam_get_bool(env, value, "LOW_DELAY", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_LOW_DELAY :
    codec->flags & ~AV_CODEC_FLAG_LOW_DELAY; }
  /**
   * Place global headers in extradata instead of every keyframe.
   */
  status = beam_get_bool(env, value, "GLOBAL_HEADER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_GLOBAL_HEADER :
    codec->flags & ~AV_CODEC_FLAG_GLOBAL_HEADER; }
  /**
   * Use only bitexact stuff (except (I)DCT).
   */
  status = beam_get_bool(env, value, "BITEXACT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_BITEXACT :
    codec->flags & ~AV_CODEC_FLAG_BITEXACT; }
  /* Fx : Flag for H.263+ extra options */
  /**
   * H.263 advanced intra coding / MPEG-4 AC prediction
   */
  status = beam_get_bool(env, value, "AC_PRED", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_AC_PRED :
    codec->flags & ~AV_CODEC_FLAG_AC_PRED; }
  /**
   * interlaced motion estimation
   */
  status = beam_get_bool(env, value, "INTERLACED_ME", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_INTERLACED_ME :
    codec->flags & ~AV_CODEC_FLAG_INTERLACED_ME; }

  status = beam_get_bool(env, value, "CLOSED_GOP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags = (flag) ?
    codec->flags | AV_CODEC_FLAG_CLOSED_GOP :
    codec->flags & ~AV_CODEC_FLAG_CLOSED_GOP; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxFlags2(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  /**
   * Allow non spec compliant speedup tricks.
   */
  status = beam_set_bool(env, result, "FAST", (codec->flags2 & AV_CODEC_FLAG2_FAST) != 0);
  CHECK_STATUS;
  /**
   * Skip bitstream encoding.
   */
  status = beam_set_bool(env, result, "NO_OUTPUT", (codec->flags2 & AV_CODEC_FLAG2_NO_OUTPUT) != 0);
  CHECK_STATUS;
  /**
   * Place global headers at every keyframe instead of in extradata.
   */
  status = beam_set_bool(env, result, "LOCAL_HEADER", (codec->flags2 & AV_CODEC_FLAG2_LOCAL_HEADER) != 0);
  CHECK_STATUS;

  /**
   * timecode is in drop frame format. DEPRECATED!!!!
   */
  status = beam_set_bool(env, result, "DROP_FRAME_TIMECODE", (codec->flags2 & AV_CODEC_FLAG2_DROP_FRAME_TIMECODE) != 0);
  CHECK_STATUS;

  /**
   * Input bitstream might be truncated at a packet boundaries
   * instead of only at frame boundaries.
   */
  status = beam_set_bool(env, result, "CHUNKS", (codec->flags2 & AV_CODEC_FLAG2_CHUNKS) != 0);
  CHECK_STATUS;
  /**
   * Discard cropping information from SPS.
   */
  status = beam_set_bool(env, result, "IGNORE_CROP", (codec->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) != 0);
  CHECK_STATUS;

  /**
   * Show all frames before the first keyframe
   */
  status = beam_set_bool(env, result, "SHOW_ALL", (codec->flags2 & AV_CODEC_FLAG2_SHOW_ALL) != 0);
  CHECK_STATUS;
  /**
   * Export motion vectors through frame side data
   */
  status = beam_set_bool(env, result, "EXPORT_MVS", (codec->flags2 & AV_CODEC_FLAG2_EXPORT_MVS) != 0);
  CHECK_STATUS;
  /**
   * Do not skip samples and export skip information as frame side data
   */
  status = beam_set_bool(env, result, "SKIP_MANUAL", (codec->flags2 & AV_CODEC_FLAG2_SKIP_MANUAL) != 0);
  CHECK_STATUS;
  /**
   * Do not reset ASS ReadOrder field on flush (resulttitles decoding)
   */
  status = beam_set_bool(env, result, "RO_FLUSH_NOOP", (codec->flags2 & AV_CODEC_FLAG2_RO_FLUSH_NOOP) != 0);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxFlags2(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  napi_valuetype type;
  AVCodecContext* codec;
  bool isArray, present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the flags2 property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the flags2 property.");
  }

  value = args[0];
  status = beam_get_bool(env, value, "FAST", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_FAST :
    codec->flags2 & ~AV_CODEC_FLAG2_FAST; }
  /**
   * Skip bitstream encoding.
   */
  status = beam_get_bool(env, value, "NO_OUTPUT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_NO_OUTPUT :
    codec->flags2 & ~AV_CODEC_FLAG2_NO_OUTPUT; }
  /**
   * Place global headers at every keyframe instead of in extradata.
   */
  status = beam_get_bool(env, value, "LOCAL_HEADER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_LOCAL_HEADER :
    codec->flags2 & ~AV_CODEC_FLAG2_LOCAL_HEADER; }
  /**
   * timecode is in drop frame format. DEPRECATED!!!!
   */
  status = beam_get_bool(env, value, "DROP_FRAME_TIMECODE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_DROP_FRAME_TIMECODE :
    codec->flags2 & ~AV_CODEC_FLAG2_DROP_FRAME_TIMECODE; }
  /**
   * Input bitstream might be truncated at a packet boundaries
   * instead of only at frame boundaries.
   */
  status = beam_get_bool(env, value, "CHUNKS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_CHUNKS :
    codec->flags2 & ~AV_CODEC_FLAG2_CHUNKS; }
  /**
   * Discard cropping information from SPS.
   */
  status = beam_get_bool(env, value, "IGNORE_CROP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_IGNORE_CROP :
    codec->flags2 & ~AV_CODEC_FLAG2_IGNORE_CROP; }
  /**
   * Show all frames before the first keyframe
   */
  status = beam_get_bool(env, value, "SHOW_ALL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_SHOW_ALL :
    codec->flags2 & ~AV_CODEC_FLAG2_SHOW_ALL; }
  /**
   * Export motion vectors through frame side data
   */
  status = beam_get_bool(env, value, "EXPORT_MVS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_EXPORT_MVS :
    codec->flags2 & ~AV_CODEC_FLAG2_EXPORT_MVS; }
  /**
   * Do not skip samples and export skip information as frame side data
   */
  status = beam_get_bool(env, value, "SKIP_MANUAL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_SKIP_MANUAL :
    codec->flags2 & ~AV_CODEC_FLAG2_SKIP_MANUAL; }
  /**
   * Do not reset ASS ReadOrder field on flush (subtitles decoding)
   */
  status = beam_get_bool(env, value, "RO_FLUSH_NOOP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->flags2 = (flag) ?
    codec->flags2 | AV_CODEC_FLAG2_RO_FLUSH_NOOP :
    codec->flags2 & ~AV_CODEC_FLAG2_RO_FLUSH_NOOP; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxExtraData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  void* resultData;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->extradata_size > 0) {
    status = napi_create_buffer_copy(env, codec->extradata_size, codec->extradata,
      &resultData, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxExtraData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  bool isBuffer;
  AVCodecContext* codec;
  uint8_t* data;
  size_t dataLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the extradata property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type == napi_null) {
    if (codec->extradata_size > 0) { // Tidy up old buffers
      av_freep(&codec->extradata);
    }
    codec->extradata_size = 0;
    goto done;
  }
  status = napi_is_buffer(env, args[0], &isBuffer);
  CHECK_STATUS;
  if (!isBuffer) {
    NAPI_THROW_ERROR("A buffer is required to set the extradata property.");
  }

  status = napi_get_buffer_info(env, args[0], (void**) &data, &dataLen);
  CHECK_STATUS;
  if (codec->extradata_size > 0) { // Tidy up old buffers
    av_freep(&codec->extradata);
    codec->extradata_size = 0;
  }
  codec->extradata = (uint8_t*) av_mallocz(dataLen + AV_INPUT_BUFFER_PADDING_SIZE);
  codec->extradata_size = dataLen;
  memcpy(codec->extradata, data, dataLen);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxTimeBase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->time_base.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->time_base.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxTimeBase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the time_base property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of two numbers is required to set the time_base property.");
  // }
  for ( int x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the time_base property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the time_base property.");
    }
  }

  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->time_base.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->time_base.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxTicksPerFrame(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->ticks_per_frame, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxTicksPerFrame(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the ticks_per_frame property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the ticks_per_frame property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->ticks_per_frame);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDelay(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->delay, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->width, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the width property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the width property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->width);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->height, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the height property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the height property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->height);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxCodedWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->coded_width, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxCodedWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the coded_width property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the coded_width property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->coded_width);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxCodedHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->coded_height, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxCodedHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the coded_height property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the coded_height property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->coded_height);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxGopSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->gop_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxGopSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the gop_size property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the gop_size property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->gop_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* pixFmtName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  pixFmtName = av_get_pix_fmt_name(codec->pix_fmt);
  if (pixFmtName != nullptr) {
    status = napi_create_string_utf8(env, (char*) pixFmtName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  AVPixelFormat pixFmt;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the pix_fmt property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->pix_fmt = AV_PIX_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the pix_fmt property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  pixFmt = av_get_pix_fmt((const char *) name);
  free(name);
  CHECK_STATUS;
  if (pixFmt != AV_PIX_FMT_NONE) {
    codec->pix_fmt = pixFmt;
  } else {
    NAPI_THROW_ERROR("Pixel format name is not known.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMaxBFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->max_b_frames, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMaxBFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the max_b_frames property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the max_b_frames property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->max_b_frames);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBQuantFactor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->b_quant_factor, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBQuantFactor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the b_quant_factor property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the b_quant_factor property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->b_quant_factor);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBQuantOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->b_quant_offset, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBQuantOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the b_quant_offset property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the b_quant_offset property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->b_quant_offset);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxHasBFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->has_b_frames, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxIQuantFactor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->i_quant_factor, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxIQuantFactor(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the i_quant_factor property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the i_quant_factor property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->i_quant_factor);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxIQuantOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->i_quant_offset, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxIQuantOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the i_quant_offset property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the i_quant_offset property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->i_quant_factor);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxLumiMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->lumi_masking, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxLumiMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the lumi_masking property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the lumi_masking property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->lumi_masking);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxTempCplxMask(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->temporal_cplx_masking, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxTempCplxMask(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the temporal_cplx_masking property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the temporal_cplx_masking property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->temporal_cplx_masking);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSpatCplxMask(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->spatial_cplx_masking, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSpatCplxMask(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the spatial_cplx_masking property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the spatial_cplx_masking property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->spatial_cplx_masking);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxPMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->p_masking, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxPMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the p_masking property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the p_masking property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->p_masking);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDarkMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->dark_masking, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxDarkMasking(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the dark_masking property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the dark_masking property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->dark_masking);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSliceCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->slice_count, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSliceCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the slice_count property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the slice_count property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->slice_count);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSliceOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if ((codec->slice_count > 0) && (codec->slice_offset != nullptr)) {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < codec->slice_count ; x++ ) {
      status = napi_create_int32(env, codec->slice_offset[x], &element);
      CHECK_STATUS;
      status = napi_set_element(env, result, x, element);
      CHECK_STATUS;
    }
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxSliceOffset(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  bool isArray;
  AVCodecContext* codec;
  int sliceCount;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the slice_offset property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->slice_count = 0;
    codec->slice_offset = nullptr;
    goto done;
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (!isArray) {
    napi_value propNames;
    status = napi_get_property_names(env, args[0], &propNames);
    CHECK_STATUS;
    status = napi_get_array_length(env, propNames, (uint32_t*) &sliceCount);
    CHECK_STATUS;
  } else {
    status = napi_get_array_length(env, args[0], (uint32_t*) &sliceCount);
    CHECK_STATUS;
  }
  for ( int x = 0 ; x < sliceCount ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of numbers is required to set the slice_offset property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("Only values of number type can be used to set the slice_offset property.");
    }
  }

  codec->slice_count = sliceCount;
  codec->slice_offset = (int*) av_malloc(sizeof(int) * sliceCount);
  for ( int x = 0 ; x < sliceCount ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    CHECK_STATUS;
    status = napi_get_value_int32(env, element, &codec->slice_offset[x]);
    CHECK_STATUS;
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSampleAspRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->sample_aspect_ratio.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->sample_aspect_ratio.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSampleAspRt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sample_aspect_ratio property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of two numbers is required to set the sample_aspect_ratio property.");
  // }
  for ( int x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the sample_aspect_ratio property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("All array elements must be numbers to set the sample_aspect_ratio property.");
    }
  }

  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->sample_aspect_ratio.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->sample_aspect_ratio.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMeCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_cmp->forward, codec->me_cmp), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMeCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the me_cmp (motion estimation comparison function) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the me_cmp (motion estimation comparison function) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_cmp->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->me_cmp = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for me_cmp (motion estimation comparison function).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMeSubCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_cmp->forward, codec->me_sub_cmp), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMeSubCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the me_sub_cmp (subpixel motion estimation comparison function) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the me_sub_cmp (subpixel motion estimation comparison function) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_cmp->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->me_sub_cmp = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for me_subcmp (subpixelmotion estimation comparison function).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMbCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_cmp->forward, codec->mb_cmp), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMbCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the mb_cmp (macroblock comparison function) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the mb_cmp (macroblock comparison function) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_cmp->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->mb_cmp = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for mb_cmp (macroblock comparison function).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxIldctCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_cmp->forward, codec->ildct_cmp), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxIldctCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the ildct_cmp (interlaced DCT comparison function) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the ildct_cmp (interlaced DCT comparison function) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_cmp->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->ildct_cmp = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for ildct_cmp (interlaced DCT comparison function).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDiaSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->dia_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxDiaSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the dia_size property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the dia_size property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->dia_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxLastPredCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->last_predictor_count, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxLastPredCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the last_predictor_count property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the last_predictor_count property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->last_predictor_count);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMePreCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_cmp->forward, codec->me_pre_cmp), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMePreCmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the me_pre_cmp (motion estimation prepass comparison function) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the me_pre_cmp (motion estimation prepass comparison function) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_cmp->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->me_pre_cmp = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for me_pre_cmp (motion estimation prepass comparison function).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxPreDiaSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->pre_dia_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxPreDiaSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the pre_dia_size property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the pre_dia_size property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->pre_dia_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMeSubpelQual(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->me_subpel_quality, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMeSubpelQual(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the me_subpel_quality property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the me_subpel_quality property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->me_subpel_quality);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMeRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->me_range, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMeRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the me_range property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the me_range property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->me_range);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSliceFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  // draw_horiz_band() is called in coded order instead of display
  status = beam_set_bool(env, result, "CODED_ORDER", (codec->slice_flags & SLICE_FLAG_CODED_ORDER) != 0);
  CHECK_STATUS;
  // allow draw_horiz_band() with field slices (MPEG-2 field pics)
  status = beam_set_bool(env, result, "ALLOW_FIELD", (codec->slice_flags & SLICE_FLAG_ALLOW_FIELD) != 0);
  CHECK_STATUS;
  // allow draw_horiz_band() with 1 component at a time (SVQ1)
  status = beam_set_bool(env, result, "ALLOW_PLANE", (codec->slice_flags & SLICE_FLAG_ALLOW_PLANE) != 0);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSliceFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  napi_valuetype type;
  AVCodecContext* codec;
  bool isArray, present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the slice_flags property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (isArray || (type != napi_object)) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the slice_flags property.");
  }

  value = args[0];
  // draw_horiz_band() is called in coded order instead of display
  status = beam_get_bool(env, value, "CODED_ORDER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->slice_flags = (flag) ?
    codec->slice_flags | SLICE_FLAG_CODED_ORDER :
    codec->slice_flags & ~SLICE_FLAG_CODED_ORDER; }
  // allow draw_horiz_band() with field slices (MPEG-2 field pics)
  status = beam_get_bool(env, value, "ALLOW_FIELD", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->slice_flags = (flag) ?
    codec->slice_flags | SLICE_FLAG_ALLOW_FIELD :
    codec->slice_flags & ~SLICE_FLAG_ALLOW_FIELD; }
  // allow draw_horiz_band() with 1 component at a time (SVQ1)
  status = beam_get_bool(env, value, "ALLOW_PLANE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->slice_flags = (flag) ?
    codec->slice_flags | SLICE_FLAG_ALLOW_PLANE :
    codec->slice_flags & ~SLICE_FLAG_ALLOW_PLANE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMbDecision(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_mb_decision->forward, codec->mb_decision), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMbDecision(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the mb_decision (macroblock decision mode) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the mb_decision (macroblock decision mode) property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_mb_decision->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->mb_decision = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown function for mb_decision (macroblock decision mode).");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxIntraMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->intra_matrix != nullptr) {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < 64 ; x++ ) {
      status = napi_create_uint32(env, codec->intra_matrix[x], &element);
      CHECK_STATUS;
      status = napi_set_element(env, result, x, element);
      CHECK_STATUS;
    }
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxIntraMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;
  uint32_t uThirtwo;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the intra_matrix property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->intra_matrix = nullptr;
    goto done;
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of numbers is required to set the intra_matrix property.");
  // }
  codec->intra_matrix = (uint16_t*) av_mallocz(sizeof(uint16_t) * 64);
  for ( int x = 0 ; x < 64 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of numbers is required to set the intra_matrix property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type == napi_number) {
      status = napi_get_value_uint32(env, element, &uThirtwo);
      CHECK_STATUS;
      codec->intra_matrix[x] = (uint16_t) uThirtwo;
    } else {
      codec->intra_matrix[x] = 0;
    }
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxInterMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->inter_matrix != nullptr) {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < 64 ; x++ ) {
      status = napi_create_uint32(env, codec->inter_matrix[x], &element);
      CHECK_STATUS;
      status = napi_set_element(env, result, x, element);
      CHECK_STATUS;
    }
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxInterMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;
  uint32_t uThirtwo;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the inter_matrix property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->intra_matrix = nullptr;
    goto done;
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of numbers is required to set the inter_matrix property.");
  // }
  codec->inter_matrix = (uint16_t*) av_mallocz(sizeof(uint16_t) * 64);
  for ( int x = 0 ; x < 64 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of 64 numbers is required to set the inter_matrix property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type == napi_number) {
      status = napi_get_value_uint32(env, element, &uThirtwo);
      CHECK_STATUS;
      codec->inter_matrix[x] = (uint16_t) uThirtwo;
    } else {
      codec->inter_matrix[x] = 0;
    }
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxIntraDCProv(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->intra_dc_precision, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxIntraDCProv(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the intra_dc_precision property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the intra_dc_precision property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->intra_dc_precision);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSkipTop(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->skip_top, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipTop(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_top property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the skip_top property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->skip_top);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSkipBottom(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->skip_bottom, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipBottom(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_bottom property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the skip_bottom property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->skip_bottom);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMbLmin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->mb_lmin, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMbLmin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the mb_lmin property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the mb_lmin property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->mb_lmin);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMbLmax(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->mb_lmax, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMbLmax(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the mb_lmax property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the mb_lmax property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->mb_lmax);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBidirRefine(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->bidir_refine, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBidirRefine(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the bidir_refine property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the bidir_refine property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->bidir_refine);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxKeyIntMin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->keyint_min, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxKeyIntMin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the keyint_min property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the keyint_min property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->keyint_min);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxKeyRefs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->refs, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxKeyRefs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the refs (nunber of reference frames) property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the refs (nunber of reference frames) property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->refs);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMv0Threshold(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->mv0_threshold, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMv0Threshold(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the mv0_threshold property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the mv0_threshold property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->mv0_threshold);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxColorPrim(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* colPrimName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  colPrimName = av_color_primaries_name(codec->color_primaries);
  status = napi_create_string_utf8(env,
    (colPrimName != nullptr) ? (char*) colPrimName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxColorPrim(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int colPrim;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the color_primaries property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->color_primaries = AVCOL_PRI_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the color_primaries property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  colPrim = av_color_primaries_from_name((const char *) name);
  free(name);
  CHECK_STATUS;
  if (colPrim >= 0) {
    codec->color_primaries = (AVColorPrimaries) colPrim;
  } else {
    NAPI_THROW_ERROR(avErrorMsg("Color primaries name is not known: ", colPrim));
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxColorTrc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* colTrcName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  colTrcName = av_color_transfer_name(codec->color_trc);
  status = napi_create_string_utf8(env,
    (colTrcName != nullptr) ? (char*) colTrcName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxColorTrc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int colTrc;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the color_trc property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->color_trc = AVCOL_TRC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the color_trc property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  colTrc = av_color_transfer_from_name((const char *) name);
  free(name);
  CHECK_STATUS;
  if (colTrc >= 0) {
    codec->color_trc = (AVColorTransferCharacteristic) colTrc;
  } else {
    NAPI_THROW_ERROR(avErrorMsg("Color transfer characteristic name is not known: ", colTrc));
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxColorspace(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* colspaceName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  colspaceName = av_color_space_name(codec->colorspace);
  status = napi_create_string_utf8(env,
    (colspaceName != nullptr) ? (char*) colspaceName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxColorspace(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int colspace;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the colorspace property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->colorspace = AVCOL_SPC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the colorspace property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  colspace= av_color_space_from_name((const char *) name);
  free(name);
  CHECK_STATUS;
  if (colspace >= 0) {
    codec->colorspace = (AVColorSpace) colspace;
  } else {
    NAPI_THROW_ERROR(avErrorMsg("Colorspace name is not known: ", colspace));
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxColorRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* colRangeName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  colRangeName = av_color_range_name(codec->color_range);
  status = napi_create_string_utf8(env,
    (colRangeName != nullptr) ? (char*) colRangeName : "unknown",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxColorRange(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int colrange;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the color_range property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->color_range = AVCOL_RANGE_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the color_range property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  colrange = av_color_range_from_name((const char *) name);
  free(name);
  CHECK_STATUS;
  if (colrange >= 0) {
    codec->color_range = (AVColorRange) colrange;
  } else {
    NAPI_THROW_ERROR(avErrorMsg("Color range name is not known: ", colrange));
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxChromaLoc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* chromaLocName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  chromaLocName = av_chroma_location_name(codec->chroma_sample_location);
  status = napi_create_string_utf8(env,
    (chromaLocName != nullptr) ? (char*) chromaLocName : "unspecified",
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxChromaLoc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int chromaLoc;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the chroma_sample_location property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->chroma_sample_location = AVCHROMA_LOC_UNSPECIFIED;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the chroma_sample_location property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  chromaLoc = av_chroma_location_from_name((const char *) name);
  free(name);
  CHECK_STATUS;
  if (chromaLoc >= 0) {
    codec->chroma_sample_location = (AVChromaLocation) chromaLoc;
  } else {
    NAPI_THROW_ERROR(avErrorMsg("Chroma location name is not known: ", chromaLoc));
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxFieldOrder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_field_order->forward, codec->field_order),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxFieldOrder(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the field_order property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the field_order property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_field_order->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->field_order = (AVFieldOrder) enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for field_order. Did you mean e.g. 'progressive'?");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSlices(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->slices, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSlices(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the slices property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the slices property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->slices);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSampleRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->sample_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSampleRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sample_rate property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the sample_rate property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->sample_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxChannels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->channels, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxChannels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the channels property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the channels property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->channels);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSampleFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* sampleFmtName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  sampleFmtName = av_get_sample_fmt_name(codec->sample_fmt);
  if (sampleFmtName != nullptr) {
    status = napi_create_string_utf8(env, (char*) sampleFmtName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxSampleFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  AVSampleFormat sampleFmt;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sample_fmt property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->sample_fmt = AV_SAMPLE_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the sample_fmt property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  sampleFmt = av_get_sample_fmt((const char *) name);
  free(name);
  CHECK_STATUS;
  if (sampleFmt != AV_SAMPLE_FMT_NONE) {
    codec->sample_fmt = sampleFmt;
  } else {
    NAPI_THROW_ERROR("Sample format name is not known.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxFrameSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->frame_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxFrameNb(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->frame_number, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxBlockAlign(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->block_align, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBlockAlign(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the block_align property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the block_align property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->block_align);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxCutoff(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->cutoff, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxCutoff(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the cutoff property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the cutoff property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->cutoff);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  char channelLayoutName[64];

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  av_get_channel_layout_string(channelLayoutName, 64, 0,
    codec->channel_layout ? codec->channel_layout : av_get_default_channel_layout(codec->channels));
  status = napi_create_string_utf8(env, channelLayoutName, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  uint64_t chanLay;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the channel_layout property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->channel_layout = 0;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the channel_layout property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  chanLay = av_get_channel_layout(name);
  free(name);
  if (chanLay != 0) {
    codec->channel_layout = chanLay;
    codec->channels = av_get_channel_layout_nb_channels(chanLay);
  } else {
    NAPI_THROW_ERROR("Channel layout name is not recognized. Set 'null' for '0 channels'.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxReqChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  char channelLayoutName[64];

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if (codec->request_channel_layout) {
    av_get_channel_layout_string(channelLayoutName, 64, 0, codec->request_channel_layout);
    status = napi_create_string_utf8(env, channelLayoutName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, "default", NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxReqChanLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  uint64_t chanLay;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the request_channel_layout property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->request_channel_layout = 0;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the request_channel_layout property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  chanLay = av_get_channel_layout(name);
  free(name);
  if (chanLay != 0) {
    codec->request_channel_layout = chanLay;
  } else {
    NAPI_THROW_ERROR("Request channel layout name is not recognized. Set 'null' for '0 channels'.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxAudioSvcType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_av_audio_service_type->forward, codec->audio_service_type),
      NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxAudioSvcType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the audio_service_type property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the audio_service_type property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_av_audio_service_type->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->audio_service_type = (AVAudioServiceType) enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for audio_service_type. Default is 'main'.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxReqSampleFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* sampleFmtName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  sampleFmtName = av_get_sample_fmt_name(codec->request_sample_fmt);
  if (sampleFmtName != nullptr) {
    status = napi_create_string_utf8(env, (char*) sampleFmtName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxReqSampleFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  AVSampleFormat sampleFmt;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the request_sample_fmt property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->request_sample_fmt = AV_SAMPLE_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the request_sample_fmt property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;


  sampleFmt = av_get_sample_fmt((const char *) name);
  free(name);
  CHECK_STATUS;
  if (sampleFmt != AV_SAMPLE_FMT_NONE) {
    codec->request_sample_fmt = sampleFmt;
  } else {
    NAPI_THROW_ERROR("Request sample format name is not known.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxQCompress(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->qcompress, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxQCompress(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the qcompress property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the qcompress property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->qcompress);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxQBlur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, codec->qblur, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxQBlur(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the qblur property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the qblur property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->qblur);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxQMin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->qmin, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxQMin(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the qmin property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the qmin property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->qmin);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxQMax(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->qmax, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxQMax(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the qmax property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the qmax property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->qmax);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcBufSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->rc_buffer_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcBufSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_buffer_size property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_buffer_size property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->rc_buffer_size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMaxQDiff(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->max_qdiff, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMaxQDiff(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the max_qdiff property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the max_qdiff property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->max_qdiff);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcOverride(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;
  for ( int x = 0 ; x < codec->rc_override_count ; x++ ) {
    status = napi_create_object(env, &element);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, element, "type", "RcOverride");
    CHECK_STATUS;
    status = beam_set_int32(env, element, "start_frame", codec->rc_override[x].start_frame);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "end_frame", codec->rc_override[x].end_frame);
    CHECK_STATUS;
    status = beam_set_int32(env, element, "qscale", codec->rc_override[x].qscale);
    CHECK_STATUS;
    status = beam_set_double(env, element, "quality_factor", codec->rc_override[x].quality_factor);
    CHECK_STATUS;
    status = napi_set_element(env, result, x, element);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxRcOverride(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  bool isArray;
  AVCodecContext* codec;
  RcOverride* list;
  uint32_t count;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_override property.");
  }
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;
  if (!isArray) {
    napi_value propNames;
    status = napi_get_property_names(env, args[0], &propNames);
    CHECK_STATUS;
    status = napi_get_array_length(env, propNames, &count);
    CHECK_STATUS;
    status = napi_get_element(env, args[0], 0, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of RcOverride values is required to set the rc_override property.");
    }
  } else {
    status = napi_get_array_length(env, args[0], &count);
    CHECK_STATUS;
  }  
  if (count == 0) {
    if (codec->rc_override != nullptr) {
      av_freep(&codec->rc_override);
    }
    codec->rc_override_count = 0;
    goto done;
  }
  list = (RcOverride*) av_malloc(sizeof(struct RcOverride) * count);
  for ( int x = 0 ; x < (int32_t) count ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    CHECK_STATUS;
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    status = napi_is_array(env, element, &isArray);
    CHECK_STATUS;
    if (isArray || (type != napi_object)) {
      av_freep(&list);
      NAPI_THROW_ERROR("An RcOverride value can only be set with an object.");
    }
    status = beam_get_int32(env, element, "start_frame", &list[x].start_frame);
    CHECK_STATUS;
    status = beam_get_int32(env, element, "end_frame", &list[x].end_frame);
    CHECK_STATUS;
    status = beam_get_int32(env, element, "qscale", &list[x].qscale);
    CHECK_STATUS;
    status = beam_get_double(env, element, "quality_factor", (double*) &list[x].quality_factor);
    CHECK_STATUS;
  }
  av_freep(&codec->rc_override);
  codec->rc_override = list;
  codec->rc_override_count = count;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcMaxRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int64(env, codec->rc_max_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcMaxRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_max_rate property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_max_rate property.");
  }

  status = napi_get_value_int64(env, args[0], &codec->rc_max_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcMinRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int64(env, codec->rc_min_rate, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcMinRate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_min_rate property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_min_rate property.");
  }

  status = napi_get_value_int64(env, args[0], &codec->rc_min_rate);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcMaxAvailVbvUse(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, (double) codec->rc_max_available_vbv_use, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcMaxAvailVbvUse(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_max_available_vbv_use property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_max_available_vbv_use property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->rc_max_available_vbv_use);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcMinVbvOverflowUse(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_double(env, (double) codec->rc_min_vbv_overflow_use, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcMinVbvOverflowUse(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_min_vbv_overflow_use property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_min_vbv_overflow_use property.");
  }

  status = napi_get_value_double(env, args[0], (double*) &codec->rc_min_vbv_overflow_use);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxRcInitBufOc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->rc_initial_buffer_occupancy, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxRcInitBufOc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the rc_initial_buffer_occupancy property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the rc_initial_buffer_occupancy property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->rc_initial_buffer_occupancy);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxTrellis(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->trellis, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxTrellis(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the trellis property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the trellis property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->trellis);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxStatsOut(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->stats_out == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, codec->stats_out, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxStatsIn(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->stats_in == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, codec->stats_in, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxStatsIn(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* stats;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the stats_in property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (codec->stats_in != nullptr) {
      av_freep(&codec->stats_in);
    }
    codec->stats_in = nullptr;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the stats_in property.");
  }

  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  stats = (char*) av_malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], stats, strLen + 1, &strLen);
  CHECK_STATUS;

  if (codec->stats_in != nullptr) {
    av_freep(&codec->stats_in);
  }
  codec->stats_in = stats;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxWorkaround(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "AUTODETECT", codec->workaround_bugs & FF_BUG_AUTODETECT);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "XVID_ILACE", codec->workaround_bugs & FF_BUG_XVID_ILACE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "UMP4", codec->workaround_bugs & FF_BUG_UMP4);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "NO_PADDING", codec->workaround_bugs & FF_BUG_NO_PADDING);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "AMV", codec->workaround_bugs & FF_BUG_AMV);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "QPEL_CHROMA", codec->workaround_bugs & FF_BUG_QPEL_CHROMA);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "STD_QPEL", codec->workaround_bugs & FF_BUG_STD_QPEL);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "QPEL_CHROMA2", codec->workaround_bugs & FF_BUG_QPEL_CHROMA2);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DIRECT_BLOCKSIZE", codec->workaround_bugs & FF_BUG_DIRECT_BLOCKSIZE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "EDGE", codec->workaround_bugs & FF_BUG_EDGE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "HPEL_CHROMA", codec->workaround_bugs & FF_BUG_HPEL_CHROMA);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DC_CLIP", codec->workaround_bugs & FF_BUG_DC_CLIP);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "MS", codec->workaround_bugs & FF_BUG_MS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "TRUNCATED", codec->workaround_bugs & FF_BUG_TRUNCATED);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "IEDGE", codec->workaround_bugs & FF_BUG_IEDGE);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxWorkaround(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the workaround_bugs property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the workaround_bugs property.");
  }
  status = beam_get_bool(env, args[0], "AUTODETECT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_AUTODETECT :
    codec->workaround_bugs & ~FF_BUG_AUTODETECT; }
  status = beam_get_bool(env, args[0], "XVID_ILACE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_XVID_ILACE :
    codec->workaround_bugs & ~FF_BUG_XVID_ILACE; }
  status = beam_get_bool(env, args[0], "UMP4", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_UMP4 :
    codec->workaround_bugs & ~FF_BUG_UMP4; }
  status = beam_get_bool(env, args[0], "NO_PADDING", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_NO_PADDING :
    codec->workaround_bugs & ~FF_BUG_NO_PADDING; }
  status = beam_get_bool(env, args[0], "AMV", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_AMV :
    codec->workaround_bugs & ~FF_BUG_AMV; }
  status = beam_get_bool(env, args[0], "QPEL_CHROMA", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_QPEL_CHROMA :
    codec->workaround_bugs & ~FF_BUG_QPEL_CHROMA; }
  status = beam_get_bool(env, args[0], "STD_QPEL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_STD_QPEL :
    codec->workaround_bugs & ~FF_BUG_STD_QPEL; }
  status = beam_get_bool(env, args[0], "QPEL_CHROMA2", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_QPEL_CHROMA2 :
    codec->workaround_bugs & ~FF_BUG_QPEL_CHROMA2; }
  status = beam_get_bool(env, args[0], "DIRECT_BLOCKSIZE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_DIRECT_BLOCKSIZE :
    codec->workaround_bugs & ~FF_BUG_DIRECT_BLOCKSIZE; }
  status = beam_get_bool(env, args[0], "EDGE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_EDGE :
    codec->workaround_bugs & ~FF_BUG_EDGE; }
  status = beam_get_bool(env, args[0], "HPEL_CHROMA", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_HPEL_CHROMA :
    codec->workaround_bugs & ~FF_BUG_HPEL_CHROMA; }
  status = beam_get_bool(env, args[0], "DC_CLIP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_DC_CLIP :
    codec->workaround_bugs & ~FF_BUG_DC_CLIP; }
  status = beam_get_bool(env, args[0], "MS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_MS :
    codec->workaround_bugs & ~FF_BUG_MS; }
  status = beam_get_bool(env, args[0], "TRUNCATED", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_TRUNCATED :
    codec->workaround_bugs & ~FF_BUG_TRUNCATED; }
  status = beam_get_bool(env, args[0], "IEDGE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->workaround_bugs = (flag) ?
    codec->workaround_bugs | FF_BUG_IEDGE :
    codec->workaround_bugs & ~FF_BUG_IEDGE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxStrictStdComp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_compliance->forward, codec->strict_std_compliance),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxStrictStdComp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
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
    codec->strict_std_compliance = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for strict_std_compliance.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxErrConceal(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "GUESS_MVS",
    codec->error_concealment & FF_EC_GUESS_MVS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DEBLOCK",
    codec->error_concealment & FF_EC_DEBLOCK);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "FAVOR_INTER",
    codec->error_concealment & FF_EC_FAVOR_INTER);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxErrConceal(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the error_concealment property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the error_concealment property.");
  }
  status = beam_get_bool(env, args[0], "GUESS_MVS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->error_concealment = (flag) ?
    codec->error_concealment | FF_EC_GUESS_MVS :
    codec->error_concealment & ~FF_EC_GUESS_MVS; }
  status = beam_get_bool(env, args[0], "DEBLOCK", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->error_concealment = (flag) ?
    codec->error_concealment | FF_EC_DEBLOCK :
    codec->error_concealment & ~FF_EC_DEBLOCK; }
  status = beam_get_bool(env, args[0], "FAVOR_INTER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->error_concealment = (flag) ?
    codec->error_concealment | FF_EC_FAVOR_INTER :
    codec->error_concealment & ~FF_EC_FAVOR_INTER; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDebug(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "PICT_INFO", codec->debug & FF_DEBUG_PICT_INFO);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "RC", codec->debug & FF_DEBUG_RC);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "BITSTREAM", codec->debug & FF_DEBUG_BITSTREAM);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "MB_TYPE", codec->debug & FF_DEBUG_MB_TYPE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "QP", codec->debug & FF_DEBUG_QP);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DCT_COEFF", codec->debug & FF_DEBUG_DCT_COEFF);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SKIP", codec->debug & FF_DEBUG_SKIP);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "STARTCODE", codec->debug & FF_DEBUG_STARTCODE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "ER", codec->debug & FF_DEBUG_ER);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "MMCO", codec->debug & FF_DEBUG_MMCO);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "BUGS", codec->debug & FF_DEBUG_BUGS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "BUFFERS", codec->debug & FF_DEBUG_BUFFERS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "THREADS", codec->debug & FF_DEBUG_THREADS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "GREEN_MD", codec->debug & FF_DEBUG_GREEN_MD);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "NOMC", codec->debug & FF_DEBUG_NOMC);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxDebug(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the debug property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the debug property.");
  }
  status = beam_get_bool(env, args[0], "PICT_INFO", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_PICT_INFO :
    codec->debug & ~FF_DEBUG_PICT_INFO; }
  status = beam_get_bool(env, args[0], "RC", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_RC :
    codec->debug & ~FF_DEBUG_RC; }
  status = beam_get_bool(env, args[0], "BITSTREAM", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_BITSTREAM :
    codec->debug & ~FF_DEBUG_BITSTREAM; }
  status = beam_get_bool(env, args[0], "MB_TYPE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_MB_TYPE :
    codec->debug & ~FF_DEBUG_MB_TYPE; }
  status = beam_get_bool(env, args[0], "QP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_QP :
    codec->debug & ~FF_DEBUG_QP; }
  status = beam_get_bool(env, args[0], "DCT_COEFF", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_DCT_COEFF :
    codec->debug & ~FF_DEBUG_DCT_COEFF; }
  status = beam_get_bool(env, args[0], "SKIP", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_SKIP :
    codec->debug & ~FF_DEBUG_SKIP; }
  status = beam_get_bool(env, args[0], "STARTCODE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_STARTCODE :
    codec->debug & ~FF_DEBUG_STARTCODE; }
  status = beam_get_bool(env, args[0], "ER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_ER :
    codec->debug & ~FF_DEBUG_ER; }
  status = beam_get_bool(env, args[0], "MMCO", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_MMCO :
    codec->debug & ~FF_DEBUG_MMCO; }
  status = beam_get_bool(env, args[0], "BUGS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_BUGS :
    codec->debug & ~FF_DEBUG_BUGS; }
  status = beam_get_bool(env, args[0], "BUFFERS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_BUFFERS :
    codec->debug & ~FF_DEBUG_BUFFERS; }
  status = beam_get_bool(env, args[0], "THREADS", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_THREADS :
    codec->debug & ~FF_DEBUG_THREADS; }
  status = beam_get_bool(env, args[0], "GREEN_MD", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_GREEN_MD :
    codec->debug & ~FF_DEBUG_GREEN_MD; }
  status = beam_get_bool(env, args[0], "NOMC", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->debug = (flag) ?
    codec->debug | FF_DEBUG_NOMC :
    codec->debug & ~FF_DEBUG_NOMC; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxErrRecog(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CRCCHECK", codec->err_recognition & AV_EF_CRCCHECK);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "BITSTREAM", codec->err_recognition & AV_EF_BITSTREAM);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "BUFFER", codec->err_recognition & AV_EF_BUFFER);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "EXPLODE", codec->err_recognition & AV_EF_EXPLODE);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "IGNORE_ERR", codec->err_recognition & AV_EF_IGNORE_ERR);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CAREFUL", codec->err_recognition & AV_EF_CAREFUL);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "COMPLIANT", codec->err_recognition & AV_EF_COMPLIANT);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "AGGRESSIVE", codec->err_recognition & AV_EF_AGGRESSIVE);

  return result;
}

napi_value setCodecCtxErrRecog(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the error_recognition property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the error_recognition property.");
  }
  status = beam_get_bool(env, args[0], "CRCCHECK", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_CRCCHECK :
    codec->err_recognition & ~AV_EF_CRCCHECK; }
  status = beam_get_bool(env, args[0], "BITSTREAM", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_BITSTREAM :
    codec->err_recognition & ~AV_EF_BITSTREAM; }
  status = beam_get_bool(env, args[0], "BUFFER", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_BUFFER :
    codec->err_recognition & ~AV_EF_BUFFER; }
  status = beam_get_bool(env, args[0], "EXPLODE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_EXPLODE :
    codec->err_recognition & ~AV_EF_EXPLODE; }

  status = beam_get_bool(env, args[0], "IGNORE_ERR", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_IGNORE_ERR :
    codec->err_recognition & ~AV_EF_IGNORE_ERR; }
  status = beam_get_bool(env, args[0], "CAREFUL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_CAREFUL :
    codec->err_recognition & ~AV_EF_CAREFUL; }
  status = beam_get_bool(env, args[0], "COMPLIANT", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_COMPLIANT :
    codec->err_recognition & ~AV_EF_COMPLIANT; }
  status = beam_get_bool(env, args[0], "AGGRESSIVE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->err_recognition = (flag) ?
    codec->err_recognition | AV_EF_AGGRESSIVE :
    codec->err_recognition & ~AV_EF_AGGRESSIVE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxReorderOpaq(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int64(env, codec->reordered_opaque, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxReorderOpaq(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the reordered_opaque property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the reordered_opaque property.");
  }

  status = napi_get_value_int64(env, args[0], &codec->reordered_opaque);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxError(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if (codec->flags & AV_CODEC_FLAG_PSNR) {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < AV_NUM_DATA_POINTERS ; x++ ) {
      // TOOD move to uint64_t big int support when finalized for Node
      status = napi_create_int64(env, codec->error[x], &element);
      CHECK_STATUS;
      status = napi_set_element(env, result, x, element);
      CHECK_STATUS;
    }
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxDctAlgo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_dct->forward, codec->dct_algo),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxDctAlgo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the dct_algo property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the dct_algo property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_dct->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->dct_algo = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for dct_algo.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxIdctAlgo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_idct->forward, codec->idct_algo),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxIdctAlgo(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the idct_algo property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the idct_algo property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_ff_idct->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->idct_algo = enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for idct_algo.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBitsPCodedSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->bits_per_coded_sample, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBitsPCodedSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the bits_per_coded_sample property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the bits_per_coded_sample property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->bits_per_coded_sample);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxBitsPRawSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->bits_per_raw_sample, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxBitsPRawSmp(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the bits_per_raw_sample property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the bits_per_raw_sample property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->bits_per_raw_sample);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxThreadCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->thread_count, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxThreadCount(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the thread_count property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the thread_count property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->thread_count);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxThreadType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "FRAME", (codec->thread_type & FF_THREAD_FRAME) == FF_THREAD_FRAME);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SLICE", (codec->thread_type & FF_THREAD_SLICE) == FF_THREAD_SLICE);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxThreadType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the thread_type property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the thread_type property.");
  }
  status = beam_get_bool(env, args[0], "FRAME", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->thread_type = (flag) ?
    codec->thread_type | FF_THREAD_FRAME :
    codec->thread_type & ~FF_THREAD_FRAME; }
  status = beam_get_bool(env, args[0], "SLICE", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->thread_type = (flag) ?
    codec->thread_type | FF_THREAD_SLICE :
    codec->thread_type & ~FF_THREAD_SLICE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxActThreadType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "FRAME", (codec->active_thread_type & FF_THREAD_FRAME) == FF_THREAD_FRAME);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SLICE", (codec->active_thread_type & FF_THREAD_SLICE) == FF_THREAD_SLICE);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxNsseWeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->nsse_weight, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxNsseWeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the nsse_weight property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the nsse_weight property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->nsse_weight);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxProfile(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  const char* profileName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  profileName = av_get_profile_name(codec->codec, codec->profile);
  if (profileName != nullptr) {
    status = napi_create_string_utf8(env,
      (char*) profileName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_int32(env, codec->profile, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxProfile(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  const AVProfile* profile;
  bool foundProfile = false;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the profilet property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->profile = FF_PROFILE_UNKNOWN;
    goto done;
  }
  if (type == napi_number) {
    status = napi_get_value_int32(env, args[0], &codec->profile);
    CHECK_STATUS;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the profile property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;

  profile = codec->codec->profiles;
  if (!profile) {
    printf("Failed to set codec profile \'%s\' - recognised profiles not available for codec \'%s\'.\n", name, codec->codec->name);
    printf("Set profile as a numeric value to work around this problem.\n");
    codec->profile = FF_PROFILE_UNKNOWN;
  } else {
    while (profile->profile != FF_PROFILE_UNKNOWN) {
      if (strcmp(name, profile->name) == 0) {
        codec->profile = profile->profile;
        foundProfile = true;
        break;
      }
      profile = profile + 1;
    }

    if (!foundProfile) {
      codec->profile = FF_PROFILE_UNKNOWN;
      printf("Failed to find codec profile \'%s\' in recognised profiles.\n", name);
    }
  }
  free(name);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxLevel(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->level, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxLevel(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the level property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->level = FF_LEVEL_UNKNOWN;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the level property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->level);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSkipLpFilter(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avdiscard->forward, codec->skip_loop_filter),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipLpFilter(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_loop_filter property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the skip_loop_filter property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_avdiscard->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->skip_loop_filter = (AVDiscard) enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for skip_loop_filter.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSkipIdct(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avdiscard->forward, codec->skip_idct),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipIdct(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_idct property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the skip_idct property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_avdiscard->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->skip_idct = (AVDiscard) enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for skip_idct.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSkipFrame(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_avdiscard->forward, codec->skip_frame),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipFrame(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* name;
  size_t strLen;
  int enumValue;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_frame property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the skip_frame property.");
  }
  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  name = (char*) malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], name, strLen + 1, &strLen);
  CHECK_STATUS;
  enumValue = beam_lookup_enum(beam_avdiscard->inverse, name);
  free(name);
  if (enumValue != BEAM_ENUM_UNKNOWN) {
    codec->skip_frame = (AVDiscard) enumValue;
  } else {
    NAPI_THROW_ERROR("Unknown value for skip_frame.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSubtitleHdr(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  void* resultData;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->extradata_size > 0) {
    status = napi_create_buffer_copy(env, codec->subtitle_header_size,
      codec->subtitle_header, &resultData, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxSubtitleHdr(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  bool isBuffer;
  AVCodecContext* codec;
  uint8_t* data;
  size_t dataLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the subtitle_header property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type == napi_null) {
    if (codec->subtitle_header_size > 0) { // Tidy up old buffers
      av_freep(&codec->subtitle_header);
    }
    codec->subtitle_header_size = 0;
    goto done;
  }
  status = napi_is_buffer(env, args[0], &isBuffer);
  CHECK_STATUS;
  if (!isBuffer) {
    NAPI_THROW_ERROR("A buffer is required to set the subtitle_header property.");
  }

  status = napi_get_buffer_info(env, args[0], (void**) &data, &dataLen);
  CHECK_STATUS;
  if (codec->subtitle_header_size > 0) { // Tidy up old buffers
    av_freep(&codec->subtitle_header);
    codec->subtitle_header_size = 0;
  }
  codec->subtitle_header = (uint8_t*) av_mallocz(dataLen + AV_INPUT_BUFFER_PADDING_SIZE);
  codec->subtitle_header_size = dataLen;
  memcpy(codec->subtitle_header, data, dataLen);

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxInitPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->initial_padding, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxFramerate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->framerate.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->framerate.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxFramerate(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the framerate property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of two numbers is required to set the framerate property.");
  // }
  for ( int x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the framerate property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the framerate property.");
    }
  }

  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->framerate.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->framerate.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSwPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;
  AVBufferRef* hwFramesContextRef;
  AVHWFramesContext* hwFramesContext = nullptr;
  AVPixelFormat sw_pix_fmt;
  const char* pixFmtName;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  hwFramesContextRef = codec->hw_frames_ctx;
  if (hwFramesContextRef)
    hwFramesContext = (AVHWFramesContext*)hwFramesContextRef->data;
  sw_pix_fmt = hwFramesContext ? hwFramesContext->sw_format : codec->sw_pix_fmt;
  pixFmtName = av_get_pix_fmt_name(sw_pix_fmt);
  if (pixFmtName != nullptr) {
    status = napi_create_string_utf8(env, pixFmtName, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxPktTimebase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_array(env, &result);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->pkt_timebase.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->pkt_timebase.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, result, 1, element);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxPktTimebase(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the pkt_timebase property.");
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of two numbers is required to set the pkt_timebase property.");
  // }
  for ( int x = 0 ; x < 2 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the pkt_timebase property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type != napi_number) {
      NAPI_THROW_ERROR("An array of two numbers is required to set the pkt_timebase property.");
    }
  }

  status = napi_get_element(env, args[0], 0, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->pkt_timebase.num);
  CHECK_STATUS;
  status = napi_get_element(env, args[0], 1, &element);
  CHECK_STATUS;
  status = napi_get_value_int32(env, element, &codec->pkt_timebase.den);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDesc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->codec_descriptor != nullptr) {
    status = fromAVCodecDescriptor(env, codec->codec_descriptor, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxSubCharenc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->sub_charenc == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, codec->sub_charenc, NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxSubCharenc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* subchar;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sub_charenc property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (codec->sub_charenc != nullptr) {
      av_freep(&codec->sub_charenc);
    }
    codec->sub_charenc = nullptr;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the sub_charenc property.");
  }

  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  subchar = (char*) av_malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], subchar, strLen + 1, &strLen);
  CHECK_STATUS;

  if (codec->sub_charenc != nullptr) {
    av_freep(&codec->sub_charenc);
  }
  codec->sub_charenc = subchar;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSubCharencMode(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_string_utf8(env,
    beam_lookup_name(beam_ff_sub_charenc_mode->forward, codec->sub_charenc_mode),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxSkipAlpha(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->skip_alpha, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSkipAlpha(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the skip_alpha property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the skip_alpha property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->skip_alpha);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSeekPreroll(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->seek_preroll, &result);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxChromaIntraMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->chroma_intra_matrix != nullptr) {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < 64 ; x++ ) {
      status = napi_create_uint32(env, codec->chroma_intra_matrix[x], &element);
      CHECK_STATUS;
      status = napi_set_element(env, result, x, element);
      CHECK_STATUS;
    }
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxChromaIntraMatrix(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  napi_valuetype type;
  // bool isArray;
  AVCodecContext* codec;
  uint32_t uThirtwo;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the chroma_intra_matrix property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    codec->chroma_intra_matrix = nullptr;
    goto done;
  }
  // status = napi_is_array(env, args[0], &isArray);
  // CHECK_STATUS;
  // if (!isArray) {
  //   NAPI_THROW_ERROR("An array of 64 numbers is required to set the chroma_intra_matrix property.");
  // }
  codec->chroma_intra_matrix = (uint16_t*) av_mallocz(sizeof(uint16_t) * 64);
  for ( int x = 0 ; x < 64 ; x++ ) {
    status = napi_get_element(env, args[0], x, &element);
    if (status != napi_ok) {
      NAPI_THROW_ERROR("An array of 64 numbers is required to set the chroma_intra_matrix property.");
    }
    status = napi_typeof(env, element, &type);
    CHECK_STATUS;
    if (type == napi_number) {
      status = napi_get_value_uint32(env, element, &uThirtwo);
      CHECK_STATUS;
      codec->chroma_intra_matrix[x] = (uint16_t) uThirtwo;
    } else {
      codec->chroma_intra_matrix[x] = 0;
    }
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxDumpSep(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->dump_separator != nullptr) {
    status = napi_create_string_utf8(env, (char*) codec->dump_separator,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxDumpSep(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  uint8_t* dumpy;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the dump_separator property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (codec->dump_separator != nullptr) {
      av_freep(&codec->dump_separator);
    }
    codec->dump_separator = nullptr;
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

  if (codec->dump_separator != nullptr) {
    av_freep(&codec->dump_separator);
  }
  codec->dump_separator = dumpy;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (codec->codec_whitelist == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_string_utf8(env, codec->codec_whitelist,
      NAPI_AUTO_LENGTH, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecCtxWhitelist(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  char* whity;
  size_t strLen;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the codec_whitelist property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (codec->codec_whitelist != nullptr) {
      av_freep(&codec->codec_whitelist);
    }
    codec->codec_whitelist = nullptr;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the codec_whitelist property.");
  }

  status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
  CHECK_STATUS;
  whity = (char*) av_malloc(sizeof(char) * (strLen + 1));
  status = napi_get_value_string_utf8(env, args[0], whity, strLen + 1, &strLen);
  CHECK_STATUS;

  if (codec->codec_whitelist != nullptr) {
    av_freep(&codec->codec_whitelist);
  }
  codec->codec_whitelist = whity;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxProps(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "LOSSLESS", codec->properties & FF_CODEC_PROPERTY_LOSSLESS);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CLOSED_CAPTIONS", codec->properties & FF_CODEC_PROPERTY_CLOSED_CAPTIONS);
  CHECK_STATUS;

  return result;
}

napi_value getCodecCtxCodedSideData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVCodecContext* codec;
  void* resultData;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if (codec->nb_coded_side_data <= 0) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_object(env, &result);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, result, "type", "PacketSideData");
    for ( int x = 0 ; x < codec->nb_coded_side_data ; x++ ) {
      status = napi_create_buffer_copy(env, codec->coded_side_data[x].size,
        codec->coded_side_data[x].data, &resultData, &element);
      CHECK_STATUS;
      status = napi_set_named_property(env, result,
        beam_lookup_name(beam_packet_side_data_type->forward,
          codec->coded_side_data[x].type), element);
      CHECK_STATUS;
    }
  }

  return result;
}

napi_value getCodecHWFramesCtx(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if (codec->hw_frames_ctx == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = fromHWFramesContext(env, codec->hw_frames_ctx, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value setCodecHWFramesCtx(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, contextExt;
  napi_valuetype type;
  AVCodecContext* codec;
  AVBufferRef* contextRef;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the hw_frames_context property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object is required to set the hw_frames_context property.");
  }
  status = napi_get_named_property(env, args[0], "_framesContext", &contextExt);
  CHECK_STATUS;
  status = napi_get_value_external(env, contextExt, (void**) &contextRef);
  CHECK_STATUS;
  codec->hw_frames_ctx = av_buffer_ref(contextRef);

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxSubTextFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->sub_text_format, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxSubTextFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sub_text_format property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the sub_text_format property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->sub_text_format);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxTrailPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->trailing_padding, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxTrailPad(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the trailing_padding property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the trailing_padding property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->trailing_padding);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxMaxPixels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int64(env, codec->max_pixels, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxMaxPixels(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the max_pixels property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the max_pixels property.");
  }

  status = napi_get_value_int64(env, args[0], &codec->max_pixels);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecHWDeviceCtx(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  if (codec->hw_device_ctx == nullptr) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = fromHWDeviceContext(env, codec->hw_device_ctx, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getCodecCtxHwAccelFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "IGNORE_LEVEL",
    codec->hwaccel_flags & AV_HWACCEL_FLAG_IGNORE_LEVEL);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "ALLOW_HIGH_DEPTH",
    codec->hwaccel_flags & AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "ALLOW_PROFILE_MISMATCH",
    codec->hwaccel_flags & AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxHwAccelFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the hwaccel_flags property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("An object of Boolean-valued flags is required to set the hwaccel_flags property.");
  }
  status = beam_get_bool(env, args[0], "IGNORE_LEVEL", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->hwaccel_flags = (flag) ?
    codec->hwaccel_flags | AV_HWACCEL_FLAG_IGNORE_LEVEL :
    codec->hwaccel_flags & ~AV_HWACCEL_FLAG_IGNORE_LEVEL; }
  status = beam_get_bool(env, args[0], "ALLOW_HIGH_DEPTH", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->hwaccel_flags = (flag) ?
    codec->hwaccel_flags | AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH :
    codec->hwaccel_flags & ~AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH; }
  status = beam_get_bool(env, args[0], "ALLOW_PROFILE_MISMATCH", &present, &flag);
  CHECK_STATUS;
  if (present) { codec->hwaccel_flags = (flag) ?
    codec->hwaccel_flags | AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH :
    codec->hwaccel_flags & ~AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxApplyCrop(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->apply_cropping, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxApplyCrop(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the apply_cropping property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the apply_cropping property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->apply_cropping);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getCodecCtxExtraHwFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVCodecContext* codec;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, (void**) &codec);
  CHECK_STATUS;
  status = napi_create_int32(env, codec->extra_hw_frames, &result);
  CHECK_STATUS;

  return result;
}

napi_value setCodecCtxExtraHwFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVCodecContext* codec;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &codec);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the extra_hw_frames property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the extra_hw_frames property.");
  }

  status = napi_get_value_int32(env, args[0], &codec->extra_hw_frames);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value failDecoding(napi_env env, napi_callback_info info) {
  NAPI_THROW_ERROR("Cannot set property when decoding.");
}

napi_value failEncoding(napi_env env, napi_callback_info info) {
  NAPI_THROW_ERROR("Cannot set property when encoding.");
}

napi_value failBoth(napi_env env, napi_callback_info info) {
  NAPI_THROW_ERROR("User cannot set this property.");
}

napi_status fromAVCodecContext(napi_env env, AVCodecContext* codec,
    napi_value* result, bool encoding) {
  napi_status status;
  napi_value jsCodec, extCodec, typeName, undef;
  bool* encodingRef = (bool*) malloc(sizeof(bool));

  *encodingRef = encoding;

  status = napi_create_object(env, &jsCodec);
  PASS_STATUS;
  status = napi_create_string_utf8(env, encoding ? "encoder" : "decoder",
    NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_get_undefined(env, &undef);
  PASS_STATUS;
  status = napi_create_external(env, codec, codecContextFinalizer, encodingRef, &extCodec);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "codec_id", nullptr, nullptr, getCodecCtxCodecID, nop, nullptr,
      napi_enumerable, codec},
    { "name", nullptr, nullptr, getCodecCtxName, nop, nullptr,
      napi_enumerable, codec},
    { "long_name", nullptr, nullptr, getCodecCtxLongName, nullptr, nullptr,
      napi_enumerable, codec},
    { "codec_tag", nullptr, nullptr, getCodecCtxCodecTag, nullptr, nullptr,
      napi_enumerable, codec},
    { "priv_data", nullptr, nullptr, getCodecCtxPrivData, setCodecCtxPrivData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    // TODO internal
    // TODO opaque
    { "bit_rate", nullptr, nullptr, getCodecCtxBitRate, setCodecCtxBitRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "bit_rate_tolerance", nullptr, nullptr, getCodecCtxBitRateTol,
      encoding ? setCodecCtxBitRateTol : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "global_quality", nullptr, nullptr, getCodecCtxGlobalQ,
      encoding ? setCodecCtxGlobalQ : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 10
    { "compression_level", nullptr, nullptr, getCodecCtxCompLvl,
      encoding ? setCodecCtxCompLvl : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "flags", nullptr, nullptr, getCodecCtxFlags, setCodecCtxFlags, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "flags2", nullptr, nullptr, getCodecCtxFlags2, setCodecCtxFlags2, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "extradata", nullptr, nullptr, getCodecCtxExtraData,
      encoding ? failEncoding : setCodecCtxExtraData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "time_base", nullptr, nullptr, getCodecCtxTimeBase,
       encoding ? setCodecCtxTimeBase : failDecoding, nullptr,
       (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "ticks_per_frame", nullptr, nullptr, getCodecCtxTicksPerFrame, setCodecCtxTicksPerFrame, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "delay", nullptr, nullptr, getCodecCtxDelay, failBoth, nullptr,
       napi_enumerable, codec},
    { "width", nullptr, nullptr, getCodecCtxWidth, setCodecCtxWidth, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "height", nullptr, nullptr, getCodecCtxHeight, setCodecCtxHeight, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "coded_width", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxCodedWidth,
      encoding ? failEncoding : setCodecCtxCodedWidth, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    // 20
    { "coded_height", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxCodedHeight,
      encoding ? failEncoding : setCodecCtxCodedHeight, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "gop_size" , nullptr, nullptr,
       encoding ? getCodecCtxGopSize : nullptr,
       encoding ? setCodecCtxGopSize : failDecoding, nullptr,
       encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "pix_fmt", nullptr, nullptr, getCodecCtxPixFmt, setCodecCtxPixFmt, nullptr,
       (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "max_b_frames", nullptr, nullptr,
      encoding ? getCodecCtxMaxBFrames : nullptr,
      encoding ? setCodecCtxMaxBFrames : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "b_quant_factor", nullptr, nullptr,
      encoding ? getCodecCtxBQuantFactor : nullptr,
      encoding ? setCodecCtxBQuantFactor : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "b_quant_offset", nullptr, nullptr,
      encoding ? getCodecCtxBQuantOffset : nullptr,
      encoding ? setCodecCtxBQuantOffset : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "has_b_frames", nullptr, nullptr, getCodecCtxHasBFrames, failBoth, nullptr,
       napi_enumerable, codec},
    { "i_quant_factor", nullptr, nullptr,
      encoding ? getCodecCtxIQuantFactor : nullptr,
      encoding ? setCodecCtxIQuantFactor : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "i_quant_offset", nullptr, nullptr,
      encoding ? getCodecCtxIQuantOffset : nullptr,
      encoding ? setCodecCtxIQuantOffset : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "lumi_masking", nullptr, nullptr,
      encoding ? getCodecCtxLumiMasking : nullptr,
      encoding ? setCodecCtxLumiMasking : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 30
    { "temporal_cplx_masking", nullptr, nullptr,
      encoding ? getCodecCtxTempCplxMask : nullptr,
      encoding ? setCodecCtxTempCplxMask : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "spatial_cplx_masking", nullptr, nullptr,
      encoding ? getCodecCtxSpatCplxMask : nullptr,
      encoding ? setCodecCtxSpatCplxMask : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "p_masking", nullptr, nullptr,
      encoding ? getCodecCtxPMasking : nullptr,
      encoding ? setCodecCtxPMasking : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "dark_masking", nullptr, nullptr,
      encoding ? getCodecCtxDarkMasking : nullptr,
      encoding ? setCodecCtxDarkMasking : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "slice_count", nullptr, nullptr, getCodecCtxSliceCount,
      encoding ? failEncoding : setCodecCtxSliceCount, nullptr,
      encoding ? napi_enumerable : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "slice_offset", nullptr, nullptr, getCodecCtxSliceOffset,
      encoding ? failEncoding : setCodecCtxSliceOffset, nullptr,
      encoding ? napi_enumerable : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "sample_aspect_ratio", nullptr, nullptr, getCodecCtxSampleAspRt,
      encoding ? setCodecCtxSampleAspRt : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "me_cmp", nullptr, nullptr,
      encoding ? getCodecCtxMeCmp: nullptr,
      encoding ? setCodecCtxMeCmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "me_sub_cmp", nullptr, nullptr,
      encoding ? getCodecCtxMeSubCmp: nullptr,
      encoding ? setCodecCtxMeSubCmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "mb_cmp", nullptr, nullptr,
      encoding ? getCodecCtxMbCmp: nullptr,
      encoding ? setCodecCtxMbCmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 40
    { "ildct_cmp", nullptr, nullptr,
      encoding ? getCodecCtxIldctCmp: nullptr,
      encoding ? setCodecCtxIldctCmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "dia_size", nullptr, nullptr,
      encoding ? getCodecCtxDiaSize: nullptr,
      encoding ? setCodecCtxDiaSize : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "last_predictor_count", nullptr, nullptr,
      encoding ? getCodecCtxLastPredCount: nullptr,
      encoding ? setCodecCtxLastPredCount : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "me_pre_cmp", nullptr, nullptr,
      encoding ? getCodecCtxMePreCmp : nullptr,
      encoding ? setCodecCtxMePreCmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "pre_dia_size", nullptr, nullptr,
      encoding ? getCodecCtxPreDiaSize: nullptr,
      encoding ? setCodecCtxPreDiaSize : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "me_subpel_quality", nullptr, nullptr,
      encoding ? getCodecCtxMeSubpelQual : nullptr,
      encoding ? setCodecCtxMeSubpelQual : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "me_range", nullptr, nullptr,
      encoding ? getCodecCtxMeRange : nullptr,
      encoding ? setCodecCtxMeRange : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "slice_flags", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSliceFlags,
      encoding ? failEncoding : setCodecCtxSliceFlags, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "mb_decision", nullptr, nullptr,
      encoding ? getCodecCtxMbDecision : nullptr,
      encoding ? setCodecCtxMbDecision : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "intra_matrix", nullptr, nullptr, getCodecCtxIntraMatrix,
      encoding ? setCodecCtxIntraMatrix : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    // 50
    { "inter_matrix", nullptr, nullptr, getCodecCtxInterMatrix,
      encoding ? setCodecCtxInterMatrix : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "intra_dc_precision", nullptr, nullptr, getCodecCtxIntraDCProv,
      encoding ? setCodecCtxIntraDCProv : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "skip_top", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipTop,
      encoding ? failEncoding : setCodecCtxSkipTop, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "skip_bottom", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipBottom,
      encoding ? failEncoding : setCodecCtxSkipBottom, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "mb_lmin", nullptr, nullptr,
      encoding ? getCodecCtxMbLmin : nullptr,
      encoding ? setCodecCtxMbLmin : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "mb_lmax", nullptr, nullptr,
      encoding ? getCodecCtxMbLmax : nullptr,
      encoding ? setCodecCtxMbLmax : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "bidir_refine", nullptr, nullptr,
      encoding ? getCodecCtxBidirRefine : nullptr,
      encoding ? setCodecCtxBidirRefine : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "keyint_min", nullptr, nullptr,
      encoding ? getCodecCtxKeyIntMin : nullptr,
      encoding ? setCodecCtxKeyIntMin : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "refs", nullptr, nullptr, getCodecCtxKeyRefs,
      encoding ? setCodecCtxKeyRefs : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "mv0_threshold", nullptr, nullptr,
      encoding ? getCodecCtxMv0Threshold : nullptr,
      encoding ? setCodecCtxMv0Threshold : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 60
    { "color_primaries", nullptr, nullptr, getCodecCtxColorPrim,
      encoding ? setCodecCtxColorPrim : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "color_trc", nullptr, nullptr, getCodecCtxColorTrc,
      encoding ? setCodecCtxColorTrc : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "colorspace", nullptr, nullptr, getCodecCtxColorspace,
      encoding ? setCodecCtxColorspace : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "color_range", nullptr, nullptr, getCodecCtxColorRange,
      encoding ? setCodecCtxColorRange : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "chroma_sample_location", nullptr, nullptr, getCodecCtxChromaLoc,
      encoding ? setCodecCtxChromaLoc : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "slices", nullptr, nullptr,
      encoding ? getCodecCtxSlices : nullptr,
      encoding ? setCodecCtxSlices : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "field_order", nullptr, nullptr, getCodecCtxFieldOrder,
      encoding ? setCodecCtxFieldOrder : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "sample_rate", nullptr, nullptr, getCodecCtxSampleRate, setCodecCtxSampleRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "channels", nullptr, nullptr, getCodecCtxChannels, setCodecCtxChannels, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "sample_fmt", nullptr, nullptr, getCodecCtxSampleFmt,
      encoding ? setCodecCtxSampleFmt : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    // 70
    { "frame_size", nullptr, nullptr, getCodecCtxFrameSize, failBoth, nullptr,
      napi_enumerable, codec},
    { "frame_number", nullptr, nullptr, getCodecCtxFrameNb, failBoth, nullptr,
      napi_enumerable, codec},
    { "block_align", nullptr, nullptr,
      encoding ? getCodecCtxBlockAlign : nullptr,
      encoding ? setCodecCtxBlockAlign : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "cutoff", nullptr, nullptr, getCodecCtxCutoff, setCodecCtxCutoff, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "channel_layout", nullptr, nullptr, getCodecCtxChanLayout, setCodecCtxChanLayout, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "request_channel_layout", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxReqChanLayout,
      encoding ? failEncoding : setCodecCtxReqChanLayout, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "audio_service_type", nullptr, nullptr, getCodecCtxAudioSvcType,
      encoding ? setCodecCtxAudioSvcType : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "request_sample_fmt", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxReqSampleFmt,
      encoding ? failEncoding : setCodecCtxReqSampleFmt, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "qcompress", nullptr, nullptr,
      encoding ? getCodecCtxQCompress : nullptr,
      encoding ? setCodecCtxQCompress : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "qblur", nullptr, nullptr,
      encoding ? getCodecCtxQBlur : nullptr,
      encoding ? setCodecCtxQBlur : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 80
    { "qmin", nullptr, nullptr,
      encoding ? getCodecCtxQMin : nullptr,
      encoding ? setCodecCtxQMin : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "qmax", nullptr, nullptr,
      encoding ? getCodecCtxQMax : nullptr,
      encoding ? setCodecCtxQMax : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "max_qdiff", nullptr, nullptr,
      encoding ? getCodecCtxMaxQDiff : nullptr,
      encoding ? setCodecCtxMaxQDiff : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_buffer_size", nullptr, nullptr,
      encoding ? getCodecCtxRcBufSize : nullptr,
      encoding ? setCodecCtxRcBufSize : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_override", nullptr, nullptr,
      encoding ? getCodecCtxRcOverride : nullptr,
      encoding ? setCodecCtxRcOverride : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_max_rate", nullptr, nullptr, getCodecCtxRcMaxRate, setCodecCtxRcMaxRate, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "rc_min_rate", nullptr, nullptr,
      encoding ? getCodecCtxRcMinRate : nullptr,
      encoding ? setCodecCtxRcMinRate : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_max_available_vbv_use", nullptr, nullptr,
      encoding ? getCodecCtxRcMaxAvailVbvUse : nullptr,
      encoding ? setCodecCtxRcMaxAvailVbvUse : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_min_vbv_overflow_use", nullptr, nullptr,
      encoding ? getCodecCtxRcMinVbvOverflowUse : nullptr,
      encoding ? setCodecCtxRcMinVbvOverflowUse : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "rc_initial_buffer_occupancy", nullptr, nullptr,
      encoding ? getCodecCtxRcInitBufOc : nullptr,
      encoding ? setCodecCtxRcInitBufOc : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    // 90
    { "trellis", nullptr, nullptr,
      encoding ? getCodecCtxTrellis : nullptr,
      encoding ? setCodecCtxTrellis : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "stats_out", nullptr, nullptr,
      encoding ? getCodecCtxStatsOut : nullptr, failBoth, nullptr,
      encoding ? napi_enumerable : napi_default, codec},
    { "stats_in", nullptr, nullptr,
      encoding ? getCodecCtxStatsIn : nullptr,
      encoding ? setCodecCtxStatsIn : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "workaround_bugs", nullptr, nullptr, getCodecCtxWorkaround, setCodecCtxWorkaround, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "strict_std_compliance", nullptr, nullptr, getCodecCtxStrictStdComp, setCodecCtxStrictStdComp, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "error_concealment", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxErrConceal,
      encoding ? failEncoding : setCodecCtxErrConceal, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "debug", nullptr, nullptr, getCodecCtxDebug, setCodecCtxDebug, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "err_recognition", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxErrRecog,
      encoding ? failEncoding : setCodecCtxErrRecog, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "reordered_opaque", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxReorderOpaq,
      encoding ? failEncoding : setCodecCtxReorderOpaq, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    // TODO hwaccel_context
    { "error", nullptr, nullptr,
      encoding ? getCodecCtxError : nullptr, failBoth, nullptr,
      encoding ? napi_enumerable : napi_default, codec},
    // 100
    { "dct_algo", nullptr, nullptr,
      encoding ? getCodecCtxDctAlgo : nullptr,
      encoding ? setCodecCtxDctAlgo : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "idct_algo", nullptr, nullptr, getCodecCtxIdctAlgo, setCodecCtxIdctAlgo, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "bits_per_coded_sample", nullptr, nullptr, getCodecCtxBitsPCodedSmp,
      encoding ? failEncoding : setCodecCtxBitsPCodedSmp, nullptr,
      encoding ? napi_enumerable : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "bits_per_raw_sample", nullptr, nullptr, getCodecCtxBitsPRawSmp,
      encoding ? setCodecCtxBitsPRawSmp : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "thread_count", nullptr, nullptr, getCodecCtxThreadCount, setCodecCtxThreadCount, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "thread_type", nullptr, nullptr, getCodecCtxThreadType, setCodecCtxThreadType, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "active_thread_type", nullptr, nullptr, getCodecCtxActThreadType, failBoth, nullptr,
      napi_enumerable, codec},
    // TODO find a way of exposing a custom getBuffer?
    { "nsse_weight", nullptr, nullptr,
      encoding ? getCodecCtxNsseWeight : nullptr,
      encoding ? setCodecCtxNsseWeight : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "profile", nullptr, nullptr, getCodecCtxProfile,
      encoding ? setCodecCtxProfile : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "level", nullptr, nullptr, getCodecCtxLevel,
      encoding ? setCodecCtxLevel : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    // 110
    { "skip_loop_filter", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipLpFilter,
      encoding ? failEncoding : setCodecCtxSkipLpFilter, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "skip_idct", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipIdct,
      encoding ? failEncoding : setCodecCtxSkipIdct, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "skip_frame", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipFrame,
      encoding ? failEncoding : setCodecCtxSkipFrame, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "subtitle_header", nullptr, nullptr, getCodecCtxSubtitleHdr,
       encoding? setCodecCtxSubtitleHdr : failDecoding, nullptr,
       encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "initial_padding", nullptr, nullptr,
      encoding ? getCodecCtxInitPad : nullptr, failBoth, nullptr,
      encoding ? napi_enumerable : napi_default, codec},
    { "framerate", nullptr, nullptr, getCodecCtxFramerate,
      encoding ? setCodecCtxFramerate : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "sw_pix_fmt", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSwPixFmt, failBoth, nullptr,
      encoding ? napi_default : napi_enumerable, codec},
    { "pkt_timebase", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxPktTimebase,
      encoding ? failEncoding : setCodecCtxPktTimebase, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "codec_descriptor", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxDesc, failBoth, nullptr,
      encoding ? napi_default : napi_enumerable, codec},
    // TODO not exposing lowres ... it's on its way out
    // not exposing PTS correct stats - "not intended to be used by user apps"
    { "sub_charenc", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSubCharenc,
      encoding ? failEncoding : setCodecCtxSubCharenc, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    // 120
    { "sub_charenc_mode", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSubCharencMode, failBoth, nullptr,
      encoding ? napi_default : napi_enumerable, codec},
    { "skip_alpha", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSkipAlpha,
      encoding ? failEncoding : setCodecCtxSkipAlpha, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "seek_preroll", nullptr, nullptr,
      encoding ? getCodecCtxSeekPreroll : nullptr, failBoth, nullptr,
      encoding ? napi_enumerable : napi_default, codec},
    { "chroma_intra_matrix", nullptr, nullptr,
      encoding ? getCodecCtxChromaIntraMatrix : nullptr,
      encoding ? setCodecCtxChromaIntraMatrix : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_default, codec},
    { "dump_separator", nullptr, nullptr, getCodecCtxDumpSep, setCodecCtxDumpSep, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "codec_whitelist", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxWhitelist,
      encoding ? failEncoding : setCodecCtxWhitelist, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "properties", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxProps, failBoth, nullptr,
      encoding ? napi_default : napi_enumerable, codec},
    { "coded_side_data", nullptr, nullptr,
      encoding ? getCodecCtxCodedSideData : nullptr, failBoth, nullptr,
      encoding ? napi_enumerable : napi_default, codec},
    { "hw_frames_ctx", nullptr, nullptr,
      getCodecHWFramesCtx,
      encoding ? setCodecHWFramesCtx : failDecoding, nullptr,
      encoding ? (napi_property_attributes) (napi_writable | napi_enumerable) : napi_enumerable, codec},
    { "sub_text_format", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxSubTextFmt,
      encoding ? failEncoding : setCodecCtxSubTextFmt, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    // 130
    { "trailing_padding", nullptr, nullptr, getCodecCtxTrailPad, setCodecCtxTrailPad, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "max_pixels", nullptr, nullptr, getCodecCtxMaxPixels, setCodecCtxMaxPixels, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "hw_device_ctx", nullptr, nullptr, getCodecHWDeviceCtx, nullptr, nullptr, napi_enumerable, codec},
    { "hwaccel_flags", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxHwAccelFlags,
      encoding ? failEncoding : setCodecCtxHwAccelFlags, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "apply_cropping", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxApplyCrop,
      encoding ? failEncoding : setCodecCtxApplyCrop, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { "extra_hw_frames", nullptr, nullptr,
      encoding ? nullptr : getCodecCtxExtraHwFrames,
      encoding ? failEncoding : setCodecCtxExtraHwFrames, nullptr,
      encoding ? napi_default : (napi_property_attributes) (napi_writable | napi_enumerable), codec},
    { encoding ? "encode" : "decode", nullptr,
      encoding ? encode : decode, nullptr, nullptr, nullptr, napi_enumerable, codec},
    { "flush", nullptr,
      encoding ? flushEnc : flushDec, nullptr, nullptr, nullptr, napi_enumerable, codec},
    { "extractParams", nullptr, extractParams, nullptr, nullptr, nullptr, napi_enumerable, nullptr},
    { "useParams", nullptr, useParams, nullptr, nullptr, nullptr, napi_enumerable, nullptr},
    // Hidden values - to allow Object.assign to work
    { "params", nullptr, nullptr, nullptr, nop, undef, // Set for muxing
      napi_writable, nullptr},
    // 140
    { "stream_index", nullptr, nullptr, nullptr, nop, undef, napi_writable, nullptr },
    { "demuxer", nullptr, nullptr, nullptr, nop, undef, napi_writable, nullptr},
    { "_CodecContext", nullptr, nullptr, nullptr, nullptr, extCodec, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsCodec, 143, desc);
  PASS_STATUS;

  *result = jsCodec;
  return napi_ok;
}

void codecContextFinalizer(napi_env env, void* data, void* hint) {
  AVCodecContext* codecCtx = (AVCodecContext*) data;
  bool* encodingRef = (bool*) hint;
  if ((codecCtx->extradata_size > 0) && (codecCtx->extradata != nullptr)) {
    av_freep(&codecCtx->extradata);
    codecCtx->extradata_size = 0;
  }
  if (codecCtx->rc_override_count > 0) {
    av_freep(&codecCtx->rc_override);
  }
  if (codecCtx->stats_in != nullptr) {
    av_freep(&codecCtx->stats_in);
  }
  if (codecCtx->sub_charenc != nullptr) {
    av_freep(&codecCtx->sub_charenc);
  }
  if (codecCtx->dump_separator != nullptr) {
    av_freep(&codecCtx->dump_separator);
  }
  if (codecCtx->codec_whitelist != nullptr) {
    av_freep(&codecCtx->codec_whitelist);
  }
  // Don't delete if allocated by libavcodec when decoding
  if (*encodingRef &&(codecCtx->subtitle_header_size > 0) &&
      (codecCtx->subtitle_header != nullptr)) {
    av_freep(&codecCtx->subtitle_header);
    codecCtx->subtitle_header_size = 0;
  }
  avcodec_close(codecCtx);
  avcodec_free_context(&codecCtx);
}

napi_status fromAVCodecDescriptor(napi_env env, const AVCodecDescriptor* codecDesc,
    napi_value *result) {
  napi_status status;
  napi_value element, props;

  status = napi_create_object(env, &element);
  PASS_STATUS;
  status = beam_set_string_utf8(env, element, "type", "CodecDescriptor");
  PASS_STATUS;
  status = beam_set_string_utf8(env, element, "name", (char*) codecDesc->name);
  PASS_STATUS;
  status = napi_create_object(env, &props);
  PASS_STATUS;
  status = beam_set_bool(env, props, "INTRA_ONLY", codecDesc->props & AV_CODEC_PROP_INTRA_ONLY);
  PASS_STATUS;
  status = beam_set_bool(env, props, "LOSSY", codecDesc->props & AV_CODEC_PROP_LOSSY);
  PASS_STATUS;
  status = beam_set_bool(env, props, "LOSSLESS", codecDesc->props & AV_CODEC_PROP_LOSSLESS);
  PASS_STATUS;
  status = beam_set_bool(env, props, "REORDER", codecDesc->props & AV_CODEC_PROP_REORDER);
  PASS_STATUS;
  status = beam_set_bool(env, props, "BITMAP_SUB", codecDesc->props & AV_CODEC_PROP_BITMAP_SUB);
  PASS_STATUS;
  status = beam_set_bool(env, props, "TEXT_SUB", codecDesc->props & AV_CODEC_PROP_TEXT_SUB);
  PASS_STATUS;
  status = napi_set_named_property(env, element, "props", props);
  PASS_STATUS;

  *result = props;
  return napi_ok;
}

napi_value extractParams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, jsCodec, extCodec;
  AVCodecContext* codecCtx;
  AVCodecParameters* codecPar = nullptr;
  int ret;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &jsCodec, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, jsCodec, "_CodecContext", &extCodec);
  CHECK_STATUS;
  status = napi_get_value_external(env, extCodec, (void**) &codecCtx);
  CHECK_STATUS;

  codecPar = avcodec_parameters_alloc();
  if ((ret = avcodec_parameters_from_context(codecPar, codecCtx))) {
    NAPI_THROW_ERROR(avErrorMsg("Failed to extract parameters from codec context: ", ret));
  }

  status = fromAVCodecParameters(env, codecPar, true, &result);
  CHECK_STATUS;

  return result;
};

napi_value useParams(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value jsCodec, extCodec, extPars;
  napi_valuetype type;
  AVCodecContext* codecCtx;
  AVCodecParameters* codecPar = nullptr;
  int ret;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, &jsCodec, nullptr);
  CHECK_STATUS;
  status = napi_get_named_property(env, jsCodec, "_CodecContext", &extCodec);
  CHECK_STATUS;
  status = napi_get_value_external(env, extCodec, (void**) &codecCtx);
  CHECK_STATUS;

  if (argc < 1) {
    NAPI_THROW_ERROR("Parameters to use must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Parameters to use must be provided as an object.");
  }
  status = napi_get_named_property(env, args[0], "_codecPar", &extPars);
  CHECK_STATUS;
  status = napi_typeof(env, extPars, &type);
  CHECK_STATUS;
  if (type != napi_external) {
    NAPI_THROW_ERROR("Value used to provide codec parameters does not contain those parameters.");
  }
  status = napi_get_value_external(env, extPars, (void**) &codecPar);
  CHECK_STATUS;

  if (codecCtx->codec_id != codecPar->codec_id) {
    NAPI_THROW_ERROR("Codec identifier for codec parameters and codec context differ.");
  }
  if (codecCtx->codec_type != codecPar->codec_type) {
    NAPI_THROW_ERROR("Codec type for codec parameters and codec context differ.");
  }
  if ((ret = avcodec_parameters_to_context(codecCtx, codecPar))) {
    NAPI_THROW_ERROR(avErrorMsg("Problem using parameters to set up codec context: ", ret));
  }

  return jsCodec;
};

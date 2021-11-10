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

#include "filter.h"
#include "beamcoder_util.h"
#include "frame.h"
#include <map>
#include <deque>

extern "C" {
  #include <libavfilter/avfilter.h>
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libavfilter/buffersink.h>
  #include <libavfilter/buffersrc.h>
}

napi_status toFilterPrivData(napi_env env, napi_value params, AVFilterContext *filterContext) {
  napi_status status;
  napi_value names, element, subel;
  napi_valuetype type, subtype;
  bool isArray, flag;
  double dValue;
  uint32_t uThirtwo;
  char* sValue;
  char* strProp;
  size_t sLen;
  const AVOption* option;
  int64_t iValue;
  int ret;
  AVRational qValue = {0,0};
  AVFilterGraph *graph = filterContext->graph;
  void* priv_data = filterContext->priv;

  if (priv_data == nullptr) {
    return napi_invalid_arg;
  }

  status = napi_typeof(env, params, &type);
  PASS_STATUS;
  status = napi_is_array(env, params, &isArray);
  PASS_STATUS;
  if ((isArray == false) && (type == napi_object)) {
    status = napi_get_property_names(env, params, &names);
    PASS_STATUS;
    status = napi_get_array_length(env, names, &uThirtwo);
    PASS_STATUS;
    for ( uint32_t x = 0 ; x < uThirtwo ; x++ ) {
      status = napi_get_element(env, names, x, &element);
      PASS_STATUS;
      status = napi_get_value_string_utf8(env, element, nullptr, 0, &sLen);
      PASS_STATUS;
      sValue = (char*) malloc(sizeof(char) * (sLen + 1));
      status = napi_get_value_string_utf8(env, element, sValue, sLen + 1, &sLen);
      PASS_STATUS;
      option = av_opt_find(priv_data, sValue, nullptr, 0, 0);
      if (option != nullptr) {
        if (option->flags & AV_OPT_FLAG_READONLY) { continue; }
        status = napi_get_named_property(env, params, sValue, &element);
        PASS_STATUS;
        status = napi_typeof(env, element, &type);
        PASS_STATUS;
        switch (type) {
          case napi_boolean:
            status = napi_get_value_bool(env, element, &flag);
            PASS_STATUS;
            ret = av_opt_set_int(priv_data, sValue, flag, 0);
            if (ret < 0) printf("DEBUG: Unable to set %s with a boolean value.\n", sValue);
            break;
          case napi_number:
            if ((option->type == AV_OPT_TYPE_DOUBLE) ||
                (option->type == AV_OPT_TYPE_FLOAT)) {
              status = napi_get_value_double(env, element, &dValue);
              PASS_STATUS;
              ret = av_opt_set_double(priv_data, sValue, dValue, 0);
              if (ret < 0) printf("DEBUG: Unable to set %s with a double value %f.\n", sValue, dValue);
              break;
            }
            status = napi_get_value_int64(env, element, &iValue);
            PASS_STATUS;
            ret = av_opt_set_int(priv_data, sValue, iValue, 0);
            if (ret < 0) printf("DEBUG: Unable to set %s with an integer value %" PRId64 ": %s.\n",
              sValue, iValue, avErrorMsg("", ret));
            break;
          case napi_string:
            status = napi_get_value_string_utf8(env, element, nullptr, 0, &sLen);
            PASS_STATUS;
            strProp = (char*) malloc(sizeof(char) * (sLen + 1));
            PASS_STATUS;
            status = napi_get_value_string_utf8(env, element, strProp, sLen + 1, &sLen);
            PASS_STATUS;
            ret = avfilter_graph_send_command (graph, filterContext->name, sValue, strProp, nullptr, 0, 0);
            free(strProp);
            if (ret < 0) printf("DEBUG: Unable to set %s with a string value %s.\n", sValue, strProp);
            break;
          case napi_object:
            status = napi_is_array(env, element, &isArray);
            PASS_STATUS;
            if (isArray && (option->type == AV_OPT_TYPE_RATIONAL)) {
              status = napi_get_element(env, element, 0, &subel);
              PASS_STATUS;
              status = napi_typeof(env, subel, &subtype);
              PASS_STATUS;
              if (subtype != napi_number) {
                printf("DEBUG: Non-number value for rational numerator of property %s.\n", sValue);
                break;
              }
              status = napi_get_value_int32(env, subel, &qValue.num);
              PASS_STATUS;
              status = napi_get_element(env, element, 1, &subel);
              PASS_STATUS;
              status = napi_typeof(env, subel, &subtype);
              PASS_STATUS;
              if (subtype != napi_number) {
                printf("DEBUG: Non-number value for rational denominator of property %s.\n", sValue);
                qValue.num = 0; qValue.den = 1;
                break;
              }
              status = napi_get_value_int32(env, subel, &qValue.den);
              PASS_STATUS;
              ret = av_opt_set_q(priv_data, sValue, qValue, 0);
              if (ret < 0) {
                printf("DEBUG: Failed to set rational property %s.\n", sValue);
              }
              qValue.num = 0; qValue.den = 1;
            } else {
              printf("DEBUG: Non-array for non-rational property %s.\n", sValue);
            }
            break;
          default:
            printf("DEBUG: Failed to set a private data value %s\n", sValue);
            break;
        }
      } else {
        printf("DEBUG: Option %s not found.\n", sValue);
      }
      free(sValue);
    }
  } else {
    return napi_invalid_arg;
  }

  return napi_ok;
}

napi_value getFilterCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext *filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  if (nullptr == filterContext->priv) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
    return result;
  }

  status = fromContextPrivData(env, filterContext->priv, &result);
  CHECK_STATUS;

  return result;
}

napi_value setFilterCtxPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, value;
  napi_valuetype type;
  AVFilterContext* filterContext;
  bool isArray;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  if (nullptr == filterContext->priv)
    NAPI_THROW_ERROR("Filter does not have private_data.");

  if (argc == 0)
    NAPI_THROW_ERROR("A value must be provided to set private_data.");

  value = args[0];
  status = napi_typeof(env, value, &type);
  CHECK_STATUS;
  status = napi_is_array(env, value, &isArray);
  CHECK_STATUS;
  if ((isArray == false) && (type == napi_object)) {
    status = toFilterPrivData(env, value, filterContext);
    CHECK_STATUS;
  } else {
    NAPI_THROW_ERROR("An object with key/value pairs is required to set private data.");
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getFilterName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilter* filter;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filter);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, filter->name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFilterDesc(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilter* filter;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filter);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, filter->description, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFilterPads(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value pad, result;
  AVFilterPad* filterPads;
  int padCount;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterPads);
  CHECK_STATUS;

  padCount = avfilter_pad_count(filterPads);
  if (0 == padCount) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_array(env, &result);
    CHECK_STATUS;
    for ( int x = 0 ; x < padCount ; x++ ) {
      status = napi_create_object(env, &pad);
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "name",
        (char*) avfilter_pad_get_name(filterPads, x));
      CHECK_STATUS;
      status = beam_set_string_utf8(env, pad, "media_type",
        (char*) av_get_media_type_string(avfilter_pad_get_type(filterPads, x)));
      CHECK_STATUS;
      status = napi_set_element(env, result, x, pad);
      CHECK_STATUS;
    }
  }
  return result;
}

napi_value getFilterPrivData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilter* filter;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filter);
  CHECK_STATUS;

  if (filter->priv_class != nullptr) {
    status = fromAVClass(env, (const AVClass*) filter->priv_class, &result);
    CHECK_STATUS;
  } else {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  }
  return result;
}

napi_value getFilterFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilter* filter;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filter);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  status = beam_set_bool(env, result, "DYNAMIC_INPUTS",
    (filter->flags & AVFILTER_FLAG_DYNAMIC_INPUTS) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DYNAMIC_OUTPUTS",
    (filter->flags & AVFILTER_FLAG_DYNAMIC_OUTPUTS) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SLICE_THREADS",
    (filter->flags & AVFILTER_FLAG_SLICE_THREADS) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SUPPORT_TIMELINE_GENERIC",
    (filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SUPPORT_TIMELINE_INTERNAL",
    (filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "SUPPORT_TIMELINE",
    (filter->flags & (AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC | AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL)) != 0);
  CHECK_STATUS;

  return result;
}

napi_status fromAVFilter(napi_env env, const AVFilter* filter, napi_value* result) {
  napi_status status;
  napi_value typeName;

  status = napi_create_object(env, result);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "Filter", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "name", nullptr, nullptr, getFilterName, nullptr, nullptr, napi_enumerable, (void*)filter },
    { "description", nullptr, nullptr, getFilterDesc, nullptr, nullptr, napi_enumerable, (void*)filter },
    { "inputs", nullptr, nullptr, getFilterPads, nullptr, nullptr, napi_enumerable, (void*)filter->inputs },
    { "outputs", nullptr, nullptr, getFilterPads, nullptr, nullptr, napi_enumerable, (void*)filter->outputs },
    { "priv_class", nullptr, nullptr, getFilterPrivData, nullptr, nullptr, napi_enumerable, (void*)filter },
    { "flags", nullptr, nullptr, getFilterFlags, nullptr, nullptr, napi_enumerable, (void*)filter }
  };
  status = napi_define_properties(env, *result, 7, desc);
  PASS_STATUS;

  return napi_ok;
}

napi_value getLinkSrc(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value strVal;
  status = napi_create_string_utf8(env, filterLink->src->name, NAPI_AUTO_LENGTH, &strVal);
  CHECK_STATUS;

  return strVal;
}

napi_value getLinkSrcPad(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value strVal;
  status = napi_create_string_utf8(env, avfilter_pad_get_name(filterLink->srcpad, 0), NAPI_AUTO_LENGTH, &strVal);
  CHECK_STATUS;

  return strVal;
}

napi_value getLinkDst(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value strVal;
  status = napi_create_string_utf8(env, filterLink->dst->name, NAPI_AUTO_LENGTH, &strVal);
  CHECK_STATUS;

  return strVal;
}

napi_value getLinkDstPad(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value strVal;
  status = napi_create_string_utf8(env, avfilter_pad_get_name(filterLink->dstpad, 0), NAPI_AUTO_LENGTH, &strVal);
  CHECK_STATUS;

  return strVal;
}

napi_value getLinkMediaType(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value strVal;
  status = napi_create_string_utf8(env, av_get_media_type_string(filterLink->type), NAPI_AUTO_LENGTH, &strVal);
  CHECK_STATUS;

  return strVal;
}

napi_value getLinkVidWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value widthVal;
  status = napi_create_int32(env, filterLink->w, &widthVal);
  CHECK_STATUS;

  return widthVal;
}

napi_value getLinkVidHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value heightVal;
  status = napi_create_int32(env, filterLink->h, &heightVal);
  CHECK_STATUS;

  return heightVal;
}

napi_value getLinkSAR(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value pair, element;
  status = napi_create_array(env, &pair);
  CHECK_STATUS;
  status = napi_create_int32(env, filterLink->sample_aspect_ratio.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, pair, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, filterLink->sample_aspect_ratio.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, pair, 1, element);
  CHECK_STATUS;

  return pair;
}

napi_value getLinkChannelCount(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value channelCountVal;
  int channelCount = av_get_channel_layout_nb_channels(filterLink->channel_layout);
  status = napi_create_int32(env, channelCount, &channelCountVal);

  return channelCountVal;
}

napi_value getLinkChannelLayout(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  char channelLayoutStr[30];
  av_get_channel_layout_string(channelLayoutStr, 30, -1, filterLink->channel_layout);

  napi_value channelLayoutVal;
  status = napi_create_string_utf8(env, channelLayoutStr, NAPI_AUTO_LENGTH, &channelLayoutVal);
  CHECK_STATUS;

  return channelLayoutVal;
}

napi_value getLinkSampleRate(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value sampleRateVal;
  status = napi_create_int32(env, filterLink->sample_rate, &sampleRateVal);
  CHECK_STATUS;

  return sampleRateVal;
}

napi_value getLinkFormat(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  const char *formatName;
  switch (filterLink->type) {
  case AVMEDIA_TYPE_VIDEO:
    formatName = av_get_pix_fmt_name((AVPixelFormat)filterLink->format);
    break;
  case AVMEDIA_TYPE_AUDIO:
    formatName = av_get_sample_fmt_name((AVSampleFormat)filterLink->format);
    break;
  default:
    formatName = "unrecognised";
  };

  napi_value formatVal;
  status = napi_create_string_utf8(env, formatName, NAPI_AUTO_LENGTH, &formatVal);
  CHECK_STATUS;

  return formatVal;
}

napi_value getLinkTimeBase(napi_env env, napi_callback_info info) {
  napi_status status;
  AVFilterLink* filterLink;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterLink);
  CHECK_STATUS;

  napi_value pair, element;
  status = napi_create_array(env, &pair);
  CHECK_STATUS;
  status = napi_create_int32(env, filterLink->time_base.num, &element);
  CHECK_STATUS;
  status = napi_set_element(env, pair, 0, element);
  CHECK_STATUS;
  status = napi_create_int32(env, filterLink->time_base.den, &element);
  CHECK_STATUS;
  status = napi_set_element(env, pair, 1, element);
  CHECK_STATUS;

  return pair;
}

napi_status fromAVFilterLink(napi_env env, const AVFilterLink* link, napi_value* result) {

  napi_status status = napi_create_object(env, result);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "src", nullptr, nullptr, getLinkSrc, nullptr, nullptr, napi_enumerable, (void*)link },
    { "srcpad", nullptr, nullptr, getLinkSrcPad, nullptr, nullptr, napi_enumerable, (void*)link },
    { "dst", nullptr, nullptr, getLinkDst, nullptr, nullptr, napi_enumerable, (void*)link },
    { "dstpad", nullptr, nullptr, getLinkDstPad, nullptr, nullptr, napi_enumerable, (void*)link },
    { "type", nullptr, nullptr, getLinkMediaType, nullptr, nullptr, napi_enumerable, (void*)link },
    { "w", nullptr, nullptr, getLinkVidWidth, nullptr, nullptr,
      (AVMEDIA_TYPE_VIDEO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "h", nullptr, nullptr, getLinkVidHeight, nullptr, nullptr,
      (AVMEDIA_TYPE_VIDEO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "sample_aspect_ratio", nullptr, nullptr, getLinkSAR, nullptr, nullptr,
      (AVMEDIA_TYPE_VIDEO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "channel_count", nullptr, nullptr, getLinkChannelCount, nullptr, nullptr,
      (AVMEDIA_TYPE_AUDIO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "channel_layout", nullptr, nullptr, getLinkChannelLayout, nullptr, nullptr,
      (AVMEDIA_TYPE_AUDIO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "sample_rate", nullptr, nullptr, getLinkSampleRate, nullptr, nullptr,
      (AVMEDIA_TYPE_AUDIO == link->type) ? napi_enumerable : napi_default, (void*)link },
    { "format", nullptr, nullptr, getLinkFormat, nullptr, nullptr, napi_enumerable, (void*)link },
    { "time_base", nullptr, nullptr, getLinkTimeBase, nullptr, nullptr, napi_enumerable, (void*)link }
  };
  status = napi_define_properties(env, *result, 13, desc);
  PASS_STATUS;

  return napi_ok;
}

napi_value getFilter(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = fromAVFilter(env, filterContext->filter, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFilterContextName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = napi_create_string_utf8(env, filterContext->name, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFiltCtxInputs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value array, element;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  uint32_t numInputs = filterContext->nb_inputs;
  if (0 == numInputs) {
    status = napi_get_null(env, &array);
  } else {
    status = napi_create_array(env, &array);
    CHECK_STATUS;
    for (uint32_t i = 0; i < numInputs; ++i) {
      status = fromAVFilterLink(env, filterContext->inputs[i], &element);
      CHECK_STATUS;
      status = napi_set_element(env, array, i, element);
      CHECK_STATUS;
    }
  }

  CHECK_STATUS;
  return array;
}

napi_value getFiltCtxOutputs(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value array, element;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  uint32_t numOutputs = filterContext->nb_outputs;
  if (0 == numOutputs) {
    status = napi_get_null(env, &array);
  } else {
    status = napi_create_array(env, &array);
    CHECK_STATUS;
    for (uint32_t i = 0; i < numOutputs; ++i) {
      status = fromAVFilterLink(env, filterContext->outputs[i], &element);
      CHECK_STATUS;
      status = napi_set_element(env, array, i, element);
      CHECK_STATUS;
    }
  }

  CHECK_STATUS;
  return array;
}

napi_value getFilterCtxThreadType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = napi_create_uint32(env, filterContext->thread_type, &result);
  CHECK_STATUS;

  return result;
}

napi_value getNumThreads(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = napi_create_uint32(env, filterContext->nb_threads, &result);
  CHECK_STATUS;

  return result;
}

napi_value getReady(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = napi_create_uint32(env, filterContext->ready, &result);
  CHECK_STATUS;

  return result;
}

napi_value getExtraHwFrames(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterContext* filterContext;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterContext);
  CHECK_STATUS;

  status = napi_create_int32(env, filterContext->extra_hw_frames, &result);
  CHECK_STATUS;

  return result;
}

napi_status fromAVFilterCtx(napi_env env, AVFilterContext* filtCtx, napi_value* result) {
  napi_status status;
  napi_value typeName;

  status = napi_create_object(env, result);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "FilterContext", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "filter", nullptr, nullptr, getFilter, nullptr, nullptr, napi_enumerable, filtCtx },
    { "name", nullptr, nullptr, getFilterContextName, nullptr, nullptr, napi_enumerable, filtCtx },
    { "inputs", nullptr, nullptr, getFiltCtxInputs, nullptr, nullptr, napi_enumerable, filtCtx },
    { "outputs", nullptr, nullptr, getFiltCtxOutputs, nullptr, nullptr, napi_enumerable, filtCtx },
    { "priv", nullptr, nullptr, getFilterCtxPrivData, setFilterCtxPrivData, nullptr, napi_enumerable, filtCtx },
    { "thread_type", nullptr, nullptr, getFilterCtxThreadType, nullptr, nullptr, napi_enumerable, filtCtx },
    { "nb_threads", nullptr, nullptr, getNumThreads, nullptr, nullptr, napi_enumerable, filtCtx },
    { "ready", nullptr, nullptr, getReady, nullptr, nullptr, napi_enumerable, filtCtx },
    { "extra_hw_frames", nullptr, nullptr, getExtraHwFrames, nullptr, nullptr, napi_enumerable, filtCtx }
  };
  status = napi_define_properties(env, *result, 10, desc);
  PASS_STATUS;

  return napi_ok;
}

napi_value getGraphFilters(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  status = napi_create_array(env, &result);
  CHECK_STATUS;

  for ( uint32_t f = 0 ; f < filterGraph->nb_filters ; ++f ) {
    if (filterGraph->filters[f] == nullptr) continue;
    status = fromAVFilterCtx(env, filterGraph->filters[f], &element);
    CHECK_STATUS;
    status = napi_set_element(env, result, f, element);
    CHECK_STATUS;
  }

  return result;
}

napi_value getGraphScaleOpts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  // printf("sws_scale_opts: \'%s\'\n", filterGraph->scale_sws_opts);
  if (nullptr == filterGraph->scale_sws_opts)
    status = napi_get_null(env, &result);
  else
    status = napi_create_string_utf8(env, filterGraph->scale_sws_opts, NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getGraphThreads(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  status = napi_create_int32(env, filterGraph->nb_threads, &result);
  CHECK_STATUS;

  return result;
}

napi_value getGraphThreadType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  status = napi_create_int32(env, filterGraph->thread_type, &result);
  CHECK_STATUS;

  return result;
}

napi_value dumpGraph(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  status = napi_create_string_utf8(env,
    avfilter_graph_dump(filterGraph, nullptr),
    NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value getFilterGraph(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, typeName;
  AVFilterGraph* filterGraph;

  status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**) &filterGraph);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = napi_create_string_utf8(env, "FilterGraph", NAPI_AUTO_LENGTH, &typeName);
  CHECK_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "filters", nullptr, nullptr, getGraphFilters, nullptr, nullptr, napi_enumerable, filterGraph },
    { "scale_sws_opts", nullptr, nullptr, getGraphScaleOpts, nullptr, nullptr, napi_enumerable, filterGraph },
    { "thread_type", nullptr, nullptr, getGraphThreadType, nullptr, nullptr, napi_enumerable, filterGraph },
    { "nb_threads", nullptr, nullptr, getGraphThreads, nullptr, nullptr, napi_enumerable, filterGraph },
    { "dump", nullptr, dumpGraph, nullptr, nullptr, nullptr, napi_enumerable, filterGraph }
  };

  status = napi_define_properties(env, result, 6, desc);
  CHECK_STATUS;

  return result;
}

class filtContexts {
public:
  filtContexts() {}
  ~filtContexts() {}

  bool add(const std::string &name, AVFilterContext *context) {
    mContextNames.push_back(name);
    auto result = mFiltContexts.emplace(name, context);
    return result.second;
  }

  AVFilterContext *getContext(const std::string &name) const {
    AVFilterContext *result = nullptr;
    auto c = mFiltContexts.find(name);
    if (c != mFiltContexts.end())
      result = c->second;
    return result;
  }

  const std::vector<std::string>& getNames() const  { return mContextNames; }

private:
  std::map<std::string, AVFilterContext *> mFiltContexts;
  std::vector<std::string> mContextNames;
};

struct filtererCarrier : carrier {
  std::string filterType;
  std::vector<std::string> inNames;
  std::vector<std::string> inParams;
  std::vector<std::string> outNames;
  std::vector<std::map<std::string, std::string> > outParams;
  std::string filterSpec;

  filtContexts *srcCtxs = nullptr;
  filtContexts *sinkCtxs = nullptr;
  AVFilterGraph *filterGraph = nullptr;
  ~filtererCarrier() {}
};

void graphFinalizer(napi_env env, void* data, void* hint) {
  AVFilterGraph* graph = (AVFilterGraph*)data;
  avfilter_graph_free(&graph);
}

void ctxsFinalizer(napi_env env, void* data, void* hint) {
  filtContexts* ctxs = (filtContexts*)data;
  delete ctxs;
}

void filtererExecute(napi_env env, void* data) {
  filtererCarrier* c = (filtererCarrier*) data;
  int ret = 0;

  c->filterGraph = avfilter_graph_alloc();

  AVFilterInOut **inputs = new AVFilterInOut*[c->outNames.size()];
  bool inAlloc = true;
  for (size_t i = 0; i < c->outNames.size(); ++i)
    if (!(inAlloc = (inputs[i] = avfilter_inout_alloc()) != NULL)) break;

  AVFilterInOut **outputs = new AVFilterInOut*[c->inParams.size()];
  bool opAlloc = true;
  for (size_t i = 0; i < c->inParams.size(); ++i)
    if (!(opAlloc = (outputs[i] = avfilter_inout_alloc()) != NULL)) break;

  if (!(inAlloc && opAlloc && c->filterGraph)) {
    c->status = BEAMCODER_ERROR_ENOMEM;
    c->errorMsg = "Failed to allocate filter resources.";
    goto end;
  }

  c->srcCtxs = new filtContexts;
  for (size_t i = 0; i < c->inParams.size(); ++i) {
    const AVFilter *buffersrc  = avfilter_get_by_name(0 == c->filterType.compare("audio")?"abuffer":"buffer");
    AVFilterContext *srcCtx = nullptr;
    ret = avfilter_graph_create_filter(&srcCtx, buffersrc, c->inNames[i].c_str(), c->inParams[i].c_str(), NULL, c->filterGraph);
    if (ret < 0) {
      c->status = BEAMCODER_ERROR_ENOMEM;
      c->errorMsg = "Failed to allocate source filter graph.";
      goto end;
    }
    outputs[i]->name       = av_strdup(c->inNames[i].c_str());
    outputs[i]->filter_ctx = srcCtx;
    outputs[i]->pad_idx    = 0;
    outputs[i]->next       = i < c->inParams.size() - 1 ? outputs[i + 1] : NULL;

    if (!c->srcCtxs->add(c->inNames[i], srcCtx)) {
      c->status = BEAMCODER_ERROR_EINVAL;
      c->errorMsg = "Filter sources must have unique names.";
      goto end;
    }
  }

  c->sinkCtxs = new filtContexts;
  for (size_t i = 0; i < c->outParams.size(); ++i) {
    const AVFilter *buffersink = avfilter_get_by_name(0 == c->filterType.compare("audio")?"abuffersink":"buffersink");
    AVFilterContext *sinkCtx = nullptr;
    ret = avfilter_graph_create_filter(&sinkCtx, buffersink, c->outNames[i].c_str(), NULL, NULL, c->filterGraph);
    if (ret < 0) {
      c->status = BEAMCODER_ERROR_ENOMEM;
      c->errorMsg = "Failed to allocate sink filter graph.";
      goto end;
    }
    if (0 == c->filterType.compare("audio")) {
      auto p = c->outParams[i].find("sample_rates");
      if (p != c->outParams[i].end()) {
        const int out_sample_rates[] = { std::stoi(p->second.c_str()), -1 };
        ret = av_opt_set_int_list(sinkCtx, "sample_rates", out_sample_rates, -1,
                                  AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) { av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n"); }
      }
      p = c->outParams[i].find("sample_fmts");
      if (p != c->outParams[i].end()) {
        const enum AVSampleFormat out_sample_fmts[] = { av_get_sample_fmt(p->second.c_str()), AV_SAMPLE_FMT_NONE };
        ret = av_opt_set_int_list(sinkCtx, "sample_fmts", out_sample_fmts, AV_SAMPLE_FMT_NONE,
                                  AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) { av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n"); }
      }
      p = c->outParams[i].find("channel_layouts");
      if (p != c->outParams[i].end()) {
        const int64_t out_channel_layouts[] = { (int64_t)av_get_channel_layout(p->second.c_str()), -1 };
        ret = av_opt_set_int_list(sinkCtx, "channel_layouts", out_channel_layouts, -1,
                                  AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) { av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n"); }
      }
    } else {
      auto p = c->outParams[i].find("pix_fmts");
      if (p != c->outParams[i].end()) {
        enum AVPixelFormat pix_fmts[] = { av_get_pix_fmt(p->second.c_str()), AV_PIX_FMT_NONE };
        ret = av_opt_set_int_list(sinkCtx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE,
                                  AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) { av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n"); }
      }
    }

    inputs[i]->name       = av_strdup(c->outNames[i].c_str());
    inputs[i]->filter_ctx = sinkCtx;
    inputs[i]->pad_idx    = 0;
    inputs[i]->next       = i < c->outNames.size() - 1 ? inputs[i + 1] : NULL;

    if (!c->sinkCtxs->add(c->outNames[i], sinkCtx)) {
      c->status = BEAMCODER_ERROR_EINVAL;
      c->errorMsg = "Filter sinks must have unique names.";
      goto end;
    }
  }

  if ((ret = avfilter_graph_parse_ptr(c->filterGraph, c->filterSpec.c_str(), inputs, outputs, NULL)) < 0) {
    c->status = BEAMCODER_ERROR_ENOMEM;
    c->errorMsg = "Failed to parse filter graph.";
    goto end;
  }

  if ((ret = avfilter_graph_config(c->filterGraph, NULL)) < 0) {
    c->status = BEAMCODER_ERROR_ENOMEM;
    c->errorMsg = "Failed to configure filter graph.";
    goto end;
  }

end:
  avfilter_inout_free(inputs);
  avfilter_inout_free(outputs);
  delete[] outputs;
}

void filtererComplete(napi_env env, napi_status asyncStatus, void* data) {
  filtererCarrier* c = (filtererCarrier*) data;
  napi_value result, typeName, filterGraphValue, srcContextsValue, sinkContextsValue;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Filterer allocator failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_object(env, &result);
  REJECT_STATUS;
  c->status = napi_create_string_utf8(env, "Filterer", NAPI_AUTO_LENGTH, &typeName);
  REJECT_STATUS;

  c->status = napi_create_external(env, c->filterGraph, graphFinalizer, nullptr, &filterGraphValue);
  REJECT_STATUS;
  c->status = napi_create_external(env, c->srcCtxs, ctxsFinalizer, nullptr, &srcContextsValue);
  REJECT_STATUS;
  c->status = napi_create_external(env, c->sinkCtxs, ctxsFinalizer, nullptr, &sinkContextsValue);
  REJECT_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "graph", nullptr, nullptr, getFilterGraph, nullptr, nullptr, napi_enumerable, c->filterGraph },
    { "filter", nullptr, filter, nullptr, nullptr, nullptr, napi_enumerable, nullptr },
    { "_filterGraph", nullptr, nullptr, nullptr, nullptr, filterGraphValue, napi_default, nullptr },
    { "_sourceContexts", nullptr, nullptr, nullptr, nullptr, srcContextsValue, napi_default, nullptr },
    { "_sinkContexts", nullptr, nullptr, nullptr, nullptr, sinkContextsValue, napi_default, nullptr }
  };
  c->status = napi_define_properties(env, result, 6, desc);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value filterer(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise;
  napi_valuetype type;
  filtererCarrier* c = new filtererCarrier;
  bool isArray;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];

  c->status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  REJECT_RETURN;

  if (argc != 1) {
    REJECT_ERROR_RETURN("Filterer requires a single options object.",
      BEAMCODER_INVALID_ARGS);
  }

  c->status = napi_typeof(env, args[0], &type);
  REJECT_RETURN;
  c->status = napi_is_array(env, args[0], &isArray);
  REJECT_RETURN;
  if ((type != napi_object) || (isArray == true)) {
    REJECT_ERROR_RETURN("Filterer must be configured with a single parameter, an options object.",
      BEAMCODER_INVALID_ARGS);
  }

  bool hasFilterType, hasInParams, hasOutParams, hasFilterSpec;
  c->status = napi_has_named_property(env, args[0], "filterType", &hasFilterType);
  REJECT_RETURN;
  c->status = napi_has_named_property(env, args[0], "inputParams", &hasInParams);
  REJECT_RETURN;
  c->status = napi_has_named_property(env, args[0], "outputParams", &hasOutParams);
  REJECT_RETURN;
  c->status = napi_has_named_property(env, args[0], "filterSpec", &hasFilterSpec);
  REJECT_RETURN;

  if (!(hasFilterType && hasInParams && hasOutParams && hasFilterSpec)) {
    REJECT_ERROR_RETURN("Filterer parameter object requires type, inputParams, outputParams and filterSpec to be defined.",
      BEAMCODER_INVALID_ARGS);
  }

  napi_value filterTypeVal;
  c->status = napi_get_named_property(env, args[0], "filterType", &filterTypeVal);
  REJECT_RETURN;
  size_t filterTypeLen;
  c->status = napi_get_value_string_utf8(env, filterTypeVal, nullptr, 0, &filterTypeLen);
  REJECT_RETURN;
  c->filterType.resize(filterTypeLen);
  c->status = napi_get_value_string_utf8(env, filterTypeVal, (char *)c->filterType.data(), filterTypeLen+1, nullptr);
  REJECT_RETURN;
  if ((0 != c->filterType.compare("audio")) && (0 != c->filterType.compare("video"))) {
    REJECT_ERROR_RETURN("Filterer expects filterType of audio or video.",
      BEAMCODER_INVALID_ARGS);
  }

  napi_value paramsArrayVal;
  uint32_t paramsArrayLen;
  c->status = napi_get_named_property(env, args[0], "inputParams", &paramsArrayVal);
  REJECT_RETURN;
  c->status = napi_is_array(env, paramsArrayVal, &isArray);
  REJECT_RETURN;
  if (!isArray) {
    // Might be array like, as proxied by VM2 decontextify
    napi_value nameResult;
    bool hasElement;
    c->status = napi_get_property_names(env, paramsArrayVal, &nameResult);
    REJECT_RETURN;
    c->status = napi_get_array_length(env, nameResult, &paramsArrayLen);
    REJECT_RETURN;
    for ( uint32_t i = 0 ; i < paramsArrayLen ; ++i ) {
      c->status = napi_has_element(env, paramsArrayVal, i, &hasElement);
      REJECT_RETURN;
      if (!hasElement) {
        REJECT_ERROR_RETURN("Filterer inputParams must be an array or array-like.",
          BEAMCODER_INVALID_ARGS);
      }
    }
  } else {
    c->status = napi_get_array_length(env, paramsArrayVal, &paramsArrayLen);
    REJECT_RETURN;
  }

  for (uint32_t i = 0; i < paramsArrayLen; ++i) {
    napi_value inParamsVal;
    c->status = napi_get_element(env, paramsArrayVal, i, &inParamsVal);
    REJECT_RETURN;

    std::string name;
    uint32_t sampleRate;
    std::string sampleFormat;
    std::string channelLayout;
    uint32_t width;
    uint32_t height;
    std::string pixFmt;
    AVRational timeBase;
    AVRational pixelAspect;

    bool hasNameVal;
    c->status = napi_has_named_property(env, inParamsVal, "name", &hasNameVal);
    REJECT_RETURN;
    if (!hasNameVal && (i > 0)) {
      REJECT_ERROR_RETURN("Filterer inputParams must include a name value if there is more than one input.",
        BEAMCODER_INVALID_ARGS);
    }
    if (hasNameVal) {
      napi_value nameVal;
      c->status = napi_get_named_property(env, inParamsVal, "name", &nameVal);
      REJECT_RETURN;
      size_t nameLen;
      c->status = napi_get_value_string_utf8(env, nameVal, nullptr, 0, &nameLen);
      REJECT_RETURN;
      name.resize(nameLen);
      c->status = napi_get_value_string_utf8(env, nameVal, (char *)name.data(), nameLen+1, nullptr);
      REJECT_RETURN;
      c->inNames.push_back(name);
    } else
      c->inNames.push_back("in");

    uint32_t arrayLen;
    if (0 == c->filterType.compare("audio")) {
      napi_value sampleRateVal;
      c->status = napi_get_named_property(env, inParamsVal, "sampleRate", &sampleRateVal);
      REJECT_RETURN;
      c->status = napi_get_value_uint32(env, sampleRateVal, &sampleRate);
      REJECT_RETURN;

      napi_value sampleFmtVal;
      c->status = napi_get_named_property(env, inParamsVal, "sampleFormat", &sampleFmtVal);
      REJECT_RETURN;
      size_t sampleFmtLen;
      c->status = napi_get_value_string_utf8(env, sampleFmtVal, nullptr, 0, &sampleFmtLen);
      REJECT_RETURN;
      sampleFormat.resize(sampleFmtLen);
      c->status = napi_get_value_string_utf8(env, sampleFmtVal, (char *)sampleFormat.data(), sampleFmtLen+1, nullptr);
      REJECT_RETURN;

      napi_value channelLayoutVal;
      c->status = napi_get_named_property(env, inParamsVal, "channelLayout", &channelLayoutVal);
      REJECT_RETURN;
      size_t channelLayoutLen;
      c->status = napi_get_value_string_utf8(env, channelLayoutVal, nullptr, 0, &channelLayoutLen);
      REJECT_RETURN;
      channelLayout.resize(channelLayoutLen);
      c->status = napi_get_value_string_utf8(env, channelLayoutVal, (char *)channelLayout.data(), channelLayoutLen+1, nullptr);
      REJECT_RETURN;
    } else {
      napi_value widthVal;
      c->status = napi_get_named_property(env, inParamsVal, "width", &widthVal);
      REJECT_RETURN;
      c->status = napi_get_value_uint32(env, widthVal, &width);
      REJECT_RETURN;
      napi_value heightVal;
      c->status = napi_get_named_property(env, inParamsVal, "height", &heightVal);
      REJECT_RETURN;
      c->status = napi_get_value_uint32(env, heightVal, &height);
      REJECT_RETURN;

      napi_value pixFmtVal;
      c->status = napi_get_named_property(env, inParamsVal, "pixelFormat", &pixFmtVal);
      REJECT_RETURN;
      size_t pixFmtLen;
      c->status = napi_get_value_string_utf8(env, pixFmtVal, nullptr, 0, &pixFmtLen);
      REJECT_RETURN;
      pixFmt.resize(pixFmtLen);
      c->status = napi_get_value_string_utf8(env, pixFmtVal, (char *)pixFmt.data(), pixFmtLen+1, nullptr);
      REJECT_RETURN;

      napi_value pixelAspectVal;
      c->status = napi_get_named_property(env, inParamsVal, "pixelAspect", &pixelAspectVal);
      REJECT_RETURN;
      c->status = napi_is_array(env, pixelAspectVal, &isArray);
      REJECT_RETURN;
      if (isArray) {
        c->status = napi_get_array_length(env, pixelAspectVal, &arrayLen);
        REJECT_RETURN;
      } else {
        napi_value arrayLikeProps;
        c->status = napi_get_property_names(env, pixelAspectVal, &arrayLikeProps);
        REJECT_RETURN;
        c->status = napi_get_array_length(env, arrayLikeProps, &arrayLen);
        REJECT_RETURN;
      }
      if (2 != arrayLen) {
        REJECT_ERROR_RETURN("Filterer inputParams pixelAspect must be an array with 2 values representing a rational number.",
          BEAMCODER_INVALID_ARGS);
      }
      for (uint32_t i = 0; i < arrayLen; ++i) {
        napi_value arrayVal;
        c->status = napi_get_element(env, pixelAspectVal, i, &arrayVal); // Will detect if not array or array-like
        REJECT_RETURN;
        c->status = napi_get_value_int32(env, arrayVal, (0==i)?&pixelAspect.num:&pixelAspect.den);
        REJECT_RETURN;
      }
    }

    napi_value timeBaseVal;
    c->status = napi_get_named_property(env, inParamsVal, "timeBase", &timeBaseVal);
    REJECT_RETURN;
    c->status = napi_is_array(env, timeBaseVal, &isArray);
    REJECT_RETURN;
    if (isArray) {
      c->status = napi_get_array_length(env, timeBaseVal, &arrayLen);
      REJECT_RETURN;
    } else {
        napi_value arrayLikeProps;
        c->status = napi_get_property_names(env, timeBaseVal, &arrayLikeProps);
        REJECT_RETURN;
        c->status = napi_get_array_length(env, arrayLikeProps, &arrayLen);
        REJECT_RETURN;
    }
    if (2 != arrayLen) {
      REJECT_ERROR_RETURN("Filterer inputParams timeBase must be an array with 2 values representing a rational number.",
        BEAMCODER_INVALID_ARGS);
    }
    for (uint32_t i = 0; i < arrayLen; ++i) {
      napi_value arrayVal;
      c->status = napi_get_element(env, timeBaseVal, i, &arrayVal); // Will detect if not array or array-like
      REJECT_RETURN;
      c->status = napi_get_value_int32(env, arrayVal, (0==i)?&timeBase.num:&timeBase.den);
      REJECT_RETURN;
    }

    char args[512];
    if (0 == c->filterType.compare("audio")) {
      snprintf(args, sizeof(args),
              "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%" PRIu64 "",
              timeBase.num, timeBase.den, sampleRate,
              sampleFormat.c_str(), av_get_channel_layout(channelLayout.c_str()));
    } else {
      snprintf(args, sizeof(args),
              "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
              width, height, av_get_pix_fmt(pixFmt.c_str()),
              timeBase.num, timeBase.den, pixelAspect.num, pixelAspect.den);
    }
    c->inParams.push_back(args);
  }

  c->status = napi_get_named_property(env, args[0], "outputParams", &paramsArrayVal);
  REJECT_RETURN;
  c->status = napi_is_array(env, paramsArrayVal, &isArray);
  REJECT_RETURN;
  if (!isArray) {    // Might be array like, as proxied by VM2 decontextify
    napi_value nameResult;
    bool hasElement;
    c->status = napi_get_property_names(env, paramsArrayVal, &nameResult);
    REJECT_RETURN;
    c->status = napi_get_array_length(env, nameResult, &paramsArrayLen);
    REJECT_RETURN;
    for ( uint32_t i = 0 ; i < paramsArrayLen ; ++i ) {
      c->status = napi_has_element(env, paramsArrayVal, i, &hasElement);
      REJECT_RETURN;
      if (!hasElement) {
        REJECT_ERROR_RETURN("Filterer outputParams must be an array.",
          BEAMCODER_INVALID_ARGS);
      }
    }
  } else {
    c->status = napi_get_array_length(env, paramsArrayVal, &paramsArrayLen);
    REJECT_RETURN;
  }

  for (uint32_t i = 0; i < paramsArrayLen; ++i) {
    napi_value outParamsVal;
    c->status = napi_get_element(env, paramsArrayVal, i, &outParamsVal);
    REJECT_RETURN;

    std::string name;
    uint32_t sampleRate;
    std::string sampleFormat;
    std::string channelLayout;
    std::string pixFmt;

    bool hasNameVal;
    c->status = napi_has_named_property(env, outParamsVal, "name", &hasNameVal);
    REJECT_RETURN;
    if (!hasNameVal && (i > 0)) {
      REJECT_ERROR_RETURN("Filterer outputParams must include a name value if there is more than one output.",
        BEAMCODER_INVALID_ARGS);
    }
    if (hasNameVal) {
      napi_value nameVal;
      c->status = napi_get_named_property(env, outParamsVal, "name", &nameVal);
      REJECT_RETURN;
      size_t nameLen;
      c->status = napi_get_value_string_utf8(env, nameVal, nullptr, 0, &nameLen);
      REJECT_RETURN;
      name.resize(nameLen);
      c->status = napi_get_value_string_utf8(env, nameVal, (char *)name.data(), nameLen+1, nullptr);
      REJECT_RETURN;
      c->outNames.push_back(name);
    } else
      c->outNames.push_back("out");

    if (0 == c->filterType.compare("audio")) {
      napi_value sampleRateVal;
      c->status = napi_get_named_property(env, outParamsVal, "sampleRate", &sampleRateVal);
      REJECT_RETURN;
      c->status = napi_get_value_uint32(env, sampleRateVal, &sampleRate);
      REJECT_RETURN;

      napi_value sampleFmtVal;
      c->status = napi_get_named_property(env, outParamsVal, "sampleFormat", &sampleFmtVal);
      REJECT_RETURN;
      size_t sampleFmtLen;
      c->status = napi_get_value_string_utf8(env, sampleFmtVal, nullptr, 0, &sampleFmtLen);
      REJECT_RETURN;
      sampleFormat.resize(sampleFmtLen);
      c->status = napi_get_value_string_utf8(env, sampleFmtVal, (char *)sampleFormat.data(), sampleFmtLen+1, nullptr);
      REJECT_RETURN;

      napi_value channelLayoutVal;
      c->status = napi_get_named_property(env, outParamsVal, "channelLayout", &channelLayoutVal);
      REJECT_RETURN;
      size_t channelLayoutLen;
      c->status = napi_get_value_string_utf8(env, channelLayoutVal, nullptr, 0, &channelLayoutLen);
      REJECT_RETURN;
      channelLayout.resize(channelLayoutLen);
      c->status = napi_get_value_string_utf8(env, channelLayoutVal, (char *)channelLayout.data(), channelLayoutLen+1, nullptr);
      REJECT_RETURN;

      std::map<std::string, std::string> paramMap;
      paramMap.emplace("sample_rates", std::to_string(sampleRate));
      paramMap.emplace("sample_fmts", sampleFormat);
      paramMap.emplace("channel_layouts", channelLayout);
      c->outParams.push_back(paramMap);
    } else {
      napi_value pixFmtVal;
      c->status = napi_get_named_property(env, outParamsVal, "pixelFormat", &pixFmtVal);
      REJECT_RETURN;
      size_t pixFmtLen;
      c->status = napi_get_value_string_utf8(env, pixFmtVal, nullptr, 0, &pixFmtLen);
      REJECT_RETURN;
      pixFmt.resize(pixFmtLen);
      c->status = napi_get_value_string_utf8(env, pixFmtVal, (char *)pixFmt.data(), pixFmtLen+1, nullptr);
      REJECT_RETURN;

      std::map<std::string, std::string> paramMap;
      paramMap.emplace("pix_fmts", pixFmt);
      c->outParams.push_back(paramMap);
    }
  }

  napi_value filterSpecJS;
  c->status = napi_get_named_property(env, args[0], "filterSpec", &filterSpecJS);
  REJECT_RETURN;
  size_t specLen;
  c->status = napi_get_value_string_utf8(env, filterSpecJS, nullptr, 0, &specLen);
  REJECT_RETURN;
  c->filterSpec.resize(specLen);
  c->status = napi_get_value_string_utf8(env, filterSpecJS, (char *)c->filterSpec.data(), specLen+1, nullptr);
  REJECT_RETURN;

  c->status = napi_create_string_utf8(env, "Filterer", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, filtererExecute,
    filtererComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}


struct filterCarrier : carrier {
  filtContexts *srcCtxs = nullptr;
  filtContexts *sinkCtxs = nullptr;
  std::unordered_map<std::string, std::deque<AVFrame *> > srcFrames;
  std::unordered_map<std::string, std::vector<AVFrame *> > dstFrames;
  std::vector<napi_ref> frameRefs;
  ~filterCarrier() {}
};

namespace {
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

  AVFrame* getFrame(napi_env env, napi_value frame) {
    napi_status status;
    napi_value value;
    frameData* result = nullptr;
    status = napi_get_named_property(env, frame, "_frame", &value);
    if (status != napi_ok) return nullptr;
    status = napi_get_value_external(env, value, (void**)&result);
    if (status != napi_ok) return nullptr;

    return result->frame;
  }
}

void filterExecute(napi_env env, void* data) {
  filterCarrier* c = (filterCarrier*) data;
  int ret = 0;
  HR_TIME_POINT filterStart = NOW;

  for (auto it = c->srcFrames.begin(); it != c->srcFrames.end(); ++it) {
    AVFilterContext *srcCtx = c->srcCtxs->getContext(it->first);
    if (!srcCtx) {
      c->status = BEAMCODER_INVALID_ARGS;
      c->errorMsg = "Frame name not found in source contexts.";
      return;
    }

    std::deque<AVFrame *> frames = it->second;
    while (frames.size() > 0) {
      ret = av_buffersrc_add_frame_flags(srcCtx, frames.front(), AV_BUFFERSRC_FLAG_KEEP_REF);
      if (ret < 0) {
        c->status = BEAMCODER_ERROR_FILTER_ADD_FRAME;
        c->errorMsg = "Error while feeding the filtergraph.";
        return;
      }
      frames.pop_front();
    }
  }

  std::vector<std::string> sinkNames = c->sinkCtxs->getNames();
  for (auto it = sinkNames.begin(); it != sinkNames.end(); ++it) {
    std::vector<AVFrame *> frames;
    while (1) {
      AVFrame *filtFrame = av_frame_alloc();
      AVFilterContext *sinkCtx = c->sinkCtxs->getContext(*it);
      if (!sinkCtx) {
        c->status = BEAMCODER_INVALID_ARGS;
        c->errorMsg = "Sink name not found in sink contexts.";
        return;
      }
      ret = av_buffersink_get_frame(sinkCtx, filtFrame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        break;
      if (ret < 0) {
        c->status = BEAMCODER_ERROR_FILTER_GET_FRAME;
        c->errorMsg = "Error while filtering.";
        return;
      }
      frames.push_back(filtFrame);
    }
    c->dstFrames.emplace(*it, frames);
  }
  c->totalTime = microTime(filterStart);
};

void filterComplete(napi_env env, napi_status asyncStatus, void* data) {
  filterCarrier* c = (filterCarrier*) data;
  napi_value result, dstFrame, nameVal, frames, frame, prop;

  for (auto it = c->frameRefs.cbegin(); it != c->frameRefs.cend(); it++) {
    c->status = napi_delete_reference(env, *it);
    REJECT_STATUS;
  }

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "Filter failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_array(env, &result);
  REJECT_STATUS;

  uint32_t outCount = 0;
  for (auto it = c->dstFrames.begin(); it != c->dstFrames.end(); it++) {
    c->status = napi_create_object(env, &dstFrame);
    REJECT_STATUS;

    c->status = napi_create_array(env, &frames);
    REJECT_STATUS;

    c->status = napi_create_string_utf8(env, it->first.c_str(), NAPI_AUTO_LENGTH, &nameVal);
    REJECT_STATUS;
    c->status = napi_set_named_property(env, dstFrame, "name", nameVal);
    REJECT_STATUS;

    uint32_t frameCount = 0;
    for (auto fit = it->second.begin(); fit != it->second.end(); ++fit) {
      frameData* f = new frameData;
      f->frame = *fit;

      c->status = fromAVFrame(env, f, &frame);
      REJECT_STATUS;

      c->status = napi_set_element(env, frames, frameCount++, frame);
      REJECT_STATUS;
    }
    c->status = napi_set_named_property(env, dstFrame, "frames", frames);
    REJECT_STATUS;

    c->status = napi_set_element(env, result, outCount++, dstFrame);
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

napi_value filter(napi_env env, napi_callback_info info) {
  napi_value resourceName, promise, filtererJS, value;
  filterCarrier* c = new filterCarrier;
  napi_ref frameRef;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];

  c->status = napi_get_cb_info(env, info, &argc, args, &filtererJS, nullptr);
  REJECT_RETURN;

  napi_value srcCtxsExt, sinkCtxsExt;
  c->status = napi_get_named_property(env, filtererJS, "_sourceContexts", &srcCtxsExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, srcCtxsExt, (void**)&c->srcCtxs);
  REJECT_RETURN;

  c->status = napi_get_named_property(env, filtererJS, "_sinkContexts", &sinkCtxsExt);
  REJECT_RETURN;
  c->status = napi_get_value_external(env, sinkCtxsExt, (void**)&c->sinkCtxs);
  REJECT_RETURN;

  if (argc != 1) {
    REJECT_ERROR_RETURN("Filter requires source frame array.",
      BEAMCODER_INVALID_ARGS);
  }

  bool isArray;
  c->status = napi_is_array(env, args[0], &isArray);
  // REJECT_RETURN;
  // if (!isArray) // Allow array-like objects
  //   REJECT_ERROR_RETURN("Expected an array of source frame objects.",
  //     BEAMCODER_INVALID_ARGS);

  napi_value item;
  c->status = napi_get_element(env, args[0], 0, &item);
  if (c->status != napi_ok) {
    REJECT_ERROR_RETURN("Expected an array or array-like object of source frame objects.",
      BEAMCODER_INVALID_ARGS);
  }
  if (napi_ok == isFrame(env, item)) {
    // Simplest case of an array of frame objects
    uint32_t framesLen;
    if (isArray) {
      c->status = napi_get_array_length(env, args[0], &framesLen);
      REJECT_RETURN;
    } else {
      napi_value propNames;
      c->status = napi_get_property_names(env, args[0], &propNames);
      REJECT_RETURN;
      c->status = napi_get_array_length(env, propNames, &framesLen);
      REJECT_RETURN;
    }
    for (uint32_t f = 0; f < framesLen; ++f) {
      c->status = napi_get_element(env, args[0], f, &value);
      REJECT_RETURN;
      c->status = isFrame(env, value);
      if (c->status != napi_ok) {
        REJECT_ERROR_RETURN("Expected an array whose elements must be of type frame.",
          BEAMCODER_INVALID_ARGS);
      }
    }
    std::deque<AVFrame *> frames;
    for (uint32_t f = 0; f < framesLen; ++f) {
      c->status = napi_get_element(env, args[0], f, &value);
      REJECT_RETURN;
      c->status = napi_create_reference(env, value, 1, &frameRef);
      REJECT_RETURN;

      c->frameRefs.push_back(frameRef);
      frames.push_back(getFrame(env, value));
    }
    auto result = c->srcFrames.emplace("in", frames);
    if (!result.second) {
      REJECT_ERROR_RETURN("Frame names must be unique.",
        BEAMCODER_INVALID_ARGS);
    }
  } else {
    // Argument is an array of filter objects with name and frames members
    uint32_t srcsLen;
    c->status = napi_get_array_length(env, args[0], &srcsLen);
    REJECT_RETURN;
    for (uint32_t i = 0; i < srcsLen; ++i) {
      napi_value item;
      c->status = napi_get_element(env, args[0], i, &item);
      REJECT_RETURN;
      std::string name;
      bool hasName;
      c->status = napi_has_named_property(env, item, "name", &hasName);
      REJECT_RETURN;
      if (hasName) {
        napi_value nameVal;
        c->status = napi_get_named_property(env, item, "name", &nameVal);
        REJECT_RETURN;
        size_t nameLen;
        c->status = napi_get_value_string_utf8(env, nameVal, nullptr, 0, &nameLen);
        REJECT_RETURN;
        name.resize(nameLen);
        c->status = napi_get_value_string_utf8(env, nameVal, (char *)name.data(), nameLen+1, nullptr);
        REJECT_RETURN;
      } else if (0 == i) {
        name = "in";
      } else {
        REJECT_ERROR_RETURN("Source frame object requires a name.",
          BEAMCODER_INVALID_ARGS);
      }

      napi_value framesArrVal;
      c->status = napi_get_named_property(env, item, "frames", &framesArrVal);
      REJECT_RETURN;

      bool isArray;
      uint32_t framesLen;
      c->status = napi_is_array(env, framesArrVal, &isArray);
      REJECT_RETURN;
      if (isArray) {
        c->status = napi_get_array_length(env, framesArrVal, &framesLen);
        REJECT_RETURN;
      } else {
        napi_value propNames;
        c->status = napi_get_property_names(env, framesArrVal, &propNames);
        REJECT_RETURN;
        c->status = napi_get_array_length(env, propNames, &framesLen);
        REJECT_RETURN;
      }

      std::deque<AVFrame *> frames;
      for (uint32_t f = 0; f < framesLen; ++f) {
        c->status = napi_get_element(env, framesArrVal, f, &value);
        REJECT_RETURN; // Blow up here if not array or array-like
        c->status = isFrame(env, value);
        if (c->status != napi_ok) {
          REJECT_ERROR_RETURN("Values in array must by of type frame.",
            BEAMCODER_INVALID_ARGS);
        }
      }
      for (uint32_t f = 0; f < framesLen; ++f) {
        c->status = napi_get_element(env, framesArrVal, f, &value);
        REJECT_RETURN;
        c->status = napi_create_reference(env, value, 1, &frameRef);
        REJECT_RETURN;

        c->frameRefs.push_back(frameRef);
        frames.push_back(getFrame(env, value));
      }
      auto result = c->srcFrames.emplace(name, frames);
      if (!result.second) {
        REJECT_ERROR_RETURN("Frame names must be unique.",
          BEAMCODER_INVALID_ARGS);
      }
    }
  }

  c->status = napi_create_string_utf8(env, "Filter", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, filterExecute,
    filterComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
};

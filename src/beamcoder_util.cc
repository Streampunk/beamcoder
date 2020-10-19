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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <string>
#include "beamcoder_util.h"
#include "node_api.h"

napi_status checkStatus(napi_env env, napi_status status,
  const char* file, uint32_t line) {

  napi_status infoStatus, throwStatus;
  const napi_extended_error_info *errorInfo;

  if (status == napi_ok) {
    // printf("Received status OK.\n");
    return status;
  }

  infoStatus = napi_get_last_error_info(env, &errorInfo);
  assert(infoStatus == napi_ok);
  printf("NAPI error in file %s on line %i. Error %i: %s\n", file, line,
    errorInfo->error_code, errorInfo->error_message);

  if (status == napi_pending_exception) {
    printf("NAPI pending exception. Engine error code: %i\n", errorInfo->engine_error_code);
    return status;
  }

  char errorCode[20];
  sprintf(errorCode, "%d", errorInfo->error_code);
  throwStatus = napi_throw_error(env, errorCode, errorInfo->error_message);
  assert(throwStatus == napi_ok);

  return napi_pending_exception; // Expect to be cast to void
}

long long microTime(std::chrono::high_resolution_clock::time_point start) {
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
}

const char* getNapiTypeName(napi_valuetype t) {
  switch (t) {
    case napi_undefined: return "undefined";
    case napi_null: return "null";
    case napi_boolean: return "boolean";
    case napi_number: return "number";
    case napi_string: return "string";
    case napi_symbol: return "symbol";
    case napi_object: return "object";
    case napi_function: return "function";
    case napi_external: return "external";
    default: return "unknown";
  }
}

napi_status checkArgs(napi_env env, napi_callback_info info, char* methodName,
  napi_value* args, size_t argc, napi_valuetype* types) {

  napi_status status;

  size_t realArgc = argc;
  status = napi_get_cb_info(env, info, &realArgc, args, nullptr, nullptr);
  PASS_STATUS;

  if (realArgc != argc) {
    char errorMsg[100];
    sprintf(errorMsg, "For method %s, expected %zi arguments and got %zi.",
      methodName, argc, realArgc);
    napi_throw_error(env, nullptr, errorMsg);
    return napi_pending_exception;
  }

  napi_valuetype t;
  for ( size_t x = 0 ; x < argc ; x++ ) {
    status = napi_typeof(env, args[x], &t);
    PASS_STATUS;
    if (t != types[x]) {
      char errorMsg[100];
      sprintf(errorMsg, "For method %s argument %zu, expected type %s and got %s.",
        methodName, x + 1, getNapiTypeName(types[x]), getNapiTypeName(t));
      napi_throw_error(env, nullptr, errorMsg);
      return napi_pending_exception;
    }
  }

  return napi_ok;
};


void tidyCarrier(napi_env env, carrier* c) {
  napi_status status;
  if (c->passthru != nullptr) {
    status = napi_delete_reference(env, c->passthru);
    FLOATING_STATUS;
  }
  if (c->_request != nullptr) {
    status = napi_delete_async_work(env, c->_request);
    FLOATING_STATUS;
  }
  // printf("Tidying carrier %p %p\n", c->passthru, c->_request);
  delete c;
}

int32_t rejectStatus(napi_env env, carrier* c, char* file, int32_t line) {
  if (c->status != BEAMCODER_SUCCESS) {
    napi_value errorValue, errorCode, errorMsg;
    napi_status status;
    if (c->status < BEAMCODER_ERROR_START) {
      const napi_extended_error_info *errorInfo;
      status = napi_get_last_error_info(env, &errorInfo);
      FLOATING_STATUS;
      c->errorMsg = std::string(
        (errorInfo->error_message != nullptr) ? errorInfo->error_message : "(no message)");
    }
    char* extMsg = (char *) malloc(sizeof(char) * c->errorMsg.length() + 200);
    sprintf(extMsg, "In file %s on line %i, found error: %s", file, line, c->errorMsg.c_str());
    char errorCodeChars[20];
    sprintf(errorCodeChars, "%d", c->status);
    status = napi_create_string_utf8(env, errorCodeChars,
      NAPI_AUTO_LENGTH, &errorCode);
    FLOATING_STATUS;
    status = napi_create_string_utf8(env, extMsg, NAPI_AUTO_LENGTH, &errorMsg);
    FLOATING_STATUS;
    status = napi_create_error(env, errorCode, errorMsg, &errorValue);
    FLOATING_STATUS;
    status = napi_reject_deferred(env, c->_deferred, errorValue);
    FLOATING_STATUS;

    delete[] extMsg;
    tidyCarrier(env, c);
  }
  return c->status;
}

// Should never get called
napi_value nop(napi_env env, napi_callback_info info) {
  napi_value value;
  napi_status status;
  status = napi_get_undefined(env, &value);
  if (status != napi_ok) NAPI_THROW_ERROR("Failed to retrieve undefined in nop.");
  return value;
}

char* avErrorMsg(const char* base, int avError) {
  char errMsg[AV_ERROR_MAX_STRING_SIZE];
  char* both;
  int ret = av_strerror(avError, errMsg, AV_ERROR_MAX_STRING_SIZE);
  if (ret < 0) {
    strcpy(errMsg, "Unable to create AV error string.");
  }
  size_t len = strlen(base) + strlen(errMsg);
  both = (char*) malloc(sizeof(char) * (len + 1));
  both[0] = '\0';
  strcat(both, base);
  strcat(both, errMsg);
  return both;
}

napi_status beam_set_uint32(napi_env env, napi_value target, const char* name, uint32_t value) {
  napi_status status;
  napi_value prop;
  status = napi_create_uint32(env, value, &prop);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
}

napi_status beam_get_uint32(napi_env env, napi_value target, char* name, uint32_t* value) {
  napi_status status;
  napi_value prop;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_uint32(env, prop, value);
  ACCEPT_STATUS(napi_number_expected);
  return napi_ok;
}

napi_status beam_set_int32(napi_env env, napi_value target, const char* name, int32_t value) {
  napi_status status;
  napi_value prop;
  status = napi_create_int32(env, value, &prop);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
}

napi_status beam_get_int32(napi_env env, napi_value target, const char* name, int32_t* value) {
  napi_status status;
  napi_value prop;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_int32(env, prop, value);
  ACCEPT_STATUS(napi_number_expected);
  return napi_ok;
}

napi_status beam_set_int64(napi_env env, napi_value target, const char* name, int64_t value) {
  napi_status status;
  napi_value prop;
  status = napi_create_int64(env, value, &prop);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
}

napi_status beam_get_int64(napi_env env, napi_value target, const char* name, int64_t* value) {
  napi_status status;
  napi_value prop;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_int64(env, prop, value);
  ACCEPT_STATUS(napi_number_expected);
  return napi_ok;
}

napi_status beam_set_double(napi_env env, napi_value target, const char* name, double value) {
  napi_status status;
  napi_value prop;
  status = napi_create_double(env, value, &prop);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
}

napi_status beam_get_double(napi_env env, napi_value target, const char* name, double* value) {
  napi_status status;
  napi_value prop;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_double(env, prop, value);
  ACCEPT_STATUS(napi_number_expected);
  return napi_ok;
}

napi_status beam_set_string_utf8(napi_env env, napi_value target, const char* name, const char* value) {
  napi_status status;
  napi_value prop;
  if (value == nullptr) {
    status = napi_get_null(env, &prop);
  }
  else {
    status = napi_create_string_utf8(env, value, NAPI_AUTO_LENGTH, &prop);
  }
  ACCEPT_STATUS(napi_string_expected);
  return napi_set_named_property(env, target, name, prop);
}

// TODO improve memory management
napi_status beam_get_string_utf8(napi_env env, napi_value target, const char* name, char** value) {
  napi_status status;
  napi_value prop;
  char* str;
  size_t len;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_string_utf8(env, prop, nullptr, 0, &len);
  if (status == napi_string_expected) {
    *value = nullptr;
    return napi_ok;
  }
  PASS_STATUS;
  str = (char*) malloc(sizeof(char) * (len + 1));
  status = napi_get_value_string_utf8(env, prop, str, len + 1, &len);
  PASS_STATUS;
  *value = str;
  return napi_ok;
}

napi_status beam_set_bool(napi_env env, napi_value target, const char* name, bool value) {
  napi_status status;
  napi_value prop;
  status = napi_get_boolean(env, value, &prop);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
}

napi_status beam_get_bool(napi_env env, napi_value target, const char* name, bool* present, bool* value) {
  napi_status status;
  napi_value prop;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_get_value_bool(env, prop, value);
  if (status == napi_boolean_expected) {
    *present = false;
    status = napi_ok;
  }
  else {
    *present = true;
  }
  PASS_STATUS;
  return napi_ok;
}

napi_status beam_set_rational(napi_env env, napi_value target, const char* name, AVRational value) {
  napi_status status;
  napi_value pair, element;
  status = napi_create_array(env, &pair);
  PASS_STATUS;
  status = napi_create_int32(env, value.num, &element);
  PASS_STATUS;
  status = napi_set_element(env, pair, 0, element);
  PASS_STATUS;
  status = napi_create_int32(env, value.den, &element);
  PASS_STATUS;
  status = napi_set_element(env, pair, 1, element);
  PASS_STATUS;
  return napi_set_named_property(env, target, name, pair);
}

napi_status beam_get_rational(napi_env env, napi_value target, const char* name, AVRational* value) {
  napi_status status;
  napi_value prop, element;
  int32_t num = 0, den = 1;
  bool isArray;
  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_is_array(env, prop, &isArray);
  PASS_STATUS;
  if (isArray) {
    status = napi_get_element(env, prop, 0, &element);
    PASS_STATUS;
    status = napi_get_value_int32(env, element, &num);
    ACCEPT_STATUS(napi_number_expected);

    status = napi_get_element(env, prop, 1, &element);
    PASS_STATUS;
    status = napi_get_value_int32(env, element, &den);
    ACCEPT_STATUS(napi_number_expected);
  }
  *value = av_make_q(num, den);
  return napi_ok;
}

napi_status beam_set_null(napi_env env, napi_value target, const char* name) {
  napi_status status;
  napi_value value;
  status = napi_get_null(env, &value);
  PASS_STATUS;
  status = napi_set_named_property(env, target, name, value);
  PASS_STATUS;

  return napi_ok;
};

napi_status beam_is_null(napi_env env, napi_value props, const char* name, bool* isNull) {
  napi_status status;
  napi_value value;
  napi_valuetype type;
  status = napi_get_named_property(env, props, name, &value);
  PASS_STATUS;
  status = napi_typeof(env, props, &type);
  PASS_STATUS;
  *isNull = (type == napi_null);

  return napi_ok;
};

napi_status beam_delete_named_property(napi_env env, napi_value props, const char* name, bool* deleted) {
  napi_status status;
  napi_value jsName;
  status = napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &jsName);
  PASS_STATUS;
  status = napi_delete_property(env, props, jsName, deleted);
  PASS_STATUS;

  return napi_ok;
}


const char* beam_lookup_name(std::unordered_map<int, std::string> m, int value) {
  auto search = m.find(value);
  if (search != m.end()) {
    return strdup(search->second.c_str());
  } else {
    return "unknown";
  }
}

int beam_lookup_enum(std::unordered_map<std::string, int> m, char* value) {
  auto search = m.find(std::string(value));
  if (search != m.end()) {
    return search->second;
  } else {
    return BEAM_ENUM_UNKNOWN;
  }
}

napi_status beam_set_enum(napi_env env, napi_value target, const char* name,
    const beamEnum* enumDesc, int value) {
  napi_status status;
  napi_value prop;
  auto search = enumDesc->forward.find(value);
  if (search != enumDesc->forward.end()) {
    status = napi_create_string_utf8(env, search->second.data(), NAPI_AUTO_LENGTH, &prop);
  } else {
    status = napi_create_string_utf8(env, "unknown", NAPI_AUTO_LENGTH, &prop);
  }
  PASS_STATUS;
  return napi_set_named_property(env, target, name, prop);
};

napi_status beam_get_enum(napi_env env, napi_value target, const char* name,
    const beamEnum* enumDesc, int* value) {
  napi_status status;
  napi_value prop;
  napi_valuetype type;
  char* enumStr;
  size_t len;

  status = napi_get_named_property(env, target, name, &prop);
  PASS_STATUS;
  status = napi_typeof(env, prop, &type);
  PASS_STATUS;
  if (type == napi_number) {
    status = napi_get_value_int32(env, prop, value);
    PASS_STATUS;
    return napi_ok;
  }
  if (type == napi_string) {
    status = napi_get_value_string_utf8(env, prop, nullptr, 0, &len);
    PASS_STATUS;
    enumStr = (char*) malloc((len + 1) * sizeof(char));
    status = napi_get_value_string_utf8(env, prop, enumStr, len + 1, &len);
    PASS_STATUS;
    auto search = enumDesc->inverse.find(std::string(enumStr));
    if (search != enumDesc->inverse.end()) {
      *value = search->second;
    } else {
      *value = BEAM_ENUM_UNKNOWN;
    }
  }
  return napi_ok;
};

napi_status fromAVClass(napi_env env, const AVClass* cls, napi_value* result) {
  napi_status status;
  napi_value desc, options, units, opt, consts, element, flags;
  napi_valuetype type;
  const AVOption* option;
  uint32_t constCount;

  status = napi_create_object(env, &desc);
  PASS_STATUS;

  status = beam_set_string_utf8(env, desc, "type", "Class");
  PASS_STATUS;
  status = beam_set_string_utf8(env, desc, "class_name", (char*) cls->class_name);
  PASS_STATUS;
  status = napi_create_object(env, &options);
  PASS_STATUS;
  status = napi_create_object(env, &units);
  PASS_STATUS;
  option = cls->option;
  while ((option != nullptr) && (option->name != nullptr)) {
    if (option->type != AV_OPT_TYPE_CONST) {
      status = napi_create_object(env, &opt);
      PASS_STATUS;
      status = napi_set_named_property(env, options, option->name, opt);
      PASS_STATUS;
      status = beam_set_string_utf8(env, opt, "name", (char*) option->name);
      PASS_STATUS;
      status = beam_set_string_utf8(env, opt, "help", (char*) option->help);
      PASS_STATUS;
      status = beam_set_string_utf8(env, opt, "option_type",
        (char*) beam_lookup_name(beam_option_type->forward, option->type));
      PASS_STATUS;
      status = napi_create_object(env, &flags);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "ENCODING_PARAM",
        option->flags & AV_OPT_FLAG_ENCODING_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "DECODING_PARAM",
        option->flags & AV_OPT_FLAG_DECODING_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "AUDIO_PARAM",
        option->flags & AV_OPT_FLAG_AUDIO_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "VIDEO_PARAM",
        option->flags & AV_OPT_FLAG_VIDEO_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "SUBTITLE_PARAM",
        option->flags & AV_OPT_FLAG_SUBTITLE_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "EXPORT",
        option->flags & AV_OPT_FLAG_EXPORT);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "READONLY",
        option->flags & AV_OPT_FLAG_READONLY);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "BSF_PARAM",
        option->flags & AV_OPT_FLAG_BSF_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "FILTERING_PARAM",
        option->flags & AV_OPT_FLAG_FILTERING_PARAM);
      PASS_STATUS;
      status = beam_set_bool(env, flags, "DEPRECATED",
        option->flags & AV_OPT_FLAG_DEPRECATED);
      PASS_STATUS;
      status = napi_set_named_property(env, opt, "flags", flags);
      PASS_STATUS;
      if (option->unit != nullptr) {
        status = beam_set_string_utf8(env, opt, "unit", (char*) option->unit);
        PASS_STATUS;
        status = napi_create_array(env, &consts);
        PASS_STATUS;
        status = napi_set_named_property(env, opt, "consts", consts);
        PASS_STATUS;
        status = napi_set_named_property(env, units, option->unit, opt);
        PASS_STATUS;
      }
    }
    option = av_opt_next(&cls, option);
  }
  option = cls->option;
  while ((option != nullptr) && (option->name != nullptr)) {
    if (option->type == AV_OPT_TYPE_CONST) {
      status = napi_get_named_property(env, units, option->unit, &opt);
      PASS_STATUS;
      status = napi_typeof(env, opt, &type);
      PASS_STATUS;
      if (type != napi_object) { // Assume badly described constant property
        // TODO work with bad params like https://github.com/FFmpeg/FFmpeg/blob/a0ac49e38ee1d1011c394d7be67d0f08b2281526/libavformat/chromaprint.c#L167
        //      parameter fp_format is missing option->unit
        option = av_opt_next(&cls, option);
        continue;
      }
      status = napi_get_named_property(env, opt, "consts", &consts);
      PASS_STATUS;
      status = napi_get_array_length(env, consts, &constCount);
      PASS_STATUS;
      status = napi_create_string_utf8(env, option->name, NAPI_AUTO_LENGTH, &element);
      PASS_STATUS;
      status = napi_set_element(env, consts, constCount, element);
      PASS_STATUS;
    }
    option = av_opt_next(&cls, option);
  }
  status = napi_set_named_property(env, desc, "options", options);
  PASS_STATUS;

  *result = desc;
  return napi_ok;
}

napi_status makeAVDictionary(napi_env env, napi_value options, AVDictionary** metadata) {
  napi_status status;
  napi_value names, key, value, valueS;
  AVDictionary* dict = nullptr;
  uint32_t propCount;
  char *keyStr, *valueStr;
  size_t strLen;
  int ret;

  status = napi_get_property_names(env, options, &names);
  PASS_STATUS;
  status = napi_get_array_length(env, names, &propCount);
  PASS_STATUS;

  // Replace all metadata values ... no partial operation
  for ( uint32_t x = 0 ; x < propCount ; x++ ) {
    status = napi_get_element(env, names, x, &key);
    PASS_STATUS;
    status = napi_get_property(env, options, key, &value);
    PASS_STATUS;
    status = napi_coerce_to_string(env, value, &valueS);
    PASS_STATUS;

    status = napi_get_value_string_utf8(env, key, nullptr, 0, &strLen);
    PASS_STATUS;
    keyStr = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, key, keyStr, strLen + 1, &strLen);
    PASS_STATUS;

    status = napi_get_value_string_utf8(env, valueS, nullptr, 0, &strLen);
    PASS_STATUS;
    valueStr = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, valueS, valueStr, strLen + 1, &strLen);
    PASS_STATUS;

    ret = av_dict_set(&dict, keyStr, valueStr, 0);
    free(keyStr);
    free(valueStr);
    if (ret < 0) {
      printf("DEBUG: %s\n", avErrorMsg("Problem setting dictionary entry: ", ret));
      return napi_invalid_arg;
    }
  }

  *metadata = dict;
  return napi_ok;
}

napi_status fromContextPrivData(napi_env env, void *privData, napi_value* result) {
  napi_status status;
  napi_value optionsVal, bufferVal;
  int64_t iValue;
  double dValue;
  uint8_t *data;
  AVRational qValue;
  AVPixelFormat pixFmt;
  AVSampleFormat sampleFmt;
  struct imageSizeData { int x; int y; };
  imageSizeData *sizeData;
  struct offsetData { uint8_t *addr; int len; };
  offsetData *offData;
  char chanLayStr[64];

  int ret;
  const AVOption *option = nullptr;
  const AVOption *prev = nullptr;

  status = napi_create_object(env, &optionsVal);
  PASS_STATUS;
  while ((option = av_opt_next(privData, option))) {
    switch (option->type) {
      case AV_OPT_TYPE_FLAGS:
        printf("fromPrivOptions: flags option %s: %s\n", option->name, "unmapped");
        status = beam_set_string_utf8(env, optionsVal, option->name, "unmapped type: flags");
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_INT:
        ret = av_opt_get_int(privData, option->name, 0, &iValue);
        if (ret < 0) {
          return napi_number_expected;
        }
        if (nullptr == option->unit) {
          status = beam_set_int32(env, optionsVal, option->name, (int32_t)iValue);
          PASS_STATUS;
        } else {
          if (iValue < 0) {
            status = beam_set_string_utf8(env, optionsVal, option->name, "unknown");
            PASS_STATUS;
          } else {
            data = (uint8_t *)option->name;
            prev = option;
            option = av_opt_next(privData, option);
            while (option && (AV_OPT_TYPE_CONST == option->type)) {
              prev = option;
              if (option->default_val.i64 == iValue) {
                // printf("fromPrivOptions: int option %s: %s\n", (const char*) data, option->name);
                status = beam_set_string_utf8(env, optionsVal, (const char*) data, option->name);
                PASS_STATUS;
                break;
              }
              option = av_opt_next(privData, option);
            }
            option = prev;
          }
        }
        break;
      case AV_OPT_TYPE_INT64:
      case AV_OPT_TYPE_UINT64:
        ret = av_opt_get_int(privData, option->name, 0, &iValue);
        if (ret < 0) {
          return napi_number_expected;
        }
        status = beam_set_int64(env, optionsVal, option->name, iValue);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_DOUBLE:
      case AV_OPT_TYPE_FLOAT:
        ret = av_opt_get_double(privData, option->name, 0, &dValue);
        if (ret < 0) {
          return napi_number_expected;
        }
        status = beam_set_double(env, optionsVal, option->name, dValue);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_STRING:
        ret = av_opt_get(privData, option->name, 0, &data);
        if (ret < 0) {
          return napi_string_expected;
        }
        status = beam_set_string_utf8(env, optionsVal, option->name, const_cast<char*>((char*) data));
        av_free(data);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_RATIONAL:
        ret = av_opt_get_q(privData, option->name, 0, &qValue);
        if (ret < 0) {
          return napi_object_expected;
        }
        status = beam_set_rational(env, optionsVal, option->name, qValue);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_BINARY:  ///< offset must point to a pointer immediately followed by an int for the length
        offData = (offsetData *)((uint8_t*)privData + option->offset);
        // printf("fromPrivOptions: binary option %s: %p, len 0x%x\n", option->name, offData->addr, offData->len);
        if ((nullptr != offData->addr) && (0 != offData->len))
          status = napi_create_buffer_copy(env, offData->len, offData->addr, (void **)&data, &bufferVal);
        else
          status = napi_get_null(env, &bufferVal);
        PASS_STATUS;
        status = napi_set_named_property(env, optionsVal, option->name, bufferVal);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_DICT:
        printf("fromPrivOptions: dict option %s: %s\n", option->name, "unmapped");
        status = beam_set_string_utf8(env, optionsVal, option->name, "unmapped type: dict");
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_CONST:
        // printf("fromPrivOptions: const option %s: %s\n", option->name, "unmapped");
        // status = beam_set_string_utf8(env, optionsVal, option->name, "unmapped type: const");
        // PASS_STATUS;
        break;
      case AV_OPT_TYPE_IMAGE_SIZE: ///< offset must point to two consecutive integers
        sizeData = (imageSizeData *)((uint8_t*)privData + option->offset);
        // printf("fromPrivOptions: image size option %s: %dx%d\n", option->name, sizeData->x, sizeData->y);
        status = beam_set_rational(env, optionsVal, option->name, av_make_q(sizeData->x, sizeData->y));
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_PIXEL_FMT:
        ret = av_opt_get_pixel_fmt(privData, option->name, 0, &pixFmt);
        if (ret < 0) {
          return napi_number_expected;
        }
        status = beam_set_string_utf8(env, optionsVal, option->name, av_get_pix_fmt_name(pixFmt));
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_SAMPLE_FMT:
        ret = av_opt_get_sample_fmt(privData, option->name, 0, &sampleFmt);
        if (ret < 0) {
          return napi_number_expected;
        }
        status = beam_set_string_utf8(env, optionsVal, option->name, av_get_sample_fmt_name(sampleFmt));
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_VIDEO_RATE: ///< offset must point to AVRational
        qValue = *(AVRational *)((uint8_t*)privData + option->offset);
        // printf("fromPrivOptions: video rate option %s: %d:%d\n", option->name, qValue.num, qValue.den);
        status = beam_set_rational(env, optionsVal, (char*) option->name, qValue);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_DURATION:
        printf("fromPrivOptions: duration option %s: %s\n", option->name, "unmapped");
        status = beam_set_string_utf8(env, optionsVal, option->name, "unmapped type: duration");
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_COLOR:
        printf("fromPrivOptions: color option %s: %s\n", option->name, "unmapped");
        status = beam_set_string_utf8(env, optionsVal, option->name, "unmapped type: color");
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_CHANNEL_LAYOUT:
        ret = av_opt_get_channel_layout(privData, option->name, 0, &iValue);
        if (ret < 0) {
          return napi_number_expected;
        }
        av_get_channel_layout_string(chanLayStr, 64, 0, iValue);
        // printf("fromPrivOptions: channel layout option %s: %lli - %s\n", option->name, iValue, chanLayStr);
        status = beam_set_string_utf8(env, optionsVal, option->name, chanLayStr);
        PASS_STATUS;
        break;
      case AV_OPT_TYPE_BOOL:
        ret = av_opt_get_int(privData, option->name, 0, &iValue);
        if (ret < 0) {
          return napi_boolean_expected;
        }
        // printf("fromPrivOptions: bool option %s: %lli\n", option->name, iValue);
        status = beam_set_bool(env, optionsVal, option->name, iValue);
        PASS_STATUS;
        break;
      default:
        printf("fromPrivOptions: unknown (type %d) option %s: %s\n", option->type, option->name, "unmapped");
        status = beam_set_string_utf8(env, optionsVal, option->name, "unknown type");
        PASS_STATUS;
        break;
    }
  }

  *result = optionsVal;
  return napi_ok;
}

napi_status toContextPrivData(napi_env env, napi_value params, void* priv_data) {
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
            ret = av_opt_set(priv_data, sValue, strProp, 0);
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

std::unordered_map<int, std::string> beam_field_order_fmap = {
  { AV_FIELD_PROGRESSIVE, "progressive" },
  { AV_FIELD_TT, "top coded first, top displayed first" },
  { AV_FIELD_BB, "bottom coded first, bottom displayed first" },
  { AV_FIELD_TB, "top coded first, bottom displayed first" },
  { AV_FIELD_BT, "bottom coded first, top displayed first" },
  { AV_FIELD_UNKNOWN, "unknown" }
};
const beamEnum* beam_field_order = new beamEnum(beam_field_order_fmap);

std::unordered_map<int, std::string> beam_ff_cmp_fmap = {
  { FF_CMP_SAD, "sad" },
  { FF_CMP_SSE, "sse" },
  { FF_CMP_SATD, "satd" },
  { FF_CMP_DCT, "dct" },
  { FF_CMP_PSNR, "psnr" },
  { FF_CMP_BIT, "bit" },
  { FF_CMP_RD, "rd" },
  { FF_CMP_ZERO, "zero" },
  { FF_CMP_VSAD, "vsad" },
  { FF_CMP_VSSE, "vsse" },
  { FF_CMP_NSSE, "nsse" },
  { FF_CMP_W53, "w53" },
  { FF_CMP_W97, "w97" },
  { FF_CMP_DCTMAX, "dctmax" },
  { FF_CMP_DCT264, "dct264" },
  { FF_CMP_MEDIAN_SAD, "median_sad" },
  { FF_CMP_CHROMA, "chroma" },
};
const beamEnum* beam_ff_cmp = new beamEnum(beam_ff_cmp_fmap);

std::unordered_map<int, std::string> beam_ff_mb_decision_fmap = {
  { FF_MB_DECISION_SIMPLE, "simple" }, //  uses mb_cmp
  { FF_MB_DECISION_BITS, "bits" }, // chooses the one which needs the fewest bits
  { FF_MB_DECISION_RD, "rd" } // rate distortion
};
const beamEnum* beam_ff_mb_decision = new beamEnum(beam_ff_mb_decision_fmap);

std::unordered_map<int, std::string> beam_av_audio_service_type_fmap = {
  { AV_AUDIO_SERVICE_TYPE_MAIN, "main" },
  { AV_AUDIO_SERVICE_TYPE_EFFECTS, "effects" },
  { AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED, "visually-impaired" },
  { AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED, "hearing-impaired" },
  { AV_AUDIO_SERVICE_TYPE_DIALOGUE, "dialogue" },
  { AV_AUDIO_SERVICE_TYPE_COMMENTARY, "commentary" },
  { AV_AUDIO_SERVICE_TYPE_EMERGENCY, "emergency" },
  { AV_AUDIO_SERVICE_TYPE_VOICE_OVER, "voice-over" },
  { AV_AUDIO_SERVICE_TYPE_KARAOKE, "karaoke" },
  { AV_AUDIO_SERVICE_TYPE_NB, "nb" }
};
const beamEnum* beam_av_audio_service_type = new beamEnum(beam_av_audio_service_type_fmap);

std::unordered_map<int, std::string> beam_ff_compliance_fmap = {
  { FF_COMPLIANCE_VERY_STRICT, "very-strict" },
  { FF_COMPLIANCE_STRICT, "strict" },
  { FF_COMPLIANCE_NORMAL, "normal" },
  { FF_COMPLIANCE_UNOFFICIAL, "unofficial" },
  { FF_COMPLIANCE_EXPERIMENTAL, "experimental" }
};
const beamEnum* beam_ff_compliance = new beamEnum(beam_ff_compliance_fmap);

std::unordered_map<int, std::string> beam_ff_dct_fmap = {
  { FF_DCT_AUTO, "auto" },
  { FF_DCT_FASTINT, "fastint "},
  { FF_DCT_INT, "int" },
  { FF_DCT_MMX, "mmx" },
  { FF_DCT_ALTIVEC, "altivec" },
  { FF_DCT_FAAN, "faan" }
};
const beamEnum* beam_ff_dct = new beamEnum(beam_ff_dct_fmap);

std::unordered_map<int, std::string> beam_ff_idct_fmap = {
  { FF_IDCT_AUTO, "auto" },
  { FF_IDCT_INT, "int" },
  { FF_IDCT_SIMPLE, "simple" },
  { FF_IDCT_SIMPLEMMX, "simplemmx" },
  { FF_IDCT_ARM, "arm" },
  { FF_IDCT_ALTIVEC, "altivec" },
  { FF_IDCT_SIMPLEARM, "simplearm" },
  { FF_IDCT_XVID, "xvid" },
  { FF_IDCT_SIMPLEARMV5TE, "simplearmv5te" },
  { FF_IDCT_SIMPLEARMV6, "simplearmv6" },
  { FF_IDCT_FAAN, "faan" },
  { FF_IDCT_SIMPLENEON, "simpleneon" },
  { FF_IDCT_NONE, "none" },
  { FF_IDCT_SIMPLEAUTO, "simpleauto" },
};
const beamEnum* beam_ff_idct = new beamEnum(beam_ff_idct_fmap);

std::unordered_map<int, std::string> beam_avdiscard_fmap = {
  { AVDISCARD_NONE, "none" },
  { AVDISCARD_DEFAULT, "default" },
  { AVDISCARD_NONREF, "nonref" },
  { AVDISCARD_BIDIR, "bidir" },
  { AVDISCARD_NONINTRA, "nonintra" },
  { AVDISCARD_NONKEY, "nonkey" },
  { AVDISCARD_ALL, "all" }
};
const beamEnum* beam_avdiscard = new beamEnum(beam_avdiscard_fmap);

std::unordered_map<int, std::string> beam_ff_sub_charenc_mode_fmap = {
  // do nothing (demuxer outputs a stream supposed to be already in UTF-8, or the codec is bitmap for instance
  { FF_SUB_CHARENC_MODE_DO_NOTHING, "do-nothing" },
  // libavcodec will select the mode itself
  { FF_SUB_CHARENC_MODE_AUTOMATIC, "automatic" },
  // the AVPacket data needs to be recoded to UTF-8 before being fed to the decoder, requires iconv
  { FF_SUB_CHARENC_MODE_PRE_DECODER, "pre-decoder" },
  // neither convert the subtitles, nor check them for valid UTF-8
  { FF_SUB_CHARENC_MODE_IGNORE, "ignore" }
};
const beamEnum* beam_ff_sub_charenc_mode = new beamEnum(beam_ff_sub_charenc_mode_fmap);

std::unordered_map<int, std::string> beam_avmedia_type_fmap = {
  { AVMEDIA_TYPE_VIDEO, "video" },
  { AVMEDIA_TYPE_AUDIO, "audio" },
  { AVMEDIA_TYPE_DATA, "data" },
  { AVMEDIA_TYPE_SUBTITLE, "subtitle" },
  { AVMEDIA_TYPE_ATTACHMENT, "attachment" }
};
const beamEnum* beam_avmedia_type = new beamEnum(beam_avmedia_type_fmap);

std::unordered_map<int, std::string> beam_option_type_fmap = {
  { AV_OPT_TYPE_FLAGS, "flags" },
  { AV_OPT_TYPE_INT, "int" },
  { AV_OPT_TYPE_INT64, "int64" },
  { AV_OPT_TYPE_DOUBLE, "double" },
  { AV_OPT_TYPE_FLOAT, "float" },
  { AV_OPT_TYPE_STRING, "string" },
  { AV_OPT_TYPE_RATIONAL, "rational" },
  { AV_OPT_TYPE_BINARY, "binary" }, ///< offset must point to a pointer immediately followed by an int for the length
  { AV_OPT_TYPE_DICT, "dict" },
  { AV_OPT_TYPE_UINT64, "uint64" },
  { AV_OPT_TYPE_CONST, "const" },
  { AV_OPT_TYPE_IMAGE_SIZE, "image_size" }, ///< offset must point to two consecutive integers
  { AV_OPT_TYPE_PIXEL_FMT, "pixel_fmt" },
  { AV_OPT_TYPE_SAMPLE_FMT, "sample_fmt" },
  { AV_OPT_TYPE_VIDEO_RATE, "video_rate" }, ///< offset must point to AVRational
  { AV_OPT_TYPE_DURATION, "duration" },
  { AV_OPT_TYPE_COLOR, "color" },
  { AV_OPT_TYPE_CHANNEL_LAYOUT, "channel_layout" },
  { AV_OPT_TYPE_BOOL, "bool" }
};
const beamEnum* beam_option_type = new beamEnum(beam_option_type_fmap);

std::unordered_map<int, std::string> beam_avoid_neg_ts_fmap = {
  { AVFMT_AVOID_NEG_TS_AUTO, "auto" },
  { AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE, "make_non_negative" },
  { AVFMT_AVOID_NEG_TS_MAKE_ZERO, "make_zero" }
};
const beamEnum* beam_avoid_neg_ts = new beamEnum(beam_avoid_neg_ts_fmap);

std::unordered_map<int, std::string> beam_avfmt_duration2_fmap = {
  { AVFMT_DURATION_FROM_PTS, "from_pts" },
  { AVFMT_DURATION_FROM_STREAM, "from_stream" },
  { AVFMT_DURATION_FROM_BITRATE, "from_bitrate" }
};
const beamEnum* beam_avfmt_duration2 = new beamEnum(beam_avfmt_duration2_fmap);

std::unordered_map<int, std::string> beam_packet_side_data_type_fmap = {
  { AV_PKT_DATA_PALETTE, "palette" },
  { AV_PKT_DATA_NEW_EXTRADATA, "new_extradata" },
  { AV_PKT_DATA_PARAM_CHANGE, "param_change" },
  { AV_PKT_DATA_H263_MB_INFO, "h263_mb_info" },
  { AV_PKT_DATA_REPLAYGAIN, "replaygain" },
  { AV_PKT_DATA_DISPLAYMATRIX, "displaymatrix" },
  { AV_PKT_DATA_STEREO3D, "stereo3d" },
  { AV_PKT_DATA_AUDIO_SERVICE_TYPE, "audio_service_type" },
  { AV_PKT_DATA_QUALITY_STATS, "quality_stats" },
  { AV_PKT_DATA_FALLBACK_TRACK, "fallback_track" },
  { AV_PKT_DATA_CPB_PROPERTIES, "cpb_properties" },
  { AV_PKT_DATA_SKIP_SAMPLES, "skip_samples" },
  { AV_PKT_DATA_JP_DUALMONO, "jp_dualmono" },
  { AV_PKT_DATA_STRINGS_METADATA, "strings_metadata" },
  { AV_PKT_DATA_SUBTITLE_POSITION, "subtitle_position" },
  { AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL, "matroska_blockadditional" },
  { AV_PKT_DATA_WEBVTT_IDENTIFIER, "webvtt_identifier" },
  { AV_PKT_DATA_WEBVTT_SETTINGS, "webvtt_settings" },
  { AV_PKT_DATA_METADATA_UPDATE, "metadata_update" },
  { AV_PKT_DATA_MPEGTS_STREAM_ID, "mpegts_stream_id" },
  { AV_PKT_DATA_MASTERING_DISPLAY_METADATA, "mastering_display_metadata" },
  { AV_PKT_DATA_SPHERICAL, "spherical" },
  { AV_PKT_DATA_CONTENT_LIGHT_LEVEL, "content_light_level" },
  { AV_PKT_DATA_A53_CC, "a53_cc" },
  { AV_PKT_DATA_ENCRYPTION_INIT_INFO, "encryption_init_info" },
  { AV_PKT_DATA_ENCRYPTION_INFO, "encyption_info" },
  { AV_PKT_DATA_AFD, "afd" }
};
const beamEnum* beam_packet_side_data_type = new beamEnum(beam_packet_side_data_type_fmap);

std::unordered_map<int, std::string> beam_frame_side_data_type_fmap = {
  { AV_FRAME_DATA_PANSCAN, "panscan" },
  { AV_FRAME_DATA_A53_CC, "a53_cc" },
  { AV_FRAME_DATA_STEREO3D, "stereo3d" },
  { AV_FRAME_DATA_MATRIXENCODING, "matrixencoding" },
  { AV_FRAME_DATA_DOWNMIX_INFO, "downmix_info" },
  { AV_FRAME_DATA_REPLAYGAIN, "replaygain" },
  { AV_FRAME_DATA_DISPLAYMATRIX, "displaymatrix" },
  { AV_FRAME_DATA_AFD, "afd" },
  { AV_FRAME_DATA_MOTION_VECTORS, "motion_vectors" },
  { AV_FRAME_DATA_SKIP_SAMPLES, "skip_samples" },
  { AV_FRAME_DATA_AUDIO_SERVICE_TYPE, "audio_service_type" },
  { AV_FRAME_DATA_MASTERING_DISPLAY_METADATA, "mastering_display_metadata" },
  { AV_FRAME_DATA_GOP_TIMECODE, "gop_timecode" },
  { AV_FRAME_DATA_SPHERICAL, "spherical" },
  { AV_FRAME_DATA_CONTENT_LIGHT_LEVEL, "light_level" },
  { AV_FRAME_DATA_ICC_PROFILE, "icc_profile" },
  { AV_FRAME_DATA_QP_TABLE_PROPERTIES, "qp_table_properties" },
  { AV_FRAME_DATA_QP_TABLE_DATA, "qp_table_data" },
  { AV_FRAME_DATA_S12M_TIMECODE, "s12m_timecode" }
};
const beamEnum* beam_frame_side_data_type = new beamEnum(beam_frame_side_data_type_fmap);

std::unordered_map<int, std::string> beam_logging_level_fmap = {
  { AV_LOG_QUIET, "quiet" },
  { AV_LOG_PANIC, "panic" },
  { AV_LOG_FATAL, "fatal" },
  { AV_LOG_ERROR, "error" },
  { AV_LOG_WARNING, "warning" },
  { AV_LOG_INFO, "info" },
  { AV_LOG_VERBOSE, "verbose" },
  { AV_LOG_DEBUG, "debug" },
  { AV_LOG_TRACE, "trace" }
};
const beamEnum* beam_logging_level = new beamEnum(beam_logging_level_fmap);

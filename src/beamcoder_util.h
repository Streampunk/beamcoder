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

#ifndef BEAMCODER_UTIL_H
#define BEAMCODER_UTIL_H

#include <chrono>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "node_api.h"

extern "C" {
  #include <libavutil/error.h>
  #include <libavutil/pixdesc.h>
  #include <libavutil/opt.h>
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}

#define DECLARE_NAPI_METHOD(name, func) { name, 0, func, 0, 0, 0, napi_default, 0 }
#define DECLARE_GETTER(name, getter, this) { name, 0, 0, getter, nop, nullptr, napi_enumerable, this }
#define DECLARE_GETTER2(name, test, getter, this) { name, 0, 0, test ? getter : nullptr, nop, nullptr, test ? napi_enumerable : napi_default, this }
#define DECLARE_GETTER3(name, test, getter, this) if ( test ) { desc[count++] = { name, 0, 0, getter, nullptr, nullptr, napi_enumerable, this }; }
// Handling NAPI errors - use "napi_status status;" where used
#define CHECK_STATUS if (checkStatus(env, status, __FILE__, __LINE__ - 1) != napi_ok) return nullptr
#define CHECK_BAIL if (checkStatus(env, status, __FILE__, __LINE__ - 1) != napi_ok) goto bail
#define PASS_STATUS if (status != napi_ok) return status
#define ACCEPT_STATUS(s) if ((status != s) && (status != napi_ok)) return status

napi_status checkStatus(napi_env env, napi_status status,
  const char * file, uint32_t line);

// High resolution timing
#define HR_TIME_POINT std::chrono::high_resolution_clock::time_point
#define NOW std::chrono::high_resolution_clock::now()
long long microTime(std::chrono::high_resolution_clock::time_point start);

// Argument processing
napi_status checkArgs(napi_env env, napi_callback_info info, char* methodName,
  napi_value* args, size_t argc, napi_valuetype* types);

// Async error handling
#define BEAMCODER_ERROR_START 5000
#define BEAMCODER_INVALID_ARGS 5001
#define BEAMCODER_ERROR_READ_FRAME 5002
#define BEAMCODER_ERROR_SEEK_FRAME 5003
#define BEAMCODER_ERROR_ALLOC_DECODER 5004
#define BEAMCODER_ERROR_EOF 5005
#define BEAMCODER_ERROR_EINVAL 5006
#define BEAMCODER_ERROR_ENOMEM 5007
#define BEAMCODER_ERROR_DECODE 5008
#define BEAMCODER_ERROR_OUT_OF_BOUNDS 5009
#define BEAMCODER_ERROR_ALLOC_ENCODER 5010
#define BEAMCODER_ERROR_ENCODE 5011
#define BEAMCODER_ERROR_OPENIO 5012
#define BEAMCODER_DICT_ERROR 5013
#define BEAMCODER_ERROR_WRITE_HEADER 5014
#define BEAMCODER_ERROR_INIT_OUTPUT 5015
#define BEAMCODER_ERROR_WRITE_FRAME 5016
#define BEAMCODER_ERROR_WRITE_TRAILER 5017
#define BEAMCODER_ERROR_FILTER_ADD_FRAME 5018
#define BEAMCODER_ERROR_FILTER_GET_FRAME 5019
#define BEAMCODER_SUCCESS 0

struct carrier {
  virtual ~carrier() {}
  napi_ref passthru = nullptr;
  int32_t status = BEAMCODER_SUCCESS;
  std::string errorMsg;
  long long totalTime;
  napi_deferred _deferred;
  napi_async_work _request = nullptr;
};

void tidyCarrier(napi_env env, carrier* c);
int32_t rejectStatus(napi_env env, carrier* c, char* file, int32_t line);

#define REJECT_STATUS if (rejectStatus(env, c, (char*) __FILE__, __LINE__) != BEAMCODER_SUCCESS) return;
#define REJECT_BAIL if (rejectStatus(env, c, (char*) __FILE__, __LINE__) != BEAMCODER_SUCCESS) goto bail;
#define REJECT_RETURN if (rejectStatus(env, c, (char*) __FILE__, __LINE__) != BEAMCODER_SUCCESS) return promise;
#define FLOATING_STATUS if (status != napi_ok) { \
  printf("Unexpected N-API status not OK in file %s at line %d value %i.\n", \
    __FILE__, __LINE__ - 1, status); \
}

#define NAPI_THROW_ERROR(msg) { \
  char errorMsg[256]; \
  sprintf(errorMsg, "%s", msg); \
  napi_throw_error(env, nullptr, errorMsg); \
  return nullptr; \
}

#define REJECT_ERROR(msg, status) { \
  c->errorMsg = msg; \
  c->status = status; \
  REJECT_STATUS; \
}

#define REJECT_ERROR_RETURN(msg, stat) { \
  c->errorMsg = msg; \
  c->status = stat; \
  REJECT_RETURN; \
}

napi_value nop(napi_env env, napi_callback_info info);
char* avErrorMsg(const char* base, int avErrorCode);

napi_status beam_set_uint32(napi_env env, napi_value target, const char* name, uint32_t value);
napi_status beam_get_uint32(napi_env env, napi_value target, const char* name, uint32_t* value);
napi_status beam_set_int32(napi_env env, napi_value target, const char* name, int32_t value);
napi_status beam_get_int32(napi_env env, napi_value target, const char* name, int32_t* value);
napi_status beam_set_int64(napi_env env, napi_value target, const char* name, int64_t value);
napi_status beam_get_int64(napi_env env, napi_value target, const char* name, int64_t* value);
napi_status beam_set_double(napi_env env, napi_value target, const char* name, double value);
napi_status beam_get_double(napi_env env, napi_value target, const char* name, double* value);
napi_status beam_set_string_utf8(napi_env env, napi_value target, const char* name, const char* value);
napi_status beam_get_string_utf8(napi_env env, napi_value target, const char* name, char** value);
napi_status beam_set_bool(napi_env env, napi_value target, const char* name, bool value);
napi_status beam_get_bool(napi_env env, napi_value target, const char* name, bool* present, bool* value);
napi_status beam_set_rational(napi_env env, napi_value target, const char* name, AVRational value);
napi_status beam_get_rational(napi_env env, napi_value target, const char* name, AVRational* value);
napi_status beam_set_null(napi_env env, napi_value target, const char* name);
napi_status beam_is_null(napi_env env, napi_value props, const char* name, bool* isNull);
napi_status beam_delete_named_property(napi_env env, napi_value props, const char* name, bool* deleted);

#define BEAM_ENUM_UNKNOWN -42

template<typename K, typename V>
std::unordered_map<V,K> inverse_map(std::unordered_map<K,V> &map)
{
	std::unordered_map<V,K> inv;
	std::for_each(map.begin(), map.end(),
				[&inv] (const std::pair<K,V> &p)
				{
					inv.insert(std::make_pair(p.second, p.first));
				});
	return inv;
}

const char* beam_lookup_name(std::unordered_map<int, std::string> m, int value);
int beam_lookup_enum(std::unordered_map<std::string, int> m, char* value);

struct beamEnum {
  std::unordered_map<int, std::string> forward;
  std::unordered_map<std::string, int> inverse;
  beamEnum(std::unordered_map<int, std::string> fwd) : forward(fwd), inverse(inverse_map(fwd)) {};
};

napi_status beam_set_enum(napi_env env, napi_value target, char* name,
  const beamEnum* enumDesc, int value);
napi_status beam_get_enum(napi_env env, napi_value target, char* name,
  const beamEnum* enumDesc, int* value);

extern const beamEnum* beam_field_order;
extern const beamEnum* beam_ff_cmp;
extern const beamEnum* beam_ff_mb_decision;
extern const beamEnum* beam_av_audio_service_type;
extern const beamEnum* beam_ff_compliance;
extern const beamEnum* beam_ff_dct;
extern const beamEnum* beam_ff_idct;
extern const beamEnum* beam_avdiscard;
extern const beamEnum* beam_ff_sub_charenc_mode;
extern const beamEnum* beam_avmedia_type;
extern const beamEnum* beam_option_type;
extern const beamEnum* beam_avoid_neg_ts;
extern const beamEnum* beam_avfmt_duration2;
extern const beamEnum* beam_packet_side_data_type;
extern const beamEnum* beam_frame_side_data_type;
extern const beamEnum* beam_logging_level;

napi_value makeFrame(napi_env env, napi_callback_info info);

struct avBufRef {
  napi_env env;
  napi_ref ref;
  int64_t pts = -1;
};

napi_status fromAVClass(napi_env env, const AVClass* cls, napi_value* result);
napi_status makeAVDictionary(napi_env env, napi_value options, AVDictionary** dict);

napi_status fromContextPrivData(napi_env env, void *privData, napi_value* result);
napi_status toContextPrivData(napi_env env, napi_value params, void* priv_data);

#endif // BEAMCODER_UTIL_H

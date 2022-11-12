#ifndef BEAMCODER_LOG_H
#define BEAMCODER_LOG_H

#include "node_api.h"
#include "beamcoder_util.h"
#include <stdio.h>
#include <unordered_map>

#define CHECK_STATUS_VOID if (checkStatus(env, status, __FILE__, __LINE__ - 1) != napi_ok) return

extern const beamEnum* beam_logging_level;
napi_value logging(napi_env env, napi_callback_info info);
napi_value setLoggingCallback(napi_env env, napi_callback_info info);

struct logCarrier : carrier {
  std::string msg;
	int level;
};

#endif // BEAMCODER_LOG_H
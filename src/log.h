#ifndef BEAMCODER_LOG_H
#define BEAMCODER_LOG_H

#include "node_api.h"
#include "beamcoder_util.h"
#include <stdio.h>
#include <unordered_map>

extern const beamEnum* beam_logging_level;
napi_value logging(napi_env env, napi_callback_info info);
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
// napi_value setLoggingCallback(napi_env env, napi_callback_info info);
#endif // BEAMCODER_LOG_H
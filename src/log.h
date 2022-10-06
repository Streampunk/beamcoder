#ifndef BEAMCODER_LOG_H
#define BEAMCODER_LOG_H

#include "node_api.h"
#include "beamcoder_util.h"

extern const beamEnum* beam_logging_level;
napi_value logging(napi_env env, napi_callback_info info);

#endif // BEAMCODER_LOG_H
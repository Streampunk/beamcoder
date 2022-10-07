#include "node_api.h"
#include <stdio.h>
#include "log.h"


extern "C" {
  #include <libavutil/log.h>
	#include <libavutil/bprint.h>
}

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

napi_threadsafe_function threadSafeFunction;

// Inspired from https://github.com/FFmpeg/FFmpeg/blob/321a3c244d0a89b2826c38611284cc403a9808fa/libavutil/log.c#L346
#define LINE_SZ 1024
void av_log_custom_callback(void* ptr, int level, const char* fmt, va_list vl)
{
		int av_log_level = av_log_get_level();
    static int print_prefix = 1;
    static int count;
    static char prev[LINE_SZ];
    AVBPrint part[4];
    char line[LINE_SZ];
    static int is_atty;
    int type[2];
    unsigned tint = 0;

    if (level >= 0) {
        tint = level & 0xff00;
        level &= 0xff;
    }

    if (level > av_log_level)
        return;
		av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);
		
		logCarrier* c = new logCarrier;
		c->msg = line;
		c->level = level;
    napi_status status;
    
		status = napi_call_threadsafe_function(threadSafeFunction, c, napi_tsfn_nonblocking);	

    return;
}

static void callJsCb(
		napi_env env,
		napi_value jsCallback,
		void* context,
		void* data
	){
		
  logCarrier* c = (logCarrier*) data;
	
	napi_value jsThis;
	napi_status status;

	status = napi_create_object(env, &jsThis);
	CHECK_STATUS_VOID;
  	
	napi_value jsStr;
	status = napi_create_string_utf8(env, c->msg.c_str(), NAPI_AUTO_LENGTH, &jsStr);
	CHECK_STATUS_VOID;

	napi_value return_val;
	status = napi_call_function(env, jsThis, jsCallback, 1, &jsStr, &return_val);
	CHECK_STATUS_VOID;

	return;
}

napi_value setLoggingCallback(napi_env env, napi_callback_info info){

	napi_status status;

  napi_value args[1];
  size_t argc = 1;
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
	CHECK_STATUS;

	if (argc != 1) {
		status = napi_throw_error(env, nullptr, "One argument required to set logging callback.");
		return nullptr;
	}
	napi_value callback = args[0];
  napi_valuetype t;
  status = napi_typeof(env, callback, &t);
  CHECK_STATUS;

  if (t != napi_function) {
    status = napi_throw_type_error(env, nullptr, "Callback argument should be a function.");
    return nullptr;
  }
	
	napi_value work_name;
	status = napi_create_string_utf8(env, "Thread-safe Function For Libav Custom Logging", NAPI_AUTO_LENGTH, &work_name);
	CHECK_STATUS;
    
	status = napi_create_threadsafe_function(
		env,
    callback,
    NULL,
    work_name,
    0,
    1,
    nullptr,
    nullptr,
    nullptr,
    callJsCb,
    &threadSafeFunction
	);
	CHECK_STATUS;
  
  status = napi_unref_threadsafe_function(env, threadSafeFunction);
	CHECK_STATUS;
  
	av_log_set_callback(av_log_custom_callback);
	return nullptr;
}
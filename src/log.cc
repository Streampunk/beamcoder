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

n_api_value set_logging_callback(napi_env env, napi_callback_info info){
	
	napi_threadsafe_function status;
	napi_value result;
	
  napi_value args[1];
  size_t argc = 1;
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
	CHECK_STATUS;
	
	if (argc != 1) {
		status = napi_throw_error(env, nullptr, "One argumentis required to set logging callback.");
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
			
	// Inspired from https://www.ffmpeg.org/doxygen/0.6/log_8c-source.html
	void av_log_custom_callback(void* ptr, int level, const char* fmt, va_list vl){
		static int print_prefix=1;
		static int count;
		static char prev[1024];
		char line[1024];
		static const uint8_t color[]={0x41,0x41,0x11,0x03,9,9,9};
		AVClass* avc= ptr ? *(AVClass**)ptr : NULL;
		if(level>av_log_level)
			return;
			
		napi_value arg;
		status = napi_create_string_utf8(env, line, &arg);
		CHECK_STATUS;
		
		napi_value* argv = &arg;
		size_t argc = 1;
		napi_value return_val;
		
		status = napi_call_function(env, global, callback, argc, argv, &return_val);
		CHECK_STATUS;
	}
	
	
	CHECK_STATUS;
	
	
}
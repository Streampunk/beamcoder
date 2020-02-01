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

#include "governor.h"
#include "adaptor.h"

struct readCarrier : carrier {
  ~readCarrier() { }
  Adaptor *adaptor;
  uint32_t readLen;
  void *readBuf = nullptr;
};

void readFinalizer(napi_env env, void* data, void* hint) {
  free(data);

  int64_t externalMemory;
  int32_t len = (int32_t)(uint64_t)hint;
  if (BEAMCODER_SUCCESS != napi_adjust_external_memory(env, -len, &externalMemory))
    printf("Error finalising governor read buffer %p, len %d\n", data, len);
}

void readExecute(napi_env env, void *data) {
  readCarrier* c = (readCarrier*) data;
  size_t bytesRead = 0;
  c->readBuf = c->adaptor->read(c->readLen, &bytesRead);
  c->readLen = bytesRead;
}

void readComplete(napi_env env, napi_status asyncStatus, void *data) {
  readCarrier* c = (readCarrier*) data;
  napi_value result;
  int64_t externalMemory;

  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "governor read failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_create_external_buffer(env, c->readLen, c->readBuf, readFinalizer, (void*)(uint64_t)c->readLen, &result);
  REJECT_STATUS;

  c->status = napi_adjust_external_memory(env, c->readLen, &externalMemory);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
}

napi_value read(napi_env env, napi_callback_info info) {
  napi_value promise;
  readCarrier* c = new readCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];
  napi_value governorValue;
  napi_status status = napi_get_cb_info(env, info, &argc, args, &governorValue, nullptr);
  REJECT_RETURN;

  if (argc < 1) {
    REJECT_ERROR_RETURN("governor read requires a read length as its argument.",
      BEAMCODER_INVALID_ARGS);
  }

  status = napi_get_value_uint32(env, args[0], &c->readLen);
  CHECK_STATUS;

  napi_value adaptorValue;
  status = napi_get_named_property(env, governorValue, "_adaptor", &adaptorValue);
  CHECK_STATUS;

  status = napi_get_value_external(env, adaptorValue, (void **)&c->adaptor);
  CHECK_STATUS;

  napi_value resourceName;
  c->status = napi_create_string_utf8(env, "Read", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, readExecute,
    readComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

struct writeCarrier : carrier {
  ~writeCarrier() { }
  Adaptor *adaptor;
  napi_ref bufRef;
  void *buf;
  size_t bufLen;
};

void writeExecute(napi_env env, void *data) {
  writeCarrier* c = (writeCarrier*) data;
  c->adaptor->write(c->bufRef, c->buf, c->bufLen);
};

void writeComplete(napi_env env, napi_status asyncStatus, void *data) {
  writeCarrier* c = (writeCarrier*) data;
  napi_value result;
  if (asyncStatus != napi_ok) {
    c->status = asyncStatus;
    c->errorMsg = "governor write failed to complete.";
  }
  REJECT_STATUS;

  c->status = napi_get_null(env, &result);
  REJECT_STATUS;

  napi_status status;
  status = napi_resolve_deferred(env, c->_deferred, result);
  FLOATING_STATUS;

  tidyCarrier(env, c);
};

napi_value write(napi_env env, napi_callback_info info) {
  napi_value promise;
  writeCarrier* c = new writeCarrier;

  c->status = napi_create_promise(env, &c->_deferred, &promise);
  REJECT_RETURN;

  size_t argc = 1;
  napi_value args[1];
  napi_value governorValue;
  napi_status status = napi_get_cb_info(env, info, &argc, args, &governorValue, nullptr);
  REJECT_RETURN;

  if (argc < 1) {
    REJECT_ERROR_RETURN("governor write requires a buffer as its argument.",
      BEAMCODER_INVALID_ARGS);
  }

  bool isBuffer;
  c->status = napi_is_buffer(env, args[0], &isBuffer);
  REJECT_RETURN;
  if (!isBuffer) {
    REJECT_ERROR_RETURN("governor write expects a node buffer",
      BEAMCODER_INVALID_ARGS);
  }

  napi_value bufferValue = args[0];
  c->status = napi_create_reference(env, bufferValue, 1, &c->bufRef);
  REJECT_RETURN;

  c->status = napi_get_buffer_info(env, bufferValue, &c->buf, &c->bufLen);
  REJECT_RETURN;

  napi_value adaptorValue;
  status = napi_get_named_property(env, governorValue, "_adaptor", &adaptorValue);
  CHECK_STATUS;

  status = napi_get_value_external(env, adaptorValue, (void **)&c->adaptor);
  CHECK_STATUS;

  napi_value resourceName;
  c->status = napi_create_string_utf8(env, "Write", NAPI_AUTO_LENGTH, &resourceName);
  REJECT_RETURN;
  c->status = napi_create_async_work(env, nullptr, resourceName, writeExecute,
    writeComplete, c, &c->_request);
  REJECT_RETURN;
  c->status = napi_queue_async_work(env, c->_request);
  REJECT_RETURN;

  return promise;
}

napi_value finish(napi_env env, napi_callback_info info) {
  size_t argc = 0;
  napi_value governorValue;
  napi_status status = napi_get_cb_info(env, info, &argc, nullptr, &governorValue, nullptr);
  CHECK_STATUS;

  napi_value adaptorValue;
  status = napi_get_named_property(env, governorValue, "_adaptor", &adaptorValue);
  CHECK_STATUS;

  Adaptor *adaptor = nullptr;
  status = napi_get_value_external(env, adaptorValue, (void **)&adaptor);
  CHECK_STATUS;

  adaptor->finish();

  napi_value result;
  napi_get_undefined(env, &result);
  return result;
}

void finalizeAdaptor(napi_env env, void* data, void* hint) {
  Adaptor *adaptor = (Adaptor *)data;
  delete adaptor;
}

napi_value governor(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value args[1];
  size_t argc = 1;
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;

  if (argc != 1) {
    status = napi_throw_error(env, nullptr, "Wrong number of arguments to create governer.");
    return nullptr;
  }

  napi_value params = args[0];
  napi_valuetype t;
  status = napi_typeof(env, params, &t);
  CHECK_STATUS;
  if (t != napi_object) {
    status = napi_throw_type_error(env, nullptr, "governer parameters must be an object.");
    return nullptr;
  }

  napi_value highWaterMarkVal;
  int32_t highWaterMark = 3;
  status = napi_get_named_property(env, params, "highWaterMark", &highWaterMarkVal);
  CHECK_STATUS;
  status = napi_typeof(env, highWaterMarkVal, &t);
  CHECK_STATUS;
  if (t == napi_number) {
    status = napi_get_value_int32(env, highWaterMarkVal, &highWaterMark);
    CHECK_STATUS;
  }

  napi_value governorObj;
  status = napi_create_object(env, &governorObj);
  CHECK_STATUS;

  Adaptor *adaptor = new Adaptor(highWaterMark);

  napi_value adaptorValue;
  status = napi_create_external(env, adaptor, finalizeAdaptor, nullptr, &adaptorValue);
  CHECK_STATUS;
  status = napi_set_named_property(env, governorObj, "_adaptor", adaptorValue);
  CHECK_STATUS;

  napi_value readValue;
  status = napi_create_function(env, "read", NAPI_AUTO_LENGTH, read, nullptr, &readValue);
  CHECK_STATUS;
  status = napi_set_named_property(env, governorObj, "read", readValue);
  CHECK_STATUS;

  napi_value writeValue;
  status = napi_create_function(env, "write", NAPI_AUTO_LENGTH, write, nullptr, &writeValue);
  CHECK_STATUS;
  status = napi_set_named_property(env, governorObj, "write", writeValue);
  CHECK_STATUS;

  napi_value finishValue;
  status = napi_create_function(env, "finish", NAPI_AUTO_LENGTH, finish, nullptr, &finishValue);
  CHECK_STATUS;
  status = napi_set_named_property(env, governorObj, "finish", finishValue);
  CHECK_STATUS;

  return governorObj;
}

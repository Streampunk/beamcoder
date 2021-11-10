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

#include "hwcontext.h"

void hwContextFinalizer(napi_env env, void* data, void* hint) {
  printf("Finalizing a hw context reference\n");
  AVBufferRef* ctxRef = (AVBufferRef*) data;
  av_buffer_unref(&ctxRef);
}

napi_value getHWDeviceCtxType(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWDeviceContext* device_context;
  const char* name;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &device_context);
  CHECK_STATUS;

  switch (device_context->type) {
    case AV_HWDEVICE_TYPE_NONE:
      name = "none";
      break;
    case AV_HWDEVICE_TYPE_VDPAU:
      name = "vdpau";
      break;
    case AV_HWDEVICE_TYPE_CUDA:
      name = "cuda";
      break;
    case AV_HWDEVICE_TYPE_VAAPI:
      name = "vaapi";
      break;
    case AV_HWDEVICE_TYPE_DXVA2:
      name = "dxva2";
      break;
    case AV_HWDEVICE_TYPE_QSV:
      name = "qsv";
      break;
    case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
      name = "videotoolbox";
      break;
    case AV_HWDEVICE_TYPE_D3D11VA:
      name = "d3d11va";
      break;
    case AV_HWDEVICE_TYPE_DRM:
      name = "drm";
      break;
    case AV_HWDEVICE_TYPE_OPENCL:
      name = "opencl";
      break;
    case AV_HWDEVICE_TYPE_MEDIACODEC:
      name = "mediacodec";
      break;
    // case AV_HWDEVICE_TYPE_VULKAN:
    //   name = "vulkan";
    //   break;
    default:
      name = nullptr;
      break;
  }

  if (name == nullptr) {
    status = napi_get_null(env, &result);
  } else {
    status = napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &result);
  }
  CHECK_STATUS;
  return result;
}

napi_status fromHWDeviceContext(napi_env env, AVBufferRef* contextRef, napi_value* result) {
  napi_status status;
  napi_value jsHWDeviceContext, extContextRef, typeName;

  status = napi_create_object(env, &jsHWDeviceContext);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "HWDeviceContext", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_create_external(env, (void*)av_buffer_ref(contextRef), hwContextFinalizer, nullptr, &extContextRef);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "device_type", nullptr, nullptr, getHWDeviceCtxType, nullptr,
      nullptr, napi_enumerable, (void*)contextRef->data },
    { "_deviceContext", nullptr, nullptr, nullptr, nullptr, extContextRef, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsHWDeviceContext, 3, desc);
  PASS_STATUS;

  *result = jsHWDeviceContext;
  return napi_ok;
}

napi_value getHWFramesCtxHWDeviceCtx(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  status = fromHWDeviceContext(env, frames_context->device_ref, &result);
  CHECK_STATUS;

  return result;
}

napi_value getHWFramesCtxInitialPoolSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  status = napi_create_uint32(env, frames_context->initial_pool_size, &result);
  CHECK_STATUS;

  return result;
}

napi_value getHWFramesCtxPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  AVPixelFormat pixFmt = frames_context->format;
  status = napi_create_string_utf8(env, av_get_pix_fmt_name(pixFmt), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setHWFramesCtxPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVHWFramesContext* frames_context;
  char* name;
  size_t strLen;
  AVPixelFormat pixFmt;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &frames_context);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the format property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    frames_context->format = AV_PIX_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the format property.");
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
    frames_context->format = pixFmt;
  } else {
    NAPI_THROW_ERROR("Pixel format name is not known.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getHWFramesCtxSwPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  AVPixelFormat pixFmt = frames_context->sw_format;
  status = napi_create_string_utf8(env, av_get_pix_fmt_name(pixFmt), NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value setHWFramesCtxSwPixFmt(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVHWFramesContext* frames_context;
  char* name;
  size_t strLen;
  AVPixelFormat pixFmt;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &frames_context);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the sw_format property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    frames_context->sw_format = AV_PIX_FMT_NONE;
    goto done;
  }
  if (type != napi_string) {
    NAPI_THROW_ERROR("A string is required to set the sw_format property.");
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
    frames_context->sw_format = pixFmt;
  } else {
    NAPI_THROW_ERROR("Pixel format name is not known.");
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getHWFramesCtxWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  status = napi_create_uint32(env, frames_context->width, &result);
  CHECK_STATUS;

  return result;
}

napi_value setHWFramesCtxWidth(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVHWFramesContext* frames_context;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &frames_context);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the width property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the width property.");
  }

  status = napi_get_value_int32(env, args[0], &frames_context->width);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getHWFramesCtxHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  AVHWFramesContext* frames_context;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &frames_context);
  CHECK_STATUS;

  status = napi_create_uint32(env, frames_context->height, &result);
  CHECK_STATUS;

  return result;
}

napi_value setHWFramesCtxHeight(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  AVHWFramesContext* frames_context;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &frames_context);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set the height property.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("A number is required to set the height property.");
  }

  status = napi_get_value_int32(env, args[0], &frames_context->height);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_status fromHWFramesContext(napi_env env, AVBufferRef* contextRef, napi_value* result) {
  napi_status status;
  napi_value jsHWFramesContext, extContextRef, typeName;

  status = napi_create_object(env, &jsHWFramesContext);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "HWFramesContext", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_create_external(env, (void*)av_buffer_ref(contextRef), hwContextFinalizer, nullptr, &extContextRef);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, nullptr, nullptr, typeName, napi_enumerable, nullptr },
    { "device_context", nullptr, nullptr, getHWFramesCtxHWDeviceCtx, nullptr, nullptr,
      napi_enumerable, (void*)contextRef->data },
    { "initial_pool_size", nullptr, nullptr, getHWFramesCtxInitialPoolSize, nullptr, nullptr,
      napi_enumerable, (void*)contextRef->data },
    { "pix_fmt", nullptr, nullptr, getHWFramesCtxPixFmt, setHWFramesCtxPixFmt, nullptr,
      napi_enumerable, (void*)contextRef->data },
    { "sw_pix_fmt", nullptr, nullptr, getHWFramesCtxSwPixFmt, setHWFramesCtxSwPixFmt, nullptr,
      napi_enumerable, (void*)contextRef->data },
    { "width", nullptr, nullptr, getHWFramesCtxWidth, setHWFramesCtxWidth, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), (void*)contextRef->data },
    { "height", nullptr, nullptr, getHWFramesCtxHeight, setHWFramesCtxHeight, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), (void*)contextRef->data },
    { "_framesContext", nullptr, nullptr, nullptr, nullptr, extContextRef, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsHWFramesContext, 8, desc);
  PASS_STATUS;

  *result = jsHWFramesContext;
  return napi_ok;
}

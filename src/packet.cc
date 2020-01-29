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

#include "packet.h"

napi_value getPacketPts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  if (p->packet->pts == AV_NOPTS_VALUE) {
    status = napi_get_null(env, &result);
  } else {
    status = napi_create_int64(env, p->packet->pts, &result);
  }
  CHECK_STATUS;
  return result;
}

napi_value setPacketPts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet PTS must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  switch (type) {
    case napi_null:
      p->packet->pts = AV_NOPTS_VALUE;
      break;
    case napi_number:
      status = napi_get_value_int64(env, args[0], &p->packet->pts);
      CHECK_STATUS;
      break;
    default:
      NAPI_THROW_ERROR("Packet PTS property must be set with a number.");
      break;
  }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketDts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  if (p->packet->dts == AV_NOPTS_VALUE) {
    status = napi_get_null(env, &result);
  } else {
    status = napi_create_int64(env, p->packet->dts, &result);
  }

  CHECK_STATUS;
  return result;
}

napi_value setPacketDts(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet DTS must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_undefined) || (type == napi_null)) {
    p->packet->dts = AV_NOPTS_VALUE;
    goto done;
  }
  if (type != napi_number) {
    NAPI_THROW_ERROR("Packet DTS property must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &p->packet->dts);
  CHECK_STATUS;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;
  AVBufferRef* hintRef;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  if (p->packet->buf == nullptr) {
    status = napi_get_null(env, &result);
  } else {
    hintRef = av_buffer_ref(p->packet->buf);
    status = napi_create_external_buffer(env, hintRef->size, hintRef->data,
      packetBufferFinalizer, hintRef, &result);
    CHECK_STATUS;
  }

  CHECK_STATUS;
  return result;
}

napi_value setPacketData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  bool isBuffer;
  packetData* p;
  uint8_t* data;
  size_t length;
  avBufRef* avr = new avBufRef;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet data must be provided with a buffer value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if ((type == napi_null) || (type == napi_undefined)) {
    if (p->dataRef != nullptr) {
      status = napi_delete_reference(env, p->dataRef);
      CHECK_STATUS;
      p->dataRef = nullptr;
    }
    if (p->packet->buf != nullptr) {
      av_buffer_unref(&p->packet->buf); // sets it to null
    }
    p->packet->data = nullptr;
    goto done;
  }
  status = napi_is_buffer(env, args[0], &isBuffer);
  CHECK_STATUS;
  if (!isBuffer) {
    NAPI_THROW_ERROR("Packet data property must be set with a buffer.");
  }

  if (p->dataRef != nullptr) {
    status = napi_delete_reference(env, p->dataRef);
    CHECK_STATUS;
    p->dataRef = nullptr;
  }
  status = napi_create_reference(env, args[0], 1, &p->dataRef);
  CHECK_STATUS;
  status = napi_create_reference(env, args[0], 1, &avr->ref);
  CHECK_STATUS;
  avr->env = env;
  status = napi_get_buffer_info(env, args[0], (void**) &data, &length);
  CHECK_STATUS;
  if (p->packet->buf != nullptr) {
    av_buffer_unref(&p->packet->buf);
  }
  p->packet->buf = av_buffer_create(data, length, packetBufferFree, avr, 0);
  CHECK_STATUS;
  p->packet->data = data;

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

// TODO is this a smell? Can this actually run from a separate thread?
void packetBufferFree(void* opaque, uint8_t* data) {
  napi_status status;
  avBufRef* avr = (avBufRef*) opaque;
  status = napi_delete_reference(avr->env, (napi_ref) avr->ref);
  if (status != napi_ok)
    printf("DEBUG: Failed to delete buffer reference associated with an AVBufferRef.");
  delete avr;
}

napi_value getPacketSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData *p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  status = napi_create_int32(env, p->packet->size, &result);
  CHECK_STATUS;

  return result;
}

napi_value setPacketSize(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet size must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Packet size property must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &p->packet->size);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;

  return result;
}

napi_value getPacketStreamIndex(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  status = napi_create_int32(env, p->packet->stream_index, &result);
  CHECK_STATUS;
  return result;
}

napi_value setPacketStreamIndex(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet stream_index must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Packet stream_index property must be set with a number.");
  }
  status = napi_get_value_int32(env, args[0], &p->packet->stream_index);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "KEY", (p->packet->flags & AV_PKT_FLAG_KEY) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "CORRUPT", (p->packet->flags & AV_PKT_FLAG_CORRUPT) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DISCARD", (p->packet->flags & AV_PKT_FLAG_DISCARD) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "TRUSTED", (p->packet->flags & AV_PKT_FLAG_TRUSTED) != 0);
  CHECK_STATUS;
  status = beam_set_bool(env, result, "DISPOSABLE", (p->packet->flags & AV_PKT_FLAG_DISPOSABLE) != 0);
  CHECK_STATUS;

  return result;
}

napi_value setPacketFlags(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;
  bool present, flag;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set frame flags must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_object) {
    NAPI_THROW_ERROR("Frame sample_rate property must be set with a number.");
  }
  status = beam_get_bool(env, args[0], "KEY", &present, &flag);
  CHECK_STATUS;
  if (present) { p->packet->flags = (flag) ?
    p->packet->flags | AV_PKT_FLAG_KEY :
    p->packet->flags & ~AV_PKT_FLAG_KEY; }
  status = beam_get_bool(env, args[0], "CORRUPT", &present, &flag);
  CHECK_STATUS;
  if (present) { p->packet->flags = (flag) ?
    p->packet->flags | AV_PKT_FLAG_CORRUPT :
    p->packet->flags & ~AV_PKT_FLAG_CORRUPT; }
  status = beam_get_bool(env, args[0], "DISCARD", &present, &flag);
  CHECK_STATUS;
  if (present) { p->packet->flags = (flag) ?
    p->packet->flags | AV_PKT_FLAG_DISCARD :
    p->packet->flags & ~AV_PKT_FLAG_DISCARD; }
  status = beam_get_bool(env, args[0], "TRUSTED", &present, &flag);
  CHECK_STATUS;
  if (present) { p->packet->flags = (flag) ?
    p->packet->flags | AV_PKT_FLAG_TRUSTED :
    p->packet->flags & ~AV_PKT_FLAG_TRUSTED; }
  status = beam_get_bool(env, args[0], "DISPOSABLE", &present, &flag);
  CHECK_STATUS;
  if (present) { p->packet->flags = (flag) ?
    p->packet->flags | AV_PKT_FLAG_DISPOSABLE :
    p->packet->flags & ~AV_PKT_FLAG_DISPOSABLE; }

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketSideData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, element;
  packetData* p;
  void* resultData;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  if (p->packet->side_data_elems <= 0) {
    status = napi_get_null(env, &result);
    CHECK_STATUS;
  } else {
    status = napi_create_object(env, &result);
    CHECK_STATUS;
    status = beam_set_string_utf8(env, result, "type", "PacketSideData");
    for ( int x = 0 ; x < p->packet->side_data_elems ; x++ ) {
      status = napi_create_buffer_copy(env, p->packet->side_data[x].size,
        p->packet->side_data[x].data, &resultData, &element);
      CHECK_STATUS;
      status = napi_set_named_property(env, result,
        beam_lookup_name(beam_packet_side_data_type->forward,
          p->packet->side_data[x].type), element);
      CHECK_STATUS;
    }
  }

  return result;
}

napi_value setPacketSideData(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsBuffer, jsBufferFrom, name, names, element, arrayData;
  napi_valuetype type;
  bool isArray, isBuffer;
  uint32_t sdCount;
  packetData* p;
  char* typeName;
  size_t strLen;
  int psdt;
  void* rawdata;
  size_t rawdataSize;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("A value is required to set side_data property.");
  }

  status = napi_get_global(env, &global);
  CHECK_STATUS;
  status = napi_get_named_property(env, global, "Buffer", &jsBuffer);
  CHECK_STATUS;
  status = napi_get_named_property(env, jsBuffer, "from", &jsBufferFrom);
  CHECK_STATUS;

  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  status = napi_is_array(env, args[0], &isArray);
  CHECK_STATUS;

  switch (type) {
    case napi_object:
    case napi_null:
    case napi_undefined:
      av_packet_free_side_data(p->packet);
      if (type != napi_object) { goto done; };
      break;
    default:
      NAPI_THROW_ERROR("Packet side_data property requires an object with Buffer-valued properties.");
  }

  status = napi_get_property_names(env, args[0], &names);
  CHECK_STATUS;
  status = napi_get_array_length(env, names, &sdCount);
  CHECK_STATUS;

  for ( uint32_t x = 0 ; x < sdCount ; x++ ) {
    status = napi_get_element(env, names, x, &name);
    CHECK_STATUS;
    status = napi_get_property(env, args[0], name, &element);
    CHECK_STATUS;
    status = napi_is_buffer(env, element, &isBuffer);
    CHECK_STATUS;
    if (!isBuffer) {
      status = napi_get_named_property(env, element, "data", &arrayData);
      CHECK_STATUS;
      // TODO more checks that this is a buffer from JSON?
      status = napi_is_array(env, arrayData, &isArray);
      CHECK_STATUS;
      if (isArray) {
        const napi_value fargs[] = { arrayData };
        status = napi_call_function(env, element, jsBufferFrom, 1, fargs, &element);
        CHECK_STATUS;
      } else {
        continue;
      }
    }
    status = napi_get_value_string_utf8(env, name, nullptr, 0, &strLen);
    CHECK_STATUS;
    typeName = (char*) malloc(sizeof(char) * (strLen + 1));
    status = napi_get_value_string_utf8(env, name, typeName, strLen + 1, &strLen);
    CHECK_STATUS;

    psdt = beam_lookup_enum(beam_packet_side_data_type->inverse, typeName);
    free(typeName);
    if (psdt == BEAM_ENUM_UNKNOWN) {
      continue;
    } else {
      status = napi_get_buffer_info(env, element, &rawdata, &rawdataSize);
      CHECK_STATUS;
      uint8_t* pktdata = av_packet_new_side_data(p->packet,
        (AVPacketSideDataType) psdt, rawdataSize);
      if (pktdata != nullptr) {
        memcpy(pktdata, rawdata, rawdataSize);
      }
    }
  }

done:
  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  status = napi_create_int64(env, p->packet->duration, &result);
  CHECK_STATUS;
  return result;
}

napi_value setPacketDuration(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet duration must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Packet duration property must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &p->packet->duration);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value getPacketPos(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  packetData* p;

  status = napi_get_cb_info(env, info, 0, nullptr, nullptr, (void**) &p);
  CHECK_STATUS;

  status = napi_create_int64(env, p->packet->pos, &result);
  CHECK_STATUS;
  return result;
}

napi_value setPacketPos(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  napi_valuetype type;
  packetData* p;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**) &p);
  CHECK_STATUS;
  if (argc < 1) {
    NAPI_THROW_ERROR("Set packet pos must be provided with a value.");
  }
  status = napi_typeof(env, args[0], &type);
  CHECK_STATUS;
  if (type != napi_number) {
    NAPI_THROW_ERROR("Packet pos property must be set with a number.");
  }
  status = napi_get_value_int64(env, args[0], &p->packet->pos);
  CHECK_STATUS;

  status = napi_get_undefined(env, &result);
  CHECK_STATUS;
  return result;
}

napi_value makePacket(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, global, jsObject, assign, jsJSON, jsParse;
  napi_valuetype type;
  bool isArray, deleted;
  packetData* p = new packetData;
  p->packet = av_packet_alloc();

  status = napi_get_global(env, &global);
  CHECK_STATUS;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  CHECK_STATUS;
  if (argc > 1) {
    NAPI_THROW_ERROR("Packet may be created with zero or one options object argument.");
  }
  if (argc == 1) {
    status = napi_typeof(env, args[0], &type);
    CHECK_STATUS;
    if (type == napi_string) {
      status = napi_get_named_property(env, global, "JSON", &jsJSON);
      CHECK_STATUS;
      status =  napi_get_named_property(env, jsJSON, "parse", &jsParse);
      CHECK_STATUS;
      const napi_value pargs[] = { args[0] };
      status = napi_call_function(env, args[0], jsParse, 1, pargs, &args[0]);
      CHECK_STATUS;
      status = napi_typeof(env, args[0], &type);
      CHECK_STATUS;
      if (type == napi_object) {
        status = beam_delete_named_property(env, args[0], "type", &deleted);
        CHECK_STATUS;
      }
    }
    status = napi_is_array(env, args[0], &isArray);
    CHECK_STATUS;
    if (isArray || (type != napi_object)) {
      NAPI_THROW_ERROR("Cannot create a packet unless argument is an object.");
    }
  }

  status = fromAVPacket(env, p, &result);
  CHECK_STATUS;

  if (argc == 1) {
    status = napi_get_named_property(env, global, "Object", &jsObject);
    CHECK_STATUS;
    status = napi_get_named_property(env, jsObject, "assign", &assign);
    CHECK_STATUS;
    const napi_value fargs[] = { result, args[0] };
    status = napi_call_function(env, result, assign, 2, fargs, &result);
    CHECK_STATUS;
  }

  return result;
}

napi_value getPacketTypeName(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;

  status =  napi_create_string_utf8(env, "Packet", NAPI_AUTO_LENGTH, &result);
  CHECK_STATUS;

  return result;
}

napi_value packetToJSON(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result, base;
  packetData* p;
  int count = 0;

  size_t argc = 0;
  status = napi_get_cb_info(env, info, &argc, nullptr, &base, (void**) &p);
  CHECK_STATUS;

  status = napi_create_object(env, &result);
  CHECK_STATUS;

  napi_property_descriptor desc[10];
  DECLARE_GETTER3("type", true, getPacketTypeName, p);
  DECLARE_GETTER3("pts", p->packet->pts != AV_NOPTS_VALUE, getPacketPts, p);
  DECLARE_GETTER3("dts", p->packet->dts != AV_NOPTS_VALUE, getPacketDts, p);
  DECLARE_GETTER3("size", p->packet->size > 0, getPacketSize, p);
  DECLARE_GETTER3("stream_index", true, getPacketStreamIndex, p);
  DECLARE_GETTER3("flags", p->packet->flags > 0, getPacketFlags, p);
  DECLARE_GETTER3("side_data", p->packet->side_data != nullptr, getPacketSideData, p);
  DECLARE_GETTER3("duration", p->packet->duration > 0, getPacketDuration, p);
  DECLARE_GETTER3("pos", p->packet->pos > 0, getPacketPos, p);

  status = napi_define_properties(env, result, count, desc);
  CHECK_STATUS;

  return result;
}

napi_status fromAVPacket(napi_env env, packetData* p, napi_value* result) {
  napi_status status;
  napi_value jsPacket, extPacket, typeName;
  int64_t externalMemory;

  status = napi_create_object(env, &jsPacket);
  PASS_STATUS;
  status = napi_create_string_utf8(env, "Packet", NAPI_AUTO_LENGTH, &typeName);
  PASS_STATUS;
  status = napi_create_external(env, p, packetDataFinalizer, nullptr, &extPacket);
  PASS_STATUS;

  napi_property_descriptor desc[] = {
    { "type", nullptr, nullptr, getPacketTypeName, nop, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), nullptr },
    { "pts", nullptr, nullptr, getPacketPts, setPacketPts, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "dts", nullptr, nullptr, getPacketDts, setPacketDts, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "data", nullptr, nullptr, getPacketData, setPacketData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "size", nullptr, nullptr, getPacketSize, setPacketSize, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "stream_index", nullptr, nullptr, getPacketStreamIndex, setPacketStreamIndex, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "flags", nullptr, nullptr, getPacketFlags, setPacketFlags, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "side_data", nullptr, nullptr, getPacketSideData, setPacketSideData, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "duration", nullptr, nullptr, getPacketDuration, setPacketDuration, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    // 10
    { "pos", nullptr, nullptr, getPacketPos, setPacketPos, nullptr,
      (napi_property_attributes) (napi_writable | napi_enumerable), p },
    { "toJSON", nullptr, packetToJSON, nullptr, nullptr, nullptr, napi_default, p },
    { "_packet", nullptr, nullptr, nullptr, nullptr, extPacket, napi_default, nullptr }
  };
  status = napi_define_properties(env, jsPacket, 12, desc);
  PASS_STATUS;

  if (p->packet->buf != nullptr) {
    p->extSize = p->packet->buf->size;
    // printf("Size of buffer is %i\n", p->extSize);
    status = napi_adjust_external_memory(env, p->extSize, &externalMemory);
    PASS_STATUS;
  }

  *result = jsPacket;
  return napi_ok;
}

void packetFinalizer(napi_env env, void* data, void* hint) {
  AVPacket* pkt = (AVPacket*) data;
  av_packet_free(&pkt);
}

void packetDataFinalizer(napi_env env, void* data, void* hint) {
  napi_status status;
  napi_ref dataRef;
  int64_t externalMemory;
  packetData* p = (packetData*) data;
  dataRef = p->dataRef;
  status = napi_adjust_external_memory(env, -p->extSize, &externalMemory);
  if (status != napi_ok) {
    printf("DEBUG: Failed to adjust external memory downwards on packet delete.\n");
  }
  delete p;
  if (dataRef != nullptr) {
    status = napi_delete_reference(env, dataRef);
    if (status != napi_ok) {
      printf("DEBUG: Failed to delete data reference for packet data, status %i.\n", status);
    }
  }
}

void packetBufferFinalizer(napi_env env, void* data, void* hint) {
  AVBufferRef* hintRef = (AVBufferRef*) hint;
  av_buffer_unref(&hintRef);
};

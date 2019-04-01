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

const test = require('tape');
const beamcoder = require('../index.js');

test('Creating codec parameters', t => {
  let cps = beamcoder.codecParameters();
  t.ok(cps, 'is truthy.');
  t.equal(cps.name, 'none', 'has no name.');
  t.equal(cps.codec_type, 'data', 'has data type.');
  t.equal(cps.codec_id, 0, 'zero coded id.');
  cps = beamcoder.codecParameters({ name: 'aac' });
  t.equal(cps.name, 'aac', 'has expected name aac.');
  t.equal(cps.codec_type, 'audio', 'has expected type.');
  t.equal(cps.codec_id, 86018, 'zero coded id.');
  cps = beamcoder.codecParameters({ name: 'h264', width: 1920 });
  t.equal(cps.width, 1920, 'constructor parameter set ok.');
  t.end();
});

test('Minimal JSON serialization', t => {
  let cp = beamcoder.codecParameters();
  let cps = JSON.stringify(cp);
  t.equal(typeof cps, 'string', 'stringify creates a string.');
  let cpj = JSON.parse(cps);
  t.deepEqual(cpj, { type: 'CodecParameters', codec_type: 'data',
    codec_id: 0, name: 'none'}, 'is minimal.');
  let rcp = beamcoder.codecParameters(cps);
  t.ok(rcp, 'roundtrip parameters are truthy.');
  t.deepEqual(rcp, { type: 'CodecParameters',
    codec_type: 'data',
    codec_id: 0,
    name: 'none',
    codec_tag: 0,
    extradata: null,
    format: null,
    bit_rate: 0,
    bits_per_coded_sample: 0,
    bits_per_raw_sample: 0,
    profile: -99,
    level: -99,
    width: 0,
    height: 0,
    sample_aspect_ratio: [ 0, 1 ],
    field_order: 'unknown',
    color_range: 'unknown',
    color_primaries: 'unknown',
    color_trc: 'unknown',
    color_space: 'unknown',
    chroma_location: 'unspecified',
    video_delay: 0,
    channel_layout: '0 channels',
    channels: 0,
    sample_rate: 0,
    block_align: 0,
    frame_size: 0,
    initial_padding: 0,
    trailing_padding: 0,
    seek_preroll: 0 }, 'has expected value.');
  t.end();
});

test('Maximal JSON serialization', t=> {
  let cp = beamcoder.codecParameters({
    codec_type: 'video',
    codec_id: 27,
    codec_tag: 'avc1',
    name: 'h264',
    extradata: Buffer.from('wibble'),
    format: 'yuv422p',
    bit_rate: 12345,
    bits_per_coded_sample: 42,
    bits_per_raw_sample: 43,
    profile: 3,
    level: 4,
    width: 44,
    height: 45,
    sample_aspect_ratio: [ 46, 47 ],
    field_order: 'progressive',
    color_range: 'pc',
    color_primaries: 'bt709',
    color_trc: 'bt709',
    color_space: 'bt709',
    chroma_location: 'left',
    video_delay: 48,
    channel_layout: 'stereo',
    channels: 2,
    sample_rate: 48000,
    block_align: 8,
    frame_size: 1920,
    initial_padding: 48,
    trailing_padding: 49,
    seek_preroll: 50 });
  let cps = JSON.stringify(cp);
  t.equal(typeof cps, 'string', 'stringify makes a string.');
  let rcp = beamcoder.codecParameters(cps);
  t.ok(rcp, 'roundtrip value is truthy.');
  t.deepEqual(rcp, { type: 'CodecParameters',
    codec_type: 'video',
    codec_id: 27,
    codec_tag: 'avc1',
    name: 'h264',
    extradata: Buffer.from('wibble'),
    format: 'yuv422p',
    bit_rate: 12345,
    bits_per_coded_sample: 42,
    bits_per_raw_sample: 43,
    profile: 3,
    level: 4,
    width: 44,
    height: 45,
    sample_aspect_ratio: [ 46, 47 ],
    field_order: 'progressive',
    color_range: 'pc',
    color_primaries: 'bt709',
    color_trc: 'bt709',
    color_space: 'bt709',
    chroma_location: 'left',
    video_delay: 48,
    channel_layout: 'stereo',
    channels: 2,
    sample_rate: 48000,
    block_align: 8,
    frame_size: 1920,
    initial_padding: 48,
    trailing_padding: 49,
    seek_preroll: 50 }, 'has expected value.');
  t.end();
});

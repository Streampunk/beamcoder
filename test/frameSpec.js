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
const util = require('util');

test('Create a frame', t => {
  let fr = beamcoder.frame();
  t.ok(fr, 'is truthy.');
  t.end();
});

test('Minimal JSON serialization', t => {
  let fr = beamcoder.frame({});
  let fp = JSON.stringify(fr);
  t.ok(fp, 'JSON serialization is truthy.');
  let pfp = JSON.parse(fp);
  t.deepEqual(pfp, { type: 'Frame', linesize: [], reordered_opaque: 0 }, 'makes minimal JSON.');
  let rf = beamcoder.frame(fp);
  t.ok(rf, 'roundtrip is truthy.');
  t.equal(util.inspect(rf), util.inspect(beamcoder.frame()), 'same as a new frame.');
  t.end();
});

test('Maximal JSON serialization', t => {
  let fr = beamcoder.frame({ type: 'Frame',
    linesize: [42],
    width: 43,
    height: 44,
    nb_samples: 45,
    format: 'fltp',
    key_frame: false,
    pict_type: 'I',
    sample_aspect_ratio: [ 16, 9 ],
    pts: 46,
    pkt_dts: 47,
    coded_picture_number: 48,
    display_picture_number: 49,
    quality: 50,
    repeat_pict: 51,
    interlaced_frame: true,
    top_field_first: true,
    palette_has_changed: true,
    reordered_opaque: 52,
    sample_rate: 48000,
    channel_layout: 'stereo',
    data: [ Buffer.from('wibble wobble wibble wobble jelly on a place at least 42 chars and some more')],
    side_data: { replaygain: Buffer.from('wibble') },
    flags: { CORRUPT: true, DISCARD: false },
    color_range: 'tv',
    color_primaries: 'bt709',
    color_trc: 'bt709',
    colorspace: 'bt709',
    chroma_location: 'top',
    best_effort_timestamp: 53,
    pkt_pos: 54,
    pkt_duration: 55,
    metadata: { fred: 'ginger' },
    decode_error_flags: { INVALID_BITSTREAM: true, MISSING_REFERENCE: false },
    channels: 2,
    pkt_size: 56,
    crop_top: 57,
    crop_bottom: 58,
    crop_left: 59,
    crop_right: 60 });
  t.ok(fr, 'frame is truthy.');
  let fs = JSON.stringify(fr);
  t.equal(typeof fs, 'string', 'stringify created a string.');
  let rfr = beamcoder.frame(fs);
  t.ok(rfr, 'roundtrip packet is truthy.');
  t.deepEqual(util.inspect(rfr), util.inspect(beamcoder.frame({
    linesize: [42],
    width: 43,
    height: 44,
    nb_samples: 45,
    format: 'fltp',
    key_frame: false,
    pict_type: 'I',
    sample_aspect_ratio: [ 16, 9 ],
    pts: 46,
    pkt_dts: 47,
    coded_picture_number: 48,
    display_picture_number: 49,
    quality: 50,
    repeat_pict: 51,
    interlaced_frame: true,
    top_field_first: true,
    palette_has_changed: true,
    reordered_opaque: 52,
    sample_rate: 48000,
    channel_layout: 'stereo',
    data: [], // Data does not roundtrip
    side_data: { replaygain: Buffer.from('wibble') },
    flags: { CORRUPT: true, DISCARD: false },
    color_range: 'tv',
    color_primaries: 'bt709',
    color_trc: 'bt709',
    colorspace: 'bt709',
    chroma_location: 'top',
    best_effort_timestamp: 53,
    pkt_pos: 54,
    pkt_duration: 55,
    metadata: { fred: 'ginger' },
    decode_error_flags: { INVALID_BITSTREAM: true, MISSING_REFERENCE: false },
    channels: 2,
    pkt_size: 56,
    crop_top: 57,
    crop_bottom: 58,
    crop_left: 59,
    crop_right: 60 })), 'roundtrips expected value.');
  t.end();
});

test('Can delete data', t => {
  let f = beamcoder.frame({ width: 1920, height: 1080, format: 'yuv420p' }).alloc();
  t.ok(Array.isArray(f.data), 'data is an array ...');
  t.ok(f.data.every(x => Buffer.isBuffer(x)), '... of buffers.');
  t.equal(f.data.length, 3, 'data buffer has length 3.');
  t.deepEqual(f.data.map(x => x.length),
    f.linesize.map(x => x * f.height),
    'buffer sizes as expected.');
  f.data = null;
  t.ok(Array.isArray(f.data), 'After reset, data is an array ...');
  t.equal(f.data.length, 0, 'of length zero.');
  t.end();
});

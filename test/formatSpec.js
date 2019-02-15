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

const isExternal = o => Object.toString(o).indexOf('native code') >= 0;

test('Creating a format', t => {
  let fmt = beamcoder.format();
  t.ok(fmt, 'is truthy.');
  t.equal(fmt.type, 'format', 'calls itself type format.');
  t.equal(fmt.iformat, null, 'has no input format.');
  t.equal(fmt.oformat, null, 'has no output format.');
  t.equal(fmt.streams.length, 0, 'has no streams.');
  fmt = beamcoder.format({ url: 'file:fred.wav', flags: { NONBLOCK: true }});
  t.ok(fmt, 'with parameters is truthy.');
  t.equal(fmt.url, 'file:fred.wav', 'has url property set as expected.');
  t.ok(fmt.flags.NONBLOCK, 'NONBLOCK flag has been set OK.');
  t.ok(fmt.flags.AUTO_BSF, 'AUTOBSF flag is still set.');
  t.end();
});

const stripNewStream = ({ newStream, ...others }) => ({ ...others }); // eslint-disable-line no-unused-vars

test('Minimal JSON serialization', t => {
  let fmt = beamcoder.format();
  let fmts = JSON.stringify(fmt);
  t.equal(typeof fmts, 'string', 'stringify creates a string.');
  let fmtj = JSON.parse(fmts);
  t.deepEqual(fmtj, { type: 'format', streams: [], interleaved: true },
    'is minimal.');
  let rfmt = beamcoder.format(fmts);
  t.ok(rfmt, 'roundtrip format is truthy.');
  console.log(stripNewStream(rfmt));
  t.deepEqual(stripNewStream(rfmt), {
    type: 'format',
    oformat: null,
    iformat: null,
    priv_data: null,
    ctx_flags: { NOHEADER: false, UNSEEKABLE: false },
    streams: [],
    url: '',
    start_time: 0,
    duration: 0,
    bit_rate: 0,
    packet_size: 0,
    max_delay: -1,
    flags:
     { GENPTS: false,
       IGNIDX: false,
       NONBLOCK: false,
       IGNDTS: false,
       NOFILLIN: false,
       NOPARSE: false,
       NOBUFFER: false,
       CUSTOM_IO: false,
       DISCARD_CORRUPT: false,
       FLUSH_PACKETS: false,
       BITEXACT: false,
       SORT_DTS: false,
       PRIV_OPT: false,
       FAST_SEEK: false,
       SHORTEST: false,
       AUTO_BSF: true },
    probesize: 5000000,
    max_analyze_duration: 0,
    key: null,
    programs: [],
    max_index_size: 1048576,
    max_picture_buffer: 3041280,
    metadata: {},
    start_time_realtime: null,
    fps_probe_size: -1,
    error_recognition: 1,
    debug: { TS: false },
    max_interleave_delta: 10000000,
    strict_std_compliance: 'normal',
    event_flags: { METADATA_UPDATED: false },
    max_ts_probe: 50,
    avoid_negative_ts: 'auto',
    audio_preload: 0,
    max_chunk_duration: 0,
    max_chunk_size: 0,
    use_wallclock_as_timestamps: false,
    avio_flags:
     { READ: false, WRITE: false, NONBLOCK: false, DIRECT: false },
    duration_estimation_method: 'from_pts',
    skip_initial_bytes: 0,
    correct_ts_overflow: true,
    seek2any: false,
    flush_packets: -1,
    probe_score: 0,
    format_probesize: 1048576,
    codec_whitelist: null,
    format_whitelist: null,
    io_repositioned: false,
    metadata_header_padding: -1,
    output_ts_offset: 0,
    dump_separator: ', ',
    protocol_whitelist: null,
    protocol_blacklist: null,
    max_streams: 1000,
    skip_estimate_duration_from_pts: false,
    interleaved: true }, 'has expected value.');
  t.end();
});

test('Maximal JSON serialization', t => {
  let fmt = beamcoder.format({
    type: 'format',
    oformat: null,
    iformat: null,
    priv_data: null,
    ctx_flags: { NOHEADER: true, UNSEEKABLE: false },
    streams: [],
    url: 'file:test.wav',
    start_time: 42,
    duration: 43,
    bit_rate: 44,
    packet_size: 45,
    max_delay: 46,
    flags:
     { GENPTS: true,
       IGNIDX: false,
       NONBLOCK: true,
       IGNDTS: false,
       NOFILLIN: true,
       NOPARSE: false,
       NOBUFFER: true,
       CUSTOM_IO: false,
       DISCARD_CORRUPT: true,
       FLUSH_PACKETS: false,
       BITEXACT: true,
       SORT_DTS: false,
       PRIV_OPT: true,
       FAST_SEEK: false,
       SHORTEST: true,
       AUTO_BSF: true },
    probesize: 6000000,
    max_analyze_duration: 47,
    key: Buffer.from('Unlocker'),
    programs: [],
    max_index_size: 48,
    max_picture_buffer: 49,
    metadata: { wibble: 'wobble', otherwise: 50 },
    start_time_realtime: 50,
    fps_probe_size: 51,
    error_recognition: 2,
    debug: { TS: true },
    max_interleave_delta: 52,
    strict_std_compliance: 'strict',
    event_flags: { METADATA_UPDATED: true },
    max_ts_probe: 53,
    avoid_negative_ts: 'make_zero',
    audio_preload: 54,
    max_chunk_duration: 55,
    max_chunk_size: 56,
    use_wallclock_as_timestamps: true,
    avio_flags:
     { READ: true, WRITE: false, NONBLOCK: true, DIRECT: false },
    duration_estimation_method: 'from_stream',
    skip_initial_bytes: 57,
    correct_ts_overflow: false,
    seek2any: true,
    flush_packets: 58,
    probe_score: 59,
    format_probesize: 60,
    codec_whitelist: 'h264,hevc',
    format_whitelist: 'mxf,avi',
    io_repositioned: true,
    metadata_header_padding: 61,
    output_ts_offset: 62,
    dump_separator: '::',
    protocol_whitelist: 'http,rtp',
    protocol_blacklist: 'rtmp',
    max_streams: 53,
    skip_estimate_duration_from_pts: true,
    interleaved: false });
  let fmts = JSON.stringify(fmt, null, 2);
  t.equal(typeof fmts, 'string', 'stringify creates a string.');
  let fmtj = JSON.parse(fmts);
  t.ok(fmtj, 'parsed JSON is truthy.');
  let rfmt = beamcoder.format(fmts);
  t.ok(rfmt, 'roundtrip format is truthy.');
  t.deepEqual(stripNewStream(rfmt), {
    type: 'format',
    oformat: null,
    iformat: null,
    priv_data: null,
    ctx_flags: { NOHEADER: false, UNSEEKABLE: false }, // set by libav
    streams: [],
    url: 'file:test.wav',
    start_time: 0, // Always set by libav
    duration: 43,
    bit_rate: 44,
    packet_size: 45,
    max_delay: 46,
    flags:
     { GENPTS: true,
       IGNIDX: false,
       NONBLOCK: true,
       IGNDTS: false,
       NOFILLIN: true,
       NOPARSE: false,
       NOBUFFER: true,
       CUSTOM_IO: false,
       DISCARD_CORRUPT: true,
       FLUSH_PACKETS: false,
       BITEXACT: true,
       SORT_DTS: false,
       PRIV_OPT: true,
       FAST_SEEK: false,
       SHORTEST: true,
       AUTO_BSF: true },
    probesize: 6000000,
    max_analyze_duration: 47,
    key: Buffer.from('Unlocker'),
    programs: [],
    max_index_size: 48,
    max_picture_buffer: 49,
    metadata: { wibble: 'wobble', otherwise: '50' }, // numbers become strings
    start_time_realtime: 50,
    fps_probe_size: 51,
    error_recognition: 2,
    debug: { TS: true },
    max_interleave_delta: 52,
    strict_std_compliance: 'strict',
    event_flags: { METADATA_UPDATED: true },
    max_ts_probe: 53,
    avoid_negative_ts: 'make_zero',
    audio_preload: 54,
    max_chunk_duration: 55,
    max_chunk_size: 56,
    use_wallclock_as_timestamps: true,
    avio_flags:
     { READ: true, WRITE: false, NONBLOCK: true, DIRECT: false },
    duration_estimation_method: 'from_pts', // Read only, set by libav
    skip_initial_bytes: 57,
    correct_ts_overflow: false,
    seek2any: true,
    flush_packets: 58,
    probe_score: 0, // Read only, set by libav
    format_probesize: 60,
    codec_whitelist: 'h264,hevc',
    format_whitelist: 'mxf,avi',
    io_repositioned: false, // Set by libav
    metadata_header_padding: 61,
    output_ts_offset: 62,
    dump_separator: '::',
    protocol_whitelist: 'http,rtp',
    protocol_blacklist: 'rtmp',
    max_streams: 53,
    skip_estimate_duration_from_pts: true,
    interleaved: false }, 'has expected value.');
  t.end();
});

test('Test minimal JSON stream', t => {
  let fmt = beamcoder.format();
  let s1 = fmt.newStream('h264');
  let ss = JSON.stringify(s1, null, 2);
  t.equal(typeof ss, 'string', 'stringify creates a string.');
  console.log(ss);
  t.deepEqual(JSON.parse(ss), {
    type: 'Stream', index: 0, id: 0, time_base: [0,0],
    codecpar: { type: 'CodecParameters', codec_type: 'video', codec_id: 27, name: 'h264' } });

  let s2 = fmt.newStream(ss);
  t.ok(s2, 'creation of new stream from string is truthy.');
  console.log(s2.toJSON());
  t.deepEqual(JSON.parse(JSON.stringify(s2)),  {
    type: 'Stream', index: 1, id: 0, time_base: [0,0],
    codecpar: { type: 'CodecParameters', codec_type: 'video', codec_id: 27, name: 'h264' } });
  t.ok(s2.codecpar && s2.codecpar._codecPar && isExternal(s2.codecpar._codecPar),
    's2 has external codec parameters.');
  t.ok(isExternal(s2._stream), 's2 has external _stream.');

  let s3 = fmt.newStream(JSON.parse(ss));
  t.ok(s3, 'creation of new stream from parsed object is truthy.');
  t.deepEqual(JSON.parse(JSON.stringify(s3)), {
    type: 'Stream', index: 2, id: 0, time_base: [0,0],
    codecpar: { type: 'CodecParameters', codec_type: 'video', codec_id: 27, name: 'h264' } });
  t.ok(s3.codecpar && s3.codecpar._codecPar && isExternal(s3.codecpar._codecPar),
    's3 has external codec parameters.');
  t.ok(isExternal(s3._stream), 's3 has external _stream.');

  let fmt2 = beamcoder.format(JSON.stringify(fmt));
  t.ok(fmt2, 'construction of new format form JSON is truthy.');
  t.equal(fmt2.streams.length, 3, 'has expected number of streams.');
  t.deepEqual(fmt2.streams.map(JSON.stringify).map(JSON.parse),
    [s1, s2, s3].map(JSON.stringify).map(JSON.parse), 'has expected streams.');

  t.throws(() => fmt2.streams = [], /construction/,
    'cannot set streams after construction.');
  t.end();
});

test('Can set IO formats on construction', t => {
  let ifmt = beamcoder.format({ iformat: 'wav' });
  t.ok(ifmt.iformat, 'iformat has become truthy.');
  t.ok(ifmt.priv_data, 'private data has been created.');
  t.equal(ifmt.type, 'demuxer', 'has turned into a demuxer.');
  t.equal(ifmt.iformat.name, 'wav', 'iformat has the expected name.');

  ifmt.iformat = null;
  t.equal(ifmt.iformat, null, 'can be set back to null.');
  t.equal(ifmt.type, 'format', 'changing the name back to format.');
  t.equal(ifmt.priv_data, null, 'resetting the priv_data.');

  let ofmt = beamcoder.format({ oformat: 'hevc' });
  t.ok(ofmt.oformat, 'oformat has become truthy.');
  t.equal(ofmt.priv_data, null, 'private data is not set.');
  t.equal(ofmt.type, 'muxer', 'has turned into a muxer.');
  t.equal(ofmt.oformat.name, 'hevc', 'oformat has the expected name.');

  ofmt.oformat = 'wav';
  t.equal(ofmt.oformat.name, 'wav', 'oformat has the expected name.');
  t.ok(ofmt.priv_data, 'has private data.');
  t.equal(typeof ofmt.priv_data.write_bext, 'boolean', 'private data appears as expected.');

  t.throws(() => { ifmt.iformat = 'wibble'; }, /Unable/, 'bad iformat name throws.');
  t.throws(() => { ofmt.oformat = 'wibble'; }, /Unable/, 'bad oformat name throws.');
  t.end();
});

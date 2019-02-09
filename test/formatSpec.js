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

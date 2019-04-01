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

test('Create a packet', t => {
  let pkt = beamcoder.packet();
  t.ok(pkt, 'is truthy.');
  t.equal(typeof pkt._packet, 'object', 'external value present.');
  t.deepEqual(pkt, { type: 'Packet',
    pts: null,
    dts: null,
    data: null,
    size: 0,
    stream_index: 0,
    flags:
     { KEY: false,
       CORRUPT: false,
       DISCARD: false,
       TRUSTED: false,
       DISPOSABLE: false },
    side_data: null,
    duration: 0,
    pos: -1 }, 'has expected minimal value.');
  t.end();
});

test('Minimal JSON serialization', t => {
  let pkt = beamcoder.packet();
  t.equal(typeof pkt.toJSON, 'function', 'has hidden toJSON function.');
  let ps = JSON.stringify(pkt);
  t.equal(typeof ps, 'string', 'stringify created a string.');
  let pps = JSON.parse(ps);
  t.deepEqual(pps, { type: 'Packet', stream_index: 0 }, 'made minimal value.');
  let rpkt = beamcoder.packet(ps);
  t.ok(rpkt, 'roundtrip packet is truthy.');
  t.deepEqual(rpkt, { type: 'Packet',
    pts: null,
    dts: null,
    data: null,
    size: 0,
    stream_index: 0,
    flags:
     { KEY: false,
       CORRUPT: false,
       DISCARD: false,
       TRUSTED: false,
       DISPOSABLE: false },
    side_data: null,
    duration: 0,
    pos: -1 }, 'has expected minimal value.');
  t.end();
});

test('Maximal JSON serialization', t => {
  let pkt = beamcoder.packet({ type: 'Packet',
    pts: 42,
    dts: 43,
    data: Buffer.from('wibble'),
    stream_index: 7,
    flags:
     { KEY: true,
       CORRUPT: false,
       DISCARD: false,
       TRUSTED: true,
       DISPOSABLE: false },
    side_data: { replaygain: Buffer.from('wobble') },
    duration: 44,
    pos: 45 });
  let ps = JSON.stringify(pkt);
  t.equal(typeof ps, 'string', 'stringify created a string.');
  let rpkt = beamcoder.packet(ps);
  t.ok(rpkt, 'roundtrip packet is truthy.');
  t.deepEqual(rpkt, { type: 'Packet',
    pts: 42,
    dts: 43,
    data: null,
    size: 0,
    stream_index: 7,
    flags:
     { KEY: true,
       CORRUPT: false,
       DISCARD: false,
       TRUSTED: true,
       DISPOSABLE: false },
    side_data:
     { type: 'PacketSideData',
       replaygain: Buffer.from([0x77, 0x6f, 0x62, 0x62, 0x6c, 0x65]) },
    duration: 44,
    pos: 45 }, 'roundtrips expected values.');
  t.end();
});

test('Reset packet data', t => {
  let p = beamcoder.packet({ pts: 42, size: 4321, data: Buffer.alloc(4321 + beamcoder.AV_INPUT_BUFFER_PADDING_SIZE) });
  t.ok(Buffer.isBuffer(p.data), 'data is a buffer.');
  t.notEqual(p.data.length, p.size, 'data length is greater than the packet size.');
  t.equal(p.data.length, 4321 + beamcoder.AV_INPUT_BUFFER_PADDING_SIZE, 'length is as expected.');
  p.data = null;
  t.equal(p.data, null, 'after reset, packet data is set to null.');
  t.equal(p.size, 4321, 'after reset, size remains at original value.');
  t.end();
});

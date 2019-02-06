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
  t.equal(cps.codec_type, 'unknown', 'has unknown type.');
  t.equal(cps.codec_id, 0, 'zero coded id.');
  cps = beamcoder.codecParameters({ name: 'aac' });
  t.equal(cps.name, 'aac', 'has expected name aac.');
  t.equal(cps.codec_type, 'audio', 'has expected type.');
  t.equal(cps.codec_id, 86018, 'zero coded id.');
  t.throws(() => beamcoder.codecParameters({ name: 'wibble' }), /Codec parameter/,
    'throws for an unknown codec name.');
  cps = beamcoder.codecParameters({ name: 'h264', width: 1920 });
  t.equal(cps.width, 1920, 'contrcutor parameter set ok.');
  t.end();
});

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

test('Creating a muxer', t => {
  let mx = beamcoder.muxer({ name: 'mpegts' });
  t.ok(mx, 'is truthy.');
  t.equal(typeof mx.iformat, 'undefined', 'input format is undefined.');
  t.ok(mx.oformat, 'has output format.');
  t.equal(mx.oformat.name, 'mpegts', 'output format is mpegts.');
  t.equal(mx.type, 'muxer', 'type name is set to muxer.');
  t.throws(() => beamcoder.muxer({ name: 'wibble' }), 'throws when unknown name.');
  t.end();
});

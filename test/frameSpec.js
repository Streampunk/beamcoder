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
  let fr = beamcoder.frame();
  let fp = JSON.stringify(fr);
  t.ok(fp, 'JSON serialization is truthy.');
  let pfp = JSON.parse(fp);
  t.deepEqual(pfp, { type: 'Frame', linesize: [] }, 'makes minimal JSON.');
  let rf = beamcoder.frame(fp);
  t.ok(rf, 'roundtrip is truthy.');
  t.equal(util.inspect(rf), util.inspect(beamcoder.frame()), 'same as a new frame.');
  t.end();
});

// TODO JSON maximal test

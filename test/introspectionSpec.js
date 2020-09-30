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

test('Version information', t => {
  const verPos = beamcoder.avVersionInfo().indexOf('4.');
  t.ok(verPos === 0 || verPos === 1, 'has expected version number.');
  t.equals(typeof beamcoder.versions(), 'object', 'versions is an object.');
  t.equals(Object.keys(beamcoder.versions()).length, 8, 'versions has 8 entries.');
  t.ok(Object.values(beamcoder.versions()).every(x => typeof x === 'number' && x > 0),
    'versions values are numbers.');
  t.equals(typeof beamcoder.versionStrings(), 'object',
    'versionStrings is an object.');
  t.equals(Object.keys(beamcoder.versions()).length, 8,
    'versionStrings is an object.');
  t.ok(Object.values(beamcoder.versionStrings()).every(x =>
    typeof x === 'string' && x.match(/\d+\.\d+\.\d+/)),
  'versionStrings match expected pattern.');
  t.equals(typeof beamcoder.configurations(), 'object',
    'configurations is an object.');
  t.equals(Object.keys(beamcoder.configurations()).length, 8,
    'configurations has 8 entries.');
  t.ok(Object.values(beamcoder.configurations()).every(x => typeof x === 'string'),
    'configurations entry are strings.');
  t.end();
});

test('Muxer information', t => {
  let muxers = beamcoder.muxers();
  t.ok(muxers, 'muxers is truthy.');
  t.equal(typeof muxers, 'object', 'muxers is an object.');
  t.ok(JSON.stringify(muxers), 'can be converted to JSON.');
  t.end();
});

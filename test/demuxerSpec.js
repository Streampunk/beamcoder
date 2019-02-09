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

test('Creating a demuxer', async t => {
  let dm = await beamcoder.demuxer('https://www.elecard.com/storage/video/bbb_1080p_c.ts');
  t.ok(dm, 'is truthy.');
  t.equal(dm.type, 'demuxer', 'type name says demuxer.');
  t.equal(typeof dm.oformat, 'undefined', 'output format is undefined.');
  t.ok(dm.iformat, 'has an input format.');
  t.equal(dm.iformat.name, 'mpegts', 'input format is mpegts.');
  t.equal(dm.streams.length, 2, 'has 2 streams.');
  try {
    await beamcoder.demuxer('file:jaberwocky.junk');
    t.fail('Did not throw when opening non-existant file.');
  } catch(e) {
    console.log(e.message);
    t.ok(e.message.match(/Problem opening/), 'throws opening non-existant file.');
  }
  t.end();
});

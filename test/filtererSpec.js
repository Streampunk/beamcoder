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

test('Create a filterer', async t => {
  let flt = await beamcoder.filterer({
    filterType: 'audio',
    inputParams: [
      {
        sampleRate: 48000,
        sampleFormat: 's16',
        channelLayout: 'mono',
        timeBase: [1, 48000]
      }
    ],
    outputParams: [
      {
        sampleRate: 8000,
        sampleFormat: 's16',
        channelLayout: 'mono'
      }
    ],
    filterSpec: 'aresample=8000, aformat=sample_fmts=s16:channel_layouts=mono'
  });
  t.ok(flt, 'is truthy.');
  t.end();
});

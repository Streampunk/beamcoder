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

const beamcoder = require('../index.js');

async function run() {
  const urls = [ 'file:../Media/sound/Countdown.wav' ];
  const spec = { start: 50, end: 58 };
  const params = {
    video: [],
    audio: [
      {
        sources: [
          { url: urls[0], ms: spec, streamIndex: 0 }
        ],
        filterSpec: '[in0:a] \
                     volume=precision=float:volume=0.8 \
                     [out0:a]',
        streams: [
          { name: 'aac', time_base: [1, 90000],
            codecpar: {
              sample_rate: 48000, format: 'fltp', channel_layout: 'stereo'
            }
          }
        ]
      },
    ],
    out: {
      formatName: 'mp4',
      url: 'file:temp.mp4'
    }
  };

  await beamcoder.makeSources(params);
  const beamStreams = await beamcoder.makeStreams(params);
  await beamStreams.run();
}

console.log('Running wav maker');
let start = Date.now();
run()
  .then(() => console.log(`Finished ${Date.now() - start}ms`))
  .catch(console.error);


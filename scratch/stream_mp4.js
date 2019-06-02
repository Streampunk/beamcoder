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
  const urls = [ 'file:../../Media/big_buck_bunny_1080p_h264.mov' ];
  const spec = { start: 0, end: 24 };

  const params = {
    video: [
      {
        sources: [
          { url: urls[0], ms: spec, streamIndex: 0 }
        ],
        filterSpec: '[in0:v] scale=1280:720, colorspace=all=bt709 [out0:v]',
        streams: [
          { name: 'h264', time_base: [1, 90000],
            codecpar: {
              width: 1280, height: 720, format: 'yuv422p', color_space: 'bt709',
              sample_aspect_ratio: [1, 1]
            }
          }
        ]
      }
    ],
    audio: [
      {
        sources: [
          { url: urls[0], ms: spec, streamIndex: 2 }
        ],
        filterSpec: '[in0:a] aformat=sample_fmts=fltp:channel_layouts=mono [out0:a]',
        streams: [
          { name: 'aac', time_base: [1, 90000],
            codecpar: {
              sample_rate: 48000, format: 'fltp', frame_size: 1024,
              channels: 1, channel_layout: 'mono'
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

console.log('Running mp4 maker');
let start = Date.now();
run()
  .then(() => console.log(`Finished ${Date.now() - start}ms`))
  .catch(console.error);


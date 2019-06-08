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
const fs = require('fs');
const util = require('util'); // eslint-disable-line

async function run() {
  let demuxerStream = beamcoder.demuxerStream({ highwaterMark: 65536 });
  // fs.createReadStream('../../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf').pipe(demuxerStream);
  fs.createReadStream('../../media/sound/BBCNewsCountdown.wav').pipe(demuxerStream);

  let demuxer = await demuxerStream.demuxer();
  console.log(demuxer.streams);

  let decoder = await beamcoder.decoder({ demuxer: demuxer, stream_index : 0 });
  // console.log(decoder);

  const audStream = demuxer.streams[0];
  let filterer = await beamcoder.filterer({
    filterType: 'audio',
    inputParams: [
      {
        name: '0:a',
        sampleRate: audStream.codecpar.sample_rate,
        sampleFormat: audStream.codecpar.format,
        channelLayout: 'stereo', //audStream.codecpar.channel_layout,
        timeBase: audStream.time_base
      }
    ],
    outputParams: [
      {
        name: 'out0:a',
        sampleRate: 8000,
        sampleFormat: 's16',
        channelLayout: 'stereo'
      }
    ],
    filterSpec: '[0:a] aresample=8000, aformat=sample_fmts=s16:channel_layouts=stereo [out0:a]'
  });
  // console.log(filterer.graph);
  // console.log(util.inspect(filterer.graph.filters[2], {depth: null}));
  console.log(filterer.graph.dump());

  // const abuffersink = filterer.graph.filters.find(f => 'abuffersink' === f.filter.name);
  // console.log(util.inspect(abuffersink, {depth: null}));

  let packet = {};
  for ( let x = 0 ; x < 10000 && packet !== null ; x++ ) {
    packet = await demuxer.read();
    if (packet && packet.stream_index == 0) {
      // console.log(packet);
      let frames = await decoder.decode(packet);
      // console.log(frames);

      let filtFrames = await filterer.filter([ // eslint-disable-line
        { name: '0:a', frames: frames.frames }
      ]);
      // console.log(filtFrames);
    }
  }
}

run().catch(console.error);

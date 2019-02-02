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
const createDemuxer = beamcoder.createDemuxer;
const fs = require('fs');
const util = require('util');

async function run() {
  // const srcStream = fs.createReadStream('../../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf');
  // let demuxer = await createDemuxer(srcStream);
  let demuxer = await createDemuxer('../../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf');
  // console.log(demuxer);

  let decoder = await beamcoder.decoder({ name: 'h264' });
  // console.log(decoder);

  const vidStream = demuxer.streams[0];
  let filterer = await beamcoder.filterer({
    filterType: 'video',
    inputParams: [
      {
        name: 'in0:v',
        width: vidStream.codecpar.width,
        height: vidStream.codecpar.height,
        pixelFormat: vidStream.codecpar.format,
        timeBase: vidStream.time_base,
        pixelAspect: vidStream.sample_aspect_ratio,
      },
      {
        name: 'in1:v',
        width: vidStream.codecpar.width,
        height: vidStream.codecpar.height,
        pixelFormat: vidStream.codecpar.format,
        timeBase: vidStream.time_base,
        pixelAspect: vidStream.sample_aspect_ratio,
      }
    ],
    outputNames: [ 'out0:v' ],
    filterSpec: '[in0:v] scale=1280:720 [left]; [in1:v] scale=640:360 [right]; [left][right] overlay=format=auto:x=640 [out0:v]'
  });
  console.log(filterer.graph);
  console.log(util.inspect(filterer.graph.filters[5], {depth: null}));
  console.log(filterer.graph.dump());

  let encParams = {
    name: 'libx264',
    width: 1280,
    height: 720,
    // bit_rate: 10000000,
    time_base: [1, 25],
    framerate: [25, 1],
    // gop_size: 50,
    // max_b_frames: 1,
    pix_fmt: 'yuv422p',
    priv_data: {
      crf: 23
      // preset: 'slow',
      // profile: 'high422',
      // level: '4.2'
    }
  };

  let encoder = beamcoder.encoder(encParams);
  // console.log(encoder);

  let outFile = fs.createWriteStream('wibble.h264');

  for ( let x = 0 ; x < 1000 ; x++ ) {
    let packet = await demuxer.read();
    if (packet.stream_index == 0) {
      // console.log(packet);
      let frames = await decoder.decode(packet);
      // console.log(frames);
      let filtFrames = await filterer.filter([
        { name: 'in0:v', frames: frames },
        { name: 'in1:v', frames: frames },
      ]);
      // console.log(filtFrames);

      let packets = await encoder.encode(filtFrames[0].frames[0]);
      // console.log(x, packets.totalTime);
      packets.packets.forEach(x => outFile.write(x.data));
    }
  }
  let frames = await decoder.flush();
  console.log('flush', frames.total_time, frames.frames.length);
}

run().catch(console.error);

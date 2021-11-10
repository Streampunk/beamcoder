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
  // let demuxer = await beamcoder.demuxer('../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf');
  // console.log(JSON.stringify(demuxer, null, 2));
  let demuxer = await beamcoder.demuxer('M:/dpp/AS11_4K_8.mxf');
  // let demuxer = await beamcoder.demuxer('M:/dpp/AS11.mxf');
  demuxer.streams.forEach(s => s.discard = (0 == s.index) ? 'default' : 'all');
  // let decoder = beamcoder.decoder({ name: 'h264', thread_count: 4, thread_type: { FRAME: false, SLICE: true } });
  let decoder = beamcoder.decoder({ name: 'h264', thread_count: 1, hwaccel: true });
  // console.dir(decoder, { getters: true, depth: 3 });
  let packet = {};
  for ( let x = 0 ; x < 2000 && packet != null; x++ ) {
    packet = await demuxer.read();
    if (packet && packet.stream_index === 0) {
      //console.log(JSON.stringify(packet, null, 2));
      let frames = await decoder.decode(packet);
      // console.log(JSON.stringify(frames.frames[0], null, 2));
      if (frames.frames[0]) {
        console.log(frames.frames[0].data);
      }
      console.log(x, frames.total_time);
    }
  }
  let frames = await decoder.flush();
  console.log('flush', frames.total_time, frames.length);
}

run();

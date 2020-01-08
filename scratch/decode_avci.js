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
  let demuxer = await beamcoder.demuxer('../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf');
  console.log(JSON.stringify(demuxer, null, 2));
  demuxer.streams.forEach(s => s.discard = (0 == s.index) ? 'default' : 'all');
  let decoder = beamcoder.decoder({ name: 'h264' });
  //console.log(JSON.stringify(decoder, null, 2));
  let packet = {};
  for ( let x = 0 ; x < 200 && packet != null; x++ ) {
    let packet = await demuxer.read();
    if (packet.stream_index === 0) {
      //console.log(JSON.stringify(packet, null, 2));
      let frames = await decoder.decode(packet);
      console.log(JSON.stringify(frames.frames[0], null, 2));
      console.log(x, frames.total_time);
    }
  }
  let frames = await decoder.flush();
  console.log('flush', frames.total_time, frames.length);
  console.log(await demuxer.seek({ pos: 79389000 }));
  console.log(await demuxer.read());
}

run();

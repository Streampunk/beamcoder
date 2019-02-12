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
  let demuxer = await beamcoder.demuxer({ url: '../media/bbb_1080p_c.ts'});
  let decoder = beamcoder.decoder({ name: 'aac' });
  let packet = {};
  for ( let x = 0 ; packet !== null && x < 100 ; x++ ) {
    packet = await demuxer.read();
    if (packet.stream_index == 1) {
      console.log(JSON.stringify(packet, null, 2));
      let frames = await decoder.decode(packet);
      console.log(JSON.stringify(frames.frames[0], null, 2));
    }
  }
}

run();

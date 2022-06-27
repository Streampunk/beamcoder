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

import beamcoder from '..';

async function run() {
  let demuxer = await beamcoder.demuxer('../media/dpp/AS11_DPP_HD_EXAMPLE_1.mxf');
  console.log('streams cnt:', demuxer.streams.length);
  let decoder = await beamcoder.decoder({ demuxer: demuxer, stream_index : 1 });
  console.log('decoder type:', decoder.type);
  for ( let x = 0 ; x < 100 ; x++ ) {
    let packet = await demuxer.read();
    if (packet.stream_index == 1) {
      //console.log(packet);
      let frames = await decoder.decode(packet);
      console.log(`i: ${x} total framews: ${frames.frames.length}`);
      console.log(`nb Channels: ${frames.frames[0].channels}`);
      console.log(`channel_layout: ${frames.frames[0].channel_layout}`);
      console.log(`sample_rate: ${frames.frames[0].sample_rate}`);
    }
  }
}

run();

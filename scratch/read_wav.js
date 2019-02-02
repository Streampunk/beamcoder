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
  let demuxer = await beamcoder.demuxer('../media/sound/BBCNewsCountdown.wav');
  let packet = {};
  for ( let x = 0 ; x < 100 && packet !== null ; x++ ) {
    packet = await demuxer.read();
    console.log(x, packet);
  }
  console.log(await demuxer.seek({ frame : 120 }));
  console.log(await demuxer.read());
}

run();

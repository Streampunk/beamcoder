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
  console.log(demuxer.streams[0].codecpar);
  let muxer = beamcoder.muxer({ filename: 'file:test.wav' });
  let stream = muxer.newStream({
    name: 'pcm_s16le',
    time_base: [1, 48000 ],
    interleaved: false, });
  Object.assign(stream.codecpar, {
    channels: 2,
    sample_rate: 48000,
    format: 's16',
    channel_layout: 'stereo',
    block_align: 4,
    bits_per_coded_sample: 16,
    bit_rate: 48000 * 4 * 8
  });
  /* let stream = muxer.newStream({
    name: 'pcm_s16le',
    time_base: [1, 48000 ],
    codecpar: {
      name: 'pcm_s16le',
      channels: 2,
      sample_rate: 48000,
      format: 's16',
      channel_layout: 'stereo',
      block_align: 4,
      bits_per_coded_sample: 16,
      bit_rate: 48000*4
    }
  }); */
  console.log(stream.codecpar);
  // stream.time_base = demuxer.streams[0].time_base;
  // stream.codecpar = demuxer.streams[0].codecpar;
  await muxer.openIO({ options: { blocksize: 8192 }}).then(console.log);
  await muxer.writeHeader({ options: { write_bext: true, write_peak: 'on', peak_format: 2 }}).then(console.log);
  let packet = {};
  for ( let x = 0 ; x < 100 && packet !== null ; x++ ) {
    packet = await demuxer.read();
    await muxer.writeFrame(packet);
  }
  await muxer.writeTrailer();
}

run();

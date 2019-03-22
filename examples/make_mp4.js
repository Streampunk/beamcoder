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

/* Generate a 200 frame test pattern and encode it as a raw H.264 file.

  Usage: node encode_h264.js <filename.h264>

  Output can be viewed in VLC. Make sure "All Files" is selected to see the file.
*/

const beamcoder = require('../index.js'); // Use require('beamcoder') externally
const fs = require('fs');

let endcode = Buffer.from([0, 0, 1, 0xb7]);

async function run() {
  let start = process.hrtime();
  let encParams = {
    name: 'libx264',
    width: 1920,
    height: 1080,
    bit_rate: 2000000,
    time_base: [1, 25],
    framerate: [25, 1],
    gop_size: 10,
    max_b_frames: 1,
    pix_fmt: 'yuv420p',
    priv_data: { preset: 'slow' }
  };

  let encoder = await beamcoder.encoder(encParams);
  console.log('Encoder', encoder);

  const mux = beamcoder.muxer({ format_name: 'mp4' });
  let vstr = mux.newStream({
    name: 'h264',
    time_base: [1, 90000],
    interleaved: true }); // Set to false for manual interleaving, true for automatic
  Object.assign(vstr.codecpar, {
    width: 1920,
    height: 1080,
    format: 'yuv420p'
  });
  console.log(vstr);
  await mux.openIO({
    url: 'file:test.mp4'
  });
  await mux.writeHeader();

  let outFile = fs.createWriteStream(process.argv[2]);

  for ( let i = 0 ; i < 200 ; i++ ) {
    let frame = beamcoder.frame({
      width: encParams.width,
      height: encParams.height,
      format: encParams.pix_fmt
    }).alloc();

    let linesize = frame.linesize;
    let [ ydata, bdata, cdata ] = frame.data;
    frame.pts = i+100;

    for ( let y = 0 ; y < frame.height ; y++ ) {
      for ( let x = 0 ; x < linesize[0] ; x++ ) {
        ydata[y * linesize[0] + x] =  x + y + i * 3;
      }
    }

    for ( let y = 0 ; y < frame.height / 2 ; y++) {
      for ( let x = 0; x < linesize[1] ; x++) {
        bdata[y * linesize[1] + x] = 128 + y + i * 2;
        cdata[y * linesize[1] + x] = 64 + x + i * 5;
      }
    }

    let packets = await encoder.encode(frame);
    if ( i % 10 === 0) console.log('Encoding frame', i);
    for (const pkt of packets.packets) {
      pkt.duration = 1;
      pkt.stream_index = vstr.index;
      pkt.pts = pkt.pts * 90000/25;
      pkt.dts = pkt.dts * 90000/25;
      await mux.writeFrame(pkt);
      outFile.write(pkt.data);
    }
  }

  let p2 = await encoder.flush();
  console.log('Flushing', p2.packets.length, 'frames.');
  for (const pkt of p2.packets) {
    pkt.duration = 1;
    pkt.stream_index = vstr.index;
    pkt.pts = pkt.pts * 90000/25;
    pkt.dts = pkt.dts * 90000/25;
    await mux.writeFrame(pkt);
    outFile.write(pkt.data);
  }
  await mux.writeTrailer();
  outFile.end(endcode);

  console.log('Total time ', process.hrtime(start));
}

if (typeof process.argv[2] === 'string') { run(); }
else { console.error('Error: Please provide a file name.'); }

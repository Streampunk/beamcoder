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

/* Main example from the README.md. Run in a folder of media files.

   Only supports 8-bit YUV 4:2:2 or 4:2:0 pixel formats.
*/

import beamcoder from '..'; // Use require('beamcoder') externally
import Koa from 'koa'; // Add koa to package.json dependencies
import fs from 'fs';
import path from 'path';
const app = new Koa();

app.use(async (ctx) => { // Assume HTTP GET with path /<file_name>/<time_in_s>

  if (ctx.path === '/') {
    let list = await fs.promises.readdir('.')
    list = list.filter(a=> a.toLowerCase().endsWith('.mp4'));
    ctx.type = 'text/html';
    ctx.body = `<html><body>
    currenty available files: <ul>${list.map(f => `<li><a href="${f}/1">${f}</a></li>`).join(' ')}</ul>
    Add files in : <b>${path.resolve('.')}</b> for more tests
    </body></html>`;
    return;
  }

  let parts = ctx.path.split('/'); // Split the path into filename and time
  if ((parts.length < 3) || (isNaN(+parts[2]))) {
    ctx.status = 404;
    ctx.type = 'text/html';
    ctx.body = `<html><body>
    expected path: /<file_name>/<time_in_s>
    </body></html>`;
    return; // Ignore favicon etc..
  }

  try {
    if ((parts.length < 3) || (isNaN(+parts[2]))) return; // Ignore favicon etc..
    let demuxer = await beamcoder.demuxer('file:' + parts[1]); // Probe the file
    await demuxer.seek({ time: +parts[2] }); // Seek to the closest keyframe to time
    let packet = await demuxer.read(); // Find the next video packet (assumes stream 0)
    for ( ; packet.stream_index !== 0 ; packet = await demuxer.read() );
    let dec = beamcoder.decoder({ demuxer, stream_index: 0 }); // Create a decoder
    let decResult = await dec.decode(packet); // Decode the frame
    if (decResult.frames.length === 0) // Frame may be buffered, so flush it out
      decResult = await dec.flush();
    // Filtering could be used to transform the picture here, e.g. scaling
    let enc = beamcoder.encoder({ // Create an encoder for JPEG data
      name : 'mjpeg', // FFmpeg does not have an encoder called 'jpeg'
      width : dec.width,
      height: dec.height,
      pix_fmt: dec.pix_fmt.indexOf('422') >= 0 ? 'yuvj422p' : 'yuvj420p',
      time_base: [1, 1] });
    let jpegResult = await enc.encode(decResult.frames[0]); // Encode the frame
    await enc.flush(); // Tidy the encoder
    ctx.type = 'image/jpeg'; // Set the Content-Type of the data
    ctx.body = jpegResult.packets[0].data; // Return the JPEG image data
  } catch (e) {
    ctx.type = 'application/json';
    ctx.status = 500;
    ctx.body = JSON.stringify(e);
  }
});

app.listen(3000); // Start the server on port 3000
console.log('listening port 3000');

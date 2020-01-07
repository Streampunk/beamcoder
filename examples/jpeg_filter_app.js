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

/* Main example from the README.md with added filtering. Run in a folder of media files.

   Will convert source pixel formats to 8-bit YUV 4:2:2
*/

const beamcoder = require('../index.js'); // Use require('beamcoder') externally
const Koa = require('koa'); // Add koa to package.json dependencies
const app = new Koa();

app.use(async (ctx) => { // Assume HTTP GET with path /<file_name>/<time_in_s>
  let parts = ctx.path.split('/'); // Split the path into filename and time
  if ((parts.length < 3) || (isNaN(+parts[2]))) return; // Ignore favicon etc..
  let dm = await beamcoder.demuxer('file:' + parts[1]); // Probe the file
  await dm.seek({ time: +parts[2] }); // Seek to the closest keyframe to time
  let packet = await dm.read(); // Find the next video packet (assumes stream 0)
  for ( ; packet.stream_index !== 0 ; packet = await dm.read() );
  let dec = beamcoder.decoder({ demuxer: dm, stream_index: 0 }); // Create a decoder
  let decResult = await dec.decode(packet); // Decode the frame
  if (decResult.frames.length === 0) // Frame may be buffered, so flush it out
    decResult = await dec.flush();

  // audio test
  const aindex = 2;
  const audStr = dm.streams[aindex];
  // console.log(audStr);
  let adec = beamcoder.decoder({ demuxer: dm, stream_index: aindex }); // Create a decoder
  // console.log(adec);
  let apkt = await dm.read();
  let afrm = await adec.decode(apkt);
  console.log(afrm.frames);
  const audEnc = beamcoder.encoder({
    name: 'aac',
    sample_fmt: 'fltp',
    sample_rate: 48000,
    channels: 1,
    channel_layout: 'mono', });

  const audFilt = await beamcoder.filterer({ // Create a filterer for audio
    filterType: 'audio',
    inputParams: [{
      sampleRate: audStr.codecpar.sample_rate,
      sampleFormat: adec.sample_fmt,
      channelLayout: audStr.codecpar.channel_layout,
      timeBase: audStr.time_base }],
    outputParams: [{
      sampleRate: 1024,
      sampleFormat: 'fltp',
      channelLayout: 'mono' }],
    filterSpec: 'aresample=1024' });

  const audFiltPkt = await audFilt.filter([{ frames: afrm }]);
  const encPkt = await audEnc.encode(audFiltPkt[0].frames[0]);
  console.log(encPkt);

  let vstr = dm.streams[0]; // Select the video stream (assumes stream 0)
  let filt = await beamcoder.filterer({ // Create a filterer for video
    filterType: 'video',
    inputParams: [{
      width: vstr.codecpar.width,
      height: vstr.codecpar.height,
      pixelFormat: vstr.codecpar.format,
      timeBase: vstr.time_base,
      pixelAspect: vstr.sample_aspect_ratio }],
    outputParams: [{ pixelFormat: 'yuv422p' }],
    filterSpec: 'scale=640:360, colorspace=range=jpeg:all=bt709' });
  let filtResult = await filt.filter([{ frames: decResult }]); // Filter the frame
  let filtFrame = filtResult[0].frames[0];
  let enc = beamcoder.encoder({ // Create an encoder for JPEG data
    name : 'mjpeg', // FFmpeg does not have an encoder called 'jpeg'
    width : filtFrame.width,
    height: filtFrame.height,
    pix_fmt: 'yuvj422p',
    time_base: [1, 1] });
  let jpegResult = await enc.encode(filtFrame); // Encode the filtered frame
  await enc.flush(); // Tidy the encoder
  ctx.type = 'image/jpeg'; // Set the Content-Type of the data
  ctx.body = jpegResult.packets[0].data; // Return the JPEG image data
});

app.listen(3000); // Start the server on port 3000

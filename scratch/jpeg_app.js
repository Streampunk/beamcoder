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
const Koa = require('koa');
const app = module.exports = new Koa();

app.use(async (ctx) => {
  let parts = ctx.path.split('/');
  if ((parts.length < 3) || (isNaN(+parts[2]))) return;
  let dm = await beamcoder.demuxer('file:' + parts[1]);
  await dm.seek({ time: +parts[2] });
  let packet = await dm.read();
  for ( ; packet.stream_index !== 0 ; packet = await dm.read() ) {
    //console.log(count++, packet.size, packet, process.hrtime(start));
  }
  let dec = beamcoder.decoder({ format: dm, stream: 0 });
  let decResult = await dec.decode(packet);
  if (decResult.frames.length === 0) decResult = await dec.flush();
  let enc = beamcoder.encoder({
    name : 'mjpeg',
    width : dec.width,
    height: dec.height,
    pix_fmt: dec.pix_fmt.indexOf('422') >= 0 ? 'yuvj422p' : 'yuvj420p',
    time_base: [1, 1] });
  let jpegData = await enc.encode(decResult.frames[0]);
  await enc.flush();
  ctx.type = 'image/jpeg';
  ctx.body = jpegData.packets[0].data;
});

if (!module.parent) app.listen(3000);

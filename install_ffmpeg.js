/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg.
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

const os = require('os');
const fs = require('fs');
const util = require('util');
const https = require('https');
const unzip = require('unzip');
const [ mkdir, access, rename ] =
  [ fs.mkdir, fs.access, fs.rename ].map(util.promisify);

async function get(ws, url, name) {
  let received = 0;
  let totalLength = 0;
  return new Promise((comp, err) => {
    https.get(url, res => {
      res.pipe(ws);
      if (totalLength == 0) {
        totalLength = +res.headers['content-length'];
      }
      res.on('end', () => {
        process.stdout.write(`Downloaded 100% of '${name}'. Total length ${received} bytes.\n`)
        comp();
      });
      res.on('error', err);
      res.on('data', x => {
        received += x.length;
        process.stdout.write(`Downloaded ${received * 100/ totalLength | 0 }% of '${name}'.\r`);
      });
    }).on('error', err);
  });
}

async function inflate(rs, folder, name) {
  return new Promise((comp, err) => {
    console.log(`Unzipping '${folder}/${name}'.`);
    rs.pipe(unzip.Extract({ path: folder }));
    rs.on('close', () => {
      console.log(`Unzipping of '${folder}/${name}' completed.`);
      comp();
    });
    rs.on('error', err);
  });
}

async function win32() {
  console.log("Installing FFmpeg dependencies for Beam Coder on Windows.");

  await mkdir('ffmpeg').catch(e => {
    if (e.code === 'EEXIST') return;
    else throw e;
  });
  await access('ffmpeg/ffmpeg-4.1-win64-shared', fs.constants.R_OK).catch(async e => {
    let ws_shared = fs.createWriteStream('ffmpeg/ffmpeg-4.1-win64-shared.zip');
    await get(ws_shared,
      'https://ffmpeg.zeranoe.com/builds/win64/shared/ffmpeg-4.1-win64-shared.zip',
      'ffmpeg-4.1-win64-shared.zip');
    let rs_shared = fs.createReadStream('ffmpeg/ffmpeg-4.1-win64-shared.zip');
    await inflate(rs_shared, 'ffmpeg', 'ffmpeg-4.1-win64-shared.zip');
  });
  await access('ffmpeg/ffmpeg-4.1-win64-dev', fs.constants.R_OK).catch(async e => {
    let ws_dev = fs.createWriteStream('ffmpeg/ffmpeg-4.1-win64-dev.zip');
    await get(ws_dev,
      'https://ffmpeg.zeranoe.com/builds/win64/dev/ffmpeg-4.1-win64-dev.zip',
      'ffmpeg-win64-dev.zip');
    let rs_dev = fs.createReadStream('ffmpeg/ffmpeg-4.1-win64-dev.zip');
    console.log(await inflate(rs_dev, 'ffmpeg', 'ffmpeg-4.1-win64-dev.zip'));
  });
}

async function linux() {
  console.log("Checking FFmpeg dependencies for Beam Coder on Linux.");

}

switch (os.platform()) {
  case 'win32':
    if (os.arch() != 'x64') {
      console.error("Only 64-bit platforms are supported.");
    } else {
      win32();
    }
    break;
  case 'linux':
  if (os.arch() != 'x64') {
    console.error("Only 64-bit platforms are supported.");
  } else {
    linux();
  }
    break;
  case 'mac':
  default:
    console.error(`Platfrom ${os.platform()} is not supported.`);
    break;
}

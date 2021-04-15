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
const cp = require('child_process');
const [ mkdir, access, rename, execFile, exec ] = // eslint-disable-line
  [ fs.mkdir, fs.access, fs.rename, cp.execFile, cp.exec ].map(util.promisify);

async function get(ws, url, name) {
  let received = 0;
  let totalLength = 0;
  return new Promise((comp, err) => {
    https.get(url, res => {
      if (res.statusCode === 301 || res.statusCode === 302) {
        err({ name: 'RedirectError', message: res.headers.location });
      } else {
        res.pipe(ws);
        if (totalLength == 0) {
          totalLength = +res.headers['content-length'];
        }
        res.on('end', () => {
          process.stdout.write(`Downloaded 100% of '${name}'. Total length ${received} bytes.\n`);
          comp();
        });
        res.on('error', err);
        res.on('data', x => {
          received += x.length;
          process.stdout.write(`Downloaded ${received * 100/ totalLength | 0 }% of '${name}'.\r`);
        });
      }
    }).on('error', err);
  });
}

async function getHTML(url, name) {
  let received = 0;
  let totalLength = 0;
  return new Promise((resolve, reject) => {
    https.get(url, res => {
      const chunks = [];
      if (totalLength == 0) {
        totalLength = +res.headers['content-length'];
      }
      res.on('end', () => {
        process.stdout.write(`Downloaded 100% of '${name}'. Total length ${received} bytes.\n`);
        resolve(Buffer.concat(chunks));
      });
      res.on('error', reject);
      res.on('data', (chunk) => {
        chunks.push(chunk);
        received += chunk.length;
        process.stdout.write(`Downloaded ${received * 100/ totalLength | 0 }% of '${name}'.\r`);
      });
    }).on('error', reject);
  });
}

async function inflate(rs, folder, name) {
  const unzip = require('unzipper');
  const directory = await unzip.Open.file(`${folder}/${name}.zip`);
  const directoryName = directory.files[0].path;
  return new Promise((comp, err) => {
    console.log(`Unzipping '${folder}/${name}.zip'.`);
    rs.pipe(unzip.Extract({ path: folder }).on('close', () => {
      fs.rename(`./${folder}/${directoryName}`, `./${folder}/${name}`, () => {
        console.log(`Unzipping of '${folder}/${name}.zip' completed.`);
        comp();
      });
    }));
    rs.on('error', err);
  });
}

async function win32() {
  console.log('Checking/Installing FFmpeg dependencies for Beam Coder on Windows.');

  await mkdir('ffmpeg').catch(e => {
    if (e.code === 'EEXIST') return;
    else throw e;
  });
  
  const ffmpegFilename = 'ffmpeg-4.x-win64-shared';
  await access(`ffmpeg/${ffmpegFilename}`, fs.constants.R_OK).catch(async () => {
    const html = await getHTML('https://github.com/BtbN/FFmpeg-Builds/wiki/Latest', 'latest autobuilds');
    const htmlStr = html.toString('utf-8');
    const autoPos = htmlStr.indexOf('<p><a href=');
    const endPos = htmlStr.indexOf('</div>', autoPos);
    const autoStr = htmlStr.substring(autoPos, endPos);
    const sharedEndPos = autoStr.lastIndexOf('">win64-gpl-shared-4.');
    if (sharedEndPos === -1)
      throw new Error('Failed to find latest v4.x autobuild from "https://github.com/BtbN/FFmpeg-Builds/wiki/Latest"');
    const startStr = '<p><a href="';
    const sharedStartPos = autoStr.lastIndexOf(startStr, sharedEndPos) + startStr.length;
    const downloadSource = autoStr.substring(sharedStartPos, sharedEndPos);

    let ws_shared = fs.createWriteStream(`ffmpeg/${ffmpegFilename}.zip`);
    await get(ws_shared, downloadSource, `${ffmpegFilename}.zip`)
      .catch(async (err) => {
        if (err.name === 'RedirectError') {
          const redirectURL = err.message;
          await get(ws_shared, redirectURL, `${ffmpegFilename}.zip`);
        } else console.error(err);
      });

    await exec('npm install unzipper --no-save');
    let rs_shared = fs.createReadStream(`ffmpeg/${ffmpegFilename}.zip`);
    await inflate(rs_shared, 'ffmpeg', `${ffmpegFilename}`);
  });
}

async function linux() {
  console.log('Checking FFmpeg dependencies for Beam Coder on Linux.');
  const { stdout } = await execFile('ldconfig', ['-p']).catch(console.error);
  let result = 0;

  if (stdout.indexOf('libavcodec.so.58') < 0) {
    console.error('libavcodec.so.58 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libavformat.so.58') < 0) {
    console.error('libavformat.so.58 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libavdevice.so.58') < 0) {
    console.error('libavdevice.so.58 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libavfilter.so.7') < 0) {
    console.error('libavfilter.so.7 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libavutil.so.56') < 0) {
    console.error('libavutil.so.56 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libpostproc.so.55') < 0) {
    console.error('libpostproc.so.55 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libswresample.so.3') < 0) {
    console.error('libswresample.so.3 is not installed.');
    result = 1;
  }
  if (stdout.indexOf('libswscale.so.5') < 0) {
    console.error('libswscale.so.5 is not installed.');
    result = 1;
  }

  if (result === 1) {
    console.log(`Try running the following (Ubuntu/Debian):
sudo add-apt-repository ppa:jonathonf/ffmpeg-4
sudo apt-get install libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev`);
    process.exit(1);
  }
  return result;
}

async function darwin() {
  console.log('Checking for FFmpeg dependencies via HomeBrew.');
  let output;
  let returnMessage;
  
  try {
    output = await exec('brew list ffmpeg');
    returnMessage = 'FFmpeg already present via Homebrew.';
  } catch (err) {
    if (err.stderr !== 'Error: No such keg: /usr/local/Cellar/ffmpeg\n') {
      console.error(err);
      console.log('Either Homebrew is not installed or something else is wrong.\nExiting');
      process.exit(1);
    }

    console.log('FFmpeg not installed. Attempting to install via Homebrew.');
    try {
      output = await exec('brew install nasm pkg-config texi2html ffmpeg');
      returnMessage = 'FFmpeg installed via Homebrew.';
    } catch (err) {
      console.log('Failed to install ffmpeg:\n');
      console.error(err);
      process.exit(1);
    }
  }

  console.log(output.stdout);
  console.log(returnMessage);

  return 0;
}

switch (os.platform()) {
case 'win32':
  if (os.arch() != 'x64') {
    console.error('Only 64-bit platforms are supported.');
    process.exit(1);
  } else {
    win32().catch(console.error);
  }
  break;
case 'linux':
  if (os.arch() != 'x64') {
    console.error('Only 64-bit platforms are supported.');
    process.exit(1);
  } else {
    linux();
  }
  break;
case 'darwin':
  if (os.arch() != 'x64') {
    console.error('Only 64-bit platforms are supported.');
    process.exit(1);
  } else {
    darwin();
  }
  break;
default:
  console.error(`Platfrom ${os.platform()} is not supported.`);
  break;
}

/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg.
  Copyright (C) 2019  Streampunk Media Ltd.
  Copyright (C) 2022 Chemouni Uriel.

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

import os from 'os';
import fs from 'fs';
import path from 'path';
import util from 'util';
import child_process, { ChildProcess } from 'child_process';
import { getHTML, getRaw } from './utils';
const { mkdir, access, rename } = fs.promises;
const [execFile, exec] = [child_process.execFile, child_process.exec].map(util.promisify);
import unzip from 'unzipper';

async function inflate(rs: NodeJS.ReadableStream, folder: string, name: string): Promise<void> {
  // const unzip = require('unzipper');
  const directory = await unzip.Open.file(`${folder}/${name}.zip`);
  const dest = path.resolve(`./${folder}/${name}`);
  const directoryName = directory.files[0].path;
  return new Promise((comp, err) => {
    console.log(`Unzipping '${folder}/${name}.zip'.`);
    rs.pipe(unzip.Extract({ path: folder }).on('close', async () => {
      await rename(`./${folder}/${directoryName}`, dest)
      console.log(`Unzipping of '${folder}/${name}.zip' to ${dest} completed.`);
      comp();
    }));
    rs.on('error', err);
  });
}

async function win32(): Promise<void> {
  console.log('Checking/Installing FFmpeg dependencies for Beam Coder on Windows.');
  try {
    await mkdir('ffmpeg');
  } catch (e) {
    if (e.code !== 'EEXIST') {
      throw e;
    }
  };
  const ffmpegFilename = 'ffmpeg-5.x-win64-shared';
  try {
    const file = `ffmpeg/${ffmpegFilename}`;
    await access(file, fs.constants.R_OK)
    console.log(`${path.resolve(file)} Present, Ok`)
  } catch (e) {
    const html = await getHTML('https://github.com/BtbN/FFmpeg-Builds/wiki/Latest', 'latest autobuilds');
    const htmlStr = html.toString('utf-8');
    const m = htmlStr.match(/<p><a href="([^"]+)">win64-gpl-shared-5[0-9.]+<\/a><\/p>/);
    if (!m) {
      throw new Error('Failed to find latest v5.x autobuild from "https://github.com/BtbN/FFmpeg-Builds/wiki/Latest"');
    }
    const downloadSource = m[1];
    const destZip = path.resolve(`ffmpeg/${ffmpegFilename}.zip`);
    console.log(`Downloading ffmpeg zip to ${destZip}`);
    let ws_shared = fs.createWriteStream(destZip);
    try {
      await getRaw(ws_shared, downloadSource, `${ffmpegFilename}.zip`)
    } catch (err) {
      if (err.name === 'RedirectError') {
        const redirectURL = err.message;
        await getRaw(ws_shared, redirectURL, `${ffmpegFilename}.zip`);
      } else
        throw err;
    }
    // await exec('npm install unzipper --no-save');
    let rs_shared = fs.createReadStream(destZip);
    await inflate(rs_shared, 'ffmpeg', `${ffmpegFilename}`);
  };
}

async function linux(): Promise<number> {
  console.log('Checking FFmpeg dependencies for Beam Coder on Linux.');
  const { stdout } = await execFile('ldconfig', ['-p']).catch(console.error);
  let result = 0;
  for (const fn of ['avcodec.so.59', 'avformat.so.59', 'avdevice.so.59', 'avfilter.so.8', 'avutil.so.57', 'postproc.so.56', 'swresample.so.4', 'swscale.so.6']) {
    if (stdout.indexOf(`lib${fn}`) < 0) {
      console.error(`lib${fn} is not installed.`);
      result = 1;
    }
  }
  if (result === 1) {
    console.log(`Try running the following (Ubuntu/Debian):
sudo add-apt-repository ppa:jonathonf/ffmpeg-4
sudo apt-get install libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev`);
    process.exit(1);
  }
  return result;
}

async function darwin(): Promise<0> {
  console.log('Checking for FFmpeg dependencies via HomeBrew.');
  let output: ChildProcess;
  let returnMessage: string;

  try {
    output = await exec('brew list ffmpeg');
    returnMessage = 'FFmpeg already present via Homebrew.';
  } catch (err) {
    if ((err as { stderr: string }).stderr !== 'Error: No such keg: /usr/local/Cellar/ffmpeg\n') {
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
    if (os.arch() != 'x64' && os.arch() != 'arm64') {
      console.error('Only 64-bit platforms are supported.');
      process.exit(1);
    } else {
      linux();
    }
    break;
  case 'darwin':
    if (os.arch() != 'x64' && os.arch() != 'arm64') {
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

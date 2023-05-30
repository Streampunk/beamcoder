/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg
  Copyright (C) 2019 Streampunk Media Ltd.

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

const path = require('path');
const os = require('os');

function getPath(name, addDotExeOnWindows) {
  const binaries = {
    darwin: ['x64', 'arm64'],
    linux: ['x64'],
    win32: ['x64'],
  };

  const platform = process.env.npm_config_platform || os.platform();

  const arch = process.env.npm_config_arch || os.arch();

  if (!binaries[platform] || binaries[platform].indexOf(arch) === -1) {
    return null;
  }

  return path.join(
    __dirname,
    'build',
    // FIXME: toggleable
    'Release',
    platform === 'win32' && addDotExeOnWindows ? name + '.exe' : name
  );
}

module.exports = {
  ffmpegPath: getPath('ffmpeg', true),
  ffprobePath: getPath('ffprobe', true),
  beamcoderPath: getPath('beamcoder.node', false),
};

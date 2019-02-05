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

const beamcoder = require('bindings')('beamcoder');

// Provide useful debug on segfault-related crash
const SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler('crash.log');

const splash = `Aerostat Beam Coder  Copyright (C) 2019  Streampunk Media Ltd
GPL v3.0 or later license. This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions. Conditions and warranty at:
https://github.com/Streampunk/beamcoder/blob/master/LICENSE`;

console.log(splash);

const { Writable } = require('stream');

function createBeamStream(params, governor) {
  const beamStream = new Writable({
    decodeStrings: params.decodeStrings || false,
    highWaterMark: params.highwaterMark || 16384,
    objectMode: false,
    write: async (chunk, encoding, cb) => {
      await governor.write(chunk);
      cb();
    }
  });
  return beamStream;
}

function demuxerStream(params) {
  const governor = new beamcoder.governor({});
  const stream = createBeamStream(params, governor);
  stream.on('close', () => governor.finish());
  stream.on('error', console.error);
  stream.demuxer = () => beamcoder.demuxer(governor);
  return stream;
}

beamcoder.demuxerStream = demuxerStream;

module.exports = beamcoder;

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

const { Writable, Readable } = require('stream');

function createBeamWritableStream(params, governor) {
  const beamStream = new Writable({
    highWaterMark: params.highwaterMark || 16384,
    write: async (chunk, encoding, cb) => {
      await governor.write(chunk);
      cb();
    }
  });
  return beamStream;
}

function demuxerStream(params) {
  const governor = new beamcoder.governor({});
  const stream = createBeamWritableStream(params, governor);
  stream.on('close', () => governor.finish());
  stream.on('finish', () => governor.finish());
  stream.on('error', console.error);
  stream.demuxer = () =>
    // delay initialisation of demuxer until stream has been written to - avoids lock-up
    new Promise(async resolve => setTimeout(() => resolve(beamcoder.demuxer(governor)), 20));
  return stream;
}

function createBeamReadableStream(params, governor) {
  const beamStream = new Readable({
    highWaterMark: params.highwaterMark || 16384,
    read: async size => {
      const chunk = await governor.read(size);
      if (0 === chunk.length)
        beamStream.push(null);
      else
        beamStream.push(chunk);
    }
  });
  return beamStream;
}

function muxerStream(params) {
  const governor = new beamcoder.governor({});
  const stream = createBeamReadableStream(params, governor);
  stream.on('close', () => governor.finish());
  stream.on('error', console.error);
  stream.muxer = options => {
    options.governor = governor;
    return beamcoder.muxer(options);
  };
  return stream;
}

beamcoder.demuxerStream = demuxerStream;
beamcoder.muxerStream = muxerStream;

module.exports = beamcoder;

/*
  Aerostat Beam Engine - Redis-backed highly-scale-able and cloud-fit media beam engine.
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

const splash = `Aerostat Beam Engine  Copyright (C) 2019  Streampunk Media Ltd
GPL v3.0 or later license. This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions. Conditions and warranty at:
https://github.com/Streampunk/aerostat/blob/master/LICENSE`;

console.log(splash);

const { Writable } = require('stream');

function createBeamStream(params) {
  const governor = new beamcoder.governor(params);

  const beamStream = new Writable({
    decodeStrings: params.decodeStrings || false,
    highWaterMark: params.highwaterMark || 16384,
    objectMode: false,
    write: async (chunk, encoding, cb) => {
      await governor.write(chunk);
      cb();
    },
    final: cb => {
      governor.finish();
      cb();
    }
  });
  beamStream.governor = governor;
  return beamStream;
}

function createDemuxer(params) {
  if (typeof params.pipe === 'function') {
    const beamStream = createBeamStream({});
    params.pipe(beamStream);
    return beamcoder.demuxer(beamStream.governor);
  } else
    return beamcoder.demuxer(params);
}

module.exports = {
  beamcoder,
  createDemuxer
};

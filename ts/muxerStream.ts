/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg
  Copyright (C) 2019 Streampunk Media Ltd.
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
import { ReadableMuxerStream } from './types/Beamstreams';
import beamcoder from './beamcoder'
import createBeamReadableStream from './createBeamReadableStream';

export default function muxerStream(params: { highwaterMark: number }): ReadableMuxerStream {
    const governor = new beamcoder.governor({ highWaterMark: 1 });
    const stream = createBeamReadableStream(params, governor) as ReadableMuxerStream;
    stream.on('end', () => governor.finish());
    stream.on('error', console.error);
    stream.muxer = (options) => {
      options = options || {};
      options.governor = governor;
      return beamcoder.muxer(options);
    };
    return stream;
  }
  
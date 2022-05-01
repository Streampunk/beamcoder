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
import type { Governor } from './types/Governor';
import { WritableDemuxerStream } from './types/Beamstreams';

import beamcoder from './beamcoder'
import createBeamWritableStream from './createBeamWritableStream';

export default function demuxerStream(params: { highwaterMark?: number }): WritableDemuxerStream {
    const governor = new beamcoder.governor({});
    const stream = createBeamWritableStream(params, governor) as WritableDemuxerStream;
    stream.on('finish', () => governor.finish());
    stream.on('error', console.error);
    stream.demuxer = (options: { governor?: Governor }) => {
      options.governor = governor;
      // delay initialisation of demuxer until stream has been written to - avoids lock-up
      return new Promise(resolve => setTimeout(async () => resolve(await beamcoder.demuxer(options)), 20));
    };
    return stream;
  }
  

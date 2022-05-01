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
import { Readable } from 'stream';
import type { Governor } from './types/Governor';

export default function createBeamReadableStream(params: { highwaterMark?: number }, governor: Governor): Readable {
    const beamStream = new Readable({
      highWaterMark: params.highwaterMark || 16384,
      read: size => {
        (async () => {
          const chunk = await governor.read(size);
          if (0 === chunk.length)
            beamStream.push(null);
          else
            beamStream.push(chunk);
        })();
      }
    });
    return beamStream;
  }
  
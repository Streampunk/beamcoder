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
import { Transform } from 'stream';
import { Timable } from './types/time'

// SRC = Frame | Frame[] | Packet
// DST = Promise<DecodedFrames>, DecodedFrames
export default function transformStream<SRC extends Timable, DST extends Timable>(
    params: { name: 'encode' | 'dice' | 'decode' | 'filter', highWaterMark: number },
    processFn: (val: SRC) => DST,
    flushFn: () => DST | null | void,
    reject: (err?: Error) => void) {
    return new Transform({
      objectMode: true,
      highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
      transform(val: SRC, encoding, cb) {
        (async () => {
          const start = process.hrtime();
          const reqTime = start[0] * 1e3 + start[1] / 1e6;
          const result = await processFn(val);
          result.timings = val.timings;
          if (result.timings)
            result.timings[params.name] = { reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
          cb(null, result);
        })().catch(cb);
      },
      flush(cb) {
        (async () => {
          const result = flushFn ? await flushFn() : null;
          if (result) result.timings = {};
          cb(null, result);
        })().catch(cb);
      }
    }).on('error', err => reject(err));
  }
  
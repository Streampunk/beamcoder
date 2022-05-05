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
import { Writable } from 'stream';
import type { ffStats } from './calcStats';
import { Timing, Timable } from './types/time';

import calcStats from './calcStats';

const doTimings = false;
const timings = [] as Array<{ [key: string]: Timing }>;

export default function writeStream<T extends Timable, R>(params: { name: string, highWaterMark?: number }, processFn: (val: T) => Promise<R>, finalFn: () => Promise<R>, reject: (err: Error) => void) {
  return new Writable({
    objectMode: true,
    highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
    write(val: T, encoding: BufferEncoding, cb: (error?: Error | null, result?: R) => void) {
      (async () => {
        const start = process.hrtime();
        const reqTime = start[0] * 1e3 + start[1] / 1e6;
        const result = await processFn(val);
        if ('mux' === params.name) {
          const pktTimings = val.timings;
          if (pktTimings) {
            pktTimings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
            if (doTimings)
              timings.push(pktTimings);
          }
        }
        cb(null, result);
      })().catch(cb);
    },
    final(cb: (error?: Error | null, result?: R | null) => void) {
      (async () => {
        const result = finalFn ? await finalFn() : null;
        if (doTimings && ('mux' === params.name)) {
          const elapsedStats = {} as { [key: string]: ffStats };
          Object.keys(timings[0]).forEach(k => elapsedStats[k] = calcStats<typeof k, 'elapsed'>(timings.slice(10, -10), k, 'elapsed'));
          console.log('elapsed:');
          console.table(elapsedStats);

          const absArr = timings.map(t => {
            const absDelays = {} as { [key: string]: { reqDelta: number } };
            const keys = Object.keys(t);
            keys.forEach((k, i) => absDelays[k] = { reqDelta: i > 0 ? t[k].reqTime - t[keys[i - 1]].reqTime : 0 });
            return absDelays;
          });
          const absStats = {} as { [key: string]: ffStats };
          Object.keys(absArr[0]).forEach(k => absStats[k] = calcStats<typeof k, 'reqDelta'>(absArr.slice(10, -10), k, 'reqDelta'));
          console.log('request time delta:');
          console.table(absStats);

          const totalsArr = timings.map(t => {
            const total = (t.mux && t.read) ? t.mux.reqTime - t.read.reqTime + t.mux.elapsed : 0;
            return { total: { total: total } };
          });
          console.log('total time:');
          console.table(calcStats<'total', 'total'>(totalsArr.slice(10, -10), 'total', 'total'));
        }
        cb(null, result);
      })().catch(cb);
    }
  }).on('error', err => reject(err));
}

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
import { Readable } from "stream";
import { Frame } from "./types/Frame";
import { Timable, Timables, Timing, TotalTimed } from "./types/time";

export type BalanceResult = { value: { timings: Timing }, done: boolean, final?: boolean };

type teeBalancerType = Readable[] & { pushFrames: (frames: Timables<Frame>, unusedFlag?: boolean) => Promise<BalanceResult | void> };

export default function teeBalancer(params: { name: 'streamTee', highWaterMark?: number }, numStreams: number): teeBalancerType {
  let resolvePush: null | ((result?: BalanceResult) => void) = null;
  const pending: Array<{ frames: null | Frame, resolve: null | ((result: { value?: Frame, done: boolean }) => void), final: boolean }> = [];
  for (let s = 0; s < numStreams; ++s)
    pending.push({ frames: null, resolve: null, final: false });

  const pullFrame = async (index: number) => {

    return new Promise<{ done: boolean, value?: Frame | null }>(resolve => {
      if (pending[index].frames) {
        resolve({ value: pending[index].frames, done: false });
        Object.assign(pending[index], { frames: null, resolve: null });
      } else if (pending[index].final)
        resolve({ done: true });
      else
        pending[index].resolve = resolve;

      if (resolvePush && pending.every(p => null === p.frames)) {
        resolvePush();
        resolvePush = null;
      }
    });
  };

  const readStreams: teeBalancerType = [] as teeBalancerType;
  for (let s = 0; s < numStreams; ++s)
    readStreams.push(new Readable({
      objectMode: true,
      highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
      read() {
        (async () => {
          const start = process.hrtime();
          const reqTime = start[0] * 1e3 + start[1] / 1e6;
          const result = await pullFrame(s);
          if (result.done)
            this.push(null);
          else {
            result.value.timings[params.name] = { reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
            this.push(result.value);
          }
        })();
      },
    }));

  // {frames: Frame[], name: string}
  readStreams.pushFrames = (frames: Array<any> & TotalTimed & Timable): Promise<BalanceResult | void> => {
    return new Promise<BalanceResult | void>(resolve => {
      pending.forEach((p, index) => {
        if (frames.length)
            p.frames = frames[index].frames;
        else
          p.final = true;
      });

      pending.forEach(p => {
        if (p.resolve) {
          if (p.frames) {
            p.frames.timings = frames.timings;
            p.resolve({ value: p.frames, done: false });
          } else if (p.final)
            p.resolve({ done: true });
        }
        Object.assign(p, { frames: null, resolve: null });
      });
      resolvePush = resolve;
    });
  };

  return readStreams;
}

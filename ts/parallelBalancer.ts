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
import { DecodedFrames, Frame, Stream } from './types';
import { Timable, Timables } from './types/time';

export type localFrame = { pkt?: Frame, ts: number, streamIndex: number, final?: boolean, resolve?: () => void };
type localResult = { done: boolean, value?: { name: string, frames: Timables<Frame> }[] & Timable };

export default class parallelBalancer extends Readable {
  private pending: localFrame[] = [];
  private resolveGet: null | ((result: localResult) => void) = null;

  get tag(): 'a' | 'v' {
    return 'video' === this.streamType ? 'v' : 'a';
  }

  constructor(params: { name: string, highWaterMark: number }, private streamType: 'video' | 'audio', numStreams: number) {
    const pullSet = () => new Promise<localResult>(resolve => this.makeSet(resolve));
    super({
      objectMode: true,
      highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
      async read() {
        const start = process.hrtime();
        const reqTime = start[0] * 1e3 + start[1] / 1e6;
        const result = await pullSet();
        if (result.done)
          this.push(null);
        else {
          result.value.timings = result.value[0].frames[0].timings;
          result.value.timings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
          this.push(result.value);
        }
      },
    })
    // initialise with negative ts and no pkt
    // - there should be no output until each stream has sent its first packet
    for (let s = 0; s < numStreams; ++s)
      this.pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });
  }

  private makeSet(resolve: (result: localResult) => void): void {
    if (!resolve)
      return;
    // console.log('makeSet', pending.map(p => p.ts));
    const nextPends = this.pending.every(pend => pend.pkt) ? this.pending : null;
    const final = this.pending.filter(pend => true === pend.final);
    if (nextPends) {
      nextPends.forEach(pend => pend.resolve());
      resolve({
        value: nextPends.map(pend => {
          return { name: `in${pend.streamIndex}:${this.tag}`, frames: [pend.pkt] };
        }),
        done: false
      });
      this.resolveGet = null;
      this.pending.forEach(pend => Object.assign(pend, { pkt: null, ts: Number.MAX_VALUE }));
    } else if (final.length > 0) {
      final.forEach(f => f.resolve());
      resolve({ done: true });
    } else {
      this.resolveGet = resolve;
    }
  };

  private async pushPkt(pkt: Frame, streamIndex: number, ts: number): Promise<localFrame> {
    return new Promise(resolve => {
      Object.assign(this.pending[streamIndex], { pkt, ts, final: pkt ? false : true, resolve });
      this.makeSet(this.resolveGet);
    })
  }

  public async pushPkts(packets: null | DecodedFrames, stream: Stream, streamIndex: number, final = false): Promise<localFrame> {
    if (packets && packets.frames.length) {
      let lst: localFrame = { ts: -Number.MAX_VALUE, streamIndex: 0 };
      for (const pkt of packets.frames) {
        const ts = pkt.pts * stream.time_base[0] / stream.time_base[1];
        pkt.timings = packets.timings;
        lst = await this.pushPkt(pkt, streamIndex, ts);
      }
      return lst;
    } else if (final) {
      return this.pushPkt(null, streamIndex, Number.MAX_VALUE);
    }
  }
}

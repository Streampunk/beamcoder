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
import { DecodedFrames } from "./types/DecodedFrames";
import { Frame } from './types/Frame';
import { Stream } from "./types/Stream";
import { Timable, Timables } from "./types/time";

export type localFrame = { pkt?: Frame, ts: number, streamIndex: number, final?: boolean, resolve?: () => void };
type localResult = { done: boolean, value?: { name: string, frames: Timables<Frame> }[] & Timable };


type parallelBalancerType = Readable & {
    pushPkts: (packets: DecodedFrames, stream: Stream, streamIndex: number, final?: boolean) => Promise<localFrame>
};

export function parallelBalancer(params: { name: string, highWaterMark: number }, streamType: 'video' | 'audio', numStreams: number): parallelBalancerType {
    let resolveGet: null | ((result: { value?: { name: string, frames: Frame[] }[] & Timable, done: boolean }) => void) = null;
    const tag = 'video' === streamType ? 'v' : 'a';
    const pending: Array<localFrame> = [];
    // initialise with negative ts and no pkt
    // - there should be no output until each stream has sent its first packet
    for (let s = 0; s < numStreams; ++s)
        pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });

    // const makeSet = (resolve: (result: {value?: { name: string, frames: any[] }, done: boolean}) => void) => {
    const makeSet = (resolve: (result: localResult) => void) => {
        if (resolve) {
            // console.log('makeSet', pending.map(p => p.ts));
            const nextPends = pending.every(pend => pend.pkt) ? pending : null;
            const final = pending.filter(pend => true === pend.final);
            if (nextPends) {
                nextPends.forEach(pend => pend.resolve());
                resolve({
                    value: nextPends.map(pend => {
                        return {
                            name: `in${pend.streamIndex}:${tag}`,
                            frames: [pend.pkt]
                        };
                    }),
                    done: false
                });
                resolveGet = null;
                pending.forEach(pend => Object.assign(pend, { pkt: null, ts: Number.MAX_VALUE }));
            } else if (final.length > 0) {
                final.forEach(f => f.resolve());
                resolve({ done: true });
            } else {
                resolveGet = resolve;
            }
        }
    };

    const pushPkt = async (pkt: Frame, streamIndex: number, ts: number): Promise<localFrame> =>
        new Promise(resolve => {
            Object.assign(pending[streamIndex], { pkt, ts, final: pkt ? false : true, resolve });
            makeSet(resolveGet);
        });

    const pullSet = async () => new Promise<localResult>(resolve => makeSet(resolve));

    const readStream: parallelBalancerType = new Readable({
        objectMode: true,
        highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
        read() {
            (async () => {
                const start = process.hrtime();
                const reqTime = start[0] * 1e3 + start[1] / 1e6;
                const result = await pullSet();
                if (result.done)
                    this.push(null);
                else {
                    const value = result.value;
                    value.timings = value[0].frames[0].timings;
                    value.timings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
                    this.push(result.value);
                }
            })();
        },
    }) as parallelBalancerType;

    readStream.pushPkts = async (packets: DecodedFrames, stream: Stream, streamIndex: number, final = false): Promise<localFrame> => {
        if (packets && packets.frames.length) {
            let lst: localFrame = {} as localFrame;
            for (const pkt of packets.frames) {
                const ts = pkt.pts * stream.time_base[0] / stream.time_base[1];
                pkt.timings = packets.timings;
                lst = await pushPkt(pkt, streamIndex, ts);
            }
            return lst;
        } else if (final) {
            return pushPkt(null, streamIndex, Number.MAX_VALUE);
        }
    };

    return readStream;
}

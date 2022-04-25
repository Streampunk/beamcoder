import { Readable } from "stream";
import { DecodedFrames } from "./types/DecodedFrames";
import { Frame } from './types/Frame';
import { Timable } from "./types/Timable";

type parallelBalancerType = Readable & {
    pushPkts: (packets, stream, streamIndex: number, final?: boolean) => any
};

export function parallelBalancer(params: { name: string, highWaterMark: number }, streamType: 'video' | 'audio', numStreams: number): parallelBalancerType {
    let resolveGet = null;
    const tag = 'video' === streamType ? 'v' : 'a';
    const pending = [];
    // initialise with negative ts and no pkt
    // - there should be no output until each stream has sent its first packet
    for (let s = 0; s < numStreams; ++s)
        pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });

    // const makeSet = (resolve: (result: {value?: { name: string, frames: any[] }, done: boolean}) => void) => {
    const makeSet = (resolve: (result: {value?: any, done: boolean}) => void) => {
                if (resolve) {
            // console.log('makeSet', pending.map(p => p.ts));
            const nextPends = pending.every(pend => pend.pkt) ? pending : null;
            const final = pending.filter(pend => true === pend.final);
            if (nextPends) {
                nextPends.forEach(pend => pend.resolve());
                resolve({
                    value: nextPends.map(pend => {
                        return { name: `in${pend.streamIndex}:${tag}`, frames: [pend.pkt] };
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

    const pushPkt = async (pkt: Frame, streamIndex: number, ts: number): Promise<{ pkt: Frame, ts: number, final: boolean, resolve: () => void }> =>
        new Promise(resolve => {
            Object.assign(pending[streamIndex], { pkt, ts, final: pkt ? false : true, resolve });
            makeSet(resolveGet);
        });

    const pullSet = async () => new Promise<{ done: any, value: { timings: any } }>(resolve => makeSet(resolve as any));

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
                    result.value.timings = result.value[0].frames[0].timings;
                    result.value.timings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
                    this.push(result.value);
                }
            })();
        },
    }) as parallelBalancerType;

    readStream.pushPkts = (packets: DecodedFrames, stream: { time_base: [number, number] }, streamIndex: number, final = false): Promise<{ pkt: Frame, ts: number, final: boolean, resolve: () => void }> => {
        if (packets && packets.frames.length) {
            // @ts-ignore
            return packets.frames.reduce(async (promise, pkt: Frame) => {
                await promise;
                const ts = pkt.pts * stream.time_base[0] / stream.time_base[1];
                (pkt as Timable).timings = packets.timings;
                return pushPkt(pkt, streamIndex, ts);
            }, Promise.resolve());
        } else if (final) {
            return pushPkt(null, streamIndex, Number.MAX_VALUE);
        }
    };

    return readStream;
}

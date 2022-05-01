import { Readable } from "stream";
import { Frame } from "./types/Frame";
import { Timable, Timables } from "./types/Timable";
import { Timing, TotalTimeed } from "./types/Timing";

export type BalanceResult = { value: { timings: Timing }, done: boolean, final?: boolean };

type teeBalancerType = Readable[] & { pushFrames: (frames: Timables<Frame>, unusedFlag?: boolean) => Promise<BalanceResult | void> };

export function teeBalancer(params: { name: 'streamTee', highWaterMark?: number }, numStreams: number): teeBalancerType {
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
  readStreams.pushFrames = (frames: Array<any> & TotalTimeed & Timable): Promise<BalanceResult | void> => {
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

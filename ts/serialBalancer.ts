export class serialBalancer {
    pending = [] as { ts: number, streamIndex: number, resolve?: (result: any) => void, pkt?: any }[];
  
    constructor(numStreams: number) {
      // initialise with negative ts and no pkt
      // - there should be no output until each stream has sent its first packet
      for (let s = 0; s < numStreams; ++s)
        this.pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });
    }
  
    adjustTS(pkt: { pts: number, dts: number, duration: number }, srcTB: [number, number], dstTB: [number, number]): void {
      const adj = (srcTB[0] * dstTB[1]) / (srcTB[1] * dstTB[0]);
      pkt.pts = Math.round(pkt.pts * adj);
      pkt.dts = Math.round(pkt.dts * adj);
      pkt.duration > 0 ? Math.round(pkt.duration * adj) : Math.round(adj);
    };
  
  
    pullPkts(pkt: {}, streamIndex: number, ts: number): Promise<void> {
      return new Promise<void>(resolve => {
        Object.assign(this.pending[streamIndex], { pkt, ts, resolve });
        const minTS = this.pending.reduce((acc, pend) => Math.min(acc, pend.ts), Number.MAX_VALUE);
        // console.log(streamIndex, pending.map(p => p.ts), minTS);
        const nextPend = this.pending.find(pend => pend.pkt && (pend.ts === minTS));
        if (nextPend) nextPend.resolve(nextPend.pkt);
        if (!pkt) resolve();
      });
    };
  
    writePkts(packets: {
      packets: Array<{ stream_index: number, pts: number, dts: number, duration: number }>
    } | null,
      srcStream: { time_base: [number, number] },
      dstStream: {
        time_base: [number, number],
        index: number
      },
      writeFn: (r: void) => void, final = false) {
      if (packets && packets.packets.length) {
        return packets.packets.reduce(async (promise, pkt) => {
          await promise;
          pkt.stream_index = dstStream.index;
          this.adjustTS(pkt, srcStream.time_base, dstStream.time_base);
          const pktTS = pkt.pts * dstStream.time_base[0] / dstStream.time_base[1];
          return writeFn(await this.pullPkts(pkt, dstStream.index, pktTS));
        }, Promise.resolve());
      } else if (final)
        return this.pullPkts(null, dstStream.index, Number.MAX_VALUE);
    };
  }
  
  
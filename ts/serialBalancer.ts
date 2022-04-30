import { EncodedPackets } from "./types/Encoder";
import { Packet } from "./types/Packet";

export class serialBalancer {
    pending = [] as { ts: number, streamIndex: number, resolve?: (result: any) => void, pkt: Packet | null }[];
  
    constructor(numStreams: number) {
      // initialise with negative ts and no pkt
      // - there should be no output until each stream has sent its first packet
      for (let streamIndex = 0; streamIndex < numStreams; ++streamIndex)
        this.pending.push({ ts: -Number.MAX_VALUE, streamIndex, pkt: null });
    }
  
    adjustTS(pkt: { pts: number, dts: number, duration: number }, srcTB: [number, number], dstTB: [number, number]): void {
      const adj = (srcTB[0] * dstTB[1]) / (srcTB[1] * dstTB[0]);
      pkt.pts = Math.round(pkt.pts * adj);
      pkt.dts = Math.round(pkt.dts * adj);
      pkt.duration > 0 ? Math.round(pkt.duration * adj) : Math.round(adj);
    };
  
    pullPkts(pkt: null | Packet, streamIndex: number, ts: number): Promise<void | Packet> {
      return new Promise<void | Packet>(resolve => {
        const pending = this.pending[streamIndex];
        pending.pkt = pkt;
        pending.ts = ts;
        pending.resolve = resolve;
        // TODO loop only once
        const minTS = this.pending.reduce((acc, pend) => Math.min(acc, pend.ts), Number.MAX_VALUE);
        // console.log(streamIndex, pending.map(p => p.ts), minTS);
        const nextPend = this.pending.find(pend => pend.pkt && (pend.ts === minTS));
        if (nextPend) nextPend.resolve(nextPend.pkt);
        if (!pkt) resolve();
      });
    };
  
    async writePkts(
      packets: EncodedPackets | null,
      srcStream: { time_base: [number, number] },
      dstStream: {
        time_base: [number, number],
        index: number
      },
      writeFn: (r: void | Packet) => void,
      final = false
    ): Promise<void | Packet> {
      if (packets && packets.packets.length) {
        for (const pkt of packets.packets) {
          pkt.stream_index = dstStream.index;
          this.adjustTS(pkt, srcStream.time_base, dstStream.time_base);
          const pktTS = pkt.pts * dstStream.time_base[0] / dstStream.time_base[1];
          const packet = await this.pullPkts(pkt, dstStream.index, pktTS)
          writeFn(packet);
        }
      } else if (final)
        return this.pullPkts(null, dstStream.index, Number.MAX_VALUE);
    };
  }
  
  
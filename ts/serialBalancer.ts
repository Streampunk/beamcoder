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
      writeFn: (r: Packet) => void,
      final = false
    ): Promise<void | Packet> {
      if (packets && packets.packets.length) {
        for (const pkt of packets.packets) {
          pkt.stream_index = dstStream.index;
          this.adjustTS(pkt, srcStream.time_base, dstStream.time_base);
          const pktTS = pkt.pts * dstStream.time_base[0] / dstStream.time_base[1];
          const packet = await this.pullPkts(pkt, dstStream.index, pktTS)
          writeFn(packet as Packet);
        }
      } else if (final)
        return this.pullPkts(null, dstStream.index, Number.MAX_VALUE);
    };
  }
  
  
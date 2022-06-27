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
import { Demuxer } from './types/Demuxer';
import { Packet } from './types/Packet';

export default function readStream(params: { highWaterMark?: number }, demuxer: Demuxer, ms: { end: number }, index: number): Readable {
    const time_base = demuxer.streams[index].time_base;
    const end_pts = ms ? ms.end * time_base[1] / time_base[0] : Number.MAX_SAFE_INTEGER;
    async function getPacket(): Promise<Packet | null> {
      let packet: Packet = {} as Packet;
      do { packet = await demuxer.read(); }
      while (packet && packet.stream_index !== index);
      return packet;
    }
  
    return new Readable({
      objectMode: true,
      highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
      read() {
        (async () => {
          const start = process.hrtime();
          const reqTime = start[0] * 1e3 + start[1] / 1e6;
          const packet = await getPacket();
          if (packet && (packet.pts < end_pts)) {
            packet.timings = {};
            packet.timings.read = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
            this.push(packet);
          } else
            this.push(null);
        })();
      }
    });
  }
  
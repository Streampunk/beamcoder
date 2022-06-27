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
import teeBalancer from './teeBalancer';
import { BalanceResult } from './teeBalancer';
import { default as parallelBalancer, localFrame } from './parallelBalancer';
import serialBalancer from './serialBalancer';
import { Stream } from './types/Stream';
import { DecodedFrames } from './types/Decoder';
import { Frame } from './types/Frame';
import { Packet } from './types/Packet';
import { TotalTimed } from './types/time';
import { BeamstreamSource, BeamstreamStream } from './types/Beamstreams';
import { Filterer, FiltererResult } from './types/Filter';
import { Timable, Timables } from './types/time'
import { EncodedPackets } from './types/Encoder';

import frameDicer from './frameDicer';
import transformStream from './transformStream';
import writeStream from './writeStream';
import { Muxer } from './types/Muxer';

export default function runStreams(
    streamType: 'video' | 'audio',
    sources: Array<BeamstreamSource>,
    filterer: Filterer,
    streams: Array<BeamstreamStream>,
    mux: Muxer,
    muxBalancer: serialBalancer): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      if (!sources.length)
        return resolve();
  
      const timeBaseStream: Stream = sources[0].format.streams[sources[0].streamIndex];
      const filterBalancer = new parallelBalancer({ name: 'filterBalance', highWaterMark: 1 }, streamType, sources.length);
  
      sources.forEach((src: BeamstreamSource, srcIndex: number) => {
        const decStream = transformStream<Packet, Timable & Promise<DecodedFrames>>(
          { name: 'decode', highWaterMark: 1 },
          (pkts: Packet) => src.decoder.decode(pkts),
          () => src.decoder.flush(), reject);
  
        const filterSource = writeStream<DecodedFrames, localFrame>(
          { name: 'filterSource', highWaterMark: 1 },
          (pkts: DecodedFrames) => filterBalancer.pushPkts(pkts, src.format.streams[src.streamIndex], srcIndex, false),
          () => filterBalancer.pushPkts(null, src.format.streams[src.streamIndex], srcIndex, true),
          reject
        );
  
        src.stream.pipe(decStream).pipe(filterSource);
      });
  
      const streamTee = teeBalancer({ name: 'streamTee', highWaterMark: 1 }, streams.length);
  
      const filtStream = transformStream<Timables<DecodedFrames>, Timable & Promise<Array<FiltererResult> & TotalTimed>>({ name: 'filter', highWaterMark: 1 }, (frms: Timables<DecodedFrames>) => {
        if (filterer.cb) filterer.cb(frms[0].frames[0].pts);
        // @ts-ignore
        return filterer.filter(frms);
      }, () => { }, reject);
  
      const streamSource = writeStream<Timables<Frame>, BalanceResult | void>(
        { name: 'streamSource', highWaterMark: 1 },
        frms => streamTee.pushFrames(frms),
        () => streamTee.pushFrames([], true),
        reject
      );
  
      filterBalancer.pipe(filtStream).pipe(streamSource);
      streams.forEach((str: BeamstreamStream, i: number) => {
        const dicer = new frameDicer(str.encoder, 'audio' === streamType);
  
        const diceStream = transformStream<Timables<Frame>, Timables<Frame>>(
          { name: 'dice', highWaterMark: 1 },
          (frms) => dicer.dice(frms),
          () => dicer.dice([], true),
          reject
        );
  
        const encStream = transformStream<Timables<Frame>, Timable & Promise<EncodedPackets>>(
          { name: 'encode', highWaterMark: 1 },
          (frms) => str.encoder.encode(frms),
          () => str.encoder.flush(),
          reject
        );
  
        const muxStream = writeStream<EncodedPackets, void | Packet>(
          { name: 'mux', highWaterMark: 1 },
          (pkts: EncodedPackets) => muxBalancer.writePkts(pkts, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts)),
          () => muxBalancer.writePkts(null, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts), true),
          reject
        );
  
        muxStream.on('finish', resolve);
  
        streamTee[i].pipe(diceStream).pipe(encStream).pipe(muxStream);
      });
    });
  }
  
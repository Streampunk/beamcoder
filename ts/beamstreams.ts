/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg
  Copyright (C) 2019 Streampunk Media Ltd.

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
import bindings from 'bindings';
import { Writable, Readable, Transform } from 'stream';
import type { ffStats } from './types';
import { teeBalancer } from './teeBalancer';
import { parallelBalancer } from './parallelBalancer';
import { serialBalancer } from './serialBalancer';
import { BeamcoderType } from './types/BeamcoderType';
import { CodecPar } from './types/CodecPar';
import { Demuxer } from './types/Demuxer';
import { Muxer } from './types/Muxer';
import { Stream } from './types/Stream';
import { DecodedFrames } from './types/DecodedFrames';
import type { Governor } from './types/Governor';
import { Frame } from './types/Frame';
import { Packet } from './types/Packet';
import { Timing } from './types/Timing';
import { BeamstreamChannel, BeamstreamParams, BeamstreamSource, BeamstreamStream, WritableDemuxerStream } from './types/Beamstreams';
import { Filterer, FilterGraph, FilterLink } from './types/Filter';
import { CodecContextBaseMin } from './types/CodecContext';
import { EncodedPackets, Encoder } from './types/Encoder';
import { Timable, Timables } from './types/Timable'

const beamcoder = bindings('beamcoder') as BeamcoderType;

const doTimings = false;
const timings = [] as Array<{ [key: string]: Timing }>;

class frameDicer {

  private addFrame: (srcFrm: Frame) => any[];
  private getLast: () => any[];
  private doDice: boolean;
  // CodecPar
  constructor(encoder: CodecContextBaseMin, private isAudio: boolean) {
    let sampleBytes = 4; // Assume floating point 4 byte samples for now...
    const numChannels = encoder.channels;
    const dstNumSamples = encoder.frame_size;
    let dstFrmBytes = dstNumSamples * sampleBytes;
    this.doDice = false === beamcoder.encoders()[encoder.name].capabilities.VARIABLE_FRAME_SIZE;

    let lastFrm: Frame = null as Frame;
    let lastBuf: Buffer[] = [];
    const nullBuf: Buffer[] = [];
    for (let b = 0; b < numChannels; ++b)
      nullBuf.push(Buffer.alloc(0));

    this.addFrame = (srcFrm: Frame): Frame[] => {
      let result: Frame[] = [];
      let dstFrm: Frame;
      let curStart = 0;
      if (!lastFrm) {
        lastFrm = beamcoder.frame(srcFrm.toJSON()) as Frame;
        lastBuf = nullBuf;
        dstFrmBytes = dstNumSamples * sampleBytes;
      }

      if (lastBuf[0].length > 0)
        dstFrm = beamcoder.frame(lastFrm.toJSON());
      else
        dstFrm = beamcoder.frame(srcFrm.toJSON());
      dstFrm.nb_samples = dstNumSamples;
      dstFrm.pkt_duration = dstNumSamples;

      while (curStart + dstFrmBytes - lastBuf[0].length <= srcFrm.nb_samples * sampleBytes) {
        const resFrm = beamcoder.frame((dstFrm as any as Stream).toJSON());
        resFrm.data = lastBuf.map((d, i) =>
          Buffer.concat([
            d, srcFrm.data[i].slice(curStart, curStart + dstFrmBytes - d.length)],
            dstFrmBytes));
        result.push(resFrm);

        dstFrm.pts += dstNumSamples;
        dstFrm.pkt_dts += dstNumSamples;
        curStart += dstFrmBytes - lastBuf[0].length;
        lastFrm.pts = 0;
        lastFrm.pkt_dts = 0;
        lastBuf = nullBuf;
      }

      lastFrm.pts = dstFrm.pts;
      lastFrm.pkt_dts = dstFrm.pkt_dts;
      lastBuf = srcFrm.data.map(d => d.slice(curStart, srcFrm.nb_samples * sampleBytes));

      return result;
    };

    this.getLast = (): any[] => {
      let result = [];
      if (lastBuf[0].length > 0) {
        const resFrm = beamcoder.frame(lastFrm.toJSON());
        resFrm.data = lastBuf.map(d => d.slice(0));
        resFrm.nb_samples = lastBuf[0].length / sampleBytes;
        resFrm.pkt_duration = resFrm.nb_samples;
        lastFrm.pts = 0;
        lastBuf = nullBuf;
        result.push(resFrm);
      }
      return result;
    };
  }
  public dice(frames: Frame[], flush = false): Frame[] {
    if (this.isAudio && this.doDice) {
      let result: Frame[] = frames.reduce((muxFrms: Frame[], frm: Frame) => {
        this.addFrame(frm).forEach(f => muxFrms.push(f));
        return muxFrms;
      }, [] as Frame[]);

      if (flush)
        this.getLast().forEach(f => result.push(f));
      return result;
    }

    return frames;
  };
}
// T = Frame | Frame[] | Packet
// D = Promise<DecodedFrames>, DecodedFrames
function transformStream<T extends Timable, D extends Timable>(
  params: { name: 'encode' | 'dice' | 'decode' | 'filter', highWaterMark: number },
  processFn: (val: T) => D,
  flushFn: () => D | null | void,
  reject: (err?: Error) => void) {
  return new Transform({
    objectMode: true,
    highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
    transform(val: T, encoding, cb) {
      (async () => {
        const start = process.hrtime();
        const reqTime = start[0] * 1e3 + start[1] / 1e6;
        const result = await processFn(val);
        result.timings = val.timings;
        if (result.timings)
          result.timings[params.name] = { reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
        cb(null, result);
      })().catch(cb);
    },
    flush(cb) {
      (async () => {
        const result = flushFn ? await flushFn() : null;
        if (result) (result as any).timings = {};
        cb(null, result);
      })().catch(cb);
    }
  }).on('error', err => reject(err));
}

const calcStats = (arr: Array<any>, elem: string, prop: string): ffStats => {
  const mean: number = arr.reduce((acc, cur) => cur[elem] ? acc + cur[elem][prop] : acc, 0) / arr.length;
  const stdDev: number = Math.pow(arr.reduce((acc, cur) => cur[elem] ? acc + Math.pow(cur[elem][prop] - mean, 2) : acc, 0) / arr.length, 0.5);
  const max: number = arr.reduce((acc, cur) => cur[elem] ? Math.max(cur[elem][prop], acc) : acc, 0);
  const min: number = arr.reduce((acc, cur) => cur[elem] ? Math.min(cur[elem][prop], acc) : acc, Number.MAX_VALUE);
  return { mean, stdDev, max, min };
};

function writeStream(params: { name: string, highWaterMark?: number }, processFn: (val: { stream_index: number, pts: number, dts: number, duration: number, timings: any }) => Promise<any>, finalFn: () => Promise<any>, reject: (err: Error) => void) {
  return new Writable({
    objectMode: true,
    highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
    write(val: { stream_index: number, pts: number, dts: number, duration: number, timings: any }, encoding: BufferEncoding, cb: (error?: Error | null, result?: any) => void) {
      (async () => {
        const start = process.hrtime();
        const reqTime = start[0] * 1e3 + start[1] / 1e6;
        const result = await processFn(val);
        if ('mux' === params.name) {
          const pktTimings = val.timings;
          pktTimings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
          if (doTimings)
            timings.push(pktTimings);
        }
        cb(null, result);
      })().catch(cb);
    },
    final(cb: (error?: Error | null, result?: any) => void) {
      (async () => {
        const result = finalFn ? await finalFn() : null;
        if (doTimings && ('mux' === params.name)) {
          const elapsedStats = {} as { [key: string]: ffStats };
          Object.keys(timings[0]).forEach(k => elapsedStats[k] = calcStats(timings.slice(10, -10), k, 'elapsed'));
          console.log('elapsed:');
          console.table(elapsedStats);

          const absArr = timings.map(t => {
            const absDelays = {} as { [key: string]: { reqDelta: number } };
            const keys = Object.keys(t);
            keys.forEach((k, i) => absDelays[k] = { reqDelta: i > 0 ? t[k].reqTime - t[keys[i - 1]].reqTime : 0 });
            return absDelays;
          });
          const absStats = {} as { [key: string]: ffStats };
          Object.keys(absArr[0]).forEach(k => absStats[k] = calcStats(absArr.slice(10, -10), k, 'reqDelta'));
          console.log('request time delta:');
          console.table(absStats);

          const totalsArr = timings.map(t => {
            const total = (t.mux && t.read) ? t.mux.reqTime - t.read.reqTime + t.mux.elapsed : 0;
            return { total: { total: total } };
          });
          console.log('total time:');
          console.table(calcStats(totalsArr.slice(10, -10), 'total', 'total'));
        }
        cb(null, result);
      })().catch(cb);
    }
  }).on('error', err => reject(err));
}

// interface Packet {
//   stream_index: number;
//   pts: number;
//   timings: {
//     read?: Timing;
//   };
// }


function readStream(params: { highWaterMark?: number }, demuxer: Demuxer, ms: { end: number }, index: number) {
  const time_base = demuxer.streams[index].time_base;
  const end_pts = ms ? ms.end * time_base[1] / time_base[0] : Number.MAX_SAFE_INTEGER;
  async function getPacket(): Promise<Packet | null> {
    let packet: Packet | null = null;
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

function createBeamWritableStream(params: { highwaterMark?: number }, governor: Governor): Writable {
  const beamStream = new Writable({
    highWaterMark: params.highwaterMark || 16384,
    write: (chunk, encoding, cb) => {
      (async () => {
        await governor.write(chunk);
        cb();
      })();
    }
  });
  return beamStream;
}
type demuxerStreamType = Writable & { demuxer: (options: { governor: Governor }) => Promise<any> };

export function demuxerStream(params: { highwaterMark?: number }): WritableDemuxerStream {
  const governor = new beamcoder.governor({});
  const stream: demuxerStreamType = createBeamWritableStream(params, governor) as demuxerStreamType;
  stream.on('finish', () => governor.finish());
  stream.on('error', console.error);
  stream.demuxer = (options: { governor: Governor }) => {
    options.governor = governor;
    // delay initialisation of demuxer until stream has been written to - avoids lock-up
    return new Promise(resolve => setTimeout(async () => resolve(await beamcoder.demuxer(options)), 20));
  };
  return stream as WritableDemuxerStream;
}


function createBeamReadableStream(params: { highwaterMark?: number }, governor: Governor) {
  const beamStream = new Readable({
    highWaterMark: params.highwaterMark || 16384,
    read: size => {
      (async () => {
        const chunk = await governor.read(size);
        if (0 === chunk.length)
          beamStream.push(null);
        else
          beamStream.push(chunk);
      })();
    }
  });
  return beamStream;
}

type muxerStreamType = Readable & { muxer: (options: { governor: Governor }) => any };

export function muxerStream(params: { highwaterMark: number }): muxerStreamType {
  const governor = new beamcoder.governor({ highWaterMark: 1 });
  const stream: muxerStreamType = createBeamReadableStream(params, governor) as muxerStreamType;
  stream.on('end', () => governor.finish());
  stream.on('error', console.error);
  stream.muxer = (options) => {
    options.governor = governor;
    return beamcoder.muxer(options);
  };
  return stream;
}
// params: { video?: Array<{ sources: any[] }>, audio?: Array<{ sources: any[] }> }
export async function makeSources(params: BeamstreamParams): Promise<void> {
  if (!params.video) params.video = [];
  if (!params.audio) params.audio = [];
  params.video.forEach(p => p.sources.forEach((src: BeamstreamSource) => {
    if (src.input_stream) {
      const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
      src.input_stream.pipe(demuxerStream);
      src.format = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
    } else
      src.format = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options as any }); // FIXME
  }));
  params.audio.forEach(p => p.sources.forEach((src: BeamstreamSource) => {
    if (src.input_stream) {
      const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
      src.input_stream.pipe(demuxerStream);
      src.format = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
    } else
      src.format = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options as any }); // FIXME
  }));

  await (params.video.reduce)(async (promise: Promise<any>, p: BeamstreamChannel) => { // FIXME
    await promise;
    return p.sources.reduce(async (promise: Promise<any>, src: BeamstreamSource) => {
      await promise;
      src.format = await src.format;
      if (src.ms && !src.input_stream)
        src.format.seek({ time: src.ms.start });
      return src.format;
    }, Promise.resolve());
  }, Promise.resolve());

  await (params.audio.reduce as any)(async (promise: Promise<any>, p: BeamstreamChannel) => {
    await promise;
    return p.sources.reduce(async (promise: Promise<any>, src: BeamstreamSource) => {
      await promise;
      src.format = await src.format;
      if (src.ms && !src.input_stream)
        src.format.seek({ time: src.ms.start });
      return src.format;
    }, Promise.resolve());
  }, Promise.resolve());

  params.video.forEach((p: BeamstreamChannel) => p.sources.forEach(src =>
    src.stream = readStream({ highWaterMark: 1 }, src.format as Demuxer, src.ms, src.streamIndex)));
  params.audio.forEach((p: BeamstreamChannel) => p.sources.forEach(src =>
    src.stream = readStream({ highWaterMark: 1 }, src.format as Demuxer, src.ms, src.streamIndex)));
}

function runStreams(
  streamType: 'video' | 'audio',
  // sources: Array<{ decoder: { decode: (pkts: any) => any, flush: () => any }, format: { streams: Array<{}> }, streamIndex: any, stream: any }>, 
  sources: Array<BeamstreamSource>,
  filterer: { cb?: (result: any) => void, filter: (stream: Array<any>) => any, graph: FilterGraph }, // Filterer?
  streams: Array<BeamstreamStream>,
  mux: { writeFrame: (pkts: any) => void },
  muxBalancer: serialBalancer) {
  // serialBalancer // { writePkts: (packets: {timings: any; }, srcStream: {}, dstStream: {}, writeFn: {}, final?: boolean) => any }
  return new Promise<void>((resolve, reject) => {
    if (!sources.length)
      return resolve();

    const timeBaseStream: Stream = (sources[0].format as Demuxer).streams[sources[0].streamIndex];
    const filterBalancer = parallelBalancer({ name: 'filterBalance', highWaterMark: 1 }, streamType, sources.length);

    sources.forEach((src: BeamstreamSource, srcIndex: number) => {
      const decStream = transformStream<Packet, Promise<DecodedFrames> & Timable>(
        { name: 'decode', highWaterMark: 1 },
        (pkts: Packet) => src.decoder.decode(pkts),
        () => src.decoder.flush(), reject);
      const filterSource = writeStream({ name: 'filterSource', highWaterMark: 1 },
        pkts => filterBalancer.pushPkts(pkts, (src.format as Demuxer).streams[src.streamIndex], srcIndex),
        () => filterBalancer.pushPkts(null, (src.format as Demuxer).streams[src.streamIndex], srcIndex, true), reject);

      src.stream.pipe(decStream).pipe(filterSource);
    });

    const streamTee = teeBalancer({ name: 'streamTee', highWaterMark: 1 }, streams.length);
    const filtStream = transformStream({ name: 'filter', highWaterMark: 1 }, (frms: Timables<DecodedFrames>) => {
      // @ts-ignore
      if (filterer.cb) filterer.cb(frms[0].frames[0].pts);
      return filterer.filter(frms);
    }, () => { }, reject);
    const streamSource = writeStream({ name: 'streamSource', highWaterMark: 1 },
      // @ts-ignore
      frms => streamTee.pushFrames(frms), () => streamTee.pushFrames([], true), reject);

    filterBalancer.pipe(filtStream).pipe(streamSource);
    streams.forEach((str: BeamstreamStream, i: number) => {
      const dicer = new frameDicer(str.encoder, 'audio' === streamType);

      const diceStream = transformStream<Timables<Frame>, Timables<Frame>>({ name: 'dice', highWaterMark: 1 },
        (frms) => dicer.dice(frms), () => dicer.dice([], true), reject);

      const encStream = transformStream<Timables<Frame>, any>({ name: 'encode', highWaterMark: 1 },
        (frms) => str.encoder.encode(frms), () => str.encoder.flush(), reject);

      const muxStream = writeStream({ name: 'mux', highWaterMark: 1 },
        (pkts: any) => muxBalancer.writePkts(pkts, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts)),
        () => muxBalancer.writePkts(null, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts), true), reject);
      muxStream.on('finish', resolve);

      streamTee[i].pipe(diceStream).pipe(encStream).pipe(muxStream);
    });
  });
}

export async function makeStreams(params: BeamstreamParams): Promise<{ run(): Promise<void> }> {
  params.video.forEach((p: BeamstreamChannel) => {
    p.sources.forEach((src: BeamstreamSource) =>
      src.decoder = beamcoder.decoder({ demuxer: src.format as Demuxer, stream_index: src.streamIndex }));
  });
  params.audio.forEach((p: BeamstreamChannel) => {
    p.sources.forEach((src: BeamstreamSource) =>
      src.decoder = beamcoder.decoder({ demuxer: src.format as Demuxer, stream_index: src.streamIndex }));
    //  {demuxer: Demuxer | Promise<Demuxer>, stream_index: number}
  });

  params.video.forEach((p: BeamstreamChannel) => {
    p.filter = beamcoder.filterer({
      // FiltererVideoOptions
      filterType: 'video',
      inputParams: p.sources.map((src, i) => {
        const stream = (src.format as Demuxer).streams[src.streamIndex];
        return {
          name: `in${i}:v`,
          width: stream.codecpar.width,
          height: stream.codecpar.height,
          pixelFormat: stream.codecpar.format,
          timeBase: stream.time_base,
          pixelAspect: stream.sample_aspect_ratio
        };
      }),
      outputParams: p.streams.map((str: BeamstreamStream, i: number) => ({ name: `out${i}:v`, pixelFormat: str.codecpar.format })),
      filterSpec: p.filterSpec
    });
  });
  const vidFilts = await Promise.all(params.video.map(p => p.filter));
  params.video.forEach((p: BeamstreamChannel, i: number) => p.filter = vidFilts[i]);
  // params.video.forEach(p => console.log(p.filter.graph.dump()));

  params.audio.forEach((p: BeamstreamChannel) => {
    p.filter = beamcoder.filterer({
      filterType: 'audio',
      inputParams: p.sources.map((src: BeamstreamSource, i: number) => {
        const stream = (src.format as Demuxer).streams[src.streamIndex];
        return {
          name: `in${i}:a`,
          sampleRate: src.decoder.sample_rate,
          sampleFormat: src.decoder.sample_fmt,
          channelLayout: src.decoder.channel_layout,
          timeBase: stream.time_base
        };
      }),
      outputParams: p.streams.map((str: BeamstreamStream, i: number) => {
        return {
          name: `out${i}:a`,
          sampleRate: str.codecpar.sample_rate,
          sampleFormat: str.codecpar.format,
          channelLayout: str.codecpar.channel_layout
        };
      }),
      filterSpec: p.filterSpec
    });
  });
  const audFilts = await Promise.all(params.audio.map((p: BeamstreamChannel) => p.filter));
  params.audio.forEach((p: BeamstreamChannel, i: number) => p.filter = audFilts[i]);
  // params.audio.forEach(p => console.log(p.filter.graph.dump()));

  let mux: Muxer;
  if (params.out.output_stream) {
    let muxerStream = beamcoder.muxerStream({ highwaterMark: 1024 });
    muxerStream.pipe(params.out.output_stream);
    mux = muxerStream.muxer({ format_name: params.out.formatName });
  } else
    mux = beamcoder.muxer({ format_name: params.out.formatName });

  params.video.forEach((p: BeamstreamChannel) => {
    p.streams.forEach((str: BeamstreamStream, i: number) => {
      const encParams = (p.filter as Filterer).graph.filters.find(f => f.name === `out${i}:v`).inputs[0];
      str.encoder = beamcoder.encoder({
        name: str.name,
        width: encParams.w,
        height: encParams.h,
        pix_fmt: encParams.format,
        sample_aspect_ratio: encParams.sample_aspect_ratio,
        time_base: encParams.time_base as [number, number],
        // framerate: [encParams.time_base[1], encParams.time_base[0]],
        // bit_rate: 2000000,
        // gop_size: 10,
        // max_b_frames: 1,
        // priv_data: { preset: 'slow' }
        priv_data: { crf: 23 }
      }); // ... more required ...
    });
  });

  params.audio.forEach((p: BeamstreamChannel) => {
    p.streams.forEach((str: BeamstreamStream, i: number) => {
      const encParams: FilterLink = (p.filter as Filterer).graph.filters.find(f => f.name === `out${i}:a`).inputs[0];
      str.encoder = beamcoder.encoder({
        name: str.name,
        sample_fmt: encParams.format,
        sample_rate: encParams.sample_rate,
        channel_layout: encParams.channel_layout,
        flags: { GLOBAL_HEADER: mux.oformat.flags.GLOBALHEADER }
      });
      str.codecpar.frame_size = str.encoder.frame_size;
    });
  });

  params.video.forEach((p: BeamstreamChannel) => {
    p.streams.forEach((str: BeamstreamStream) => {
      str.stream = mux.newStream({
        name: str.name,
        time_base: str.time_base,
        interleaved: true
      }); // Set to false for manual interleaving, true for automatic
      Object.assign(str.stream.codecpar, str.codecpar);
    });
  });

  params.audio.forEach((p: BeamstreamChannel) => {
    p.streams.forEach((str: BeamstreamStream) => {
      str.stream = mux.newStream({
        name: str.name,
        time_base: str.time_base,
        interleaved: true
      }); // Set to false for manual interleaving, true for automatic
      Object.assign(str.stream.codecpar, str.codecpar);
    });
  });

  return {
    run: async () => {
      await mux.openIO({
        url: params.out.url ? params.out.url : '',
        flags: params.out.flags ? params.out.flags : {}
      });
      await mux.writeHeader({ options: params.out.options ? params.out.options : {} });

      const muxBalancer = new serialBalancer(mux.streams.length);
      const muxStreamPromises: Promise<any>[] = [];
      params.video.forEach(p => muxStreamPromises.push(runStreams('video', p.sources, p.filter as Filterer, p.streams, mux, muxBalancer)));
      params.audio.forEach(p => muxStreamPromises.push(runStreams('audio', p.sources, p.filter as Filterer, p.streams, mux, muxBalancer)));
      await Promise.all(muxStreamPromises);

      await mux.writeTrailer();
    }
  };
}

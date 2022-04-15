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
import { Frame, Stream, Muxer, Codec, CodecPar } from '.'; // CodecContext, 
const beamcoder = bindings('beamcoder');
import { Writable, Readable, Transform } from 'stream';

const doTimings = false;
const timings = [];


type FrameJSONable = Frame & { toJSON(): string };

class frameDicer {

  private addFrame: (srcFrm: FrameJSONable) => any[];
  private getLast: () => any[];
  private doDice: boolean;

  constructor (encoder: CodecPar, private isAudio: boolean) {
  let sampleBytes = 4; // Assume floating point 4 byte samples for now...
  const numChannels = encoder.channels;
  const dstNumSamples = encoder.frame_size;
  let dstFrmBytes = dstNumSamples * sampleBytes;
  this.doDice = false === beamcoder.encoders()[encoder.name].capabilities.VARIABLE_FRAME_SIZE;

  let lastFrm: FrameJSONable = null as any as FrameJSONable;
  let lastBuf: Buffer[] = [];
  const nullBuf: Buffer[] = [];
  for (let b = 0; b < numChannels; ++b)
    nullBuf.push(Buffer.alloc(0));

  this.addFrame = (srcFrm: FrameJSONable): any[] => {
    let result = [];
    let dstFrm: Frame;
    let curStart = 0;
    if (!lastFrm) {
      lastFrm = beamcoder.frame(srcFrm.toJSON());
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
      (lastFrm as Frame).pts = 0;
      (lastFrm as Frame).pkt_dts = 0;
      lastBuf = nullBuf;
    }

    (lastFrm as Frame).pts = dstFrm.pts;
    (lastFrm as Frame).pkt_dts = dstFrm.pkt_dts;
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
  public dice(frames: FrameJSONable[], flush = false): any {
    if (this.isAudio && this.doDice) {
      let result = frames.reduce((muxFrms, frm) => {
        this.addFrame(frm).forEach(f => muxFrms.push(f));
        return muxFrms;
      }, []);
  
      if (flush)
        this.getLast().forEach(f => result.push(f));
      return result;
    }
  
    return frames;
  };
}








class serialBalancer {
  pending = [];

  constructor(numStreams: any) {
  // initialise with negative ts and no pkt
  // - there should be no output until each stream has sent its first packet
  for (let s = 0; s < numStreams; ++s)
    this.pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });
  }

  adjustTS(pkt, srcTB, dstTB) {
    const adj = (srcTB[0] * dstTB[1]) / (srcTB[1] * dstTB[0]);
    pkt.pts = Math.round(pkt.pts * adj);
    pkt.dts = Math.round(pkt.dts * adj);
    pkt.duration > 0 ? Math.round(pkt.duration * adj) : Math.round(adj);
  };
    

  pullPkts(pkt, streamIndex, ts): Promise<void> {
    return new Promise<void>(resolve => {
      Object.assign(this.pending[streamIndex], { pkt: pkt, ts: ts, resolve: resolve });
      const minTS = this.pending.reduce((acc, pend) => Math.min(acc, pend.ts), Number.MAX_VALUE);
      // console.log(streamIndex, pending.map(p => p.ts), minTS);
      const nextPend = this.pending.find(pend => pend.pkt && (pend.ts === minTS));
      if (nextPend) nextPend.resolve(nextPend.pkt);
      if (!pkt) resolve();
    });
  };

  writePkts(packets, srcStream, dstStream, writeFn, final = false) {
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







function parallelBalancer(params: {name: string, highWaterMark: number}, streamType, numStreams) {
  let resolveGet = null;
  const tag = 'video' === streamType ? 'v' : 'a';
  const pending = [];
  // initialise with negative ts and no pkt
  // - there should be no output until each stream has sent its first packet
  for (let s = 0; s < numStreams; ++s)
    pending.push({ ts: -Number.MAX_VALUE, streamIndex: s });

  const makeSet = resolve => {
    if (resolve) {
      // console.log('makeSet', pending.map(p => p.ts));
      const nextPends = pending.every(pend => pend.pkt) ? pending : null;
      const final = pending.filter(pend => true === pend.final);
      if (nextPends) {
        nextPends.forEach(pend => pend.resolve());
        resolve({
          value: nextPends.map(pend => {
            return { name: `in${pend.streamIndex}:${tag}`, frames: [ pend.pkt ] }; }), 
          done: false });
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

  const pushPkt = async (pkt, streamIndex, ts) =>
    new Promise(resolve => {
      Object.assign(pending[streamIndex], { pkt: pkt, ts: ts, final: pkt ? false : true, resolve: resolve });
      makeSet(resolveGet);
    });

  const pullSet = async () => new Promise(resolve => makeSet(resolve));

  const readStream = new Readable({
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
  });

  readStream.pushPkts = (packets, stream, streamIndex, final = false) => {
    if (packets && packets.frames.length) {
      return packets.frames.reduce(async (promise, pkt) => {
        await promise;
        const ts = pkt.pts * stream.time_base[0] / stream.time_base[1];
        pkt.timings = packets.timings;
        return pushPkt(pkt, streamIndex, ts);
      }, Promise.resolve());
    } else if (final) {
      return pushPkt(null, streamIndex, Number.MAX_VALUE);
    }
  };

  return readStream;
}

function teeBalancer(params, numStreams) {
  let resolvePush = null;
  const pending = [];
  for (let s = 0; s < numStreams; ++s)
    pending.push({ frames: null, resolve: null, final: false });
  
  const pullFrame = async index => {
    return new Promise(resolve => {
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

  const readStreams = [];
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
            result.value.timings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
            this.push(result.value);
          }
        })();
      },
    }));

  readStreams.pushFrames = frames => {
    return new Promise(resolve => {
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

function transformStream(params: { name: 'decode'| 'filter', highWaterMark : number }, processFn, flushFn, reject) {
  return new Transform({
    objectMode: true,
    highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
    transform(val, encoding, cb) {
      (async () => {
        const start = process.hrtime();
        const reqTime = start[0] * 1e3 + start[1] / 1e6;
        const result = await processFn(val);
        result.timings = val.timings;
        if (result.timings)
          result.timings[params.name] = { reqTime: reqTime, elapsed: process.hrtime(start)[1] / 1000000 };
        cb(null, result);
      })().catch(cb);
    },
    flush(cb) {
      (async () => {
        const result = flushFn ? await flushFn() : null;
        if (result) result.timings = {};
        cb(null, result);
      })().catch(cb);
    }
  }).on('error', err => reject(err));
}

const calcStats = (arr, elem, prop) => {
  const mean = arr.reduce((acc, cur) => cur[elem] ? acc + cur[elem][prop] : acc, 0) / arr.length;
  const stdDev = Math.pow(arr.reduce((acc, cur) => cur[elem] ? acc + Math.pow(cur[elem][prop] - mean, 2) : acc, 0) / arr.length, 0.5);
  const max = arr.reduce((acc, cur) => cur[elem] ? Math.max(cur[elem][prop], acc) : acc, 0);
  const min = arr.reduce((acc, cur) => cur[elem] ? Math.min(cur[elem][prop], acc) : acc, Number.MAX_VALUE);
  return { mean: mean, stdDev: stdDev, max: max, min: min };
};

function writeStream(params, processFn, finalFn, reject) {
  return new Writable({
    objectMode: true,
    highWaterMark: params.highWaterMark ? params.highWaterMark || 4 : 4,
    write(val, encoding, cb) {
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
    final(cb) {
      (async () => {
        const result = finalFn ? await finalFn() : null;
        if (doTimings && ('mux' === params.name)) {
          const elapsedStats = {};
          Object.keys(timings[0]).forEach(k => elapsedStats[k] = calcStats(timings.slice(10, -10), k, 'elapsed'));
          console.log('elapsed:');
          console.table(elapsedStats);

          const absArr = timings.map(t => {
            const absDelays = {};
            const keys = Object.keys(t);
            keys.forEach((k, i) => absDelays[k] = { reqDelta: i > 0 ? t[k].reqTime - t[keys[i-1]].reqTime : 0 });
            return absDelays;
          });
          const absStats = {};
          Object.keys(absArr[0]).forEach(k => absStats[k] = calcStats(absArr.slice(10, -10), k, 'reqDelta'));
          console.log('request time delta:');
          console.table(absStats);

          const totalsArr = timings.map(t => { 
            const total = (t.mux && t.read) ? t.mux.reqTime - t.read.reqTime + t.mux.elapsed : 0;
            return { total: { total: total }};
          });
          console.log('total time:');
          console.table(calcStats(totalsArr.slice(10, -10), 'total', 'total'));
        }
        cb(null, result);
      })().catch(cb);
    }
  }).on('error', err => reject(err));
}

function readStream(params, demuxer, ms, index) {
  const time_base = demuxer.streams[index].time_base;
  const end_pts = ms ? ms.end * time_base[1] / time_base[0] : Number.MAX_SAFE_INTEGER;
  async function getPacket() {
    let packet = null;
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

function createBeamWritableStream(params, governor) {
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

export function demuxerStream(params) {
  const governor = new beamcoder.governor({});
  const stream = createBeamWritableStream(params, governor);
  stream.on('finish', () => governor.finish());
  stream.on('error', console.error);
  stream.demuxer = options => {
    options.governor = governor;
    // delay initialisation of demuxer until stream has been written to - avoids lock-up
    return new Promise(resolve => setTimeout(async () => resolve(await beamcoder.demuxer(options)), 20));
  };
  return stream;
}

function createBeamReadableStream(params, governor) {
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

export function muxerStream(params) {
  const governor = new beamcoder.governor({ highWaterMark: 1 });
  const stream = createBeamReadableStream(params, governor);
  stream.on('end', () => governor.finish());
  stream.on('error', console.error);
  stream.muxer = options => {
    options.governor = governor;
    return beamcoder.muxer(options);
  };
  return stream;
}

export async function makeSources(params) {
  if (!params.video) params.video = [];
  if (!params.audio) params.audio = [];

  params.video.forEach(p => p.sources.forEach(src => {
    if (src.input_stream) {
      const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
      src.input_stream.pipe(demuxerStream);
      src.format = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
    } else
      src.format = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options });
  }));
  params.audio.forEach(p => p.sources.forEach(src => {
    if (src.input_stream) {
      const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
      src.input_stream.pipe(demuxerStream);
      src.format = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
    } else
      src.format = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options });
  }));

  await params.video.reduce(async (promise, p) => {
    await promise;
    return p.sources.reduce(async (promise, src) => {
      await promise;
      src.format = await src.format;
      if (src.ms && !src.input_stream)
        src.format.seek({ time: src.ms.start });
      return src.format;
    }, Promise.resolve());
  }, Promise.resolve());
  await params.audio.reduce(async (promise, p) => {
    await promise;
    return p.sources.reduce(async (promise, src) => {
      await promise;
      src.format = await src.format;
      if (src.ms && !src.input_stream)
        src.format.seek({ time: src.ms.start });
      return src.format;
    }, Promise.resolve());
  }, Promise.resolve());

  params.video.forEach(p => p.sources.forEach(src => 
    src.stream = readStream({ highWaterMark : 1 }, src.format, src.ms, src.streamIndex)));
  params.audio.forEach(p => p.sources.forEach(src => 
    src.stream = readStream({ highWaterMark : 1 }, src.format, src.ms, src.streamIndex)));
}

function runStreams(streamType, sources, filterer, streams, mux, muxBalancer) {
  return new Promise<void>((resolve, reject) => {
    if (!sources.length)
      return resolve();

    const timeBaseStream = sources[0].format.streams[sources[0].streamIndex];
    const filterBalancer = parallelBalancer({ name: 'filterBalance', highWaterMark : 1 }, streamType, sources.length);

    sources.forEach((src, srcIndex) => {
      const decStream = transformStream({ name: 'decode', highWaterMark : 1 },
        pkts => src.decoder.decode(pkts), () => src.decoder.flush(), reject);
      const filterSource = writeStream({ name: 'filterSource', highWaterMark : 1 },
        pkts => filterBalancer.pushPkts(pkts, src.format.streams[src.streamIndex], srcIndex),
        () => filterBalancer.pushPkts(null, src.format.streams[src.streamIndex], srcIndex, true), reject);

      src.stream.pipe(decStream).pipe(filterSource);
    });

    const streamTee = teeBalancer({ name: 'streamTee', highWaterMark : 1 }, streams.length);
    const filtStream = transformStream({ name: 'filter', highWaterMark : 1 }, frms => {
      if (filterer.cb) filterer.cb(frms[0].frames[0].pts);
      return filterer.filter(frms);
    }, () => {}, reject);
    const streamSource = writeStream({ name: 'streamSource', highWaterMark : 1 },
      frms => streamTee.pushFrames(frms), () => streamTee.pushFrames([], true), reject);

    filterBalancer.pipe(filtStream).pipe(streamSource);

    streams.forEach((str: Codec, i) => {
      const dicer = new frameDicer(str.encoder, 'audio' === streamType);
      const diceStream = transformStream({ name: 'dice', highWaterMark : 1 },
        frms => dicer.dice(frms), () => dicer.dice([], true), reject);
      const encStream = transformStream({ name: 'encode', highWaterMark : 1 },
        frms => str.encoder.encode(frms), () => str.encoder.flush(), reject);
      const muxStream = writeStream({ name: 'mux', highWaterMark : 1 },
        pkts => muxBalancer.writePkts(pkts, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts)),
        () => muxBalancer.writePkts(null, timeBaseStream, str.stream, pkts => mux.writeFrame(pkts), true), reject);
      muxStream.on('finish', resolve);

      streamTee[i].pipe(diceStream).pipe(encStream).pipe(muxStream);
    });
  });
}

export async function makeStreams(params) {
  params.video.forEach(p => {
    p.sources.forEach(src =>
      src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
  });
  params.audio.forEach(p => {
    p.sources.forEach(src =>
      src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
  });

  params.video.forEach(p => {
    p.filter = beamcoder.filterer({
      filterType: 'video',
      inputParams: p.sources.map((src, i) => {
        const stream = src.format.streams[src.streamIndex];
        return {
          name: `in${i}:v`,
          width: stream.codecpar.width,
          height: stream.codecpar.height,
          pixelFormat: stream.codecpar.format,
          timeBase: stream.time_base,
          pixelAspect: stream.sample_aspect_ratio };
      }),
      outputParams: p.streams.map((str, i) => { return { name: `out${i}:v`, pixelFormat: str.codecpar.format }; }),
      filterSpec: p.filterSpec });
  });
  const vidFilts = await Promise.all(params.video.map(p => p.filter));
  params.video.forEach((p, i) => p.filter = vidFilts[i]);
  // params.video.forEach(p => console.log(p.filter.graph.dump()));

  params.audio.forEach(p => {
    p.filter = beamcoder.filterer({
      filterType: 'audio',
      inputParams: p.sources.map((src, i) => {
        const stream = src.format.streams[src.streamIndex];
        return {
          name: `in${i}:a`,
          sampleRate: src.decoder.sample_rate,
          sampleFormat: src.decoder.sample_fmt,
          channelLayout: src.decoder.channel_layout,
          timeBase: stream.time_base };
      }),
      outputParams: p.streams.map((str, i) => { 
        return { 
          name: `out${i}:a`,
          sampleRate: str.codecpar.sample_rate,
          sampleFormat: str.codecpar.format,
          channelLayout: str.codecpar.channel_layout }; }),
      filterSpec: p.filterSpec });
  });
  const audFilts = await Promise.all(params.audio.map(p => p.filter));
  params.audio.forEach((p, i) => p.filter = audFilts[i]);
  // params.audio.forEach(p => console.log(p.filter.graph.dump()));

  let mux: Muxer;
  if (params.out.output_stream) {
    let muxerStream = beamcoder.muxerStream({ highwaterMark: 1024 });
    muxerStream.pipe(params.out.output_stream);
    mux = muxerStream.muxer({ format_name: params.out.formatName });
  } else
    mux = beamcoder.muxer({ format_name: params.out.formatName });

  params.video.forEach(p => {
    p.streams.forEach((str, i) => {
      const encParams = p.filter.graph.filters.find(f => f.name === `out${i}:v`).inputs[0];
      str.encoder = beamcoder.encoder({
        name: str.name,
        width: encParams.w,
        height: encParams.h,
        pix_fmt: encParams.format,
        sample_aspect_ratio: encParams.sample_aspect_ratio,
        time_base: encParams.time_base,
        // framerate: [encParams.time_base[1], encParams.time_base[0]],
        // bit_rate: 2000000,
        // gop_size: 10,
        // max_b_frames: 1,
        // priv_data: { preset: 'slow' }
        priv_data: { crf: 23 } }); // ... more required ...
    });
  });

  params.audio.forEach(p => {
    p.streams.forEach((str, i) => {
      const encParams = p.filter.graph.filters.find(f => f.name === `out${i}:a`).inputs[0];
      str.encoder = beamcoder.encoder({
        name: str.name,
        sample_fmt: encParams.format,
        sample_rate: encParams.sample_rate,
        channel_layout: encParams.channel_layout,
        flags: { GLOBAL_HEADER: mux.oformat.flags.GLOBALHEADER } });
      
      str.codecpar.frame_size = str.encoder.frame_size;
    });
  });

  params.video.forEach(p => {
    p.streams.forEach(str => {
      str.stream = mux.newStream({ 
        name: str.name,
        time_base: str.time_base,
        interleaved: true }); // Set to false for manual interleaving, true for automatic
      Object.assign(str.stream.codecpar, str.codecpar);
    });
  });

  params.audio.forEach(p => {
    p.streams.forEach(str => {
      str.stream = mux.newStream({
        name: str.name,
        time_base: str.time_base,
        interleaved: true }); // Set to false for manual interleaving, true for automatic
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
      const muxStreamPromises = [];
      params.video.forEach(p => muxStreamPromises.push(runStreams('video', p.sources, p.filter, p.streams, mux, muxBalancer)));
      params.audio.forEach(p => muxStreamPromises.push(runStreams('audio', p.sources, p.filter, p.streams, mux, muxBalancer)));
      await Promise.all(muxStreamPromises);

      await mux.writeTrailer();
    }
  };
}

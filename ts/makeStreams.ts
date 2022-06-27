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

import beamcoder from './beamcoder'
import { BeamstreamChannel, BeamstreamParams, BeamstreamSource, BeamstreamStream, Filterer, FilterLink, Muxer } from './types';
import serialBalancer from './serialBalancer';
import runStreams from './runStreams';
import MuxerStream from './MuxerStream';

/**
 * Initialise the output streams for the beamstream process.
 * Note - the params object is updated by the function.
 * @returns Promise which resolves to an object with a run function that starts the processing
 */
export default async function makeStreams(params: BeamstreamParams): Promise<{ run(): Promise<void> }> {
  if (!params.video) params.video = [];
  if (!params.audio) params.audio = [];

  params.video.forEach((channel: BeamstreamChannel) => {
    channel.sources.forEach((src: BeamstreamSource) =>
      src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
  });
  params.audio.forEach((channel: BeamstreamChannel) => {
    channel.sources.forEach((src: BeamstreamSource) =>
      src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
  });

  const promises: Promise<Filterer>[] = [];

  // VIDEO
  //////////
  params.video.forEach((channel: BeamstreamChannel) => {
    const inputParams = channel.sources.map((src: BeamstreamSource, i: number) => {
      const { codecpar, time_base, sample_aspect_ratio } = src.format.streams[src.streamIndex];
      return {
        name: `in${i}:v`,
        width: codecpar.width,
        height: codecpar.height,
        pixelFormat: codecpar.format,
        timeBase: time_base,
        pixelAspect: sample_aspect_ratio
      };
    });
    const outputParams = channel.streams.map((str: BeamstreamStream, i: number) => ({ name: `out${i}:v`, pixelFormat: str.codecpar.format }));
    const { filterSpec } = channel;
    const prms = beamcoder.filterer({ filterType: 'video', inputParams, outputParams, filterSpec })
    promises.push(prms.then(filter => channel.filter = filter));
  });

  // AUDIO
  //////////
  params.audio.forEach((channel: BeamstreamChannel) => {
    const inputParams = channel.sources.map((src: BeamstreamSource, i: number) => {
      const { sample_rate, sample_fmt, channel_layout } = src.decoder;
      return {
        name: `in${i}:a`,
        sampleRate: sample_rate,
        sampleFormat: sample_fmt,
        channelLayout: channel_layout,
        timeBase: src.format.streams[src.streamIndex].time_base
      };
    });
    const outputParams = channel.streams.map((str: BeamstreamStream, i: number) => {
      const { sample_rate, format, channel_layout } = str.codecpar;
      return {
        name: `out${i}:a`,
        sampleRate: sample_rate,
        sampleFormat: format,
        channelLayout: channel_layout
      };
    });
    const { filterSpec } = channel;
    const prms = beamcoder.filterer({ filterType: 'audio', inputParams, outputParams, filterSpec })
    promises.push(prms.then(filter => channel.filter = filter));
  });
  await Promise.all(promises);

  /**
   * all channel filter are no filled
   */

  // params.video.forEach(p => console.log(p.filter.graph.dump()));
  // params.audio.forEach(p => console.log(p.filter.graph.dump()));

  let mux: Muxer;
  if (params.out.output_stream) {
    let muxerStream = new MuxerStream({ highwaterMark: 1024 });
    muxerStream.pipe(params.out.output_stream);
    mux = muxerStream.muxer({ format_name: params.out.formatName });
  } else {
    mux = beamcoder.muxer({ format_name: params.out.formatName });
  }
  params.video.forEach((channel: BeamstreamChannel) => {
    channel.streams.forEach((str: BeamstreamStream, i: number) => {
      const encParams = (channel.filter as Filterer).graph.filters.find(f => f.name === `out${i}:v`).inputs[0];
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

  // VIDEO
  //////////
  params.video.forEach((channel: BeamstreamChannel) => {
    channel.streams.forEach((str: BeamstreamStream) => {
      str.stream = mux.newStream({
        name: str.name,
        time_base: str.time_base,
        interleaved: true
      }); // Set to false for manual interleaving, true for automatic
      Object.assign(str.stream.codecpar, str.codecpar);
    });
  });

  // AUDIO
  //////////
  params.audio.forEach((channel: BeamstreamChannel) => {
    channel.streams.forEach((str: BeamstreamStream, i: number) => {
      const encParams: FilterLink = (channel.filter as Filterer).graph.filters.find(f => f.name === `out${i}:a`).inputs[0];
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

  params.audio.forEach((channel: BeamstreamChannel) => {
    channel.streams.forEach((str: BeamstreamStream) => {
      str.stream = mux.newStream({
        name: str.name,
        time_base: str.time_base,
        interleaved: true
      }); // Set to false for manual interleaving, true for automatic
      Object.assign(str.stream.codecpar, str.codecpar);
    });
  });
  // create runner
  const run = async () => {
    await mux.openIO({
      url: params.out.url ? params.out.url : '',
      flags: params.out.flags ? params.out.flags : {}
    });
    await mux.writeHeader({ options: params.out.options ? params.out.options : {} });

    const muxBalancer = new serialBalancer(mux.streams.length);
    const muxStreamPromises: Promise<void>[] = [];
    params.video.forEach(ch => muxStreamPromises.push(runStreams('video', ch.sources, ch.filter as Filterer, ch.streams, mux, muxBalancer)));
    params.audio.forEach(ch => muxStreamPromises.push(runStreams('audio', ch.sources, ch.filter as Filterer, ch.streams, mux, muxBalancer)));
    await Promise.all(muxStreamPromises);

    await mux.writeTrailer();
  }
  return { run };
}

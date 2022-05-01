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
import { serialBalancer } from './serialBalancer';
import { Muxer } from './types/Muxer';
import { BeamstreamChannel, BeamstreamParams, BeamstreamSource, BeamstreamStream, ReadableMuxerStream, WritableDemuxerStream } from './types/Beamstreams';
import { Filterer, FilterLink } from './types/Filter';
import beamcoder from './beamcoder'
import runStreams from './runStreams';

export default async function makeStreams(params: BeamstreamParams): Promise<{ run(): Promise<void> }> {
    params.video.forEach((channel: BeamstreamChannel) => {
      channel.sources.forEach((src: BeamstreamSource) =>
        src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
    });
    params.audio.forEach((channel: BeamstreamChannel) => {
      channel.sources.forEach((src: BeamstreamSource) =>
        src.decoder = beamcoder.decoder({ demuxer: src.format, stream_index: src.streamIndex }));
    });
  
    const promises: Promise<Filterer>[] = [];
  
    params.video.forEach((channel: BeamstreamChannel) => {
      const inputParams = channel.sources.map((src, i) => {
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
  
      const p = beamcoder.filterer({
        filterType: 'video',
        inputParams,
        outputParams: channel.streams.map((str: BeamstreamStream, i: number) => ({ name: `out${i}:v`, pixelFormat: str.codecpar.format })),
        filterSpec: channel.filterSpec
      }).then(filter => channel.filter = filter);
      promises.push(p);
    });
  
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
      const p = beamcoder.filterer({ filterType: 'audio', inputParams, outputParams, filterSpec: channel.filterSpec })
      .then(filter => channel.filter = filter);
      promises.push(p);
    });
    await Promise.all(promises);
  
    // params.video.forEach(p => console.log(p.filter.graph.dump()));
    // params.audio.forEach(p => console.log(p.filter.graph.dump()));
  
    let mux: Muxer;
    if (params.out.output_stream) {
      let muxerStream = beamcoder.muxerStream({ highwaterMark: 1024 });
      muxerStream.pipe(params.out.output_stream);
      mux = muxerStream.muxer({ format_name: params.out.formatName });
    } else {
      mux = beamcoder.muxer({ format_name: params.out.formatName });
    }
    params.video.forEach((channel: BeamstreamChannel) => {
      channel.streams.forEach((str: BeamstreamStream, i: number) => {
        const encParams = channel.filter.graph.filters.find(f => f.name === `out${i}:v`).inputs[0];
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
  
    params.audio.forEach((channel: BeamstreamChannel) => {
      channel.streams.forEach((str: BeamstreamStream, i: number) => {
        const encParams: FilterLink = channel.filter.graph.filters.find(f => f.name === `out${i}:a`).inputs[0];
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
  
    return {
      run: async () => {
        await mux.openIO({
          url: params.out.url ? params.out.url : '',
          flags: params.out.flags ? params.out.flags : {}
        });
        await mux.writeHeader({ options: params.out.options ? params.out.options : {} });
  
        const muxBalancer = new serialBalancer(mux.streams.length);
        const muxStreamPromises: Promise<void>[] = [];
        params.video.forEach(p => muxStreamPromises.push(runStreams('video', p.sources, p.filter, p.streams, mux, muxBalancer)));
        params.audio.forEach(p => muxStreamPromises.push(runStreams('audio', p.sources, p.filter, p.streams, mux, muxBalancer)));
        await Promise.all(muxStreamPromises);
  
        await mux.writeTrailer();
      }
    };
  }
  
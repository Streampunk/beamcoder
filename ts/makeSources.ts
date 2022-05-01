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
import { BeamstreamChannel, BeamstreamParams, BeamstreamSource } from './types/Beamstreams';
import beamcoder from './beamcoder'
import { readStream } from './readStream';

export default async function makeSources(params: BeamstreamParams): Promise<void> {
    if (!params.video) params.video = [];
    if (!params.audio) params.audio = [];
    params.video.forEach(p => p.sources.forEach((src: BeamstreamSource) => {
      if (src.input_stream) {
        const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
        src.input_stream.pipe(demuxerStream);
        src.formatP = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
      } else
        src.formatP = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options });
    }));
    params.audio.forEach(p => p.sources.forEach((src: BeamstreamSource) => {
      if (src.input_stream) {
        const demuxerStream = beamcoder.demuxerStream({ highwaterMark: 1024 });
        src.input_stream.pipe(demuxerStream);
        src.formatP = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
      } else
        src.formatP = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options });
    }));
  
    for (const video of params.video) {
      for (const src of video.sources) {
        src.format = await src.formatP;
        if (src.ms && !src.input_stream)
          src.format.seek({ time: src.ms.start });
        await src.formatP;
      }
    }
  
    for (const audio of params.audio) {
      for (const src of audio.sources) {
        src.format = await src.formatP;
        if (src.ms && !src.input_stream)
          src.format.seek({ time: src.ms.start });
        await src.formatP;
      }
    }
  
    params.video.forEach((p: BeamstreamChannel) => p.sources.forEach(src =>
      src.stream = readStream({ highWaterMark: 1 }, src.format, src.ms, src.streamIndex)));
    params.audio.forEach((p: BeamstreamChannel) => p.sources.forEach(src =>
      src.stream = readStream({ highWaterMark: 1 }, src.format, src.ms, src.streamIndex)));
  }
  
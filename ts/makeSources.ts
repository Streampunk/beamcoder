
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
import { BeamstreamParams, BeamstreamSource, Demuxer } from './types';
import readStream from './readStream';
import DemuxerStream from './DemuxerStream';

/**
 * Initialise the sources for the beamstream process.
 * Note - the params object is updated by the function.
 */
export default async function makeSources(params: BeamstreamParams): Promise<void> {
  if (!params.video) params.video = [];
  if (!params.audio) params.audio = [];
  // collect All source stream from video and audio channels
  const sources: Array<BeamstreamSource> = [];
  for (const channel of [...params.video, ...params.audio]){
    channel.sources.forEach(src => sources.push(src));
  }
  // demult all channels
  const promises = sources.map((src: BeamstreamSource) => {
    let p: Promise<Demuxer>;
    if (src.input_stream) {
      const demuxerStream = new DemuxerStream({ highwaterMark: 1024 });
      src.input_stream.pipe(demuxerStream);
      p = demuxerStream.demuxer({ iformat: src.iformat, options: src.options });
    } else {
      p = beamcoder.demuxer({ url: src.url, iformat: src.iformat, options: src.options });
    }
    p = p.then((fmt) => {
      src.format = fmt;
      if (src.ms && !src.input_stream)
        src.format.seek({ time: src.ms.start });
      return fmt;
    })
    return p;
  });
  await Promise.all(promises);
  sources.forEach((src) => src.stream = readStream({ highWaterMark: 1 }, src.format, src.ms, src.streamIndex));
}

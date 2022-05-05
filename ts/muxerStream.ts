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
import { Readable } from 'stream';
import { governor, Muxer, MuxerCreateOptions } from './types';

/**
 * Create a ReadableMuxerStream to allow streaming from a Muxer
 * 
 * A [Node.js Readable stream](https://nodejs.org/docs/latest-v12.x/api/stream.html#stream_readable_streams)
 * allowing data to be streamed from the muxer to a file or other stream destination such as a network connection
 * 
 * @param options.highwaterMark The maximum number of bytes to store in the internal buffer before ceasing to read from the underlying resource.
 * @returns A ReadableMuxerStream that can be streamed from.
 */
export default class MuxerStream extends Readable {
  private governor = new beamcoder.governor({ highWaterMark: 1 });
  constructor(params: { highwaterMark: number }) {
    super({
      highWaterMark: params.highwaterMark || 16384,
      read: size => {
        (async () => {
          const chunk = await this.governor.read(size);
          if (0 === chunk.length)
            this.push(null);
          else
            this.push(chunk);
        })
      }
    })
    this.on('end', () => this.governor.finish());
    this.on('error', console.error);
  }


  /**
   * Create a demuxer for this source
   * @param options a DemuxerCreateOptions object
   * @returns a promise that resolves to a Demuxer when it has determined sufficient
   * format details by consuming data from the source. The promise will wait indefinitely 
   * until sufficient source data has been read.
   */
  public muxer(options?: MuxerCreateOptions & { governor?: governor }): Muxer {
    options = options || {};
    options.governor = this.governor;
    return beamcoder.muxer(options);
  };
}

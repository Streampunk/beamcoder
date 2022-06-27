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
import { governor as Governor } from './types/governor';
import beamcoder from './beamcoder'
import { Writable } from 'stream';
import { Demuxer, InputFormat } from './types';

/**
 * Create a WritableDemuxerStream to allow streaming to a Demuxer
 * @param options.highwaterMark Buffer level when `stream.write()` starts returng false.
 * @returns A WritableDemuxerStream that can be streamed to.
 */
export default class DemuxerStream extends Writable {
  private governor = new beamcoder.governor({});
  constructor(params: { highwaterMark?: number }) {
    super({
      highWaterMark: params.highwaterMark || 16384,
      write: (chunk, encoding, cb) => {
        (async () => {
          await this.governor.write(chunk);
          cb();
        })();
      }
    })
    this.on('finish', () => this.governor.finish());
    this.on('error', console.error);
  }
  
  /**
   * Create a demuxer for this source
   * @param options a DemuxerCreateOptions object
   * @returns a promise that resolves to a Demuxer when it has determined sufficient
   * format details by consuming data from the source. The promise will wait indefinitely 
   * until sufficient source data has been read.
   */
  public demuxer(options?: { iformat?: InputFormat, options?: { [key: string]: any }, governor?: Governor }): Promise<Demuxer>{
    options.governor = this.governor;
    // delay initialisation of demuxer until stream has been written to - avoids lock-up
    return new Promise(resolve => setTimeout(async () => resolve(await beamcoder.demuxer(options)), 20));
  };

}
  

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
import { CodecContext } from "./types/CodecContext";
import { Frame } from "./types/Frame";
import beamcoder from './beamcoder'

export default class frameDicer {
  private doDice: boolean;
  private lastBuf: Buffer[] = [];
  private lastFrm: Frame = null as Frame;
  private sampleBytes = 4; // Assume floating point 4 byte samples for now...
  private nullBuf: Buffer[] = [];
  private dstFrmBytes: number;//  = dstNumSamples * this.sampleBytes;
  private dstNumSamples: number;// = encoder.frame_size;

  constructor(encoder: CodecContext, private isAudio: boolean) {
    const numChannels = encoder.channels;
    this.dstNumSamples = encoder.frame_size;
    this.dstFrmBytes = this.dstNumSamples * this.sampleBytes;
    this.doDice = false === beamcoder.encoders()[encoder.name].capabilities.VARIABLE_FRAME_SIZE;

    for (let b = 0; b < numChannels; ++b)
      this.nullBuf.push(Buffer.alloc(0));
  }

  private addFrame(srcFrm: Frame): Frame[] {
    let result: Frame[] = [];
    let dstFrm: Frame;
    let curStart = 0;
    if (!this.lastFrm) {
      this.lastFrm = beamcoder.frame(srcFrm.toJSON());
      this.lastBuf = this.nullBuf;
      this.dstFrmBytes = this.dstNumSamples * this.sampleBytes;
    }

    if (this.lastBuf[0].length > 0)
      dstFrm = beamcoder.frame(this.lastFrm.toJSON());
    else
      dstFrm = beamcoder.frame(srcFrm.toJSON());
    dstFrm.nb_samples = this.dstNumSamples;
    dstFrm.pkt_duration = this.dstNumSamples;

    while (curStart + this.dstFrmBytes - this.lastBuf[0].length <= srcFrm.nb_samples * this.sampleBytes) {
      const resFrm = beamcoder.frame(dstFrm.toJSON());
      resFrm.data = this.lastBuf.map((d, i) =>
        Buffer.concat([
          d, srcFrm.data[i].slice(curStart, curStart + this.dstFrmBytes - d.length)],
          this.dstFrmBytes));
      result.push(resFrm);

      dstFrm.pts += this.dstNumSamples;
      dstFrm.pkt_dts += this.dstNumSamples;
      curStart += this.dstFrmBytes - this.lastBuf[0].length;
      this.lastFrm.pts = 0;
      this.lastFrm.pkt_dts = 0;
      this.lastBuf = this.nullBuf;
    }

    this.lastFrm.pts = dstFrm.pts;
    this.lastFrm.pkt_dts = dstFrm.pkt_dts;
    this.lastBuf = srcFrm.data.map(d => d.slice(curStart, srcFrm.nb_samples * this.sampleBytes));

    return result;
  };

  private getLast(): Frame[] {
    let result: Frame[] = [];
    if (this.lastBuf[0].length > 0) {
      const resFrm = beamcoder.frame(this.lastFrm.toJSON());
      resFrm.data = this.lastBuf.map(d => d.slice(0));
      resFrm.nb_samples = this.lastBuf[0].length / this.sampleBytes;
      resFrm.pkt_duration = resFrm.nb_samples;
      this.lastFrm.pts = 0;
      this.lastBuf = this.nullBuf;
      result.push(resFrm);
    }
    return result;
  };

  public dice(frames: Frame[], flush = false): Frame[] {
    if (this.isAudio && this.doDice) {
      let result: Frame[] = [];
      for (const frm of frames)
        this.addFrame(frm).forEach(f => result.push(f));
      if (flush)
        this.getLast().forEach(f => result.push(f));
      return result;
    }
    return frames;
  };
}




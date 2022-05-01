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
import { CodecContextBaseMin } from "./types/CodecContext";
import { Frame } from "./types/Frame";
import beamcoder from './beamcoder'

export default class frameDicer {
    private addFrame: (srcFrm: Frame) => Frame[];
    private getLast: () => Frame[];
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
          const resFrm = beamcoder.frame(dstFrm.toJSON());
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
  
      this.getLast = (): Frame[] => {
        let result: Frame[] = [];
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
  
  
  
  
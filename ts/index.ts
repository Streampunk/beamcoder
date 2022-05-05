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

import beamcoder from './beamcoder';

export { default as demuxerStream } from './DemuxerStream';
export { default as muxerStream } from './MuxerStream';

export { default as DemuxerStream } from './DemuxerStream';
export { default as MuxerStream } from './MuxerStream';

export { default as makeSources } from './makeSources';
export { default as makeStreams } from './makeStreams';

export { getRaw, getHTML } from './utils';

// Provide useful debug on segfault-related crash
import SegfaultHandler from 'segfault-handler';
SegfaultHandler.registerHandler('crash.log');

export function splash() {
  const splash = `Aerostat Beam Coder  Copyright (C) 2019  Streampunk Media Ltd
  GPL v3.0 or later license. This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions. Conditions and warranty at:
  https://github.com/Streampunk/beamcoder/blob/master/LICENSE`;
  
  console.log(splash);
  console.log('Using FFmpeg version', beamcoder.avVersionInfo());  
}

export { Codec, CodecContext, CodecPar, Decoder, DecodedFrames, Demuxer, EncodedPackets, Encoder, Filter } from './types';
export { MediaType, FilterLink, FilterContext, FilterGraph, Filterer, InputFormat, OutputFormat, Frame } from './types';
export { SampleFormat, HWDeviceContext, HWFramesContext, Muxer, Packet, PrivClass, Disposition, Stream } from './types';
export type { governor} from './types';

export default beamcoder;

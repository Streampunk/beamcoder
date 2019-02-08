/*
  Aerostat Beam Coder - Node.js native bindings for FFmpeg.
  Copyright (C) 2019  Streampunk Media Ltd.

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

// Work in progress

const beamcoder = require('../index.js');

const STREAM_FRAME_RATE = 25;

function allocAudioFrame(sampleFormat, channelLayout, sampleRate, nbSamples) {

  return beamcoder.frame({
    format: sampleFormat,
    channel_layout: channelLayout,
    sample_rate: sampleRate,
    nb_samples: nbSamples
  }).alloc();
}

function allocPicture(pixelFmt, width, height) { // eslint-disable-line

  return beamcoder.frame({
    format: pixelFmt,
    width: width,
    height: height
  }).alloc();
}

async function addStream(stream, muxer, codecID) { // eslint-disable-line
  let codec = await beamcoder.encoder({ codec_id: codecID });

  stream.st = muxer.newStream();
  stream.enc = codec;
  switch (codec.media_type) {
  case 'video':
    codec.setParameters({
      codec_id: codecID,
      bit_rate: 400000,
      width: 352,
      height: 288,
      time_base: [1, STREAM_FRAME_RATE],
      gop_size: 12
    });
    break;
  case 'audio':
    break;
  default:
    break;
  }
}

async function run() {
  allocAudioFrame('s32p', 'stereo', 48000, 1920);
}

run();

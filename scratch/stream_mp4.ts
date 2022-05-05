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

import beamcoder from '..';
import md5File from 'md5-file';
import WebTorrent from 'webtorrent';
import fs from 'fs';
import os from 'os';

// https://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_320x180.mp4
// https://download.blender.org/peach/bigbuckbunny_movies/big_buck_bunny_1080p_h264.mov
// http://www.legittorrents.info/download.php?id=7f34612e0fac5e7b051b78bdf1060113350ebfe0&f=Big%20Buck%20Bunny%20(1920x1080%20h.264).torrent
async function run() {
  const mediaFile = '../big_buck_bunny_1080p_h264.mov'
  if (!fs.existsSync(mediaFile)) {
    console.log(`${mediaFile} is missing Downloading if now`);
    const client = new WebTorrent()
    const magnetURI = 'magnet:?xt=urn:btih:P42GCLQPVRPHWBI3PC67CBQBCM2Q5P7A&dn=big_buck_bunny_1080p_h264.mov&xl=725106140&tr=http%3A%2F%2Fblender.waag.org%3A6969%2Fannounce'
    await new Promise<void>((done) => {
      client.add(magnetURI, { path: '..' }, function (torrent) {
        // Got torrent metadata!
        console.log('Client is downloading:', torrent.infoHash)
        torrent.files.forEach(function (file) {
          // Display the file by appending it to the DOM. Supports video, audio, images, and
          // more. Specify a container element (CSS selector or reference to DOM node).
          console.log(file);
        })
        torrent.on("done", () => done());
      })
    })
    console.log(`${mediaFile} Downloaded`);
  }
  if (!fs.existsSync(mediaFile)) {
    console.log(`${mediaFile} still missing`);
    return;
  }
  const sumSrc = await md5File(mediaFile);
  if (sumSrc !== 'c23ab2ff12023c684f46fcc02c57b585')
    throw ('invalid Src md5');

  const urls = [`file:${mediaFile}`];
  const spec = { start: 0, end: 24 };

  const params = {
    video: [
      {
        sources: [
          { url: urls[0], ms: spec, streamIndex: 0 }
        ],
        filterSpec: '[in0:v] scale=1280:720, colorspace=all=bt709 [out0:v]',
        streams: [
          {
            name: 'h264', time_base: [1, 90000],
            codecpar: {
              width: 1280, height: 720, format: 'yuv422p', color_space: 'bt709',
              sample_aspect_ratio: [1, 1]
            }
          }
        ]
      }
    ],
    audio: [
      {
        sources: [
          { url: urls[0], ms: spec, streamIndex: 2 }
        ],
        filterSpec: '[in0:a] aformat=sample_fmts=fltp:channel_layouts=mono [out0:a]',
        streams: [
          {
            name: 'aac', time_base: [1, 90000],
            codecpar: {
              sample_rate: 48000, format: 'fltp', frame_size: 1024,
              channels: 1, channel_layout: 'mono'
            }
          }
        ]
      },
    ],
    out: {
      formatName: 'mp4',
      url: 'file:temp.mp4'
    }
  };

  await beamcoder.makeSources(params);
  const beamStreams = await beamcoder.makeStreams(params);

  await beamStreams.run();

  const sumDest = await md5File('temp.mp4');
  if (os.platform() === 'darwin') {
    if (sumDest !== '784983c8128db6797be07076570aa179')
      throw ('invalid Dst md5');
  } else {
    if (sumDest !== 'f08742dd1982073c2eb01ba6faf86d63')
      throw ('invalid Dst md5');
  }
}

console.log('Running mp4 maker');
let start = Date.now();
run()
  .then(() => console.log(`Finished ${Date.now() - start}ms`))
  .catch(console.error);


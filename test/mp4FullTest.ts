import test from 'tape';
import { makeSources, makeStreams } from '..';
import md5File from 'md5-file';
import WebTorrent from 'webtorrent';
import fs from 'fs';
import os from 'os';

test('recompress mp4', async t => {
  async function run() {
    const mediaFile = '../big_buck_bunny_1080p_h264.mov'
    if (!fs.existsSync(mediaFile)) {
      console.log(`${mediaFile} is missing Downloading it, now using torrent magnet link.`);
      const client = new WebTorrent()
      const magnetURI = 'magnet:?xt=urn:btih:P42GCLQPVRPHWBI3PC67CBQBCM2Q5P7A&dn=big_buck_bunny_1080p_h264.mov&xl=725106140&tr=http%3A%2F%2Fblender.waag.org%3A6969%2Fannounce'
      await new Promise<void>((done) => {
        client.add(magnetURI, { path: '..' }, function (torrent) {
          let len = 0;
          let ready = '0';
          // Got torrent metadata!
          console.log('Client is downloading:', torrent.infoHash)
          torrent.files.forEach(function (file) {
            // Display the file by appending it to the DOM. Supports video, audio, images, and
            // more. Specify a container element (CSS selector or reference to DOM node).
            console.log(file.length);
          })
          torrent.on("done", () => done());
          torrent.on("wire", (wire, addr) => console.log(`wire ${wire} addr: ${addr}`));
          torrent.on('download', (bytes: number) => {
            len += bytes;
            let ready2 = (len / (1024 * 1024)).toFixed(2);
            if (ready != ready2) {
              ready = ready2;
              console.log(`${ready2} downloaded to ${mediaFile}`);
            }
          });
        })
      })
      console.log(`${mediaFile} Downloaded`);
      client.destroy();
    }
    if (!fs.existsSync(mediaFile)) {
      console.log(`${mediaFile} still missing`);
      return;
    }

    const src = mediaFile;
    const sumSrc = await md5File(src);

    t.equal(sumSrc, 'c23ab2ff12023c684f46fcc02c57b585', 'source File have incrrrect md5sum');

    const urls = [`file:${src}`];
    const spec = {
      start: 0,
      end: 24
    };

    const params = {
      video: [{
        sources: [{
          url: urls[0],
          ms: spec,
          streamIndex: 0
        }],
        filterSpec: '[in0:v] scale=1280:720, colorspace=all=bt709 [out0:v]',
        streams: [{
          name: 'h264',
          time_base: [1, 90000],
          codecpar: {
            width: 1280,
            height: 720,
            format: 'yuv422p',
            color_space: 'bt709',
            sample_aspect_ratio: [1, 1]
          }
        }]
      }],
      audio: [{
        sources: [{
          url: urls[0],
          ms: spec,
          streamIndex: 2
        }],
        filterSpec: '[in0:a] aformat=sample_fmts=fltp:channel_layouts=mono [out0:a]',
        streams: [{
          name: 'aac',
          time_base: [1, 90000],
          codecpar: {
            sample_rate: 48000,
            format: 'fltp',
            frame_size: 1024,
            channels: 1,
            channel_layout: 'mono'
          }
        }]
      },],
      out: {
        formatName: 'mp4',
        url: 'file:temp.mp4'
      }
    };

    await makeSources(params);
    const beamStreams = await makeStreams(params);

    await beamStreams.run();

    const sumDest = await md5File('temp.mp4');

    if (os.platform() === 'darwin') {
      t.equal(sumDest, '784983c8128db6797be07076570aa179', 'dest File have incorrect md5sum');
    } else {
      t.equal(sumDest, 'f08742dd1982073c2eb01ba6faf86d63', 'dest File have incorrect md5sum');
    }
  }

  console.log('Running mp4 maker');
  return run();
  // .then(() => console.log(`Finished ${Date.now() - start}ms`))
  //.catch(console.error);
});

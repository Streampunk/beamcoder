import beamcoder, { demuxerStream } from '..'; // Use require('beamcoder') externally
import path from 'path';
import fs from 'fs';
import { Demuxer, getRaw } from '..';
import md5File from 'md5-file';

const streamUrl = 'https://github.com/awslabs/amazon-kinesis-video-streams-producer-c/raw/master/samples/h264SampleFrames/';
async function getFiles(): Promise<string[]> {
    const src = path.join(__dirname, 'capture', 'h264SampleFrames');
    if (!fs.existsSync(src)) {
        fs.mkdirSync(src, { recursive: true });
    }
    let filelist = (await fs.promises.readdir(src)).filter(f => f.endsWith('.h264'));
    const toto: Promise<void>[] = [];
    if (filelist.length < 403) {
        for (let i = 1; i < 404; i++) {
            const fn = `frame-${i.toFixed().padStart(3, '0')}.h264`;
            const url = `${streamUrl}${fn}`;
            const dest = path.join(src, fn)
            if (!fs.existsSync(dest)) {
                let ws = fs.createWriteStream(dest);
                toto.push(getRaw(ws, url).catch(async (err) => {
                    if (err.name === 'RedirectError') {
                        const redirectURL = err.message;
                        await getRaw(ws, redirectURL, fn);
                    } else throw err;
                }))
            }
        }
        await Promise.all(toto);
        filelist = (await fs.promises.readdir(src)).filter(f => f.endsWith('.h264'));
    }
    filelist.sort();
    return filelist.map(f => path.join(src, f));
}

async function run() {
    const filelist = await getFiles();
    const stream = new demuxerStream({ highwaterMark: 3600 });
    const demuxPromise = stream.demuxer({})
    demuxPromise.then(async (demuxer: Demuxer) => {
        const packet = await demuxer.read();
        let dec = beamcoder.decoder({ demuxer, stream_index: 0 }); // Create a decoder
        let decResult = await dec.decode(packet); // Decode the frame
        if (decResult.frames.length === 0) // Frame may be buffered, so flush it out
            decResult = await dec.flush();
        // Filtering could be used to transform the picture here, e.g. scaling
        let enc = beamcoder.encoder({ // Create an encoder for JPEG data
            name: 'mjpeg', // FFmpeg does not have an encoder called 'jpeg'
            width: dec.width,
            height: dec.height,
            pix_fmt: dec.pix_fmt.indexOf('422') >= 0 ? 'yuvj422p' : 'yuvj420p',
            time_base: [1, 1]
        });
        let jpegResult = await enc.encode(decResult.frames[0]); // Encode the frame
        await enc.flush(); // Tidy the encoder
        const jpgDest = 'capture.jpg';
        fs.writeFileSync(jpgDest, jpegResult.packets[0].data);
        const sumDest = await md5File(jpgDest);
        const expectedMd5Mac = '63a5031f882ad85a964441f61333240c';
        const expectedMd5PC = 'e16f49626e71b4be46a3211ed1d4e471';
        if (expectedMd5Mac !== sumDest && expectedMd5PC !== sumDest) {
            console.error(`MD5 missmatch get ${sumDest}`)
        }
        console.log(`saving in stream img as ${jpgDest}`);
        demuxer.forceClose();
    });
    // https://github.com/awslabs/amazon-kinesis-video-streams-producer-c/raw/master/samples/h264SampleFrames/frame-001.h264
    for (const fullname of filelist) {
        const buf = await fs.promises.readFile(fullname);
        stream.write(buf);
    }
    stream.emit('finish');
}

run().catch(e => console.error(e));

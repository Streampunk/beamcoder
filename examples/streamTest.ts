import beamcoder from '..'; // Use require('beamcoder') externally
import path, { resolve } from 'path';
import fs from 'fs';


const delay = (ms: number) => new Promise((resolve) => { setTimeout(resolve, ms)})


async function process() {
}

async function run() {
    debugger;
    const stream = beamcoder.demuxerStream({highwaterMark: 360000});
    console.log(stream);
    console.log('stream');
    const demuxPromise = stream.demuxer({})
    demuxPromise.then(demux => console.log(demux));
    const src = path.join(__dirname, 'capture');
    const filelist = fs.readdirSync(src);
    filelist.sort();
    for (const f of filelist) {
        const fullname = path.join(src, f);
        const buf = fs.readFileSync(fullname);
        // console.log(fullname);
        stream.write(buf);
        await delay(100);
    }
}

run();

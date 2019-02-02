# Beam Coder

[Node.js](https://nodejs.org/) native bindings to [FFmpeg](https://www.ffmpeg.org/) with support for asynchronous processing via streams and promises.

The aim of this module is to facilitate access to the capabilities of FFmpeg - including media muxing, demuxing, encoding, decoding and filtering - from Node.js applications. Rather than using the filesystem and controlling the FFmpeg as an external command line process, the beam coder executes functions of the FFmpeg _libav*_ libraries directly. Work is configured with Javascript objects and jobs execute over data buffers that are shared between Javascript and C. Long running media processing operations are asynchronous, running as promises that execute native code separately from the main event loop.

### Example

The following code snippet is an app that allows a user to access key frames of video from the current folder, e.g. `.MP4` media files on a camera memory card, as JPEG images in a browser. For example, to access a key frame near to 42.5 seconds from the start of a file called `GOPR9502.MP4`:

    http://localhost:3000/GOPR9502.MP4/42.5

Beam coder uses promises and so the code for the server works well with [koa](https://koajs.com):

```Javascript
const beamcoder = require('beamcoder');
const Koa = require('koa');
const app = new Koa();

app.use(async (ctx) => { // Assume HTTP GET with path /<file_name>/<time_in_s>
  let parts = ctx.path.split('/'); // Split the path into filename and time
  if ((parts.length < 3) || (isNaN(+parts[2]))) return; // Ignore favicon etc..
  let dm = await beamcoder.demuxer('file:' + parts[1]); // Probe the file
  await dm.seek({ time: +parts[2] }); // Seek to the closest keyframe to time
  let packet = await dm.read(); // Find the next video packet (assumes stream 0)
  for ( ; packet.stream_index !== 0 ; packet = await dm.read() );
  let dec = beamcoder.decoder({ format: dm, stream: 0 }); // Create a decoder
  let decResult = await dec.decode(packet); // Decode the frame
  if (decResult.frames.length === 0) // Frame may be buffered, so flush it out
    decResult = await dec.flush();
  // Filtering could be used to transform the picture here, e.g. scaling
  let enc = beamcoder.encoder({ // Create an encoder for JPEG data
    name : 'mjpeg',
    width : dec.width,
    height: dec.height,
    pix_fmt: dec.pix_fmt.indexOf('422') >= 0 ? 'yuvj422p' : 'yuvj420p',
    time_base: [1, 1] });
  let jpegResult = await enc.encode(decResult.frames[0]); // Encode the frame
  await enc.flush(); // Tidy the encoder
  ctx.type = 'image/jpeg'; // Set the Content-Type of the data
  ctx.body = jpegResult.packets[0].data; // Return the JPEG image data
});

app.listen(3000); // Start the server on port 3000
```

### Scope

The developers created beam coder to enable development of highly-scalable frame-by-frame, packet-by-packet web-fit nanoservices built out the web-platform, combining media IO functions with the comprehensive library of other scalable IO modules for Node, such as [express](https://expressjs.com/), [koa](https://koajs.com/), [ioredis](https://www.npmjs.com/package/ioredis) etc..

If you are looking to write your own frame-by-frame transcoder, media mangler or muxer, you are in the right place. However, if you want to control FFmpeg as a command line application over complete files or piped streams from a Node.JS application, many other projects are available, such as [fluent-ffmpeg](https://www.npmjs.com/package/fluent-ffmpeg).

Does beam coder support X, Y or Z protocol / format / codec / file type / stream type / hardware etc.? If FFmpeg supports it, its possible and likely. You have to start somewhere, and the developers have been testing with the codecs and formats they are familiar with. Please raise any problems or requests for additional features as issues or raise a pull request to add in missing features. Automated testing will be extended in due course.

Beam coder will be a cross-platform module for Windows, Mac and Linux. In this early release, only Windows and Linux are supported. The Mac platform will follow shortly. The release version of FFmpeg that beam coder links with is 4.1.

### Aerostat

Beam coder is the first release of Streampunk Media's [_Aerostat_](https://en.wikipedia.org/wiki/Aerostat) open-source product set, whereby a fleet of media-oriented _aerostats_ (_blimps_, _air ships_, _zeppelins_ etc.) are launched into the clouds. Media content is beamed between the fleet as if light beams, and beamed to and from locations on the planet surface as required. See also the [_Aerostat Beam Engine_](https://www.npmjs.com/package/beamengine).

## Installation

### Pre-requisites

1. Install the LTS version of [Node.js](https://nodejs.org/en/) for your platform.
2. Enable [node-gyp - the Node.js native addon build tool](https://github.com/nodejs/node-gyp) for your platform by following the [installation instructions](https://github.com/nodejs/node-gyp#installation).

Note: For MacOSX _Mojave_, install the following package after `xcode-select --install`:

    /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg

This release of Beamcoder only supports 64-bit (`x86_64`) architectures.

### Installing

#### Windows

Beam coder is intended to be used as a module/package from other Node.js packages and applications. Install beam coder from the root folder of your package as follows:

    npm install beamcoder

This will install all necessary dependencies, download the FFmpeg shared and dev packages with include and library files, and then compile the native extensions.

If you want to use a local version of FFmpeg, before the install, symbolic link appropriate folders to:

    node_modules/beamcoder/ffmpeg/ffmpeg-4.1-win64-shared
    node_modules/beamcoder/ffmpeg/ffmpeg-4.1-win64-dev

To ensure that sufficient threads are available to process several requests in parallel, set the `UV_THREADPOOL_SIZE` environment variable, e.g.:

    set UV_THREADPOOL_SIZE=16

#### Linux

On Linux, use the appropriate package manager to install the FFmpeg 4.1 development dependencies first. An error will be printed if these cannot be found at expected locations. For example, on Ubuntu:

    sudo add-apt-repository ppa:jonathonf/ffmpeg-4
    sudo apt-get install libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev

An example for Redhat / Fedora / CentOs will follow.

Beam coder is intended to be used as a module/package from other applications. Install beam coder from the root folder of your package as follows:

    npm install beamcoder

This will check for all the necessary dependencies and then compile the native extensions.

To ensure that sufficient threads are available to process several requests in parallel, set the `UV_THREADPOOL_SIZE` environment variable, e.g.:

    export UV_THREADPOOL_SIZE 32

#### Mac

To follow.

## Usage

### Introduction

The basic usage pattern is as follows:

1. Use promises to create processing stages for an item of media, for example a demuxer and a decoder. One can be used to configure the other.
2. In a loop, use a promise to move data through a sequence of processing stages, working with the data as required. For example, read some frames from a file and send them to a decoder, receiving uncompressed data for each frame. This data may be suitable for display or analysis.
3. Flush the buffers of each processing stage, close resources and collect any outstanding frames.

Here is a simple example as code:

```Javascript
const beamcoder = require('beamcoder');

async function run() {
  let demuxer = await beamcoder.demuxer('/path/to/file.mp4'); // Create a demuxer for a file
  let decoder = beamcoder.decoder({ name: 'h264' }); // Codec asserted. Can pass in demuxer.
  let packet = {};
  for ( let x = 0 ; x < 1000 && packet != null ; x++ ) {
    packet = await format.read(); // Read next frame. Note: returns null for EOF
    if (packet.stream_index === 0) { // Check demuxer to find index of video stream
      let frames = await decoder.decode(packet);
      // Do something with the frame data
      console.log(x, frames.total_time); // Optional log of time taken to decode each frame
    }
  }
  let frames = await decoder.flush(); // Must tell the decoder when we are done
  console.log('flush', frames.total_time, frames.length);
}

run();
```

For mode examples, see the `examples` folder. Note that the eventual aim is that every example in the FFmpeg source tree will have a corresponding example for Node.JS.

### Values

In design, each of the main structures of FFmpeg have mappings to Javascript objects and values are automatically marshalled and unmarshalled between native and Javascript representations. With the exception of encoders and decoders, these objects contain an internal instance of the native C structure, have a `type` property for the type name, and use getters and setters to expose the underlying native properties. Wherever possible, the names of the properties are the same as those in the C library, meaning that [FFmpeg's doxygen documentation](https://www.ffmpeg.org/doxygen/4.1/index.html) should be used as a reference.

#### Construction

To create one of the main data types _packet_, _frame_, _demuxer_, _muxer_, _decoder_, _filterer_, or _encoder_, use the  methods of `beamcoder` with the same name e.g. `packet`, `frame`, etc. With no arguments, the value constructed has FFmpeg's default values. To contruct a packet:

    let pkt = beamcoder.packet();

To configure the object on initialisation, pass is an options object:

```Javascript
let q = beamcoder.packet({ pts: 10, dts: 10, stream_index: 3 });
console.log(q);
{ type: 'Packet',
  pts: 10,
  dts: 10,
  data: null,
  size: 0,
  stream_index: 3,
  /* ... */ }
```

Note that some special cases exist to this rule, such as creating a _demuxer_ from a URL or filename. As this is a common use case, a single string can be passed in rather than an options object. As the URL or file must be accessed before the demuxer is constructed, a promise to create the demuxer is returned rather than a demuxer directly. This allows the file or streams operation to be asynchronous. See the description of each processing stage for details.

#### Reading and modifying

After construction, Javascript properties of an object can be used in a natural way, with dot notation and array-style access:

```Javascript
let q_pts = q.pts; // Current value of pts for q, q_pts= 10
let same = q['pts'] === q.pts; // true ... the current value of pts for q = 10
q.pts = 12; // Set the value pts for q to 12
q.data = Buffer.from('Text data for this packet.'); // Set packet data
```

This is achieved with [setters and getters](https://www.w3schools.com/js/js_object_accessors.asp). Note that not every readable (_enumerable_) property is writeable. This may depend on context. For example, some properties are set only by _libav*_ or can only be updated by the user when encoding. The work to convert a value from C to Javascript is only done when each separate property is requested, which is worth bearing in mind before being too liberal with, say, `console.log()` that enumerates every property of an object.

#### Freeing and deleting

Care has been taken to ensure that the reference-counted then garbage collected data structures of Javascript work in tandem with the allocation and free mechanisms of _libav*_. As such, there is no explicit requirement to free or delete objects. As with any Javascript application, if you hold onto references to objects when they are no longer required, garbage collection is prevented and this may have a detrimental impact on performance.

#### Type mappings

Property value mappings from C to Javascript and vice versa are as follows:

* C `int`, `int64_t` and other integer types map to and from Javascript's `number` type. Try to use integer representations in Javascript wherever possible, e.g. `|0` when dividing. (Note that the full ranges of C `int64_t` and `uint64_t` are not yet supported by Beam Coder. This feature is waiting on Javascript `BigInt` to move from experimental support to full support in Node.)
* C `float` and `double` types map to and from Javascript's `number` type. Note that as the underlying representation of floating point values is double length in Javascript, causing conversion to and from C `float` to lose precision. It is best practice to test for small value ranges around a value rather than exact values.
* `AVRational` values map to and from a Javascript array containing two integer `number`s. For example, `(AVRational){1,25}` maps to Javascript `[1,25]`.
* C `char *` and `const char *` map to and from Javascript's `string` type.
* FFmpeg enumerations are converted to Javascript strings representing their name. FFmpeg provides its own utility functions for converting most of its enumerations to and from strings, for example [`av_color_space_name`](https://www.ffmpeg.org/doxygen/4.1/pixdesc_8c.html#a7a5b3f4d128f0a0112b4a91f75055339) and [`av_color_space_from_name`](https://www.ffmpeg.org/doxygen/4.1/pixdesc_8c.html#a0c7d0a9d7470c49397a72e1169d2a8e3).
* FFmpeg flags are converted to and from a Javascript object of Boolean-valued properties, where each property is named to match the distinguishing part of the `#define` macro name. For example, the _corrupt_ and _discard_ flags of an `AVFrame` `flags` property become `{ CORRUPT: false, DISCARD: false }` in Javascript.
* `AVBufferRef` and other blobs of binary data are represented by [Node.JS Buffers](https://nodejs.org/docs/latest-v10.x/api/buffer.html). Wherever possible, the data is not copied. For example, on reading an `AVBuffer` from Javascript, a new reference to the underlying `AVBuffer` is created and a view of the data made available via an external buffer. The reference is held until the Javascript buffer is garbage collected. Coversely, when a Javascript buffer is to be read from an FFmpeg structure, the `AVBufferRef` structure holds a V8 reference to the underlying data of the Javascript buffer. The Javascript buffer is kept alive until both of any AV processing functions and Javascript have finished with it.
* `AVDictionary` metadata values have a natural mapping to Javascript objects as keys as property names and values as string values.

Private data used for configuring encoders, filters and other objects is supported where required. See the encoding section for details.

### Demuxing

The process of demuxing (de-multiplexing) extracts time-labelled packets of data from streams contained in a media stream or file. FFmpeg provides a diverse range of demuxing capability with support for a wide range of input formats.

To see a list and details of all the available demuxer input formats:

    let dms = beamcoder.demuxers();

The output is an object where each property key is the name of a demuxer and each value is an object describing the input format. For example, to find the demuxer for '.mp4' files:

```Javascript
Object.values(dms).filter(x => x.extensions.indexOf('mp4') >= 0);
[ { type: 'InputFormat',
    name: 'mov,mp4,m4a,3gp,3g2,mj2',
    long_name: 'QuickTime / MOV',
    mime_type: '', // Generally not available for demuxers
    extensions: 'mov,mp4,m4a,3gp,3g2,mj2',
    flags:
     { NOFILE: true, // Demuxer will manage IO operations
       NEEDNUMBER: false, // Needs '%d' in filename
       SHOW_IDS: true, // Show format stream IDs numbers.
       GENERIC_INDEX: true, // Use generic index building code
       TS_DISCONT: true, // Format allows timestamp discontinuities.
       NOBINSEARCH: true, // Format does not allow to fall back on binary search via read_timestamp
       NOGENSEARCH: true, // Format does not allow to fall back on generic search
       NO_BYTE_SEEK: false, // Format does not allow seeking by byte
       SEEK_TO_PTS: false }, // Seeking is based on PTS
    raw_codec_id: 0,
    priv_data_size: 400,
    priv_class: null } ]
```

The easiest way to create a demuxer is with a filename or URL, for example to open a transport stream containing the [Big Buck Bunny]() short movie in file `bbb_1080p_c.ts` in the `movie` sub-directory:

    let tsDemuxer = await beamcoder.demuxer('file:media/bbb_1080p_c.ts');

The `demuxer` operation performs file system and/or network access and so is asynchronous. On successful resolution, the value is a Javascript object describing the contents of the media input after the contents of the file or stream has been probed. Here is a summary of the created demuxer:

```Javascript
{ type: 'demuxer',
  iformat:
   { type: 'InputFormat',
     name: 'mpegts',
     \* ... *\ },
  ctx_flags: { NOHEADER: false, UNSEEKABLE: false },
  streams:
  [ { type: 'Stream',
      index: 0,
      id: 301,
      time_base: [ 1, 90000 ], /* ... */
      codecpar:
       { type: 'CodecParameters',
         codec_type: 'video',
         codec_id: 173,
         name: 'hevc',
         codec_tag: 'HEVC',
         format: 'yuv420p', /* ... */
         width: 1920,
         height: 1080,
        /* ... */ } },
    { type: 'Stream',
      index: 1,
      id: 302,
      time_base: [ 1, 90000 ], /* ... */
      codecpar:
       { type: 'CodecParameters',
         codec_type: 'audio',
         codec_id: 86018,
         name: 'aac', /* ... */
         format: 'fltp', /* ... */
         channel_layout: 'stereo',
         channels: 2,
         /* ... */ } } ],
  url: '../media/bbb_1080p_c.ts',
  start_time: 80000,
  duration: 596291667,
  bit_rate: 2176799, /* ... */
  stream: [Function],
  read: [Function: readFrame],
  seek: [Function: seekFrame] }
```

From this it is possible to determine that the file contains two streams, a HD video stream encoded with H.265/HEVC and a stereo audio stream encoded with AAC. The duration of the media is measure in microseconds (`AV_TIME_BASE`), so is approximately 596 seconds or 9m56s.

For formats that require additional metadata, such as raw video formats, it may be necessary to pass additional information such as image size or pixel format. To do this, pass in an options object with a `url` property for the filename(s) (may contain `%d` for a sequence of numbered files) and `options` property for the values. For example:

```Javascript
let rawDemuxer = await beamcoder.demuxer({
  url: 'file:movie/bbb/raw_pictures_%d.rgb',
  options: {
    video_size: '640x480',
    pixel_format: 'rgb24'
  }
});
```

#### Reading frames

To read data from the demuxer, use the `read` method of a demuxer-type object, a method that takes no arguments. This reads the next data packet from the file or stream at the current position, where a frame could be from any of the streams. Typically, a packet is one frame of video data or a blob representing a codec-dependent number of audio samples. Use the `stream_index` property of returned packet to find out which stream it is associated with and dimensions including height, width or audio sample rate. For example:

```Javascript
let wavDemuxer = await beamcoder.demuxer('my_audio.wav');
let packet = {};
while (packet != null) {
  packet = await wavDemuxer.read();
  // Do something with the packet
}
```

The read method is asynchronous and returns a promise. The promise resolves to an object of type `Packet` if it succeeds or a `null` value at the end of the file. If an error occurs, the promise rejects with an error message. An example of a successful read from a WAVE file is shown below:

```Javascript
{ type: 'Packet',
  pts: 2792448,
  dts: 2792448,
  data:
   <Buffer fd ff fe ff 02 00 01 00 ff ff 00 00 00 00 ff ff 00 00 02 00 02 00 fe ff fd ff 01 00 04 00 00 00 fb ff ff ff 04 00 01 00 fe ff ff ff 01 00 01 00 00 00 ... >,
  size: 4160,
  stream_index: 0,
  flags:
   { KEY: true, // Packet represents a key frame
     CORRUPT: false, // Corruption detected
     DISCARD: false, // Can be dropped after decoding
     TRUSTED: false, // Packet from a trusted source
     DISPOSABLE: false }, // Frames that can be discarded by the decoder
  duration: 1024,
  pos: 11169836 }
```

#### Seeking

Beam coder offers FFmpeg's many options for seeking a particular frame in a file, either by time reference, frame count or file position. To do this, use the `seek` method of a demuxer-type object with an options object to configure the operation.

To seek forward to a keyframe in a given stream or file at a given timestamp:

    await demuxer.seek({ stream_index: 0, timestamp: 2792448 });

The timestamp is the presentation timestamp of the packet measured in the timebase of the stream, which is `[1, 48000]` in the example. To seek based on elapsed time from the beginning of the primary stream (as determined by FFmpeg, normally the first video stream where available), use the `time` property:

    await demuxer.seek({ time: 58.176 });

Another form of seek is to use a byte offset position into the file:

    await demuxer.seek({ pos: 11169836 });

The final form of seeking supported is by number of frames into a given stream:

    await demuxer.seek({ frame: 42, stream_index: 0});

All seek call resolve to a `null` value or rejects if there is an error. You have to call `read` to get the next frame. Note that if you seek beyond the end of the file or stream, the call resolves OK and the next read operation resolves to `null`.

The seek operation has two additional flags that can be specified. The `backward` Boolean-valued property can be used to enable seeking backwards where supported. The `any` Boolean-valued property enables seeking to both

#### Using Node.JS streams

___Simon___

### Decoding

Decoding is the process of taking a stream of compressed data in the form of _packets_ and converting it into _frames_. In general, to decode an interleaved (multiplexed) media file, you need a decoder for each of the video and the audio streams. For the purpose of keeping the examples simple in this section, a single stream is decoded but it is possible to set up more than one decoder - say for a video and audio stream - and run them asynchronously, i.e. to decode the video and audio for a frame in parallel.

To see a list of available decoders, use:

    let decs = beamcoder.decoders();

As with the demuxers, the result is an object where the keys are the names of the decoders and the values are objects describing the codec. This includes the codec type (`video`, `audio`, `subtitle`), a _descriptor_ for the family of codecs, some capability flags, supported profiles and more. Here are some examples of querying the available decoders:

```Javascript
// Find a decoder that deals with H.264 / AVC
decs['h264'];
{ type: 'Codec',
  name: 'h264',
  long_name: 'H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10',
  codec_type: 'video',
  id: 27, /* ... */ }

// Find all other decoders that can decode H.264
Object.values(decs).filter(x => x.id === 27).map(x => x.name);
[ 'h264', 'h264_qsv', 'h264_cuvid' ]
// Note: h264_qsv and h264_cuvid are wrappers to hardware-accelerated decoders
//       Appearance in the list does not imply that the codec is supported by current hardware.

// Find all decoders claiming support for the MP3 format
Object.values(decs)
  .filter(x => x.long_name.indexOf('MP3') >= 0)
  .map(x => ({ name: x.name, long_name: x.long_name }));
[ { name: 'mp3float', long_name: 'MP3 (MPEG audio layer 3)' },
  { name: 'mp3', long_name: 'MP3 (MPEG audio layer 3)' },
  { name: 'mp3adufloat',
    long_name: 'ADU (Application Data Unit) MP3 (MPEG audio layer 3)' },
  { name: 'mp3adu',
    long_name: 'ADU (Application Data Unit) MP3 (MPEG audio layer 3)' },
  { name: 'mp3on4float', long_name: 'MP3onMP4' },
  { name: 'mp3on4', long_name: 'MP3onMP4' } ]

// List all audio decoders
Object.values(decs).filter(x => x.codec_type === 'audio').map(x => x.name);
[ 'comfortnoise', 'dvaudio', '8svx_exp', '8svx_fib', 's302m', 'sdx2_dpcm',
  'aac', 'aac_fixed', 'aac_latm' /* ... */ ]
```

#### Decoder

To get an instance of a decoder, request a `decoder` from beam coder, specifying either the decoder's `name`, a `codec_id`, or by providing a `demuxer` and a `stream_id`. For example:

```Javascript
// Create a decoder by name - note this is synchronous
let decoder = beamcoder.decoder({ name: 'h264' });
// or for the first choice codec in the H.264 family
let decoder = beamcoder.decoder({ codec_id: 27 });
// Alternatively, use a demuxer and a stream_index
//   The codec parameters of the streams will be used to set the decoding parameters
let tsDemux = await beamcoder.demuxer('media/bbb_1080p_c.ts');
let decoder = beamcoder.decoder({ demuxer: tsDemux, stream_index: 0 });
```

Other properties of the decoder can be provided on initialisation and may be required in certain cases. For example, the width and height of the video. These will override the default values.  Note that certain values for the decoder may not be available until a the first few packets have been decoded.

```Javascript
let decoder = beamcoder.decoder({ name: 'h264', width: 1920, height: 1080 });
```

A decoder has many properties. These can be set before decoding in the usual way for a Javascript object. Some of the properties are more appropriate for encoding but are listed for information. Some properties can only be set by _libav*_ and others can only be set by the user. Follow the [FFmpeg documentation]() for details.

#### Decode

To decode an encoded data _packet_ and create an uncompressed _frame_ (may be a frames-worth of audio), user the asynchronous _decode_ method of a decoder. Decoders may need more than one packet to produce a frame and may subsequently produce more than one frame per packet. This is particularly the case for _long-GOP_ formats.

```Javascript
while (packet != null) {
  let dec_result = await decoder.decode(packet);
  // dec_result.frames - possibly empty array of frames for further processing
  // dec_result.total_time - log the microseconds that the operation took to complete
  // Get the data for the packet
}
```

As long as decoding was successful, the decode operation resolves to an object containing the `total_time` (measured in microseconds) that the operation took to execute, and an array of decoded frames that are now available. If the array is empty, the decoder has buffered the packet as part of the process of producing future frames. Frames are delivered in presentation order.

It is possible to pass more than one packet at a time to the decoder, either as an array of packets or a list of arguments:

```Javascript
// Array of packets
let dec_result = await decoder.decode([packet1, packet2, packet3 /* ... */ ]);
// List of packets as arguments
let dec_result = await decoder.decode(packet1, packet2, packet3 /* ... */ );
```

#### Flush

Once all packets have been passed to the decoder, it is necessary to call the asynchronous `flush` operation. If any frames are yet to be delivered by the decoder, they will be provided in the resolved value.

```Javascript
let flush_result = await decoder.flush();
// flush_result.frames - array of any remaining frames to be decoded
// flush_result.total_time - microseconds taken to execute the flush operation
```

The decoder will be cleaned up as part of the Javascript garbage collection process, so make sure that the reference to the decoder goes out of scope.

### Filtering

___Simon___

### Encoding

### Muxing

#### Opening output

#### Writing header

#### Writing packets and frames

#### Writing trailer


## Status, support and further development

Although the architecture of the aerostat beam coder is such that it could be used at scale in production environments, development is not yet complete. In its current state, it is recommended that this software is used in development environments, primarily for building prototypes. Future development will make this more appropriate for production use.

The developers of beam coder aimed to find a balance between being a faithful mapping of FFmpeg to Javascript while creating a Javascript API that is useful and easy to use. This may mean that certain features of FFmpeg are not yet exposed or choices have been made that lead to sub-optimal performance. Areas that are known to need further development and optimisation include:

* shared memory management between Javascript and C, specifically adding support for pools;
* hardware acceleration;
* side data and extended data.

Contributions can be made via pull requests and will be considered by the author on their merits. Enhancement requests and bug reports should be raised as github issues. For support, please contact [Streampunk Media](http://www.streampunk.media/).

## License

This project is licensed under the GNU General Public License, version 3 or later. Copyright (C) 2019, Streampunk Media Ltd.

This software links to libraries from the FFmpeg project, including optional parts and optimizations covered by the GPL v2.0 or later. Your attention is drawn to the FFmpeg project's page [FFmpeg License and Legal Considerations](https://www.ffmpeg.org/legal.html) and the Copyright notices of the FFmpeg developers.

### Acknowledgements

A massive thank you to the FFmpeg development team whom's tireless and ongoing work make this and so many other media software projects possible.

# Beam Coder

[Node.JS](https://nodejs.org/) native bindings to [FFmpeg](https://www.ffmpeg.org/) with support for asynchronous processing via streams and promises.

The aim of this module is to facilitate relatively easy access to the capabilities of FFmpeg - including media muxing, demuxing, encoding, decoding and filtering - from Node.JS applications. Rather than using the filesystem and controlling the FFmpeg as an external command line process, the beam coder executes functions of the FFmpeg _libav*_ libraries directly. Work is configured by Javascript objects and executes over data buffers that are shared between Javascript and C. Long running media processing operations are asynchronous, running as promises that execute native code separately from the main event loop. The developers created beam coder to enable development of highly-scalable frame-by-frame, packet-by-packet media nanoservices built out the web-platform, combining media IO functions with the comprehensive library of other scalable IO modules for Node, such as [express](https://expressjs.com/), [koa](https://koajs.com/), [ioredis](https://www.npmjs.com/package/ioredis) etc..

If you are looking to write your own frame-by-frame transcoder, media mangler or muxer, you are in the right place. However, if you want to control FFmpeg as a command line application over complete files or piped streams from a Node.JS application, many other projects are available, such as [fluent-ffmpeg](https://www.npmjs.com/package/fluent-ffmpeg).

Beam coder will be a cross-platform module for Windows, Mac and Linux. In this early release, only Windows is supported. Other platforms will follow shortly.

Beam coder is the first release of Streampunk Media's [_Aerostat_](https://en.wikipedia.org/wiki/Aerostat) product set, whereby a fleet of media-oriented _aerostats_ (_blimps_, _air ships_, _zeppelins_ etc.) are launched into the clouds. Media content is beamed between the fleet as if beams of light and beamed to and from locations on the planet surface as required. See also the [_Aerostat Beam Engine_](https://www.npmjs.com/package/beamengine).

## Installation

### Pre-requisites

### Installing

## Usage

### Introduction

The basic usage pattern is as follows:

1. Use promises to create processing stages for some media, for example a demuxer and a decoder.
2. In a loop, use a promise to get data from one processing stage and feed it into the next, using the data as required. For example, read some frames from a file and send them to a decoder, receiving uncompressed representations of each frame.
3. Flush the buffers of each processing stage and collect any outstanding frames.

Here is a simple example as code:

```Javascript
const beamcoder = require('beamcoder');

async function run() {
  let demuxer = await beamcoder.format('/path/to/file.mp4'); // Create a demuxer for a file
  let decoder = await beamcoder.decoder({ name: 'h264' }); // Codec assumed, can pass in demuxer
  for ( let x = 0 ; x < 1000 ; x++ ) {
    let packet = await format.readFrame(); // Read next frame
    if (packet.stream_index === 0) { // Check format to find index of video stream
      let frames = await decoder.decode(packet);
      // Do something with the frame data
      console.log(x, frames.totalTime); // Optional log of time taken to decode each frame
    }
  }
  let frames = await decoder.flush(); // Must tell the decoder when we are done
  console.log('flush', frames.totalTime, frames.length);
}

run();
```

For mode examples, see the `examples` folder.

### Marshalling

In design, each of the main structures of FFmpeg have mappings to Javascript objects and values are automatically marshalled and unmarshalled between native and Javascript representations. With the exception of codec contexts (encoders and decoders), these objects contain an internal instance of the native C structure, have a `type` property for the type name, and use getters and setters to expose the properties. Wherever possible, the names of the properties are the same as those in the C library, meaning that [FFmpeg's doxygen documentation](https://www.ffmpeg.org/doxygen/4.1/index.html) can be used find out the details of each property.

To create one of the main data types, use the `make...` methods of `beamcoder`. With no arguments, the value has default values.

    let pkt = beamcoder.makePacket();

To configure the object an initialisation, pass is an options object:

```Javascript
let q = beamcoder.makePacket({ pts: 10, dts: 10, stream_index: 3 });
/* q is now ...
{ type: 'Packet',
  pts: 10,
  dts: 10,
  data: null,
  size: 0,
  stream_index: 3,
... } */
```

Property value mappings are as follows:

* C `int`, `int64_t` and other integer types map to Javascript's `number` type. Try to use integer representations in Javascript wherever possible, e.g. `|0` when dividing. Note that the full range of a C `int64_t` is not yet supported by Beam Coder, waiting on Javascript `BigInt` to move from experimental implementation in Node.
* C `float` and `double` types map the Javascript's `number` type. Note that as the underlying representation of floating point values is double length in Javascript, conversion to and from C `float` can cause a lose of precision. Make sure to test for value ranges, not exact values.
* `AVRational` maps to a Javascript array containing two integer `number`s. For example, `(AVRational){1,25}` maps to Javascript `[1,25]`.
* C `char *` and `const char *` map to Javascript's `string` type.
* AV enumerations are converted to Javascript strings representing their name. FFmpeg provides its own utility functions for converting most of its enumerations to and from strings, for example [`av_color_space_name`](https://www.ffmpeg.org/doxygen/4.1/pixdesc_8c.html#a7a5b3f4d128f0a0112b4a91f75055339) and [`av_color_space_from_name`](https://www.ffmpeg.org/doxygen/4.1/pixdesc_8c.html#a0c7d0a9d7470c49397a72e1169d2a8e3).
* AV flags are converted to a Javascript object of Boolean-valued properties, where each property is named to match variable part of the `#define` in the C-library header files and documentation. For example, the _corrupt_ and _discard_ flags of an `AVFrame` `flags` property become `{ CORRUPT: false, DISCARD: false }` in Javascript.
* AVBufferRef and other blobs of binary data are represented by [Node.JS Buffers](https://nodejs.org/docs/latest-v10.x/api/buffer.html). Wherever possible, the data is not copied. For example, on reading a new counted reference to the underlying AVBuffer is created and held onto until the Javascript buffer is garbage collected. When a Javascript buffer is written to an AV structure, the structure holds a reference and the underlying data is kept alive until both of any AV processing functions and Javascript have finished with it.

Private data used for configuring codecs is supported in some cases. See the encoding section for details.


### Demuxing

### Decoding

### Encoding

### Muxing

### Filtering

## Status, support and further development

Although the architecture of the aerostat beam engine is such that it could be used at scale in production environments, development is not yet complete. In its current state, it is recommended that this software is used in development environments and for building prototypes. Future development will make this more appropriate for production use.

The developers of beam coder aimed to find a balance between being a faithful mapping of FFmpeg to Javascript while creating a Javascript API that is useful and easy to use. This may mean that certain features of FFmpeg are not yet exposed or choices have been made that lead to sub-optimal performance. Areas that are known to need further development and optimisation include:

* shared memory management between Javascript and C;
* hardware acceleration;
* side data and extended data.

Contributions can be made via pull requests and will be considered by the author on their merits. Enhancement requests and bug reports should be raised as github issues. For support, please contact [Streampunk Media](http://www.streampunk.media/).

## License

This project is licensed under the GNU General Public License, version 3 or later. Copyright (C) 2019, Streampunk Media Ltd.

This software links to libraries from the FFmpeg project, including optional parts and optimizations covered by the GPL v2.0 or later. Your attention is drawn to the FFmpeg project's page [FFmpeg License and Legal Considerations](https://www.ffmpeg.org/legal.html).

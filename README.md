# Beam Coder

[Node.JS](https://nodejs.org/) native bindings to [FFmpeg](https://www.ffmpeg.org/) with support for asynchronous processing via streams and promises.

The aim of this module is to facilitate relatively easy access to the capabilities of FFmpeg - including media muxing, demuxing, encoding, decoding and filtering - from Node.JS applications. Rather than using the filesystem and controlling the FFmpeg as an external command line process, the beam coder executes functions of the FFmpeg _libav*_ libraries directly. Work is configured by Javascript objects and executes over data buffers that are shared between Javascript and C. Long running media processing operations are asynchronous, running as promises that execute native code separately from the main event loop. The developers created beam coder to enable development of highly-scalable frame-by-frame, packet-by-packet media nanoservices built out the web-platform, combining media IO functions with the comprehensive library of other scalable IO modules for Node, such as [express](https://expressjs.com/), [koa](https://koajs.com/), [ioredis](https://www.npmjs.com/package/ioredis) etc..

If you are looking to write your own frame-by-frame transcoder, media mangler or muxer, you are in the right place. However, if you want to control FFmpeg as a command line application over complete files or piped streams from a Node.JS application, many other projects are available, such as [fluent-ffmpeg](https://www.npmjs.com/package/fluent-ffmpeg).

Beam coder will be a cross-platform module for Windows, Mac and Linux. In this early release, only Windows is supported. Other platforms will follow shortly.

Beam coder is the first release of Streampunk Media's _Aerostat_ product set, whereby a fleet of media-oriented _aerostats_ (_blimps_, _air ships_, _zeppelins_ etc.) are launched into the clouds. Media content is beamed between the fleet as if beams of light and beamed to and from locations on the planet surface as required. See also the [_Aerostat Beam Engine_](https://www.npmjs.com/package/beamengine).

## Installation

### Pre-requisites

### Installing

## Usage

### Simple example

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

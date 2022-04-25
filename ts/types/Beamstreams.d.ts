import { Demuxer, DemuxerCreateOptions } from "./Demuxer"
import { Muxer, MuxerCreateOptions } from "./Muxer"
import { InputFormat } from "./FormatContext"
import { Stream } from "./Stream"
import { Filterer } from "./Filter"
import { Decoder } from "./Decoder"
import { Encoder } from "./Encoder"
import { Readable, Writable } from "stream"


/**
 * OLD Typing
 * 
 * A [Node.js Writable stream](https://nodejs.org/docs/latest-v12.x/api/stream.html#stream_writable_streams)
 * allowing source data to be streamed to the demuxer from a file or other stream source such as a network connection
 */
// export interface WritableDemuxerStream extends NodeJS.WritableStream {
// // export interface WritableDemuxerStream implements WritableStream extends NodeJS.Writable {
// 	/**
// 	 * Create a demuxer for this source
// 	 * @param options a DemuxerCreateOptions object
//      * @returns a promise that resolves to a Demuxer when it has determined sufficient
// 	 * format details by consuming data from the source. The promise will wait indefinitely 
// 	 * until sufficient source data has been read.
// 	 */
// 	demuxer(options?: DemuxerCreateOptions | string): Promise<Demuxer>
// }



/**
 * WritableDemuxerStream is not a Writable Class augmented by a demuxer function, should be replace by a new class
 * A [Node.js Writable stream](https://nodejs.org/docs/latest-v12.x/api/stream.html#stream_writable_streams)
 * allowing source data to be streamed to the demuxer from a file or other stream source such as a network connection
 */
 export type WritableDemuxerStream = Writable & {
	/**
	 * Create a demuxer for this source
	 * @param options a DemuxerCreateOptions object
     * @returns a promise that resolves to a Demuxer when it has determined sufficient
	 * format details by consuming data from the source. The promise will wait indefinitely 
	 * until sufficient source data has been read.
	 */
	 demuxer: (options?: { iformat?: InputFormat, options?: { [key: string]: any }, governor?: Governor }) => Promise<Demuxer>
};

/**
 * OLD TYPING
 * 
 * A [Node.js Readable stream](https://nodejs.org/docs/latest-v12.x/api/stream.html#stream_readable_streams)
 * allowing data to be streamed from the muxer to a file or other stream destination such as a network connection
 */
// export interface ReadableMuxerStream extends NodeJS.ReadableStream {
// 	/**
// 	 * Create a muxer for this source
// 	 * @param options a MuxerCreateOptions object
// 	 * @returns A Muxer object
// 	 */
// 	muxer(options: MuxerCreateOptions): Muxer
// }


/**
 * A [Node.js Readable stream](https://nodejs.org/docs/latest-v12.x/api/stream.html#stream_readable_streams)
 * allowing data to be streamed from the muxer to a file or other stream destination such as a network connection
 */
export type ReadableMuxerStream = Readable & {
	/**
	 * Create a demuxer for this source
	 * @param options a DemuxerCreateOptions object
     * @returns a promise that resolves to a Demuxer when it has determined sufficient
	 * format details by consuming data from the source. The promise will wait indefinitely 
	 * until sufficient source data has been read.
	 */
	 muxer: (options?: MuxerCreateOptions & {governor?: Governor }) => Muxer
};




/** Create object for AVIOContext based buffered I/O */
// export function governor(options: { highWaterMark: number }): {
//   read(len: number): Promise<Buffer>
//   write(data: Buffer): Promise<null>
//   finish(): undefined
// }


/** Source definition for a beamstream channel, from either a file or NodeJS ReadableStream */
export interface BeamstreamSource {
	url?: string
	input_stream?: NodeJS.ReadableStream
	ms?: { start: number, end: number }
	streamIndex?: number
	iformat?: InputFormat
	options?: { [key: string]: any }
	format?: Demuxer | Promise<Demuxer>;
	stream?: any; // FIXME
	decoder?: Decoder; // FIXME
}

/** Codec definition for the destination channel */
export interface BeamstreamStream {
	name: string
	time_base: Array<number>
	codecpar: { [key: string]: any }
	// added later
	encoder?: Encoder;
	stream?: Stream;
}
/** Definition for a channel of beamstream processing */
export interface BeamstreamChannel {
	sources: Array<BeamstreamSource>
	filterSpec: string
	streams: Array<BeamstreamStream>
	filter?: Filterer | Promise<Filterer>;
}
/**
 * Definition for a beamstream process consisting of a number of audio and video sources
 * that are to be processed and multiplexed into an output file or stream
 */
export interface BeamstreamParams {
	video: Array<BeamstreamChannel>
	audio: Array<BeamstreamChannel>
	/** Destination definition for the beamstream process, to either a file or NodeJS WritableStream */
	out: {
		formatName: string
		url?: string
		output_stream?: NodeJS.WritableStream
		flags?: {
			READ?: boolean
			WRITE?: boolean
			NONBLOCK?: boolean
			DIRECT?: boolean
		}
		options?: { [key:string]: any }
	}
}

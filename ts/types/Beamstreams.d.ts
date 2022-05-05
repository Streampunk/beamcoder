import { Demuxer, DemuxerCreateOptions } from "./Demuxer"
import { Muxer, MuxerCreateOptions } from "./Muxer"
import { InputFormat } from "./FormatContext"

/** Source definition for a beamstream channel, from either a file or NodeJS ReadableStream */
export interface BeamstreamSource {
	url?: string
	input_stream?: NodeJS.ReadableStream
	ms?: { start: number, end: number }
	streamIndex?: number
	iformat?: InputFormat
    options?: { [key: string]: any }

	format?: Demuxer; // filled by makeSources
	stream?: Readable; // filled by makeSources
	decoder?: Decoder; // FIXME
}
/** Codec definition for the destination channel */
export interface BeamstreamStream {
	name: string
	time_base: Array<number>
	codecpar: { [key: string]: any }

	encoder?: Encoder; 	// filled by runStreams
	stream?: Stream;	// filled by runStreams

}
/** Definition for a channel of beamstream processing */
export interface BeamstreamChannel {
	sources: Array<BeamstreamSource>
	filterSpec: string
	streams: Array<BeamstreamStream>
	filter?: Filterer; // filled by makeStreams
}
/**
 * Definition for a beamstream process consisting of a number of audio and video sources
 * that are to be processed and multiplexed into an output file or stream
 */
export interface BeamstreamParams {
	video?: Array<BeamstreamChannel>
	audio?: Array<BeamstreamChannel>
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


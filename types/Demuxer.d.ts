import { Packet } from "./Packet"
import { InputFormat, FormatContext } from "./FormatContext"

export interface SeekOptions {
	/**
	 * The stream where to seek
	 * Use in conjunction with property frame or timestamp
	 */
	stream_index?: number
	/**
	 * Seek by the number of frames into a given stream
	 * Use in conjunction with stream_index
	 */
	frame?: number
	/**
	 * Seek forward to a keyframe in a given stream or file at a given timestamp
	 * The timestamp is the presentation timestamp of the packet measured in the timebase of the stream
	 * Use in conjunction with stream_index
	 */
	timestamp?: number
	/**
	 * seek based on elapsed time from the beginning of the primary stream
	 * (as determined by FFmpeg, normally the first video stream where available)
	 */
	time?: number
	/**
	 * byte offset position into the file
	 */
	pos?: number
	/**
	 * The backward Boolean-valued property is interpreted as:
	 *  true:  find the nearest key frame before the timestamp
	 *  false: find the nearest keyframe after the timestamp
	 */
	backward?: boolean
	/**
	 * The any Boolean-valued property enables seeking to both key and non-key frames
	 */
	any?: boolean
}

/**
 * The process of demuxing (de-multiplexing) extracts time-labelled packets of data 
 * contained in a media stream or file.
 */
export interface Demuxer extends Omit<FormatContext,
	'oformat' | 'max_interleave_delta' | 'avoid_negative_ts' | 'audio_preload' |
  'max_chunk_duration' | 'max_chunk_size' | 'flush_packets' | 'metadata_header_padding'
> {
	/** Object name. */
	readonly type: 'demuxer'
	readonly iformat: InputFormat
	readonly url: string
	readonly duration: number

	/**
	 * Beam coder offers FFmpeg's many options for seeking a particular frame in a file,
	 * either by time reference, frame count or file position.
	 * https://github.com/Streampunk/beamcoder#seeking
   * @param options an object that specifies details on how the seek is to be calculated.
   * @returns a promise that resolves when the seek has completed
	 */
	seek(options: SeekOptions): Promise<null>
	/**
   * Read the next blob of data from the file or stream at the current position,
	 * where that data could be from any of the streams.
	 * Typically, a packet is one frame of video data or a data blob representing
	 * a codec-dependent number of audio samples.
	 * Use the stream_index property of returned packet to find out which stream it is 
	 * associated with and dimensions including height, width or audio sample rate.
	 * https://github.com/Streampunk/beamcoder#reading-data-packets
   * @returns a promise that resolves to a Packet when the read has completed
	 */
	read(): Promise<Packet>
	/**
	 * Abandon the demuxing process and forcibly close the file or stream without waiting for it to finish
	 */
	forceClose(): undefined
}

/**
 * Provides a list and details of all the available demuxer input formats
 * @returns an object with details of all the available demuxer input formats
 */
export function demuxers(): { [key: string]: InputFormat }

/**
 * Create a demuxer to read from a URL or filename
 * @param url a string describing the source to be read from (may contain %d for a sequence of numbered files).
 * @returns a promise that resolves to a Demuxer when it has determined sufficient 
 * format details by consuming data from the source. The promise will wait indefinitely 
 * until sufficient source data has been read.
 */
export function demuxer(url: string): Promise<Demuxer>

/** Object to provide additional metadata on Demuxer creation */
export interface DemuxerCreateOptions {
	/** String describing the source to be read from (may contain %d for a sequence of numbered files). */
	url?: string
	/** Object that provides format details */
	iformat?: InputFormat
	/** Object allowing additional information to be provided */
	options?: { [key: string]: any }
}
/**
 * For formats that require additional metadata, such as the rawvideo format,
 * it may be necessary to pass additional information such as image size or pixel format to Demuxer creation.
 * @param options a DemuxerCreateOptions object
 * @returns a promise that resolves to a Demuxer when it has determined sufficient 
 * format details by consuming data from the source. The promise will wait indefinitely 
 * until sufficient source data has been read.
 */
export function demuxer(options: DemuxerCreateOptions): Promise<Demuxer>

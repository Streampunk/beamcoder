import { Packet } from "./Packet"
import { Frame } from "./Frame"
import { OutputFormat, FormatContext } from "./FormatContext"

export interface Muxer extends Omit<FormatContext,
	'iformat' | 'start_time' | 'probesize' | 'max_analyze_duration' | 'max_index_size' |
	'fps_probe_size' | 'error_recognition' | 'max_ts_probe' | 'use_wallclock_as_timestamps' |
	'avio_flags' | 'duration_estimation_method' | 'skip_initial_bytes' | 'correct_ts_overflow' |
	'seek2any' | 'probe_score' | 'format_probesize' | 'codec_whitelist' | 'format_whitelist' |
  'io_repositioned' | 'output_ts_offset' | 'protocol_whitelist' | 'protocol_blacklist' |
  'max_streams' | 'skip_estimate_duration_from_pts'
> {
	/** Object name. */
	type: 'muxer'

	/**
	 * Open the output file or stream - requires that a filename or URL has been provided with the
	 * creation of the muxer using the filename property
	 * @returns Promise that resolves to _undefined_ on success
	 */
	openIO(): Promise<undefined>
	/**
	 * Open the output file or stream by passing in an object containing the filename or url.
   * @param openOptions An object containing the filename or url. The object can also contain an options
	 * object to further configure the protocol with private data and a flags parameter to configure bytestream AVIO flags.
	 * @returns Promise that resolves to _undefined_ on success or to an object with an unset property detailing
	 * which of the properties could not be set. 
	 */
	openIO(openOptions: {
		url?: string
		filename?: string
		options?: { [key: string]: any }
		flags?: {
			READ: boolean
			WRITE: boolean
			NONBLOCK: boolean
			DIRECT: boolean
		}
	}): Promise<undefined | { unset: {[key: string]: any}}>

	/**
	 * In some cases, it is necessary to initialize the structures of the muxer before writing the header.
	 * Allows passing in of private data to set private options of the muxer.
	 * @returns Promise that resolves to an object that indicates whether the stream parameters were
	 * intialised in writeHeader or initOutput, together with an unset property if any properties could not be set
	 */
	initOutput(options?: { [key:string]: any }) : Promise<{
		INIT_IN: 'WRITE_HEADER' | 'INIT_OUTPUT'
		unset?: {[key: string]: any} 
	}>
	/**
	 * Write the header to the file, optionally passing in private data to set private options of the muxer.
	 * This must be done even for formats that don't have a header as part of the internal structure
	 * as this step also initializes the internal data structures for writing.
	 * @returns Promise that resolves to an object that indicates whether the stream parameters were
	 * intialised in writeHeader or initOutput, together with an unset property if any properties could not be set
	 */
	writeHeader(options?: { [key:string]: any }) : Promise<{
		INIT_IN: 'WRITE_HEADER' | 'INIT_OUTPUT'
		unset?: {[key: string]: any} 
	}>

  /**
	 * Write media data to the file by sending a packet containing data for a media stream.
	 * @param packet Packet of compressed data, must contain the stream index and timestamps measured in the
	 * `time_base` of the stream.
	 * @returns Promise that resolves to _undefined_ on success
	 */
	writeFrame(packet: Packet) : Promise<undefined>
  /**
	 * Write media data to the file by sending a packet containing data for a media stream.
	 * @param options Object containing a packet property of a compressed data Packet, must contain the
	 * stream index and timestamps measured in the `time_base` of the stream.
	 * @returns Promise that resolves to _undefined_ on success
	 */
	writeFrame(options: { packet: Packet }) : Promise<undefined>
  /**
	 * Write media data to the file by sending a packet containing data for a media stream.
	 * @param options Object containing a stream index property and a frame property of an
	 * uncompressed Frame, which must contain the timestamps measured in the `time_base` of the stream.
	 * @returns Promise that resolves to _undefined_ on success
	 */
	writeFrame(options: { frame: Frame, stream_index: number }) : Promise<undefined>

  /**
	 * Write the trailer at the end of the file or stream. It is written after the muxer has drained its
	 * buffers of all remaining packets and frames. Writing the trailer also closes the file or stream.
	 * @returns Promise that resolves to _undefined_ on success
	 */
	writeTrailer(): Promise<undefined>

	/**
	 * Abandon the muxing process and forcibly close the file or stream without completing it
	 */
	forceClose(): undefined
}

/**
 * Provides a list and details of all the available muxer output formats
 * @returns an object with details of all the available muxer output formats
 */
export function muxers(): { [key: string]: OutputFormat }

/** Object to provide additional metadata on Muxer creation */
export interface MuxerCreateOptions {
	/** The name of a chosen OutputFormat */
  name?: string
	format_name?: string
	/** String describing the destinatione to be written to (may contain %d for a sequence of numbered files). */
	filename?: string
	/** Object that provides format details */
	oformat?: OutputFormat
	/** Object allowing additional information to be provided */
	[key: string]: any
}
/**
 * Create a muxer to write to a URL or filename
 * @param options a MuxerCreateOptions object
 * @returns A Muxer object
 */
export function muxer(options: MuxerCreateOptions): Muxer

import { CodecPar } from "./CodecPar"
import { Packet } from "./Packet"
import { Frame } from "./Frame"
import { Codec } from "./Codec"
import { CodecContext, CodecContext_base } from "./CodecContext"
import { Demuxer } from "./Demuxer"

/** The DecodedFrames object is returned as the result of a decode operation */
export interface DecodedFrames {
	/** Object name. */
	readonly type: 'frames'
	/** 
	   * Decoded frames that are now available. If the array is empty, the decoder has buffered
	 * the packet as part of the process of producing future frames
	   */
	readonly frames: Array<Frame>
	/** Total time in microseconds that the decode operation took to complete */
	readonly total_time: number
}

export interface Decoder extends CodecContext_base {
	// readonly type: 'decoder'
	// readonly time_base: [number, number]
	// readonly sample_aspect_ratio: [number, number]
	// readonly intra_matrix: Array<number> | null
	// readonly inter_matrix: Array<number> | null
	// readonly intra_dc_precision: number
	// readonly refs: number
	// readonly color_primaries?: string
	// readonly color_trc: string
	// readonly colorspace: string
	// readonly color_range: string
	// readonly chroma_sample_location: 'unspecified' | 'left' | 'center' | 'topleft' | 'top' | 'bottomleft' | 'bottom'
	// readonly field_order: 'progressive' |
	// 'top coded first, top displayed first' |
	// 'bottom coded first, bottom displayed first' |
	// 'top coded first, bottom displayed first' |
	// 'bottom coded first, top displayed first' |
	// 'unknown'
	// readonly sample_fmt: string | null
	// readonly audio_service_type: 'main' | 'effects' | 'visually-impaired' | 'hearing-impaired' | 'dialogue' |
	// 'commentary' | 'emergency' | 'voice-over' | 'karaoke' | 'nb'
	// readonly bits_per_raw_sample: number
	// readonly profile: string | number
	// readonly level: number
	// readonly subtitle_header: Buffer | null
	// readonly framerate: [number, number]

	/**
	 * Decode an encoded data packet or array of packets and create an uncompressed frame
	 * or frames (may be a frames-worth of audio).
	 * Decoders may need more than one packet to produce a frame and may subsequently
	 * produce more than one frame per packet. This is particularly the case for long-GOP formats.
	 * @param packet A packet or an array of packets to be decoded
	 * @returns a promise that resolves to a DecodedFrames object when the decode has completed successfully
	 */
	decode(packet: Packet | Packet[]): Promise<DecodedFrames>
	/**
	 * Decode a number of packets passed as separate parameters and create uncompressed frames
	 * (may be a frames-worth of audio).
	 * Decoders may need more than one packet to produce a frame and may subsequently
	 * produce more than one frame per packet. This is particularly the case for long-GOP formats.
	 * @param packets An arbitrary number of packets to be decoded
     * @returns a promise that resolves to a DecodedFrames object when the decode has completed successfully
	 */
	decode(...packets: Packet[]): Promise<DecodedFrames>
	/**
	 * Once all packets have been passed to the decoder, it is necessary to call its
	 * asynchronous flush() method. If any frames are yet to be delivered by the decoder
	 * they will be provided in the resolved value.
	 * 
	 * Call the flush operation once and do not use the decoder for further decoding once it has
	 * been flushed. The resources held by the decoder will be cleaned up as part of the Javascript
	 * garbage collection process, so make sure that the reference to the decoder goes out of scope.
	 * @returns a promise that resolves to a DecodedFrames object when the flush has completed successfully
	 */
	flush(): Promise<DecodedFrames>
	/**
	 * Extract the CodecPar object for the Decoder
	 * @returns A CodecPar object
	 */
	extractParams(): any
	/** 
	 * Initialise the decoder with parameters from a CodecPar object
	 * @param param The CodecPar object that is to be used to override the current Decoder parameters
	 * @returns the modified Decoder object
     */
	useParams(params: CodecPar): Decoder
}

/**
 * Provides a list and details of all the available decoders
 * @returns an object with name and details of each of the available decoders
 */
export function decoders(): { [key: string]: Codec }
/** 
 * Create a decoder by name
 * @param name The codec name required
 * @param ... Any non-readonly parameters from the Decoder object as required
 * @returns A Decoder object - note creation is synchronous
 */
export function decoder(options: { name: string, [key: string]: any }): Decoder
/**
 * Create a decoder by codec_id
 * @param codec_id The codec ID from AV_CODEC_ID_xxx
 * @param ... Any non-readonly parameters from the Decoder object as required
 * @returns A Decoder object - note creation is synchronous
 */
export function decoder(options: { codec_id: number, [key: string]: any }): Decoder
/**
 * Create a decoder from a demuxer and a stream_index
 * @param demuxer An initialised Demuxer object
 * @param stream_index The stream number of the demuxer object to be used to initialise the decoder
 * @param ... Any non-readonly parameters from the Decoder object as required
 * @returns A Decoder object - note creation is synchronous
 */
export function decoder(options: { demuxer: Demuxer, stream_index: number, [key: string]: any }): Decoder
/**
 * Create a decoder from a CodecPar object
 * @param params CodecPar object whose codec name or id will be used to initialise the decoder
 * @param ... Any non-readonly parameters from the Decoder object as required
 * @returns A Decoder object - note creation is synchronous
 */
export function decoder(options: { params: CodecPar, [key: string]: any }): Decoder

import { CodecPar } from "./CodecPar"
import { Packet } from "./Packet";
import { Frame } from "./Frame";
import { Codec } from "./Codec"
import { CodecContext } from "./CodecContext"

/** The EncodedPackets object is returned as the result of a encode operation */
export interface EncodedPackets {
	/** Object name. */
	readonly type: 'packets'
  /** 
	 * Encoded packets that are now available. If the array is empty, the encoder has buffered
   * the frame as part of the process of producing future packets
	 */
	readonly packets: Array<Packet>
	/** Total time in microseconds that the encode operation took to complete */
	readonly total_time: number
}
/**
 * Encoder takes a stream of uncompressed data in the form of Frames and converts them into coded Packets.
 * Encoding takes place on a single type of stream, for example audio or video.
 */
export interface Encoder extends Omit<CodecContext,
	'coded_width' |	'coded_height' | 'slice_flags' | 'skip_top' | 'skip_bottom' |
	'request_channel_layout' | 'request_sample_fmt' | 'error_concealment' | 'err_recognition' |
	'reordered_opaque' | 'skip_loop_filter' | 'skip_idct' | 'skip_frame' | 'sw_pix_fmt' |
	'pkt_timebase' | 'codec_descriptor' | 'sub_charenc' | 'sub_charenc_mode' | 'skip_alpha' |
	'codec_whitelist' | 'properties' | 'sub_text_format' | 'hwaccel_flags' | 'apply_cropping' |	'extra_hw_frames'
> {
	readonly type: 'encoder'
	readonly extradata: Buffer | null
	readonly slice_count: number
	readonly slice_offset: Array<number> | null
	readonly bits_per_coded_sample: number

	/**
	 * Encode a Frame or array of Frames and create a compressed Packet or Packets.
	 * Encoders may need more than one Frame to produce a Packet and may subsequently
	 * produce more than one Packet per Frame. This is particularly the case for long-GOP formats.
	 * @param frame A Frame or an array of Frames to be encoded
   * @returns a promise that resolves to a EncodedPackets object when the encode has completed successfully
	 */
  encode(frame: Frame | Frame[]): Promise<EncodedPackets>
	/**
	 * Encode a number of Frames passed as separate parameters and create compressed Packets
	 * Encoders may need more than one Frame to produce a Packet and may subsequently
	 * produce more than one Packet per Frame. This is particularly the case for long-GOP formats.
	 * @param frames An arbitrary number of Frames to be encoded
   * @returns a promise that resolves to a EncodedPackets object when the encode has completed successfully
	 */
	encode(...frames: Frame[]): Promise<EncodedPackets>
  /**
	 * Once all Frames have been passed to the encoder, it is necessary to call its
	 * asynchronous flush() method. If any Packets are yet to be delivered by the encoder
	 * they will be provided in the resolved value.
	 * 
	 * Call the flush operation once and do not use the encoder for further encoding once it has
	 * been flushed. The resources held by the encoder will be cleaned up as part of the Javascript
	 * garbage collection process, so make sure that the reference to the encoder goes out of scope.
   * @returns a promise that resolves to a EncodedPackets object when the flush has completed successfully
	 */
	flush(): Promise<EncodedPackets>
	/**
	 * Extract the CodecPar object for the Encoder
	 * @returns A CodecPar object
	 */
	extractParams(): any
	/** 
	 * Initialise the encoder with parameters from a CodecPar object
	 * @param param The CodecPar object that is to be used to override the current Encoder parameters
	 * @returns the modified Encoder object
   */
	useParams(params: CodecPar): Encoder
}

/**
 * Provides a list and details of all the available encoders
 * @returns an object with name and details of each of the available encoders
 */
export function encoders(): { [key: string]: Codec }
/** 
 * Create an encoder by name
 * @param name The codec name required
 * @param ... Any non-readonly parameters from the Encoder object as required
 * @returns An Encoder object - note creation is synchronous
 */
export function encoder(options: { name: string, [key: string]: any }): Encoder
/**
 * Create an encoder by codec_id
 * @param codec_id The codec ID from AV_CODEC_ID_xxx
 * @param ... Any non-readonly parameters from the Encoder object as required
 * @returns An Encoder object - note creation is synchronous
 */
export function encoder(options: { codec_id: number, [key: string]: any }): Encoder

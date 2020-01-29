/**
 * This object stores compressed data. It is typically exported by demuxers
 * and then passed as input to decoders, or received as output from encoders and
 * then passed to muxers.
 *
 * For video, it should typically contain one compressed frame. For audio it may
 * contain several compressed frames. Encoders are allowed to output empty
 * packets, with no compressed data, containing only side data
 * (e.g. to update some stream parameters at the end of encoding).
 */
export interface Packet {
	/** Object name. */
	readonly type: 'Packet'
	/**
	 * Presentation timestamp in AVStream->time_base units the time at which
	 * the decompressed packet will be presented to the user.
	 * Can be AV_NOPTS_VALUE if it is not stored in the file.
	 * pts MUST be larger or equal to dts as presentation cannot happen before
	 * decompression, unless one wants to view hex dumps. Some formats misuse
	 * the terms dts and pts/cts to mean something different. Such timestamps
	 * must be converted to true pts/dts before they are stored in Packet.
	 */
	pts: number
	/**
	 * Decompression timestamp in AVStream->time_base units the time at which
	 * the packet is decompressed.
	 * Can be AV_NOPTS_VALUE if it is not stored in the file.
	 */
	dts: number
	/**
	 * The raw data of the packet
	 * Packet data buffers are shared between C and Javascript so can be written to and modified without having to write the buffer back into the packet
	 */
	data: Buffer
  /** The size in bytes of the raw data */
	size: number
	/** The index in the format's stream array that this packet belongs to */
	stream_index: number
  /** A combination of AV_PKT_FLAG values */
	flags: {
		/** The packet contains a keyframe */
		KEY: boolean
		/** The packet content is corrupted */
		CORRUPT: boolean
		/**
		 * Flag is used to discard packets which are required to maintain valid
		 * decoder state but are not required for output and should be dropped
		 * after decoding.
		 **/
    DISCARD: boolean
		/**
		 * The packet comes from a trusted source.
		 *
		 * Otherwise-unsafe constructs such as arbitrary pointers to data
		 * outside the packet may be followed.
		 */
		TRUSTED: boolean
		/**
		 * Flag is used to indicate packets that contain frames that can
		 * be discarded by the decoder.  I.e. Non-reference frames.
		 */
		DISPOSABLE: boolean // Frames that can be discarded by the decoder
	}
	/**
	 * Additional packet data that can be provided by the container.
	 * Packet can contain several types of side information.
	 */
	side_data: { type: string, [key: string]: Buffer | string } | null
	/**
	 * Duration of this packet in AVStream->time_base units, 0 if unknown.
	 * Equals next_pts - this_pts in presentation order.
	 */
	duration: number
	/** byte position in stream, -1 if unknown */
	pos: number
}

/**
 * Packets for decoding can be created without reading them from a demuxer
 * Set parameters as required from the Packet object, passing in a buffer and the required size in bytes
 */
export function packet(options: { [key: string]: any, data: Buffer, size: number }): Packet

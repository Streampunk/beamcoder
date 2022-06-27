export interface FormatContextOut {
	/** The output format description. */
	set oformat(format: string);
	// @ts-ignore
	get oformat(): OutputFormat
	/**
	 * Maximum buffering duration for interleaving.
	 *
	 * To ensure all the streams are interleaved correctly,
	 * av_interleaved_write_frame() will wait until it has at least one packet
	 * for each stream before actually writing any packets to the output file.
	 * When some streams are "sparse" (i.e. there are large gaps between
	 * successive packets), this can result in excessive buffering.
	 *
	 * This field specifies the maximum difference between the timestamps of the
	 * first and the last packet in the muxing queue, above which libavformat
	 * will output a packet regardless of whether it has queued a packet for all
	 * the streams.
	 *
	 * Muxing only, set by the caller before avformat_write_header().
	 */
	max_interleave_delta: number
	/**
	   * Avoid negative timestamps during muxing. Any value of the AVFMT_AVOID_NEG_TS_* constants.
	   * Note, this only works when using av_interleaved_write_frame. (interleave_packet_per_dts is in use)
	   */
	avoid_negative_ts: 'auto' | 'make_non_negative' | 'make_zero'
	/** Audio preload in microseconds. Note, not all formats support this and unpredictable things may happen if it is used when not supported. */
	audio_preload: number
	/** Max chunk time in microseconds. Note, not all formats support this and unpredictable things may happen if it is used when not supported. */
	max_chunk_duration: number
	/** Max chunk size in bytes Note, not all formats support this and unpredictable things may happen if it is used when not supported. */
	max_chunk_size: number
	/** Flush the I/O context after each packet. */
	flush_packets: number
	/** Number of bytes to be written as padding in a metadata header. */
	metadata_header_padding: number
	// not exposing opaque
	/** Output timestamp offset, in microseconds. */
	// may need to be remove ??
	output_ts_offset: number
}

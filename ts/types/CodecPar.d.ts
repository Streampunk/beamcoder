/**
 * CodecPar describes the properties of an encoded stream.
 */
export interface CodecPar extends toJSONAble {
	/** Object name. */
	readonly type: 'CodecParameters'

	/** General type of the encoded data. */
	codec_type: string | 'data' | 'video'

	/** Specific type of the encoded data (the codec used). */
	codec_id: number

	/** The name corresponding to the codec_id. */
	name: 'node' | 'h264' | string

	/** Additional information about the codec (corresponds to the AVI FOURCC). */
	codec_tag: number | string

	/** Extra binary data needed for initializing the decoder, codec-dependent. */
	extradata: Buffer | null

	/**
	 * - video: the pixel format.
	 * - audio: the sample format.
	 */
	format: string | null

	/** The average bitrate of the encoded data (in bits per second). */
	bit_rate: number

	/**
	 * The number of bits per sample in the codedwords.
	 *
	 * This is basically the bitrate per sample. It is mandatory for a bunch of
	 * formats to actually decode them. It's the number of bits for one sample in
	 * the actual coded bitstream.
	 *
	 * This could be for example 4 for ADPCM
	 * For PCM formats this matches bits_per_raw_sample
	 * Can be 0
	 */
	bits_per_coded_sample: number

	/**
	 * This is the number of valid bits in each output sample. If the
	 * sample format has more bits, the least significant bits are additional
	 * padding bits, which are always 0. Use right shifts to reduce the sample
	 * to its actual size. For example, audio formats with 24 bit samples will
	 * have bits_per_raw_sample set to 24, and format set to AV_SAMPLE_FMT_S32.
	 * To get the original sample use "(int32_t)sample >> 8"."
	 *
	 * For ADPCM this might be 12 or 16 or similar
	 * Can be 0
	 */
	bits_per_raw_sample: number

	/** Codec-specific bitstream restrictions that the stream conforms to. */
	profile: string | number

	level: number

	/** Video only. The video frame width in pixels. */
	width: number

	/** Video only. The video frame height in pixels. */
	height: number

	/**
	 * Video only. The aspect ratio (width / height) which a single pixel
	 * should have when displayed.
	 *
	 * When the aspect ratio is unknown / undefined, the numerator should be
	 * set to 0 (the denominator may have any value).
	 */
	sample_aspect_ratio: [number, number]

	/** Video only. The order of the fields in interlaced video. */
	field_order: string | 'unknown' | 'progressive'

	/** Video only. Additional colorspace characteristics. */
	color_range: string | 'unknown' | 'pc'

	/** Video only. Additional colorspace characteristics. */
	color_primaries: string | 'unknown' | 'bt709'

	/** Video only. Additional colorspace characteristics. */
	color_trc: string | 'unknown' | 'bt709'

	/** Video only. Additional colorspace characteristics. */
	color_space: string | 'unknown' | 'bt709'

	/** Video only. Additional colorspace characteristics. */
	chroma_location: string | 'unspecified' | 'left'

	/** Video only. Number of delayed frames. */
	video_delay: number

	/**
	 * Audio only. A description of the channel layout.
	 * ex: "0 channels"
	 */
	channel_layout: string | '0 channels'


	/** Audio only. The number of audio channels. */
	channels: number

	/** Audio only. The number of audio samples per second. */
	sample_rate: number

	/**
	 * Audio only. The number of bytes per coded audio frame, required by some
	 * formats.
	 *
	 * Corresponds to nBlockAlign in WAVEFORMATEX.
	 */
	block_align: number

	/** Audio only. Audio frame size, if known. Required by some formats to be static. */
	frame_size: number

	/**
	 * Audio only. The amount of padding (in samples) inserted by the encoder at
	 * the beginning of the audio. I.e. this number of leading decoded samples
	 * must be discarded by the caller to get the original audio without leading
	 * padding.
	 */
	initial_padding: number

	trailing_padding: number

	/** Audio only. Number of samples to skip after a discontinuity. */
	seek_preroll: number

	// native code;
	readonly _codecPar: {};
}


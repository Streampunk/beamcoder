import { HWDeviceContext, HWFramesContext } from "./HWContext"

export type MotionEstimationString = 'sad' | 'sse' | 'satd' | 'dct' | 'psnr' | 'bit' | 'rd' | 'zero' | 'vsad' |
                                     'vsse' | 'nsse' | 'w53' | 'w97' | 'dctmax' | 'dct264' | 'median_sad' | 'chroma'
export type FrameSkipString = 'none' | 'default' | 'nonref' | 'bidir' | 'nonintra' | 'nonkey' | 'all'

/** The CodecContext object */
export interface CodecContext {
	/** Object name. */
	readonly type: string
  /** see AV_CODEC_ID_xxx */
	readonly codec_id: number
	/**
	 * Name of the codec implementation.
   * The name is globally unique among encoders and among decoders (but an
   * encoder and a decoder can share the same name).
   * This is the primary way to find a codec from the user perspective.
	 */
	readonly name: string
  /** Descriptive name for the codec, meant to be more human readable than name. */
	readonly long_name: string
	/**
	 * A fourcc string by default, will be a number if not recognised
	 *  - LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
	 */
	readonly codec_tag: string | number
	/** Codec private data. */
	priv_data: { [key: string]: any } | null
  /** The average bitrate */
	bit_rate: number
  /**
	 * Number of bits the bitstream is allowed to diverge from the reference.
   * The reference can be CBR (for CBR pass1) or VBR (for pass2)
   */
	bit_rate_tolerance: number
  /** Global quality for codecs which cannot change it per frame. This should be proportional to MPEG-1/2/4 qscale. */
	global_quality: number
	compression_level: number
	/** AV_CODEC_FLAG_*. */
	flags: { [key: string]: boolean }
	/** AV_CODEC_FLAG2_*. */
	flags2: { [key: string]: boolean }
	/**
	 * some codecs need / can use extradata like Huffman tables.
	 * MJPEG: Huffman tables
	 * rv10: additional flags
	 * MPEG-4: global headers (they can be in the bitstream or here)
	 * The allocated memory should be AV_INPUT_BUFFER_PADDING_SIZE bytes larger
	 * than extradata_size to avoid problems if it is read with the bitstream reader.
	 * The bytewise contents of extradata must not depend on the architecture or CPU endianness.
   */
	extradata: Buffer | null
	/**
	 * This is the fundamental unit of time (in seconds) in terms
	 * of which frame timestamps are represented. For fixed-fps content,
	 * timebase should be 1/framerate and timestamp increments should be
	 * identically 1.
	 * This often, but not always is the inverse of the frame rate or field rate
	 * for video. 1/time_base is not the average frame rate if the frame rate is not
	 * constant.
	 *
	 * Like containers, elementary streams also can store timestamps, 1/time_base
	 * is the unit in which these timestamps are specified.
	 * As example of such codec time base see ISO/IEC 14496-2:2001(E)
	 * vop_time_increment_resolution and fixed_vop_rate
	 * (fixed_vop_rate == 0 implies that it is different from the framerate)
   */
	time_base: Array<number>
	/**
	 * For some codecs, the time base is closer to the field rate than the frame rate.
	 * Most notably, H.264 and MPEG-2 specify time_base as half of frame duration
	 * if no telecine is used ...
	 *
	 * Set to time_base ticks per frame. Default 1, e.g., H.264/MPEG-2 set it to 2.
	 */
	ticks_per_frame: number
	/**
	 * Number of frames delay in addition to what a standard decoder
	 * as specified in the spec would produce.
	 *
	 * Video:
	 *   Number of frames the decoded output will be delayed relative to the
	 *   encoded input.
	 *
	 * Audio:
	 *   Number of samples the decoder needs to output before the decoder's output is valid.
	 *   When seeking, you should start decoding this many samples prior to your desired seek point.
	 */
	readonly delay: number
	/**
	 * picture width / height.
	 *
	 * @note Those fields may not match the values of the last
	 * Frame output due to frame reordering.
	 *
	 * May be set by the user before opening the decoder if known e.g.
	 * from the container. Some decoders will require the dimensions
	 * to be set by the caller. During decoding, the decoder may
	 * overwrite those values as required while parsing the data.
	 */
	width: number
	height: number
	/**
	 * Bitstream width / height, may be different from width/height e.g. when
	 * the decoded frame is cropped before being output or lowres is enabled.
	 *
	 * @note Those field may not match the value of the last
	 * Frame output due to frame reordering.
	 *
	 * May be set by the user before opening the decoder if known
	 * e.g. from the container. During decoding, the decoder may
	 * overwrite those values as required while parsing the data.
	 */
	coded_width: any
	coded_height: any
	/**
	 * Pixel format, see AV_PIX_FMT_xxx.
	 * May be set by the demuxer if known from headers.
	 * May be overridden by the decoder if it knows better.
	 *
	 * @note This field may not match the value of the last
	 * Frame output due to frame reordering.
	 */
	pix_fmt: string | null
	/**
	 * Maximum number of B-frames between non-B-frames
   * Note: The output will be delayed by max_b_frames+1 relative to the input.
	 */
	max_b_frames: number
  /** qscale factor between IP and B-frames
   * If > 0 then the last P-frame quantizer will be used (q= lastp_q*factor+offset).
   * If < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset).
	 */
	b_quant_factor: number
  /** qscale offset between IP and B-frames */
	b_quant_offset: number
	/**
	 * Size of the frame reordering buffer in the decoder.
	 * For MPEG-2 it is 1 IPB or 0 low delay IP.
   */
	readonly has_b_frames: number
  /** qscale factor between P- and I-frames
   * If > 0 then the last P-frame quantizer will be used (q = lastp_q * factor + offset).
   * If < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset).
	 */
	i_quant_factor: number
  /** qscale offset between P and I-frames */
	i_quant_offset: number
  /** luminance masking (0-> disabled) */
  lumi_masking: number
  /** temporal complexity masking (0-> disabled) */
  temporal_cplx_masking: number
  /** spatial complexity masking (0-> disabled) */
  spatial_cplx_masking: number
  /** p block masking (0-> disabled) */
  p_masking: number
  /** darkness masking (0-> disabled) */
	dark_masking: number
	/** slice count */
  slice_count: number
	/** slice offsets in the frame in bytes */
	slice_offset: Array<number> | null
	/**
	 * sample aspect ratio (0/1 if unknown)
	 * That is the width of a pixel divided by the height of the pixel.
	 * Numerator and denominator must be relatively prime and smaller than 256 for some video standards.
   */
	sample_aspect_ratio: Array<number>
  /** motion estimation comparison function */
	me_cmp: MotionEstimationString
  /** subpixel motion estimation comparison function */
	me_sub_cmp: MotionEstimationString
  /** macroblock comparison function (not supported yet) */
  mb_cmp: MotionEstimationString
  /** interlaced DCT comparison function */
  ildct_cmp: MotionEstimationString
  /** ME diamond size & shape */
  dia_size: number
  /** amount of previous MV predictors (2a+1 x 2a+1 square) */
  last_predictor_count: number
  /** motion estimation prepass comparison function */
  me_pre_cmp: MotionEstimationString
  /** ME prepass diamond size & shape */
  pre_dia_size: number
  /** subpel ME quality */
  me_subpel_quality: number
  /** maximum motion estimation search range in subpel units. If 0 then no limit. */
  me_range: number
	slice_flags: {
		/** draw_horiz_band() is called in coded order instead of display */
		CODED_ORDER: boolean
		/** allow draw_horiz_band() with field slices (MPEG-2 field pics)  */
    ALLOW_FIELD: boolean
		/** allow draw_horiz_band() with 1 component at a time (SVQ1) */
		ALLOW_PLANE: boolean
	}
	/**
	 * macroblock decision mode
	 * simple: uses mb_cmp
	 * bits: chooses the one which needs the fewest bits
	 * rd: rate distortion
	 */
	mb_decision: 'simple' | 'bits' | 'rd'
	/** custom intra quantization matrix */
	intra_matrix: Array<number> | null
	/** custom inter quantization matrix */
	inter_matrix: Array<number> | null
  /** precision of the intra DC coefficient - 8 */
	intra_dc_precision: number
  /** Number of macroblock rows at the top which are skipped. */
	skip_top: number
  /** Number of macroblock rows at the bottom which are skipped. */
	skip_bottom: number
  /** minimum MB Lagrange multiplier */
	mb_lmin: number
  /** maximum MB Lagrange multiplier */
	mb_lmax: number
  /**  */
  bidir_refine: number
  /** minimum GOP size */
  keyint_min: number
	/** number of reference frames */
	refs: number
	/** Value depends upon the compare function used for fullpel ME. */
	mv0_threshold: number
	/** Chromaticity coordinates of the source primaries. */
	color_primaries?: string
	/** Color Transfer Characteristic. */
	color_trc: string
	/** YUV colorspace type. */
	colorspace: string
	/** MPEG vs JPEG YUV range. */
	color_range: string
	/**
	 * Location of chroma samples.
	 *
	 * Illustration showing the location of the first (top left) chroma sample of the
	 * image, the left shows only luma, the right
	 * shows the location of the chroma sample, the 2 could be imagined to overlay
	 * each other but are drawn separately due to limitations of ASCII
	 *``` 
	 *                1st 2nd       1st 2nd horizontal luma sample positions
	 *                 v   v         v   v
	 *                 ______        ______
	 *1st luma line > |X   X ...    |3 4 X ...     X are luma samples,
   *.               |             |1 2           1-6 are possible chroma positions
	 *2nd luma line > |X   X ...    |5 6 X ...     0 is undefined/unknown position
	 *```
	 */
	chroma_sample_location: 'unspecified' | 'left' | 'center' | 'topleft' | 'top' | 'bottomleft' | 'bottom'
	/** Number of slices. Indicates number of picture subdivisions. Used for parallelized decoding. */
	slices: number
	field_order: 'progressive' |
	             'top coded first, top displayed first' |
	             'bottom coded first, bottom displayed first' |
               'top coded first, bottom displayed first' |
               'bottom coded first, top displayed first' |
               'unknown'
  /** Audio only - samples per second */
	sample_rate: number
  /** Audio only - number of audio channels */
	channels: number
  /** audio sample format */
	sample_fmt: string | null
	/**
	 * Number of samples per channel in an audio frame.
	 * May be set by some decoders to indicate constant frame size
	 */
	readonly frame_size: number
	/**
	 * Frame counter - total number of frames returned from the decoder so far.
	 * @note the counter is not incremented if encoding/decoding resulted in an error.
	 */
	readonly frame_number: number
	/** number of bytes per packet if constant and known or 0. Used by some WAV based audio codecs. */
	block_align: number
  /** Audio cutoff bandwidth (0 means "automatic") */
	cutoff: number
  /** Audio channel layout. */
	channel_layout: string
  /** Request decoder to use this channel layout if it can (0 for default) */
	request_channel_layout: string
	/** Type of service that the audio stream conveys. */
	audio_service_type: 'main' | 'effects' | 'visually-impaired' | 'hearing-impaired' | 'dialogue' |
                      'commentary' | 'emergency' | 'voice-over' | 'karaoke' | 'nb'
	/** Desired sample format - decoder will decode to this format if it can. */
	request_sample_fmt: string | null
  /** amount of qscale change between easy & hard scenes (0.0-1.0) */
	qcompress: number
	/** amount of qscale smoothing over time (0.0-1.0) */
  qblur: number
	/** minimum quantizer */
  qmin: number
	/** maximum quantizer */
  qmax: number
	/** maximum quantizer difference between frames */
  max_qdiff: number
	/** decoder bitstream buffer size */
  rc_buffer_size: number
	/** ratecontrol override */
  rc_override: Array<{
    type: 'RcOverride'
    start_frame: number
    end_frame: number
    /** If this is 0 then quality_factor will be used instead. */
		qscale: number
    quality_factor: number
	}>
	/** maximum bitrate */
	rc_max_rate: number
  /** minimum bitrate */
	rc_min_rate: number
	/** Ratecontrol attempt to use, at maximum, <value> of what can be used without an underflow. */
  rc_max_available_vbv_use: number
	/** Ratecontrol attempt to use, at least, <value> times the amount needed to prevent a vbv overflow. */
  rc_min_vbv_overflow_use: number
	/** Number of bits which should be loaded into the rc buffer before decoding starts. */
  rc_initial_buffer_occupancy: number
	/** trellis RD quantization */
  trellis: number
	/** pass1 encoding statistics output buffer */
  readonly stats_out: string | null
	/** pass2 encoding statistics input buffer. Concatenated stuff from stats_out of pass1 should be placed here. */
  stats_in: string | null
  /** Work around bugs in codecs which sometimes cannot be detected automatically. */
	workaround_bugs: { [key: string]: boolean }
	/**
	 * strictly follow the standard (MPEG-4, ...).
	 * Setting this to STRICT or higher means the encoder and decoder will
	 * generally do stupid things, whereas setting it to unofficial or lower
	 * will mean the encoder might produce output that is not supported by all
	 * spec-compliant decoders. Decoders don't differentiate between normal,
	 * unofficial and experimental (that is, they always try to decode things
	 * when they can) unless they are explicitly asked to behave stupidly
	 * (=strictly conform to the specs)
	 */
	strict_std_compliance: 'very-strict' | 'strict' | 'normal' | 'unofficial' | 'experimental'
  /** Error concealment flags */
	error_concealment: {
		GUESS_MVS?: boolean
		DEBLOCK?: boolean
		FAVOR_INTER?: boolean
	}
	debug: { [key: string]: boolean }
  /** Error recognition - may misdetect some more or less valid parts as errors. */
	err_recognition: { [key: string]: boolean }
  /** Opaque 64-bit number (generally a PTS) that will be reordered and output in Frame.reordered_opaque */
	reordered_opaque: number
	readonly error: ReadonlyArray<number> | null
  /** DCT algorithm */
	dct_algo: 'auto' | 'fastint' | 'int' | 'mmx' | 'altivec' | 'faan'
	/** IDCT algorithm */
	idct_algo: 'auto' | 'int' | 'simple' | 'simplemmx' | 'arm' | 'altivec' | 'simplearm' | 'xvid' | 'simplearmv5te' | 'simplearmv6' | 'faan' | 'simpleneon' | 'none' | 'simpleauto'
  /** Bits per sample/pixel from the demuxer (needed for huffyuv). */
	bits_per_coded_sample: number
  /** Bits per sample/pixel of internal libavcodec pixel/sample format. */
	bits_per_raw_sample: number
  /** Thread count is used to decide how many independent tasks should be passed to execute() */
	thread_count: number
  /**
	 * Which multithreading methods to use.
	 * Use of FRAME will increase decoding delay by one frame per thread,
	 * so clients which cannot provide future frames should not use it.
   */
	thread_type: { FRAME?: boolean, SLICE?: boolean }
  /** Which multithreading methods are in use by the codec. */
	readonly active_thread_type: { FRAME?: boolean, SLICE?: boolean }
	/**
	 * Set by the client if its custom get_buffer() callback can be called
	 * synchronously from another thread, which allows faster multithreaded decoding.
	 * draw_horiz_band() will be called from other threads regardless of this setting.
	 * Ignored if the default get_buffer() is used.
   */
	thread_safe_callbacks: number
  /** nsse_weight */
	nsse_weight: number

	profile: string | number
	level: number
  /** Skip loop filtering for selected frames. */
	skip_loop_filter: FrameSkipString
  /** Skip IDCT/dequantization for selected frames. */
	skip_idct: FrameSkipString
  /** Skip decoding for selected frames. */
	skip_frame: FrameSkipString
	/**
	 * Header containing style information for text subtitles.
	 * For SUBTITLE_ASS subtitle type, it should contain the whole ASS
	 * [Script Info] and [V4+ Styles] section, plus the [Events] line and
	 * the Format line following. It shouldn't include any Dialogue line.
	 */
	subtitle_header: Buffer | null
	/**
	 * Audio only. The number of "priming" samples (padding) inserted by the
	 * encoder at the beginning of the audio. I.e. this number of leading
	 * decoded samples must be discarded by the caller to get the original audio
	 * without leading padding.
	 *
	 * The timestamps on the output packets are adjusted by the encoder so that 
	 * they always refer to the first sample of the data actually contained in the packet,
	 * including any added padding.  E.g. if the timebase is 1/samplerate and
	 * the timestamp of the first input sample is 0, the timestamp of the 
	 * first output packet will be -initial_padding.
	 */
	readonly inital_padding: number
  /**
   * For codecs that store a framerate value in the compressed
   * bitstream, the decoder may export it here. [ 0, 1 ] when unknown.
	 */
	framerate: Array<number>
  /** Nominal unaccelerated pixel format, see AV_PIX_FMT_xxx. */
	readonly sw_pix_fmt: string | null
  /** Timebase in which pkt_dts/pts and Packet dts/pts are. */
	pkt_timebase: Array<number>

	readonly codec_descriptor: {
		INTRA_ONLY: boolean
		LOSSY: boolean
		LOSSLESS: boolean
		REORDER: boolean
		BITMAP_SUB: boolean
		TEXT_SUB: boolean
	} | null
  /** Character encoding of the input subtitles file. */
	sub_charenc: string | null
	/**
	 * Subtitles character encoding mode. Formats or codecs might be adjusting
	 * this setting (if they are doing the conversion themselves for instance).
   */
	readonly sub_charenc_mode: 'do-nothing' | 'automatic' | 'pre-decoder' | 'ignore'
	/**
	 * Skip processing alpha if supported by codec.
	 * Note that if the format uses pre-multiplied alpha (common with VP6,
	 * and recommended due to better video quality/compression)
	 * the image will look as if alpha-blended onto a black background.
	 * However for formats that do not use pre-multiplied alpha
	 * there might be serious artefacts (though e.g. libswscale currently
	 * assumes pre-multiplied alpha anyway).
   */
	skip_alpha: number
	/** Number of samples to skip after a discontinuity */
	readonly seek_preroll: number
  /** custom intra quantization matrix */
	chroma_intra_matrix: Array<number>
	/** Dump format separator - can be ", " or "\n      " or anything else */
	dump_separator: string | null
  /** ',' separated list of allowed decoders - if null then all are allowed */
	codec_whitelist: string | null
  /** Properties of the stream that gets decoded */
	readonly properties: { LOSSLESS: boolean, CLOSED_CAPTIONS: boolean }
  /** Additional data associated with the entire coded stream. */
	readonly coded_side_data: {
		type: 'PacketSideData'
		[key: string]: Buffer | string
	}
	/**
	 * A reference to the AVHWFramesContext describing the input (for encoding)
	 * or output (decoding) frames. The reference is set by the caller and
	 * afterwards owned (and freed) by libavcodec - it should never be read by
	 * the caller after being set.
	 *
	 * - decoding: This field should be set by the caller from the get_format()
	 *             callback. The previous reference (if any) will always be
	 *             unreffed by libavcodec before the get_format() call.
	 *
	 *             If the default get_buffer2() is used with a hwaccel pixel
	 *             format, then this AVHWFramesContext will be used for
	 *             allocating the frame buffers.
	 *
	 * - encoding: For hardware encoders configured to use a hwaccel pixel
	 *             format, this field should be set by the caller to a reference
	 *             to the AVHWFramesContext describing input frames.
	 *             AVHWFramesContext.format must be equal to
	 *             AVCodecContext.pix_fmt.
	 *
	 *             This field should be set before avcodec_open2() is called.
	 */
	hw_frames_ctx: HWFramesContext
	/** Control the form of AVSubtitle.rects[N]->ass */
	sub_text_format: number
	/**
	 * Audio only. The amount of padding (in samples) appended by the encoder to
	 * the end of the audio. I.e. this number of decoded samples must be
	 * discarded by the caller from the end of the stream to get the original
	 * audio without any trailing padding.
   */
	trailing_padding: number
  /** The number of pixels per image to maximally accept. */
	max_pixels: number
	/**
	 * A reference to the HWDeviceContext describing the device which will
	 * be used by a hardware encoder/decoder.  The reference is set by the
	 * caller and afterwards owned (and freed) by libavcodec.
	 *
	 * This should be used if either the codec device does not require
	 * hardware frames or any that are used are to be allocated internally by
	 * libavcodec.  If the user wishes to supply any of the frames used as
	 * encoder input or decoder output then hw_frames_ctx should be used
	 * instead.  When hw_frames_ctx is set in get_format() for a decoder, this
	 * field will be ignored while decoding the associated stream segment, but
	 * may again be used on a following one after another get_format() call.
	 *
	 * For both encoders and decoders this field should be set before
	 * avcodec_open2() is called and must not be written to thereafter.
	 *
	 * Note that some decoders may require this field to be set initially in
	 * order to support hw_frames_ctx at all - in that case, all frames
	 * contexts used must be created on the same device.
	 */
	hw_device_ctx: HWDeviceContext
	/**
	 * Bit set of AV_HWACCEL_FLAG_* flags, which affect hardware accelerated
	 * decoding (if active).
   */
	hwaccel_flags: { IGNORE_LEVEL?: boolean, ALLOW_HIGH_DEPTH?: boolean, ALLOW_PROFILE_MISMATCH?: boolean }
	/**
	 * Video decoding only. Certain video codecs support cropping, meaning that
	 * only a sub-rectangle of the decoded frame is intended for display.  This
	 * option controls how cropping is handled by libavcodec.
	 *
	 * When set to 1 (the default), libavcodec will apply cropping internally.
	 * I.e. it will modify the output frame width/height fields and offset the
	 * data pointers (only by as much as possible while preserving alignment, or
	 * by the full amount if the AV_CODEC_FLAG_UNALIGNED flag is set) so that
	 * the frames output by the decoder refer only to the cropped area. The
	 * crop_* fields of the output frames will be zero.
	 *
	 * When set to 0, the width/height fields of the output frames will be set
	 * to the coded dimensions and the crop_* fields will describe the cropping
	 * rectangle. Applying the cropping is left to the caller.
	 *
	 * @warning When hardware acceleration with opaque output frames is used,
	 * libavcodec is unable to apply cropping from the top/left border.
	 *
	 * @note when this option is set to zero, the width/height fields of the
	 * AVCodecContext and output AVFrames have different meanings. The codec
	 * context fields store display dimensions (with the coded dimensions in
	 * coded_width/height), while the frame fields store the coded dimensions
	 * (with the display dimensions being determined by the crop_* fields).
	 */
	apply_cropping: number
	/*
		* Video decoding only.  Sets the number of extra hardware frames which
		* the decoder will allocate for use by the caller.  This must be set
		* before avcodec_open2() is called.
		*
		* Some hardware decoders require all frames that they will use for
		* output to be defined in advance before decoding starts.  For such
		* decoders, the hardware frame pool must therefore be of a fixed size.
		* The extra frames set here are on top of any number that the decoder
		* needs internally in order to operate normally (for example, frames
		* used as reference pictures).
		*/
	extra_hw_frames: number
}

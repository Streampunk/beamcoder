import { HWFramesContext } from "./HWContext";

/**
 * This object describes decoded (raw) audio or video data.
 */
export interface Frame {
	/** Object name. */
	readonly type: 'Frame'
	/**
	 * For video, size in bytes of each picture line.
	 * For audio, size in bytes of each plane.
	 *
	 * For audio, only linesize[0] may be set. For planar audio, each channel
	 * plane must be the same size.
	 *
	 * For video the linesizes should be multiples of the CPUs alignment
	 * preference, this is 16 or 32 for modern desktop CPUs.
	 * Some code requires such alignment other code can be slower without
	 * correct alignment, for yet other it makes no difference.
	 *
	 * @note The linesize may be larger than the size of usable data -- there
	 * may be extra padding present for performance reasons.
	 */
	linesize: Array<number>
	/**
	 * Video dimensions
	 * Video frames only. The coded dimensions (in pixels) of the video frame,
	 * i.e. the size of the rectangle that contains some well-defined values.
	 *
	 * @note The part of the frame intended for display/presentation is further
	 * restricted by the "Cropping rectangle".
	 */
	width: number
	height: number
	/** number of audio samples (per channel) described by this frame */
  nb_samples: number
	/** format of the frame, null if unknown or unset */
	format: string | null
	/** Whether this frame is a keyframe */
	key_frame: boolean
  /** Picture type of the frame. */
	pict_type: 'I' | 'P' | 'B' | 'S' | 'SI' | 'SP' | 'BI' | null
  /** Sample aspect ratio for the video frame, 0/1 if unknown/unspecified. */
	sample_aspect_ratio: Array<number>
  /** Presentation timestamp in time_base units (time when frame should be shown to user). */
	pts: number
	/**
	 * DTS copied from the Packet that triggered returning this frame. (if frame threading isn't used)
	 * This is also the Presentation time of this Frame calculated from
	 * only Packet.dts values without pts values.
	 */
	pkt_dts: number
	/** picture number in bitstream order */
	coded_picture_number: number
	/** picture number in display order */
	display_picture_number: number
	/** quality (between 1 (good) and FF_LAMBDA_MAX (bad)) */
	quality: number
	/**
	 * When decoding, this signals how much the picture must be delayed.
	 * extra_delay = repeat_pict / (2*fps)
	 */
	repeat_pict: number
	/** The content of the picture is interlaced. */
	interlaced_frame: boolean
	/** If the content is interlaced, is top field displayed first. */
	top_field_first: boolean
  /** Tell user application that palette has changed from previous frame. */
	palette_has_changed: boolean
	/**
	 * reordered opaque 64 bits (generally an integer or a double precision float
	 * PTS but can be anything).
	 * The user sets AVCodecContext.reordered_opaque to represent the input at
	 * that time,
	 * the decoder reorders values as needed and sets AVFrame.reordered_opaque
	 * to exactly one of the values provided by the user through AVCodecContext.reordered_opaque
	 * @deprecated in favor of pkt_pts
	 */
	reordered_opaque: number | null
  /** Sample rate of the audio data. */
	sample_rate: number
  /** Channel layout of the audio data. */
	channel_layout: string
	/**
	 * Raw data for the picture/channel planes.
	 *
	 * Some decoders access areas outside 0,0 - width,height, please
	 * see avcodec_align_dimensions2(). Some filters and swscale can read
	 * up to 16 bytes beyond the planes, if these filters are to be used,
	 * then 16 extra bytes must be allocated.
	 */
	data: Array<Buffer>
	/**
	 * Additional data that can be provided by the container.
	 * Frame can contain several types of side information.
	 */
	side_data: { type: string, [key: string]: Buffer | string } | null
  /** Frame flags */
	flags: {
		CORRUPT?: boolean
		DISCARD?: boolean
	}
	/** MPEG vs JPEG YUV range. */
	color_range: string
	/** Chromaticity coordinates of the source primaries. */
	color_primaries?: string
	/** Color Transfer Characteristic. */
	color_trc: string
	/** YUV colorspace type. */
	colorspace: string
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
	chroma_location: 'unspecified' | 'left' | 'center' | 'topleft' | 'top' | 'bottomleft' | 'bottom'
  /** frame timestamp estimated using various heuristics, in stream time base */
	best_effort_timestamp: number
  /** reordered pos from the last AVPacket that has been input into the decoder */
	pkt_pos: number
  /** duration of the corresponding packet, expressed in Stream->time_base units, 0 if unknown. */
	pkt_duration: number
	metadata: { [key: string]: string }
	/**
	 * decode error flags of the frame, set if the decoder produced a frame, but there
	 * were errors during the decoding.
   */
	decode_error_flags: {
		INVALID_BITSTREAM: boolean
		MISSING_REFERENCE: boolean
	}
  /** number of audio channels, only used for audio. */
	channels: number
  /**
   * size of the corresponding packet containing the compressed frame.
   * It is set to a negative value if unknown.
	 */
	pkt_size: number
  /**
	 * For hwaccel-format frames, this should be a reference to the
	 * HWFramesContext describing the frame.
	 */
	hw_frames_ctx: HWFramesContext
	/**
	* Video frames only. The number of pixels to discard from the the
	* top/bottom/left/right border of the frame to obtain the sub-rectangle of
	* the frame intended for presentation.
  */
	crop_top: number
	crop_bottom: number
	crop_left: number
	crop_right: number

	/**
	 * Beam coder exposes some of FFmpeg's ability to calculate the size of data buffers.
	 * If you pass width, height and format properties for video frames, or channels/channel_layout,
	 * sample_rate and format for audio frames, as options to the frame constructor then the linesize
	 * array (number of bytes per line per plane) is computed. For video, multiply each value by the
	 * height to get the minimum buffer size for the plane. For audio, the first element of the array
	 * is the buffer size for each plane.
	 *
	 * To use the linesize numbers to automatically allocate buffers of the correct size,
	 * call alloc() after the factory method. For example:
	 * `let f = beamcoder.frame({ width: 1920, height: 1080, format: 'yuv422p' }).alloc()`
	 */
	alloc(): Frame
}

/**
 * Create a frame for encoding or filtering
 * Set parameters as required from the Frame object
 */
export function frame(options: { [key: string]: any, data?: Array<Buffer> }): Frame

/** Pixel format description */
export interface PixelFormat {
	name: string
	/** The number of components each pixel has, (1-4) */
	nb_components: number
	/**
	 * Amount to shift the luma height right to find the chroma height.
	 * For YV12 this is 1 for example.
	 * chroma_height= AV_CEIL_RSHIFT(luma_height, log2_chroma_h)
	 * The note above is needed to ensure rounding up.
	 * This value only refers to the chroma components.
	 */
	log2_chroma_h: number
	/**
	 * Amount to shift the luma width right to find the chroma width.
	 * For YV12 this is 1 for example.
	 * chroma_width = AV_CEIL_RSHIFT(luma_width, log2_chroma_w)
	 * The note above is needed to ensure rounding up.
	 * This value only refers to the chroma components.
	 */
	log2_chroma_w: number
	flags: {
    /** Pixel format is big-endian. */
		BE: boolean
		/** Pixel format has a palette in data[1], values are indexes in this palette. */
		PAL: boolean
    /** All values of a component are bit-wise packed end to end. */
		BITSTREAM: boolean
    /** Pixel format is an HW accelerated format. */
		HWACCEL: boolean
    /** At least one pixel component is not in the first data plane. */
		PLANAR: boolean
    /** The pixel format contains RGB-like data (as opposed to YUV/grayscale). */
		RGB: boolean
    /**
		 * The pixel format is "pseudo-paletted". This means that it contains a
  	 * fixed palette in the 2nd plane but the palette is fixed/constant for each
		 * PIX_FMT. This allows interpreting the data as if it was PAL8, which can
		 * in some cases be simpler. Or the data can be interpreted purely based on
		 * the pixel format without using the palette.
		 * An example of a pseudo-paletted format is AV_PIX_FMT_GRAY8
		 * @deprecated This flag is deprecated, and will be removed.
		 */
		PSEUDOPAL: boolean
    /**
		 * The pixel format has an alpha channel. This is set on all formats that
  	 * support alpha in some way, including AV_PIX_FMT_PAL8. The alpha is always
  	 * straight, never pre-multiplied.

  	 * If a codec or a filter does not support alpha, it should set all alpha to
  	 * opaque, or use the equivalent pixel formats without alpha component, e.g.
  	 * AV_PIX_FMT_RGB0 (or AV_PIX_FMT_RGB24 etc.) instead of AV_PIX_FMT_RGBA.
		 */
		ALPHA: boolean
    /** The pixel format is following a Bayer pattern. */
		BAYER: boolean
    /**
		 * The pixel format contains IEEE-754 floating point values. Precision (double,
     * single, or half) should be determined by the pixel size (64, 32, or 16 bits).
     */
		FLOAT: boolean
	}
	comp: Array< {
    /** Code letter for the contents of the component */
		code: 'R' | 'G' | 'B' | 'Y' | 'U' | 'V' | 'A'
		/** Which of the 4 planes contains the component. */
		plane: number
		/**
		 * Number of elements between 2 horizontally consecutive pixels.
     * Elements are bits for bitstream formats, bytes otherwise.
		 */
		step: number
		/**
		 * Number of elements before the component of the first pixel.
     * Elements are bits for bitstream formats, bytes otherwise.
		 */
		offset: number
		/** Number of least significant bits that must be shifted away to get the value. */
		shift: number
		/** Number of bits in the component. */
		depth: number
	}>
	/** Alternative comma-separated names. */
	alias: string
}
/** Format details for all supported pixel format names */
export function pix_fmts(): { [key: string]: PixelFormat }

/** Audio sample formats */
export interface SampleFormat {
	type: 'SampleFormat'
	name: string
  /** The packed alternative form of the sample format. */
	packed: string
	/** The planar alternative form of the sample format. */
	planar: string
	/** Number of bytes per sample or zero if unknown. */
	bytes_per_sample: number
	/** Whether the sample format is planar. */
	is_planar: boolean
}
/** Format details for all supported sample format names */
export function sample_fmts(): { [key: string]: SampleFormat }

/**
 * Note that when creating buffers from Javascript,
 * FFmpeg recommends that a small amount of headroom is added to the minimum length of each buffer.
 * The minimum amount of padding is exposed to Javascript as constant
 */
export const AV_INPUT_BUFFER_PADDING_SIZE: number

import { PrivClass } from "./PrivClass"

export interface Codec {
	/** Object name. */
	readonly type: 'Codec'
	/**
	 * Name of the codec implementation.
   * The name is globally unique among encoders and among decoders (but an
   * encoder and a decoder can share the same name).
   * This is the primary way to find a codec from the user perspective.
	 */
	readonly name: string
  /** Descriptive name for the codec, meant to be more human readable than name. */
	readonly long_name: string
  /** String describing the media type */
	readonly codec_type: 'unknown' | 'video' | 'audio' | 'data' | 'subtitle' | 'attachment' | 'nb'
  /** Number that identifies the syntax and semantics of the bitstream. */
	readonly id: number
  /** true if codec is an decoder */
	readonly decoder: boolean
  /** true if codec is an encoder */
	readonly encoder: boolean
  /** Codec capabilities - see AV_CODEC_CAP_* */
	readonly capabilities: {
		/** Decoder can use draw_horiz_band callback. */
		DRAW_HORIZ_BAND: boolean
		/** Codec uses get_buffer() for allocating buffers and supports custom allocators. */
		DR1: boolean
		TRUNCATED: boolean
		/**
		 * Decoder requires flushing with NULL input at the end in order to
     * give the complete and correct output.

		 * NOTE: If this flag is not set, the codec is guaranteed to never be fed with
		 * with NULL data. The user can still send NULL data to the decode function,
		 * but it will not be passed along to the codec unless this flag is set.
		 * 
		 * The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
		 * avpkt->size=0 at the end to get the delayed data until the decoder no longer
		 * returns frames.
		 */
		DELAY: boolean
    /** Codec can be fed a final frame with a smaller size. This can be used to prevent truncation of the last audio samples. */
		SMALL_LAST_FRAME: boolean
		/**
		 * Codec can output multiple frames per APacket
		 * Normally demuxers return one frame at a time, demuxers which do not do
		 * are connected to a parser to split what they return into proper frames.
		 * This flag is reserved to the very rare category of codecs which have a
		 * bitstream that cannot be split into frames without timeconsuming
		 * operations like full decoding. Demuxers carrying such bitstreams thus
		 * may return multiple frames in a packet. This has many disadvantages like
		 * prohibiting stream copy in many cases thus it should only be considered
		 * as a last resort.
 		 */
		SUBFRAMES: boolean
    /** Codec is experimental and is thus avoided in favor of non experimental codecs */
		EXPERIMENTAL: boolean
    /** Codec should fill in channel configuration and samplerate instead of container */
		CHANNEL_CONF: boolean
		/** Codec supports frame-level multithreading. */
		FRAME_THREADS: boolean
		/** Codec supports slice-based (or partition-based) multithreading. */
		SLICE_THREADS: boolean
		/** Codec supports changed parameters at any point. */
		PARAM_CHANGE: boolean
		/** Codec supports avctx->thread_count == 0 (auto). */
		AUTO_THREADS: boolean
		/** Audio encoder supports receiving a different number of samples in each call. */
		VARIABLE_FRAME_SIZE: boolean
    /**
		 * Decoder is not a preferred choice for probing.
		 * This indicates that the decoder is not a good choice for probing.
		 * It could for example be an expensive to spin up hardware decoder,
		 * or it could simply not provide a lot of useful information about
		 * the stream.
		 * A decoder marked with this flag should only be used as last resort
		 * choice for probing.
		 */
		AVOID_PROBING: boolean
    /** Codec is intra only. */
		INTRA_ONLY: boolean
		/** Codec is lossless. */
		LOSSLESS: boolean
		/** Codec is backed by a hardware implementation. Typically used to identify a non-hwaccel hardware decoder. */
		HARDWARE: boolean
		/**
		 * Codec is potentially backed by a hardware implementation, but not necessarily.
		 * This is used instead of the HARDWARE flag if the implementation provides some sort of internal fallback.
		 */
		HYBRID: boolean
	}
  /** Array of supported framerates (as a rational [num, den]), or null if unknown. */
	readonly supported_framerates: ReadonlyArray<ReadonlyArray<number>> | null
  /** Array of supported pixel formats, or null if unknown. */
	readonly pix_fmts: ReadonlyArray<string> | null
  /** Array of supported audio samplerates, or null if unknown */
	readonly supported_samplerates: ReadonlyArray<number> | null
  /** Array of supported sample formats, or NULL if unknown, */
	readonly sample_fmts: ReadonlyArray<string>
  /** */
	readonly channel_layouts: ReadonlyArray<string>
  /** */
	readonly max_lowres: number
	/** Class for private context */
	readonly priv_class: PrivClass
  /** */
	readonly profiles: ReadonlyArray<string> | null
  /** */
	readonly wrapper_name?: string
  /** */
	readonly descriptor: {
	  INTRA_ONLY: boolean
	  LOSSY: boolean
  	LOSSLESS: boolean
  	REORDER: boolean
  	BITMAP_SUB: boolean
		TEXT_SUB: boolean
	}
}

/** List the available codecs */
export function codecs(): { [key: string]: { encoder?: Codec, decoder?: Codec }}

import { Stream } from "./Stream"
import { PrivClass } from "./PrivClass"

/**
 * Describes a supported input format
 */
export interface InputFormat {
	/** Object name. */
	readonly type: 'InputFormat'
	/** A comma separated list of short names for the format. */
	readonly name: string
	/** Descriptive name for the format, meant to be more human-readable */
	readonly long_name: string
  readonly flags: {
		NOFILE: boolean
    /** Needs '%d' in filename. */
		NEEDNUMBER: boolean
    /** Show format stream IDs numbers. */
		SHOW_IDS: boolean
    /** Use generic index building code. */
    GENERIC_INDEX: boolean
		/** Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps */
		TS_DISCONT: boolean
    /** Format does not allow to fall back on binary search via read_timestamp */
		NOBINSEARCH: boolean
    /** Format does not allow to fall back on generic search */
		NOGENSEARCH: boolean
    /** Format does not allow seeking by bytes */
		NO_BYTE_SEEK: boolean
    /** Seeking is based on PTS */
		SEEK_TO_PTS: boolean
	}
	/**
	 * If extensions are defined, then no probe is done. You should
	 * usually not use extension format guessing because it is not
	 * reliable enough
	 */
	readonly extensions: string
  /**
   * List of supported codec_id-codec_tag pairs, ordered by "better
   * choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
   */
  readonly codec_tag: ReadonlyArray<{
		id: number
		tag: string | number
	}>
	/** Class for private context */
	readonly priv_class: PrivClass | null
	/**
	 * Comma-separated list of mime types.
	 * It is used check for matching mime types while probing.
	 */
	readonly mime_type: string
  /** Raw demuxers store their codec ID here. */
  readonly raw_codec_id: string
	/** Size of private data so that it can be allocated. */
	readonly priv_data_size: number
}

/**
 * Describes a supported input format
 */
export interface OutputFormat {
	/** Object name. */
	readonly type: 'OutputFormat'
	/** A comma separated list of short names for the format. */
	name: string
	/** Descriptive name for the format, meant to be more human-readable */
	long_name: string
	/**
	 * Comma-separated list of mime types.
	 * It is used check for matching mime types while probing.
	 */
	mime_type: string
  /** comma-separated filename extensions */
	extensions: string
  /** default audio codec */
	audio_codec: string
  /** default video codec */
	video_codec: string
  /** default subtitle codec */
  subtitle_codec: string
	flags: {
		NOFILE?: boolean
    /** Needs '%d' in filename. */
		NEEDNUMBER?: boolean
    /** Format wants global header. */
		GLOBALHEADER?: boolean
		/** Format does not need / have any timestamps. */
		NOTIMESTAMPS?: boolean
		/** Format allows variable fps. */
		VARIABLE_FPS?: boolean
		/** Format does not need width/height */
		NODIMENSIONS?: boolean
		/** Format does not require any streams */
		NOSTREAMS?: boolean
		/** Format allows flushing. If not set, the muxer will not receive a NULL packet in the write_packet function. */
		ALLOW_FLUSH?: boolean
		/** Format does not require strictly increasing timestamps, but they must still be monotonic */
		TS_NONSTRICT?: boolean
    /**
		 * Format allows muxing negative timestamps. If not set the timestamp will be shifted in av_write_frame and
     * av_interleaved_write_frame so they start from 0.
		 * The user or muxer can override this through AVFormatContext.avoid_negative_ts
     */		
		TS_NEGATIVE?: boolean
	}
  /**
   * List of supported codec_id-codec_tag pairs, ordered by "better
   * choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
   */
  codec_tag: Array<{
		id: number
		tag: string | number
	}>
	/** Class for private context */
	priv_class: PrivClass | null
  /** size of private data so that it can be allocated */
  priv_data_size: number
}

/**
 * Return the output format in the list of registered output formats which best matches the provided name,
 * or return null if there is no match.
 */
export function guessFormat(name: string): OutputFormat | null;

/**
* Format I/O context.
 */
export interface FormatContext {
	/** Object name. */
	readonly type: string
	/** The input format description. */
	iformat: InputFormat
	/** The output format description. */
	oformat: OutputFormat
	/** Format private data. */
	priv_data: {
		[key: string]: any
	}
	/** Flags signalling stream properties. */
	readonly ctx_flags: {
		NOHEADER: boolean
		UNSEEKABLE: boolean
	}
	/** An array of all streams in the file. */
	streams: Array<Stream>
	/** input or output URL. Unlike the old filename field, this field has no length restriction. */
	url: string
	/**
	 * Position of the first frame of the component, in
	 * AV_TIME_BASE fractional seconds. NEVER set this value directly:
	 * It is deduced from the AVStream values. Demuxing only
	 */
	readonly start_time: number
	/**
	 * Duration of the stream, in AV_TIME_BASE fractional
	 * seconds. Only set this value if you know none of the individual stream
	 * durations and also do not set any of them. This is deduced from the
	 * Stream values if not set.
	 */
	duration: number
	/**
	 * Total stream bitrate in bit/s, 0 if not
	 * available. Never set it directly if the file_size and the
	 * duration are known as FFmpeg can compute it automatically.
	 */
	bit_rate: number

	packet_size: number
	max_delay: number
	/** Flags modifying the demuxer behaviour. A combination of AVFMT_FLAG_*. */
	flags: {
		/** Generate missing pts even if it requires parsing future frames. */
		GENPTS?: boolean
    /** Ignore index. */
		IGNIDX?: boolean
		/** Do not block when reading packets from input. */
		NONBLOCK?: boolean
		/** Ignore DTS on frames that contain both DTS & PTS */
		IGNDTS?: boolean
		/** Do not infer any values from other values, just return what is stored in the container */
		NOFILLIN?: boolean
		/** Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled */
		NOPARSE?: boolean
		/** Do not buffer frames when possible */
		NOBUFFER?: boolean
		/** The caller has supplied a custom AVIOContext, don't avio_close() it. */
		CUSTOM_IO?: boolean
		/** Discard frames marked corrupted */
		DISCARD_CORRUPT?: boolean
		/** Flush the AVIOContext every packet. */
		FLUSH_PACKETS?: boolean
		/** This flag is mainly intended for testing. */
		BITEXACT?: boolean
		/** try to interleave outputted packets by dts (using this flag can slow demuxing down) */
		SORT_DTS?: boolean
		/** Enable use of private options by delaying codec open (this could be made default once all code is converted) */
		PRIV_OPT?: boolean
		/** Enable fast, but inaccurate seeks for some formats */
		FAST_SEEK?: boolean
		/** Stop muxing when the shortest stream stops. */
		SHORTEST?: boolean
		/** Add bitstream filters as requested by the muxer */
		AUTO_BSF?: boolean
	}
	/** Maximum size of the data read from input for determining the input container format. */
	probesize: number
	/**
	 * Maximum duration (in AV_TIME_BASE units) of the data read
	 * from input in avformat_find_stream_info().
	 * Demuxing only, set by the caller before avformat_find_stream_info().
	 * Can be set to 0 to let avformat choose using a heuristic.
	 */
	max_analyze_duration: number
	readonly key: Buffer
	readonly programs: ReadonlyArray<object>
	// video_codec_id / audio_codec_id / subtitle_codec_id
	/**
	 * Maximum amount of memory in bytes to use for the index of each stream.
	 * If the index exceeds this size, entries will be discarded as
	 * needed to maintain a smaller size. This can lead to slower or less
	 * accurate seeking (depends on demuxer).
	 * Demuxers for which a full in-memory index is mandatory will ignore
	 * this.
	 */
	max_index_size: number
	/**
	 * Maximum amount of memory in bytes to use for buffering frames
	 * obtained from realtime capture devices.
	 */
	max_picture_buffer: number
	// chapters?
	/**
	 * Metadata that applies to the whole file.
	 */
	metadata: { [key: string]: string }
	/**
	 * Start time of the stream in real world time, in microseconds
	 * since the Unix epoch (00:00 1st January 1970). That is, pts=0 in the
	 * stream was captured at this real world time.
	 * AV_NOPTS_VALUE if unknown. Note that the value may become known after 
	 * some number of frames have been received.
	 */
	start_time_realtime: number | null
	/** The number of frames used for determining the framerate */
	fps_probe_size: number
	/**
	 * Error recognition - higher values will detect more errors but may
	 * misdetect some more or less valid parts as errors.
	 */
	error_recognition: number
	/** Flags to enable debugging. */
	debug: { TS: boolean }
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
	/** Allow non-standard and experimental extension */
	strict_std_compliance: 'very-strict' | 'strict' | 'normal' | 'unofficial' | 'experimental'
	/**
	 * Flags for the user to detect events happening on the file. Flags must
	 * be cleared by the user once the event has been handled.
	 * A combination of AVFMT_EVENT_FLAG_*.
	 */
	event_flags: { METADATA_UPDATED?: boolean }
	/** Maximum number of packets to read while waiting for the first timestamp. */
	max_ts_probe: number
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
	/**
	 * forces the use of wallclock timestamps as pts/dts of packets
	 * This has undefined results in the presence of B frames.
	 */
	use_wallclock_as_timestamps: boolean
	/** avio flags, used to force AVIO_FLAG_DIRECT. */
	avio_flags: {
		READ?: boolean
		WRITE?: boolean
		NONBLOCK?: boolean
		DIRECT?: boolean
	}
	/**
	 * The duration field can be estimated through various ways, and this field can be used
	 * to know how the duration was estimated.
	 */
	readonly duration_estimation_method: 'from_pts' | 'from_stream' | 'from_bitrate'
	/** Skip initial bytes when opening stream */
	skip_initial_bytes: number
	/** Correct single timestamp overflows */
	correct_ts_overflow: boolean
	/** Force seeking to any (also non key) frames. */
	seek2any: boolean
  /** Flush the I/O context after each packet. */
	flush_packets: number
	/**
	 * format probing score.
	 * The maximal score is AVPROBE_SCORE_MAX, its set when the demuxer probes
	 * the format.
	 */
	readonly probe_score: number
	/** number of bytes to read maximally to identify format. */
	format_probesize: number
	/**
	 * ',' separated list of allowed decoders.
	 * If NULL then all are allowed
	 */
	codec_whitelist: string | null
	/**
	 * ',' separated list of allowed demuxers.
	 * If NULL then all are allowed
	 */
	format_whitelist: string | null
	/**
	 * IO repositioned flag.
	 * This is set by avformat when the underlaying IO context read pointer
	 * is repositioned, for example when doing byte based seeking.
	 * Demuxers can use the flag to detect such changes.
	 */
	readonly io_repositioned: boolean
  /** Number of bytes to be written as padding in a metadata header. */
	metadata_header_padding: number
	// not exposing opaque
  /** Output timestamp offset, in microseconds. */
	output_ts_offset: number
	/**
	 * dump format separator.
	 * can be ", " or "\n      " or anything else
	 */
	dump_separator: string
	/** ',' separated list of allowed protocols. */
	protocol_whitelist: string
	/** ',' separated list of disallowed protocols. */
	protocol_blacklist: string
	/** The maximum number of streams. */
	max_streams: number
	/** Skip duration calcuation in estimate_timings_from_pts. */
	skip_estimate_duration_from_pts: boolean

  /**
	 * Add a stream to the format with the next available stream index.
	 * @param options Object including the codec name for the stream and any other parameters that need
	 * to be initialised in the Stream object
	 * @returns A Stream object
	 */
	newStream(options: { name: string, [key: string]: any	}): Stream
  /**
	 * Add a stream to the format with the next available stream index.
	 * @param stream Source stream from which to copy the parameters for the new stream
	 * @returns A Stream object
	 */
	newStream(stream: Stream): Stream
  /** Retun a JSON string containing the object properties. */
  toJSON(): string
}

export function format(options: { [key: string]: any }): FormatContext

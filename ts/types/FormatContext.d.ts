import { Stream } from "./Stream"
import { PrivClass } from "./PrivClass"
import { FormatContextIn } from "./FormatContextIn"
import { FormatContextOut } from "./FormatContextOut"

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

export interface FormatContextFlags {
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
//  extends FormatContextIn
export interface FormatContextBase {
	/** Object name. */
	readonly type: 'demuxer' | 'format' | 'muxer'

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
	flags: FormatContextFlags
	readonly key: Buffer
	readonly programs: ReadonlyArray<object>
	// video_codec_id / audio_codec_id / subtitle_codec_id

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
	/** Flags to enable debugging. */
	debug: { TS: boolean }

	/** Allow non-standard and experimental extension */
	strict_std_compliance: 'very-strict' | 'strict' | 'normal' | 'unofficial' | 'experimental'
	/**
	 * Flags for the user to detect events happening on the file. Flags must
	 * be cleared by the user once the event has been handled.
	 * A combination of AVFMT_EVENT_FLAG_*.
	 */
	event_flags: { METADATA_UPDATED?: boolean }
	/**
	 * IO repositioned flag.
	 * This is set by avformat when the underlaying IO context read pointer
	 * is repositioned, for example when doing byte based seeking.
	 * Demuxers can use the flag to detect such changes.
	 */
	readonly io_repositioned: boolean
	/**
	 * dump format separator.
	 * can be ", " or "\n      " or anything else
	 */
	dump_separator: string
	/**
	 * Add a stream to the format with the next available stream index.
	 * @param options Object including the codec name for the stream and any other parameters that need
	 * to be initialised in the Stream object
	 * @returns A Stream object
	 */
	newStream(options: string | { name: string, [key: string]: any }): Stream
	/**
	 * Add a stream to the format with the next available stream index.
	 * @param stream Source stream from which to copy the parameters for the new stream
	 * @returns A Stream object
	 */
	newStream(stream: Stream): Stream
}


/**
* Format I/O context.
 */
export interface FormatContext extends FormatContextBase, FormatContextOut, FormatContextIn {
}

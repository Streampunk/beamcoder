import { CodecPar } from "./CodecPar";
import { Packet } from "./Packet"

export interface Disposition {
	DEFAULT?: boolean
	DUB?: boolean
	ORIGINAL?: boolean
	COMMENT?: boolean
	LYRICS?: boolean
	KARAOKE?: boolean
  /**
   * Track should be used during playback by default.
   * Useful for subtitle track that should be displayed
   * even when user did not explicitly ask for subtitles.
   */
  FORCED?: boolean
  /** Stream for hearing impaired audiences */
  HEARING_IMPAIRED?: boolean
  /** Stream for visual impaired audiences */
  VISUAL_IMPAIRED?: boolean
  /** Stream without voice */
  CLEAN_EFFECTS?: boolean
  /**
   * The stream is stored in the file as an attached picture/"cover art" (e.g.
   * APIC frame in ID3v2). The first (usually only) packet associated with it
   * will be returned among the first few packets read from the file unless
   * seeking takes place. It can also be accessed at any time in
   * Stream.attached_pic.
   */
  ATTACHED_PIC?: boolean
  /**
   * The stream is sparse, and contains thumbnail images, often corresponding
   * to chapter markers. Only ever used with Disposition ATTACHED_PIC.
   */
  TIMED_THUMBNAILS?: boolean
  /** To specify text track kind (different from subtitles default). */
  CAPTIONS?: boolean
	DESCRIPTIONS?: boolean
	METADATA?: boolean
  /** Dependent audio stream (mix_type=0 in mpegts) */
  DEPENDENT?: boolean
  /** Still images in video stream (still_picture_flag=1 in mpegts) */
  STILL_IMAGE?: boolean
}

export interface EventFlags {
  METADATA_UPDATED?: boolean
}

/**
 * Stream describes the properties of a stream.
 */
export interface Stream {
	/** Object name. */
  readonly type: 'Stream'
	/** The stream index in the container. */
  readonly index: number
  /**
   * Format-specific stream ID.
   * decoding: set by beamcoder
   * encoding: set by the user, replaced by beamcoder if left unset
   */
  id: number
  /**
   * This is the fundamental unit of time (in seconds) in terms
   * of which frame timestamps are represented.
   *
   * decoding: set by beamcoder
   * encoding: May be set by the caller before writeHeader() to
   *           provide a hint to the muxer about the desired timebase. In
   *           writeHeader(), the muxer will overwrite this field
   *           with the timebase that will actually be used for the timestamps
   *           written into the file (which may or may not be related to the
   *           user-provided one, depending on the format).
   */
  time_base: Array<number>
  /**
   * Decoding: pts of the first frame of the stream in presentation order, in stream time base.
   * Only set this if you are absolutely 100% sure that the value you set
   * it to really is the pts of the first frame.
   * This may be undefined (AV_NOPTS_VALUE).
   * @note The ASF header does NOT contain a correct start_time the ASF
   * demuxer must NOT set this.
   */
  start_time: number | null
  /**
   * Decoding: duration of the stream, in stream time base.
   * If a source file does not specify a duration, but does specify
   * a bitrate, this value will be estimated from bitrate and file size.
   *
   * Encoding: May be set by the caller before writeHeader() to
   * provide a hint to the muxer about the estimated duration.
   */
  duration: number | null
  /** Number of frames in this stream if known or 0 */
  nb_frames: number
  disposition: Disposition
  /** Selects which packets can be discarded at will and do not need to be demuxed. */
  discard: 'none' | 'default' | 'nonref' | 'bidir' | 'nonintra' | 'nonkey' | 'all'
  /**
   * sample aspect ratio (0 if unknown)
   * - encoding: Set by user.
   * - decoding: Set by beamcoder.
   */
  sample_aspect_ratio: Array<number>

  metadata: { [key: string]: string }
  /**
   * Average framerate
   *
   * - demuxing: May be set by beamcoder when creating the stream
   * - muxing: May be set by the caller before writeHeader().
   */
  avg_frame_rate: Array<number>
  /**
   * For streams with AV_DISPOSITION_ATTACHED_PIC disposition, this packet
   * will contain the attached picture.
   *
   * decoding: set by beamcoder, must not be modified by the caller.
   * encoding: unused
   */
  readonly attached_pic: Packet | null
  /**
   * An array of side data that applies to the whole stream (i.e. the
   * container does not allow it to change between packets).
   *
   * There may be no overlap between the side data in this array and side data
   * in the packets. I.e. a given side data is either exported by the muxer
   * (demuxing) / set by the caller (muxing) in this array, then it never
   * appears in the packets, or the side data is exported / sent through
   * the packets (always in the first packet where the value becomes known or
   * changes), then it does not appear in this array.
   *
   * - demuxing: Set by beamcoder when the stream is created.
   * - muxing: May be set by the caller before writeHeader().
   */
	side_data: {
		type: 'PacketSideData'
		[key: string]: Buffer | string
	}
  /**
   * Flags for the user to detect events happening on the stream. Flags must
   * be cleared by the user once the event has been handled.
   */
  event_flags: EventFlags
  /**
   * Real base framerate of the stream.
   * This is the lowest framerate with which all timestamps can be
   * represented accurately (it is the least common multiple of all
   * framerates in the stream). Note, this value is just a guess!
   * For example, if the time base is 1/90000 and all frames have either
   * approximately 3600 or 1800 timer ticks, then r_frame_rate will be 50/1.
   */
  r_frame_rate: Array<number>
  /**
   * Codec parameters associated with this stream.
   *
   * - demuxing: filled by beamcoder on stream creation
   * - muxing: filled by the caller before writeHeader()
   */
  codecpar: CodecPar

  /** Retun a JSON string containing the object properties. */
  toJSON(): string
}

export interface FormatContextIn {
    /** The input format description. */
    set iformat(format: string);
    //@ts-ignore
    get iformat(): InputFormat
    /**
     * Position of the first frame of the component, in
     * AV_TIME_BASE fractional seconds. NEVER set this value directly:
     * It is deduced from the AVStream values. Demuxing only
     */
    readonly start_time: number
    /** Maximum size of the data read from input for determining the input container format. */
    probesize: number
    /**
 * Maximum duration (in AV_TIME_BASE units) of the data read
 * from input in avformat_find_stream_info().
 * Demuxing only, set by the caller before avformat_find_stream_info().
 * Can be set to 0 to let avformat choose using a heuristic.
 */
    max_analyze_duration: number

    /**
* Maximum amount of memory in bytes to use for the index of each stream.
* If the index exceeds this size, entries will be discarded as
* needed to maintain a smaller size. This can lead to slower or less
* accurate seeking (depends on demuxer).
* Demuxers for which a full in-memory index is mandatory will ignore
* this.
*/
    max_index_size: number
    /** The number of frames used for determining the framerate */
    fps_probe_size: number
    /**
     * Error recognition - higher values will detect more errors but may
     * misdetect some more or less valid parts as errors.
     */
    error_recognition: number

    /** Maximum number of packets to read while waiting for the first timestamp. */
    max_ts_probe: number

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
    /** ',' separated list of allowed protocols. */
    protocol_whitelist: string
    /** ',' separated list of disallowed protocols. */
    protocol_blacklist: string
    /** The maximum number of streams. */
    max_streams: number
    /** Skip duration calcuation in estimate_timings_from_pts. */
    skip_estimate_duration_from_pts: boolean
    // interleaved: true,

}

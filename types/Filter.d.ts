import { Frame } from "./Frame"
import { PrivClass } from "./PrivClass"

export interface Filter {
	readonly type: 'Filter'
  /** Filter name. Must be non-NULL and unique among filters. */
	readonly name: string
	/** A description of the filter. May be NULL. */
	readonly description: string
	/**
	 * List of inputs.
	 *
	 * NULL if there are no (static) inputs. Instances of filters with
	 * AVFILTER_FLAG_DYNAMIC_INPUTS set may have more inputs than present in
	 * this list.
	 */
	readonly inputs: ReadonlyArray<FilterPad>
	/**
	 * List of outputs.
	 *
	 * NULL if there are no (static) outputs. Instances of filters with
	 * AVFILTER_FLAG_DYNAMIC_OUTPUTS set may have more outputs than present in
	 * this list.
	 */
	readonly outputs: ReadonlyArray<FilterPad>
	/**
	 * A class for the private data, used to declare filter private AVOptions.
	 * This field is NULL for filters that do not declare any options.
	 */
	readonly priv_class: PrivClass | null
	/** A combination of AVFILTER_FLAG_* */
	readonly flags: {
		/**
		 * The number of the filter inputs is not determined just by AVFilter.inputs.
		 * The filter might add additional inputs during initialization depending on the
		 * options supplied to it.
		 */
		DYNAMIC_INPUTS: boolean
		/**
		 * The number of the filter outputs is not determined just by AVFilter.outputs.
     * The filter might add additional outputs during initialization depending on
     * the options supplied to it.
		 */
		DYNAMIC_OUTPUTS: boolean
		/**
		 * The filter supports multithreading by splitting frames into multiple parts and
		 * processing them concurrently.
		 */
		SLICE_THREADS: boolean
		/**
		 * Some filters support a generic "enable" expression option that can be used
     * to enable or disable a filter in the timeline. Filters supporting this
     * option have this flag set. When the enable expression is false, the default
     * no-op filter_frame() function is called in place of the filter_frame()
     * callback defined on each input pad, thus the frame is passed unchanged to
     * the next filters.
		 */
		SUPPORT_TIMELINE_GENERIC: boolean
		/**
		 * Same as AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC, except that the filter will
     * have its filter_frame() callback(s) called as usual even when the enable
     * expression is false. The filter will disable filtering within the
     * filter_frame() callback(s) itself, for example executing code depending on
     * the AVFilterContext->is_disabled value.
		 */
		SUPPORT_TIMELINE_INTERNAL: boolean
		/**
		 * Handy mask to test whether the filter supports or no the timeline feature
		 * (internally or generically).
		 */
		SUPPORT_TIMELINE: boolean
	}
}

export type MediaType = 'unknown' | 'video' | 'audio' | 'data' | 'subtitle' | 'attachment' | 'nb'

export interface FilterPad {
	name: string
	media_type: MediaType
}

export interface FilterLink {
  /** source filter name */
	readonly src: string
	/** output pad on the source filter */
	readonly srcpad: string
	/** dest filter name */
	readonly dst: string
	/** input pad on the dest filter */
	readonly dstpad: string
	/** filter media type */
	readonly type: MediaType
	/** video only - agreed upon image width */
	readonly w?: number
	/** video only - agreed upon image height */
	readonly h?: number
	/** video only - agreed upon sample aspect ratio */
	readonly sample_aspect_ratio?: ReadonlyArray<number>
	/** audio only - number of channels in the channel layout. */
	readonly channel_count?: number
	/** audio only - channel layout of current buffer */
	readonly channel_layout?: string
	/** audio only - samples per second */
	readonly sample_rate?: number
	/** agreed upon media format */
	readonly format: string
	/**
	 * Define the time base used by the PTS of the frames/samples which will pass through this link.
   * During the configuration stage, each filter is supposed to change only the output timebase,
	 * while the timebase of the input link is assumed to be an unchangeable property.
	*/
	readonly time_base: ReadonlyArray<number>
}

export interface FilterContext {
	readonly type: 'FilterContext'
  /** the AVFilter of which this is an instance */
	readonly filter: Filter
  /** name of this filter instance */
	readonly name: string
  /** array of input pads */
	readonly input_pads: ReadonlyArray<FilterPad>
  /** array of pointers to input links */
	readonly inputs: ReadonlyArray<FilterLink> | null
  /** array of output pads */
	readonly output_pads: ReadonlyArray<FilterPad>
  /** array of pointers to output links */
	readonly outputs: ReadonlyArray<FilterLink> | null
  /** private data for use by the filter */
	priv: { [key: string]: any } | null
  /**
	 * Type of multithreading being allowed/used. A combination of
	 * AVFILTER_THREAD_* flags.
	 *
	 * May be set by the caller before initializing the filter to forbid some
	 * or all kinds of multithreading for this filter. The default is allowing
	 * everything.
	 *
	 * When the filter is initialized, this field is combined using bit AND with
	 * AVFilterGraph.thread_type to get the final mask used for determining
	 * allowed threading types. I.e. a threading type needs to be set in both
	 * to be allowed.
	 *
	 * After the filter is initialized, libavfilter sets this field to the
	 * threading type that is actually used (0 for no multithreading).
	 */
	readonly thread_type: number
  /**
	 * Max number of threads allowed in this filter instance.
	 * If <= 0, its value is ignored.
	 * Overrides global number of threads set per filter graph.
	 */
	readonly nb_threads: number
  /**
	 * Ready status of the filter.
	 * A non-0 value means that the filter needs activating,
	 * a higher value suggests a more urgent activation.
	 */
	readonly ready: number
  /**
	 * Sets the number of extra hardware frames which the filter will
	 * allocate on its output links for use in following filters or by
	 * the caller.
	 *
	 * Some hardware filters require all frames that they will use for
	 * output to be defined in advance before filtering starts.  For such
	 * filters, any hardware frame pools used for output must therefore be
	 * of fixed size.  The extra frames set here are on top of any number
	 * that the filter needs internally in order to operate normally.
	 *
	 * This field must be set before the graph containing this filter is
	 * configured.
	 */
  readonly extra_hw_frames: number
}

export interface FilterGraph {
	readonly type: 'FilterGraph'

	readonly filters: ReadonlyArray<FilterContext>
  /** sws options to use for the auto-inserted scale filters */
	readonly scale_sws_opts: string | null
	/**
	 * Type of multithreading allowed for filters in this graph. A combination of AVFILTER_THREAD_* flags.
   * May be set by the caller at any point, the setting will apply to all filters initialized after that.
	 * The default is allowing everything.
   *
   * When a filter in this graph is initialized, this field is combined using bit AND with
	 * AVFilterContext.thread_type to get the final mask used for determining allowed threading types.
	 * I.e. a threading type needs to be set in both to be allowed.
	 */
	readonly thread_type: number
  /**
	 * Maximum number of threads used by filters in this graph. May be set by
   * the caller before adding any filters to the filtergraph. Zero (the
   * default) means that the number of threads is determined automatically.
	 */
	readonly nb_threads: number
  /**
	 * Dump a graph into a human-readable string representation.
	 * @returns: String representation of the filter graph
	 */
	dump(): string
}

export interface FiltererResult {
	/** Output pad name in the filterSpec string used in the filterer setup. */
	readonly name: string
	/** Array of output frames for the pad */
	readonly frames: Array<Frame>
}

export interface Filterer {
	readonly type: 'Filterer'
	readonly graph: FilterGraph

  /**
	 * Filter an array of frames
	 * For a filter that has only one input pass an array of frame objects directly
	 * and the filter input will have a default name applied.
	 * This name will match a filter specification that doesn't name its inputs.
	 * @param frames Array of Frame objects to be applied to the single input pad
	 * @returns Array of objects containing Frame arrays for each output pad of the filter
	 */
	filter(frames: Array<Frame>): Promise<Array<FiltererResult> & { total_time: number }>
  /**
	 * Filter an array of frames
	 * Pass an array of objects, one per filter input, each with a name string property
	 * and a frames property that contains an array of frame objects
	 * The name must match the input name in the filter specification
	 * @param framesArr Array of objects with name and Frame array for each input pad
	 * @returns Array of objects containing Frame arrays for each output pad of the filter
   */
	filter(framesArr: Array<{ name: string, frames: Array<Frame> }>): Promise<Array<FiltererResult> & { total_time: number }>
}

/**
 * Provides a list and details of all the available filters
 * @returns an object with name and details of each of the available filters
 */
export function filters(): { [key: string]: Filter }

/** List the available bitstream filters */
export function bsfs(): {
  [key: string]: {
		name: string
		codec_ids: Array<string>
		priv_class: PrivClass | null }
}

/** The required parameters for setting up filter inputs */
export interface InputParam {
	/**
	 * Input pad name that matches the filter specification string
	 * For a single input filter without a name in the filter specification the name can be omitted
	 */
	name?: string
	/** Define the time base used by the PTS of the frames/samples for this filter.	*/
	timeBase: Array<number>
}
/** The required parameters for setting up video filter inputs */
export interface VideoInputParam extends InputParam {
	width: number
	height: number
	pixelFormat: string
	pixelAspect: Array<number>
}
/** The required parameters for setting up audio filter inputs */
export interface AudioInputParam extends InputParam {
	sampleRate: number
	sampleFormat: string
	channelLayout: string
}
/** The required parameters for setting up filter inputs */
export interface OutputParam {
	/**
	 * Output pad name that matches the filter specification string
	 * For a single output filter without a name in the filter specification the name can be omitted
	 */
	name?: string
}
/** The required parameters for setting up video filter outputs */
export interface VideoOutputParam extends OutputParam {
	pixelFormat: string
}
/** The required parameters for setting up audio filter outputs */
export interface AudioOutputParam extends OutputParam {
	sampleRate: number
	sampleFormat: string
	channelLayout: string
}

export interface FiltererOptions {
	/** The filter type - video or audio */
	filterType: MediaType
	filterSpec: string
}

export interface FiltererVideoOptions extends FiltererOptions {
	/** Video filter type */
	filterType: 'video'
	/** Video input parameters for the filter */
	inputParams: Array<VideoInputParam>
	/** Video output parameters for the filter */
	outputParams: Array<VideoOutputParam>
}
export interface FiltererAudioOptions extends FiltererOptions {
	/** Audio filter type */
	filterType: 'audio'
	/** Audio input parameters for the filter */
	inputParams: Array<AudioInputParam>
	/** Audio output parameters for the filter */
	outputParams: Array<AudioOutputParam>
}

/**
 * Create a filterer
 * @param options parameters to set up the type, inputs, outputs and spec of the filter
 * @returns Promise that resolve to a Filterer on success
 */
export function filterer(options: FiltererVideoOptions | FiltererAudioOptions): Promise<Filterer>

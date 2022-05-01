import { Frame } from "./Frame"
import { Timable, TotalTimeed } from "./time"

/** The DecodedFrames object is returned as the result of a decode operation */
export interface DecodedFrames extends Timable, TotalTimeed {
	/** Object name. */
	readonly type: 'frames'
	/** 
	   * Decoded frames that are now available. If the array is empty, the decoder has buffered
	 * the packet as part of the process of producing future frames
	   */
	readonly frames: Array<Frame>

	// "encode" | "dice" | "decode" | "filter"
	// timings?: { [key: string]: Timing; };
}

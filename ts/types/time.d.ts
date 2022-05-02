export interface Timing {
    reqTime: number;
    elapsed: number;
}

export interface TotalTimed {
	/** Total time in microseconds that the decode operation took to complete */
    total_time: number
}

export interface Timable {
    timings?: { [key: string]: Timing; };
}

export interface toJSONAble {
	/** Retun a JSON string containing the object properties. */
	toJSON(): string
}

export type Timables<T> = Array<T> & Timable;
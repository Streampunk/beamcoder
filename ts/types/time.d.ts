export interface Timing {
    reqTime: number;
    elapsed: number;
}

export interface TotalTimeed {
	/** Total time in microseconds that the decode operation took to complete */
    total_time: number
}

export interface Timable {
    timings?: { [key: string]: Timing; };
}

export type Timables<T> = Array<T> & Timable;
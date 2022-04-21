import { Timing } from './Timing'

export interface Timable {
    timings?: { [key: string]: Timing; };
}

export type Timables<T> = Array<T> & Timable;
/**
 * Create object for AVIOContext based buffered I/O
 * 
 * Type only definition for Governor class
 */
 export class governor {
    constructor(options: { highWaterMark?: number });
    read(len: number): Promise<Buffer>
    write(data: Buffer): Promise<null>
    finish(): undefined
    private _adaptor: unknown;
}

declare class GovernorClass {
    constructor(options: { highWaterMark?: number });
    read(len: number): Promise<Buffer>
    write(data: Buffer): Promise<null>
    finish(): undefined
    private _adaptor: unknown;
}

export type Governor = GovernorClass;

export interface commonEncoderParms {
    width?: number;
    height?: number;
    sample_rate?: number,
    sample_fmt?: string,
    sample_aspect_ratio?: ReadonlyArray<number>,
    channels?: number;
    bit_rate?: number;
    channel_layout?: string;
    time_base?: [number, number];
    framerate?: [number, number];
    gop_size?: number;
    max_b_frames?: number;
    pix_fmt?: string;// pixelFormat,
    priv_data?: any; // { preset?: 'slow' };
    flags?: any;
    // [key: string]: any 

}

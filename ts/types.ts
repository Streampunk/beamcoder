import type { Frame, Stream, Muxer, CodecPar, Codec, Governor, WritableDemuxerStream, ReadableMuxerStream, FiltererVideoOptions, FiltererAudioOptions, Filterer, Decoder, Demuxer, Encoder, InputFormat } from '..'; // Codec, CodecContext, 

export type governorType = {
    read(len: number): Promise<Buffer>;
    write(data: Buffer): Promise<null>;
    finish(): undefined;
};

export interface BeamcoderType extends ReadableMuxerStream {
    /** Create object for AVIOContext based buffered I/O */
    governor: typeof Governor;

    /**
     * Create a demuxer for this source
     * @param options a DemuxerCreateOptions object
     * @returns a promise that resolves to a Demuxer when it has determined sufficient 
     * format details by consuming data from the source. The promise will wait indefinitely 
     * until sufficient source data has been read.
     */
    // this code look to have error....
    demuxer(options: { governor?: governorType, url?: string, iformat?: InputFormat, options?: { governor: governorType } } | string): Promise<Demuxer>
    // url: src.url, iformat: src.iformat, options: src.options 
    /**
    * Provides a list and details of all the available encoders
    * @returns an object with name and details of each of the available encoders
    */
    encoders(): { [key: string]: Codec };
    /**
     * Create a WritableDemuxerStream to allow streaming to a Demuxer
     * @param options.highwaterMark Buffer level when `stream.write()` starts returng false.
     * @returns A WritableDemuxerStream that can be streamed to.
     */
    demuxerStream(options: { highwaterMark?: number }): WritableDemuxerStream;

    /**
     * Create a ReadableMuxerStream to allow streaming from a Muxer
     * @param options.highwaterMark The maximum number of bytes to store in the internal buffer before ceasing to read from the underlying resource.
     * @returns A ReadableMuxerStream that can be streamed from.
     */
    muxerStream(options: { highwaterMark?: number }): ReadableMuxerStream;
    /**
     * Create a filterer
     * @param options parameters to set up the type, inputs, outputs and spec of the filter
     * @returns Promise that resolve to a Filterer on success
     */
    filterer(options: FiltererVideoOptions | FiltererAudioOptions): Promise<Filterer>

    /** 
     * Create a decoder by name
     * @param name The codec name required
     * @param ... Any non-readonly parameters from the Decoder object as required
     * @returns A Decoder object - note creation is synchronous
     */
    decoder(options: { name: string, [key: string]: any }): Decoder
    /**
     * Create a decoder by codec_id
     * @param codec_id The codec ID from AV_CODEC_ID_xxx
     * @param ... Any non-readonly parameters from the Decoder object as required
     * @returns A Decoder object - note creation is synchronous
     */
    decoder(options: { codec_id: number, [key: string]: any }): Decoder
    /**
     * Create a decoder from a demuxer and a stream_index
     * @param demuxer An initialised Demuxer object
     * @param stream_index The stream number of the demuxer object to be used to initialise the decoder
     * @param ... Any non-readonly parameters from the Decoder object as required
     * @returns A Decoder object - note creation is synchronous
     */
    decoder(options: { demuxer: Demuxer, stream_index: number, [key: string]: any }): Decoder
    /**
     * Create a decoder from a CodecPar object
     * @param params CodecPar object whose codec name or id will be used to initialise the decoder
     * @param ... Any non-readonly parameters from the Decoder object as required
     * @returns A Decoder object - note creation is synchronous
     */
    decoder(options: { params: CodecPar, [key: string]: any }): Decoder
    /**
     * Create a frame for encoding or filtering
     * Set parameters as required from the Frame object
     */
    frame(options: { [key: string]: any, data?: Array<Buffer> } | string): Frame;
    /**
     * Provides a list and details of all the available encoders
     * @returns an object with name and details of each of the available encoders
     */
    encoders(): { [key: string]: Codec };
    /** 
     * Create an encoder by name
     * @param name The codec name required
     * @param ... Any non-readonly parameters from the Encoder object as required
     * @returns An Encoder object - note creation is synchronous
     */
    encoder(options: { name: string, [key: string]: any }): Encoder
    /**
     * Create an encoder by codec_id
     * @param codec_id The codec ID from AV_CODEC_ID_xxx
     * @param ... Any non-readonly parameters from the Encoder object as required
     * @returns An Encoder object - note creation is synchronous
     */
    encoder(options: { codec_id: number, [key: string]: any }): Encoder
}


export interface BeamstreamStream {
    name: string;
    time_base: any;
    encoder: any;
    stream: any;
    codecpar: {
        sample_rate: number;
        frame_size: number;
        format: any,
        channel_layout: any;
    };
}
export interface BeamstreamSource {
    decoder: any;
    format: any;
    streamIndex: number;
    stream: any;
}


import { Codec } from "./Codec";
import { CodecPar } from "./CodecPar";
import { Decoder } from "./Decoder";
import { Demuxer, DemuxerCreateOptions } from "./Demuxer";
import { Encoder } from "./Encoder";
import { FormatContext, InputFormat, OutputFormat } from "./FormatContext";
import { Frame, PixelFormat, SampleFormat } from "./Frame";
import { Packet, PacketFlags } from "./Packet";
import { PrivClass } from "./PrivClass";
import { WritableDemuxerStream, ReadableMuxerStream, BeamstreamParams } from './Beamstreams';
import { Filter, Filterer, FiltererAudioOptions, FiltererVideoOptions } from "./Filter";
import { commonEncoderParms } from './params';
import { Muxer, MuxerCreateOptions } from "./Muxer";
import type { Governor } from './type/Governor';

export interface BeamcoderType extends ReadableMuxerStream {
    /** Create object for AVIOContext based buffered I/O */
    governor: typeof Governor;
    /**
     * FFmpeg version string. This usually is the actual release
     * version number or a git commit description. This string has no fixed format
     * and can change any time. It should never be parsed by code.
     */
    avVersionInfo(): string
    /**
     * Create a demuxer for this source
     * @param options a DemuxerCreateOptions object
     * @returns a promise that resolves to a Demuxer when it has determined sufficient 
     * format details by consuming data from the source. The promise will wait indefinitely 
     * until sufficient source data has been read.
     */
    // this code look to have error....
    demuxer(options: { governor?: Governor, url?: string, iformat?: InputFormat, options?: { governor: Governor } } | string): Promise<Demuxer>
    // url: src.url, iformat: src.iformat, options: src.options 
    /**
     * Create a WritableDemuxerStream to allow streaming to a Demuxer
     * @param options.highwaterMark Buffer level when `stream.write()` starts returng false.
     * @returns A WritableDemuxerStream that can be streamed to.
     */
    demuxerStream(options: { highwaterMark?: number }): WritableDemuxerStream;

    /**
     * Initialise the sources for the beamstream process.
     * Note - the params object is updated by the function.
     */
    makeSources(params: BeamstreamParams): Promise<void>
    /**
     * Initialise the output streams for the beamstream process.
     * Note - the params object is updated by the function.
     * @returns Promise which resolves to an object with a run function that starts the processing
     */
    makeStreams(params: BeamstreamParams): Promise<{ run(): Promise<void> }>
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
     * Provides a list and details of all the available decoders
     * @returns an object with name and details of each of the available decoders
     */
    decoders(): { [key: string]: Codec }
    /** 
     * Create a decoder by name
     * @param name The codec name required
     * @param ... Any non-readonly parameters from the Decoder object as required
     * @returns A Decoder object - note creation is synchronous
     */
    // decoder(options: { demuxer: Demuxer | Promise<Demuxer>, stream_index: number }): Decoder // { name: string, [key: string]: any }
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
    encoder(options: commonEncoderParms & { name: string }): Encoder
    /**
     * Create an encoder by codec_id
     * @param codec_id The codec ID from AV_CODEC_ID_xxx
     * @param ... Any non-readonly parameters from the Encoder object as required
     * @returns An Encoder object - note creation is synchronous
     */
    encoder(options: commonEncoderParms & { codec_id: number }): Encoder
    /**
     * Packets for decoding can be created without reading them from a demuxer
     * Set parameters as required from the Packet object, passing in a buffer and the required size in bytes
     */
    packet(options?: string | {
        flags?: Partial<PacketFlags>,
        pts?: number,
        dts?: number,
        stream_index?: number,
        data: Buffer,
        size?: number,
        side_data?: any,
        [key: string]: any,
    }): Packet

    /** List the available codecs */
    codecs(): { [key: string]: { encoder?: Codec, decoder?: Codec } }

    /**
     * Provides a list and details of all the available demuxer input formats
     * @returns an object with details of all the available demuxer input formats
     */
    demuxers(): { [key: string]: InputFormat }

    /**
    * Create a demuxer to read from a URL or filename
    * @param url a string describing the source to be read from (may contain %d for a sequence of numbered files).
    * @returns a promise that resolves to a Demuxer when it has determined sufficient 
    * format details by consuming data from the source. The promise will wait indefinitely 
    * until sufficient source data has been read.
    */
    demuxer(url: string): Promise<Demuxer>

    /**
     * For formats that require additional metadata, such as the rawvideo format,
     * it may be necessary to pass additional information such as image size or pixel format to Demuxer creation.
     * @param options a DemuxerCreateOptions object
     * @returns a promise that resolves to a Demuxer when it has determined sufficient 
     * format details by consuming data from the source. The promise will wait indefinitely 
     * until sufficient source data has been read.
     */
    demuxer(options: DemuxerCreateOptions): Promise<Demuxer>

    /**
 * Provides a list and details of all the available encoders
 * @returns an object with name and details of each of the available encoders
 */
    encoders(): { [key: string]: Codec }
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
    /**
     * Return the output format in the list of registered output formats which best matches the provided name,
     * or return null if there is no match.
     */
    guessFormat(name: string): OutputFormat | null;

    format(options?: string | { [key: string]: any }): FormatContext;
    /**
     * Create a frame for encoding or filtering
     * Set parameters as required from the Frame object
     */
    frame(options?: string | { [key: string]: any, data?: Array<Buffer> }): Frame

    /** Format details for all supported pixel format names */
    pix_fmts(): { [key: string]: PixelFormat }
    /** Format details for all supported sample format names */
    sample_fmts(): { [key: string]: SampleFormat }
    /**
     * Note that when creating buffers from Javascript,
     * FFmpeg recommends that a small amount of headroom is added to the minimum length of each buffer.
     * The minimum amount of padding is exposed to Javascript as constant
     */
    AV_INPUT_BUFFER_PADDING_SIZE: number;

    /**
     * Create a muxer to write to a URL or filename
     * @param options a MuxerCreateOptions object
     * @returns A Muxer object
     */
    muxer(options: MuxerCreateOptions): Muxer


    /**
     * Provides a list and details of all the available muxer output formats
     * @returns an object with details of all the available muxer output formats
     */
    muxers(): { [key: string]: OutputFormat }


    /**
 * Provides a list and details of all the available filters
 * @returns an object with name and details of each of the available filters
 */
    filters(): { [key: string]: Filter }

    /** List the available bitstream filters */
    bsfs(): {
        [key: string]: {
            name: string
            codec_ids: Array<string>
            priv_class: PrivClass | null
        }
    }

    /**
 * Create a ReadableMuxerStream to allow streaming from a Muxer
 * @param options.highwaterMark The maximum number of bytes to store in the internal buffer before ceasing to read from the underlying resource.
 * @returns A ReadableMuxerStream that can be streamed from.
 */
    muxerStream(options: { highwaterMark?: number }): ReadableMuxerStream
    /**
     * Initialise the sources for the beamstream process.
     * Note - the params object is updated by the function.
     */
    makeSources(params: BeamstreamParams): Promise<void>
    /**
     * Initialise the output streams for the beamstream process.
     * Note - the params object is updated by the function.
     * @returns Promise which resolves to an object with a run function that starts the processing
     */
    makeStreams(params: BeamstreamParams): Promise<{ run(): Promise<void> }>
    /**
    * Create a WritableDemuxerStream to allow streaming to a Demuxer
    * @param options.highwaterMark Buffer level when `stream.write()` starts returng false.
    * @returns A WritableDemuxerStream that can be streamed to.
    */
    demuxerStream(options: { highwaterMark?: number }): WritableDemuxerStream

    codecParameters(options?: string | Partial<Omit<CodecPar, 'type' | '_codecPar' | 'toJSON'>>): CodecPar;
    // { [key: string]: any }

    /**
     * Create a filterer
     * @param options parameters to set up the type, inputs, outputs and spec of the filter
     * @returns Promise that resolve to a Filterer on success
     */
    filterer(options: FiltererVideoOptions | FiltererAudioOptions): Promise<Filterer>
}


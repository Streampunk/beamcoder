// import type { Frame, Stream, Muxer, CodecPar, Codec, Governor, WritableDemuxerStream, ReadableMuxerStream, FiltererVideoOptions, FiltererAudioOptions, Filterer, Decoder, Demuxer, Encoder, InputFormat, BeamstreamParams } from '..'; // Codec, CodecContext, 

// import { Governor, ReadableMuxerStream } from "../types";

// export type governorType = {
//     read(len: number): Promise<Buffer>;
//     write(data: Buffer): Promise<null>;
//     finish(): undefined;
// };

export type pixelFormat = 'yuv420p' | 'yuyv422' | 'rgb24' | 'bgr24' | 'yuv422p' | 'yuv444p' | 'yuv410p' | 'yuv411p' | 'gray' | 'monow' | 'monob' | 'pal8' | 'yuvj420p' | 'yuvj422p' | 'yuvj444p' | 'uyvy422' | 'uyyvyy411' | 'bgr8' | 'bgr4' | 'bgr4_byte' | 'rgb8' | 'rgb4' | 'rgb4_byte' | 'nv12' | 'nv21' | 'argb' | 'rgba' | 'abgr' | 'bgra' | 'gray16be' | 'gray16le' | 'yuv440p' | 'yuvj440p' | 'yuva420p' | 'rgb48be' | 'rgb48le' | 'rgb565be' | 'rgb565le' | 'rgb555be' | 'rgb555le' | 'bgr565be' | 'bgr565le' | 'bgr555be' | 'bgr555le' | 'vaapi_moco' | 'vaapi_idct' | 'vaapi_vld' | 'yuv420p16le' | 'yuv420p16be' | 'yuv422p16le' | 'yuv422p16be' | 'yuv444p16le' | 'yuv444p16be' | 'dxva2_vld' | 'rgb444le' | 'rgb444be' | 'bgr444le' | 'bgr444be' | 'ya8' | 'bgr48be' | 'bgr48le' | 'yuv420p9be' | 'yuv420p9le' | 'yuv420p10be' | 'yuv420p10le' | 'yuv422p10be' | 'yuv422p10le' | 'yuv444p9be' | 'yuv444p9le' | 'yuv444p10be' | 'yuv444p10le' | 'yuv422p9be' | 'yuv422p9le' | 'gbrp' | 'gbrp9be' | 'gbrp9le' | 'gbrp10be' | 'gbrp10le' | 'gbrp16be' | 'gbrp16le' | 'yuva422p' | 'yuva444p' | 'yuva420p9be' | 'yuva420p9le' | 'yuva422p9be' | 'yuva422p9le' | 'yuva444p9be' | 'yuva444p9le' | 'yuva420p10be' | 'yuva420p10le' | 'yuva422p10be' | 'yuva422p10le' | 'yuva444p10be' | 'yuva444p10le' | 'yuva420p16be' | 'yuva420p16le' | 'yuva422p16be' | 'yuva422p16le' | 'yuva444p16be' | 'yuva444p16le' | 'vdpau' | 'xyz12le' | 'xyz12be' | 'nv16' | 'nv20le' | 'nv20be' | 'rgba64be' | 'rgba64le' | 'bgra64be' | 'bgra64le' | 'yvyu422' | 'ya16be' | 'ya16le' | 'gbrap' | 'gbrap16be' | 'gbrap16le' | 'qsv' | 'mmal' | 'd3d11va_vld' | 'cuda' | '0rgb' | 'rgb0' | '0bgr' | 'bgr0' | 'yuv420p12be' | 'yuv420p12le' | 'yuv420p14be' | 'yuv420p14le' | 'yuv422p12be' | 'yuv422p12le' | 'yuv422p14be' | 'yuv422p14le' | 'yuv444p12be' | 'yuv444p12le' | 'yuv444p14be' | 'yuv444p14le' | 'gbrp12be' | 'gbrp12le' | 'gbrp14be' | 'gbrp14le' | 'yuvj411p' | 'bayer_bggr8' | 'bayer_rggb8' | 'bayer_gbrg8' | 'bayer_grbg8' | 'bayer_bggr16le' | 'bayer_bggr16be' | 'bayer_rggb16le' | 'bayer_rggb16be' | 'bayer_gbrg16le' | 'bayer_gbrg16be' | 'bayer_grbg16le' | 'bayer_grbg16be' | 'xvmc' | 'yuv440p10le' | 'yuv440p10be' | 'yuv440p12le' | 'yuv440p12be' | 'ayuv64le' | 'ayuv64be' | 'videotoolbox_vld' | 'p010le' | 'p010be' | 'gbrap12be' | 'gbrap12le' | 'gbrap10be' | 'gbrap10le' | 'mediacodec' | 'gray12be' | 'gray12le' | 'gray10be' | 'gray10le' | 'p016le' | 'p016be' | 'd3d11' | 'gray9be' | 'gray9le' | 'gbrpf32be' | 'gbrpf32le' | 'gbrapf32be' | 'gbrapf32le' | 'drm_prime' | 'opencl' | 'gray14be' | 'gray14le' | 'grayf32be' | 'grayf32le' | 'yuva422p12be' | 'yuva422p12le' | 'yuva444p12be' | 'yuva444p12le' | 'nv24' | 'nv42';

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

export interface ffStats {
    mean: number;
    stdDev: number;
    max: number;
    min: number;
}
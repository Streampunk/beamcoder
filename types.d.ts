export * from "./ts/models/CodecPar.ts"
export * from "./ts/models/Packet"
export * from "./ts/models/Frame"
export * from "./ts/models/Stream"
export * from "./ts/models/Codec"
export * from "./ts/models/FormatContext"
export * from "./types/Demuxer"
export * from "./types/Decoder"
export * from "./ts/models/Filter"
export * from "./types/Encoder"
export * from "./types/Muxer"
export * from "./ts/models/Beamstreams"
export * from "./types/HWContext"

export const AV_NOPTS_VALUE: number

/** The LIBAV**_VERSION_INT for each FFmpeg library */
export function versions(): {
  avcodec: number
  avdevice: number
  avfilter: number
  avformat: number
  avutil: number
  postproc: number
  swresample: number
  swscale: number
}
/**
 * FFmpeg version string. This usually is the actual release
 * version number or a git commit description. This string has no fixed format
 * and can change any time. It should never be parsed by code.
 */
export function avVersionInfo(): string
/** Informative version strings for each FFmpeg library */
export function versionStrings(): {
  avcodec: string
  avdevice: string
  avfilter: string
  avformat: string
  avutil: string
  postproc: string
  swresample: string
  swscale: string
}
/** Build configuration strings for each FFmpeg library */
export function configurations(): {
  avcodec: string
  avdevice: string
  avfilter: string
  avformat: string
  avutil: string
  postproc: string
  swresample: string
  swscale: string
}
/** License strings for each FFmpeg library */
export function licenses(): {
  avcodec: string
  avdevice: string
  avfilter: string
  avformat: string
  avutil: string
  postproc: string
  swresample: string
  swscale: string
}
/** List the available protocols */
export function protocols(): { inputs: Array<string>, outputs: Array<string> }

/** Read or set the logging level
 * `quiet` - print no output.
 * `panic` - something went really wrong - crash will follow
 * `fatal` - recovery not possible
 * `error` - lossless recovery not possible
 * `warning` - something doesn't look correct
 * `info` - standard information - the default
 * `verbose` - detailed information
 * `debug` - stuff which is only useful for libav* developers
 * `trace` - extremely verbose debugging for libav* developers
 */
export function logging(level?: string): string | undefined

export as namespace Beamcoder

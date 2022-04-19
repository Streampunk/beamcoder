export type HWDeviceType = 'none' | 'vdpau' | 'cuda' | 'vaapi' | 'dxva2' | 'qsv' | 'videotoolbox' |
                           'd3d11va' | 'drm' | 'opencl' | 'mediacodec' | 'vulkan'

export interface HWDeviceContext {
	/** Object name. */
  readonly type: 'HWDeviceContext'
  /**
   * This field identifies the underlying API used for hardware access.
   * This field is set when the context is allocated and never changed afterwards.
   */
  readonly device_type: HWDeviceType
}

export interface HWFramesContext {
	/** Object name. */
  readonly type: 'HWFramesContext'
  /**
   * A reference to the parent HWDeviceContext. This reference is owned and
   * managed by the enclosing HWFramesContext, but the caller may derive
   * additional references from it.
   */
  readonly device_context: HWDeviceContext
  /**
   * Initial size of the frame pool. If a device type does not support
   * dynamically resizing the pool, then this is also the maximum pool size.
   *
   * May be set by the caller before calling av_hwframe_ctx_init(). Must be
   * set if pool is NULL and the device type does not support dynamic pools.
   */
  readonly initial_pool_size: number
  /**
   * The pixel format identifying the underlying HW surface type.
   *
   * Must be a hwaccel format, i.e. the corresponding descriptor must have the
   * AV_PIX_FMT_FLAG_HWACCEL flag set.
   *
   * Must be set by the user before calling av_hwframe_ctx_init().
   */
  readonly pix_fmt: string
  /**
   * The pixel format identifying the actual data layout of the hardware
   * frames.
   *
   * Must be set by the caller before calling av_hwframe_ctx_init().
   *
   * @note when the underlying API does not provide the exact data layout, but
   * only the colorspace/bit depth, this field should be set to the fully
   * planar version of that format (e.g. for 8-bit 420 YUV it should be
   * AV_PIX_FMT_YUV420P, not AV_PIX_FMT_NV12 or anything else).
   */
  readonly sw_pix_fmt: string
  /**
   * The allocated width of the frames in this pool.
   * Must be set by the user before calling av_hwframe_ctx_init().
   */
  readonly width: number
  /**
   * The allocated height of the frames in this pool.
   * Must be set by the user before calling av_hwframe_ctx_init().
   */
  readonly height: number
}

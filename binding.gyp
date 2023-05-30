{
  "targets": [{
    "target_name" : "beamcoder",
    "sources" : [ "src/beamcoder.cc", "src/beamcoder_util.cc",
                  "src/governor.cc", "src/demux.cc",
                  "src/decode.cc", "src/filter.cc",
                  "src/encode.cc", "src/mux.cc",
                  "src/packet.cc", "src/frame.cc",
                  "src/codec_par.cc", "src/format.cc",
                  "src/codec.cc", "src/hwcontext.cc" ],
    "defines": [
       "NAPI_VERSION=<(napi_build_version)"
    ],
    "conditions": [
      ['OS=="linux"', {
        "defines": [
          "__STDC_CONSTANT_MACROS"
        ],
        "cflags_cc!": [
          "-fno-rtti",
          "-fno-exceptions"
        ],
        "cflags_cc": [
          "-std=c++11",
          "-fexceptions"
        ],
        "link_settings": {
          "libraries": [
            "-lavcodec",
            "-lavdevice",
            "-lavfilter",
            "-lavformat",
            "-lavutil",
            "-lpostproc",
            "-lswresample",
            "-lswscale",
            "-lzimg"
          ]
        },
        "copies": [
            {
              "destination": "<(PRODUCT_DIR)",
              "files": [
                "node_modules/ffmpeg-ffprobe-static/ffmpeg",
                "node_modules/ffmpeg-ffprobe-static/ffprobe",
              ]
            }
          ]
      }],
      ["OS=='mac'", {
        "variables": {
           "ffmpeg_version": "1.33rc3",
           "target_arch_override": "<!(node -p \"'<(target_arch)' === 'x64' ? 'x86_64' : '<(target_arch)'\")",
        },
        "defines": [
          "__STDC_CONSTANT_MACROS"
        ],
        "cflags_cc!": [
          "-fno-rtti",
          "-fno-exceptions"
        ],
        "cflags_cc": [
          "-std=c++11",
          "-fexceptions"
        ],
        "include_dirs": [
          "<(module_root_dir)/ffmpeg/ffmpeg-ffprobe-shared-darwin-<(target_arch_override).<(ffmpeg_version)/include/"
        ],
        "link_settings": {
          "library_dirs": [
            "<(module_root_dir)/ffmpeg/ffmpeg-ffprobe-shared-darwin-<(target_arch_override).<(ffmpeg_version)/"
          ],
          "libraries": [
            "-Wl,-rpath,@loader_path",
            "-lavcodec",
            "-lavdevice",
            "-lavfilter",
            "-lavformat",
            "-lavutil",
            "-lpostproc",
            "-lswresample",
            "-lswscale",
            "-lzimg"
          ],
        },
        'xcode_settings': {
          'MACOSX_DEPLOYMENT_TARGET': '10.11',
        },
        "copies": [
            {
              "destination": "<(PRODUCT_DIR)",
              "files": [
                "<!@(node -p \"require('fs').readdirSync('ffmpeg/ffmpeg-ffprobe-shared-darwin-<(target_arch_override).<(ffmpeg_version)').map(f => 'ffmpeg/ffmpeg-ffprobe-shared-darwin-<(target_arch_override).<(ffmpeg_version)/' + f).join(' ')\")"
              ]
            }
          ]
      }
      ],
      ['OS=="win"', {
        "configurations": {
          "Release": {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeTypeInfo": "true",
                "AdditionalOptions": ['/WX']
              }
            }
          }
        },
        "include_dirs" : [
          "ffmpeg/ffmpeg-4.x-win64-shared/include"
        ],
        "libraries": [
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/avcodec",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/avdevice",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/avfilter",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/avformat",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/avutil",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/postproc",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/swresample",
          "-l../ffmpeg/ffmpeg-4.x-win64-shared/lib/swscale"
        ],
        "copies": [
            {
              "destination": "<(PRODUCT_DIR)",
              "files": [
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/avcodec-58.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/avdevice-58.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/avfilter-7.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/avformat-58.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/avutil-56.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/postproc-55.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/swresample-3.dll",
                "ffmpeg/ffmpeg-4.x-win64-shared/bin/swscale-5.dll",
                "node_modules/ffmpeg-ffprobe-static/ffmpeg.exe",
                "node_modules/ffmpeg-ffprobe-static/ffprobe.exe"
              ]
            }
          ]
    }]
  ]
}]
}

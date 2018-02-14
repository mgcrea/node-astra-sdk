{
  "targets": [{
    "target_name": "addon",
    "include_dirs": [
      "<!(node -e \"require('nan')\")"
    ],
    "conditions": [
      ["OS=='linux'", {
        "sources": [
          "src/addon.cc",
          "src/openni/openni.cc",
          "src/openni/stream_reader.cc"
        ],
        "cflags": [
          "-I<!@(pwd)/bundle/OpenNI-linux-x64/Include"
        ],
        "ldflags": [
          "-L<!@(pwd)/bundle/OpenNI-linux-x64/Redist"
        ],
        "libraries": [
          "-lOpenNI2"
        ]
      }],
      ["OS=='mac'", {
        "sources": [
          "src/addon.cc",
          "src/astra/astra.cc",
          "src/astra/stream_reader.cc"
        ],
        "xcode_settings": {
          "OTHER_CFLAGS": [
            "-I<!@(pwd)/bundle/AstraSDK-darwin-x64/include"
          ],
          "OTHER_LDFLAGS": [
            "-Wl,-bind_at_load",
            "-L<!@(pwd)/bundle/AstraSDK-darwin-x64/lib"
          ],
          "GCC_ENABLE_CPP_RTTI": "YES",
          "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
          "MACOSX_DEPLOYMENT_TARGET": "10.13",
          "CLANG_CXX_LIBRARY": "libc++",
          "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
          "GCC_VERSION": "com.apple.compilers.llvm.clang.1_0"
        },
        "link_settings": {
          "libraries": [
            "-lastra -lastra_core -lastra_core_api"
          ]
        }
      }]
    ]
  }]
}

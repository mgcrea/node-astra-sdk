{
  "targets": [{
    "target_name": "addon",
    "sources": [
      "src/addon.cc",
      "src/addon/astra.cc",
      "src/addon/stream_reader.cc"
    ],
    "include_dirs": [
      "<!(node -e \"require('nan')\")"
    ],
    "conditions": [
      ["OS=='linux'", {
        "cflags": [
          "<!@(pkg-config --cflags opencv Qt5Core)"
        ],
        "ldflags": [
          "<!@(pkg-config --libs-only-L --libs-only-other opencv Qt5Core)"
        ],
        "libraries": [
          "-lopenbr",
          "<!@(pkg-config --libs-only-l opencv Qt5Core)"
        ]
      }],
      ["OS=='mac'", {
        "xcode_settings": {
          "OTHER_CFLAGS": [
            "-I<!@(pwd)/bundle/AstraSDK/include"
          ],
          "OTHER_LDFLAGS": [
            "-Wl,-bind_at_load",
            "-L<!@(pwd)/bundle/AstraSDK/lib"
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

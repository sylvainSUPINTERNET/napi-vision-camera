{
  "targets": [
    {
      "target_name": "addon",
      "sources": ["native/addon.cpp"],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include\")",
        "<(module_root_dir)/node_modules/node-addon-api"
      ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_CPP_EXCEPTIONS"],
      "libraries": []
    }
  ]
}

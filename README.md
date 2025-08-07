# Genicam-nodejs

Standard : 

[https://www.emva.org/standards-technology/genicam/genicam-downloads/?utm_source=chatgpt.com](https://www.emva.org/standards-technology/genicam/genicam-downloads/?utm_source=chatgpt.com)


# Requirements
- Python 
- [Download "Desktop Development with C++" from Visual Studio build tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)




```` bash

npm i -g node-gyp

node-gyp install # Download include files for node-addon-api and could be use in VSCODE config "C:/Users/User/AppData/Local/node-gyp/Cache/22.9.0/include/node"


npm i 

npm run build

npm run test

npm publish


npx node-gyp clean # error with LINKER

````


# Conf exemple

`.vscode/c_cpp_properties.json`

```` json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/node_modules/node-addon-api",
                "C:/Users/User/AppData/Local/node-gyp/Cache/22.9.0/include/node"
            ], 
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.22000.0",
            "compilerPath": "cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}

````

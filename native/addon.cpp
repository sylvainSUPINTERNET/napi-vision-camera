#pragma once

#include <napi.h>
#include "GenTL.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <dlfcn.h>
#endif 

using namespace std;


Napi::String Hello(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "Hello from C++!");
}


void loadCtiFile(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if ( info.Length() < 1 || !info[0].IsString() ) {
    Napi::TypeError::New(env, "Expected file path as argument").ThrowAsJavaScriptException();
    return;
  }

  Napi::String ctiPath = info[0].As<Napi::String>();

  #ifdef _WIN32

    HMODULE gentl = LoadLibraryA(ctiPath.Utf8Value().c_str());
   
    if (!gentl) {
      DWORD error = GetLastError();
      std::cerr << "LoadLibraryA failed with error code: " << error << std::endl;
      Napi::Error::New(env, "Failed to load cti file: " + ctiPath.Utf8Value()).ThrowAsJavaScriptException();
      return;
    }

    GenTL::PGCInitLib GCInitLib = (GenTL::PGCInitLib)GetProcAddress(gentl, "GCInitLib");
    GenTL::PTLOpen fnTLOpen = (GenTL::PTLOpen)GetProcAddress(gentl, "TLOpen");
    GenTL::PTLUpdateInterfaceList fnTLUpdateInterfaceList = (GenTL::PTLUpdateInterfaceList)GetProcAddress(gentl, "TLUpdateInterfaceList");
    
    //GC_API_P(PTLUpdateInterfaceList   )( TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout );


    if ( GCInitLib ) {
      GenTL::GC_ERROR status = GCInitLib();
      if (status != GenTL::GC_ERR_SUCCESS) {
        Napi::Error::New(env, "GCInitLib failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
        return;
      }
    } else {
      Napi::Error::New(env, "Failed to get GCInitLib from CTI: " + ctiPath.Utf8Value()).ThrowAsJavaScriptException();
      return;
    }

    if ( fnTLOpen) {
        GenTL::TL_HANDLE tlHandle = nullptr;
      GenTL::GC_ERROR status = fnTLOpen(&tlHandle);
      if (status != GenTL::GC_ERR_SUCCESS) {
        Napi::Error::New(env, "TLOpen failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
        return;
      } else {
        std::cout << "TLOpen succeeded, TL_HANDLE: " << tlHandle << std::endl;


        bool8_t changed = true;
        uint64_t timeout = 1000; // 1 second timeout
        if ( fnTLUpdateInterfaceList ) {
          GenTL::GC_ERROR status = fnTLUpdateInterfaceList(tlHandle, &changed, timeout);
          if (status != GenTL::GC_ERR_SUCCESS) {
            Napi::Error::New(env, "TLUpdateInterfaceList failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
            return;
          } else {
            std::cout << "TLUpdateInterfaceList succeeded, changed: " << (changed ? "true" : "false") << std::endl;
          
          }

      }
    }
  }




    // TODO end => call GCCloseLib()


    
    #elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
      // TODO
    #endif

}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("hello", Napi::Function::New(env, Hello));
  exports.Set("loadCtiFile", Napi::Function::New(env, loadCtiFile));
  return exports;
}




NODE_API_MODULE(addon, Init)

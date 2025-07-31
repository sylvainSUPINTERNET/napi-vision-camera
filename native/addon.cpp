#pragma once

#include <napi.h>
#include "GenTL.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <dlfcn.h>
#endif 


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
      Napi::Error::New(env, "Failed to load cti file: " + ctiPath.Utf8Value()).ThrowAsJavaScriptException();
      return;
    }

    GenTL::PGCInitLib GCInitLib = (GenTL::PGCInitLib)GetProcAddress(gentl, "GCInitLib");

    if (GCInitLib) {
      GenTL::GC_ERROR status = GCInitLib();
      if (status != GenTL::GC_ERR_SUCCESS) {
        Napi::Error::New(env, "GCInitLib failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
        return;
      }
    } else {
      Napi::Error::New(env, "Failed to get GCInitLib from CTI: " + ctiPath.Utf8Value()).ThrowAsJavaScriptException();
      return;
    }


    
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

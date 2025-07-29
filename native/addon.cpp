#pragma once

#include <napi.h>
#include "GenTL.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <dlfcn.h>
#endif 




// typedef GC_ERROR (GC_CALLTYPE *PFN_GCInitLib)(void);
// PFN_GCInitLib GCInitLib_ptr = reinterpret_cast<PFN_GCInitLib>(
//     GetProcAddress(gentlLib, "GCInitLib"));

Napi::String Hello(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "Hello from C++!");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("hello", Napi::Function::New(env, Hello));
  return exports;
}


void LoadCtiFile(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if ( info.Length() < 1 || !info[0].IsString() ) {
    Napi::TypeError::New(env, "Expected file path as argument").ThrowAsJavaScriptException();
    return;
  }

  Napi::String ctiPath = info[0].As<Napi::String>();

  HMODULE gentl = LoadLibraryA(ctiPath.Utf8Value().c_str());
  if (!gentl) {
    Napi::Error::New(env, "Failed to load cti file: " + ctiPath.Utf8Value()).ThrowAsJavaScriptException();
    return;
  }



}



NODE_API_MODULE(addon, Init)

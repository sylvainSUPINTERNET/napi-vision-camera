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
    GenTL::PTLGetNumInterfaces fnTLGetNumInterfaces = (GenTL::PTLGetNumInterfaces)GetProcAddress(gentl, "TLGetNumInterfaces");
    GenTL::PTLGetInterfaceID fnTLGetInterfaceID = (GenTL::PTLGetInterfaceID)GetProcAddress(gentl, "TLGetInterfaceID");
    GenTL::PTLGetInterfaceInfo fnTLGetInterfaceInfo = (GenTL::PTLGetInterfaceInfo)GetProcAddress(gentl, "TLGetInterfaceInfo");

 //    GC_API_P(PTLGetInterfaceInfo      )( TL_HANDLE hTL, const char *sIfaceID, INTERFACE_INFO_CMD iInfoCmd, INFO_DATATYPE *piType, void *pBuffer, size_t *piSize );


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

            uint32_t numIfaces = 0;
            if ( fnTLGetNumInterfaces ) {
              fnTLGetNumInterfaces(tlHandle, &numIfaces);
              if ( numIfaces == 0 ) {
                Napi::Error::New(env, "No interfaces found").ThrowAsJavaScriptException();
                return;
              } else {
                std::cout << "Number of interfaces found: " << numIfaces << std::endl;
                if ( fnTLGetInterfaceID ) {
                  for ( uint32_t i=0; i < numIfaces; ++i ) {
                    size_t idSize = 0;
                    fnTLGetInterfaceID(tlHandle, i, nullptr, &idSize);
                    std::vector<char> buf(idSize); // allocate and destroy automatically
                    GenTL::GC_ERROR status = fnTLGetInterfaceID(tlHandle, i, buf.data(), &idSize);
                    if (status != GenTL::GC_ERR_SUCCESS) {
                      Napi::Error::New(env, "TLGetInterfaceID failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
                      return;
                    }
                    std::string ifaceID(buf.data(), idSize);
                    std::cout << "Interface ID " << i << ": " << ifaceID << std::endl;
                  }
                }

                // if (fnTLGetInterfaceInfo) {
                //   for ( uint32_t i = 0; i < numIfaces; ++i ) {
                //     const char *ifaceID = nullptr;
                //     GenTL::INTERFACE_INFO_DISPLAYNAME;
                //     GenTL::INTERFACE_INFO_CMD_LIST cmd = GenTL::INTERFACE_INFO_DISPLAYNAME;
                //     GenTL::INFO_DATATYPE type = 1;
                //     size_t size = 0;
                //     GenTL::GC_ERROR status = fnTLGetInterfaceInfo(tlHandle, ifaceID, cmd, &type, nullptr, &size);
                //     if (status != GenTL::GC_ERR_SUCCESS) {
                //       Napi::Error::New(env, "TLGetInterfaceInfo failed with status code: " + std::to_string(status)).ThrowAsJavaScriptException();
                //       return;
                //     }
                //   }
                // }


              }
            }
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


/*

Après avoir chargé ta lib avec GCInitLib() et chargé la .cti, tu fais :

TLOpen() – ouvre une instance du transport layer
→ Donne un TL_HANDLE (hTL)

TLUpdateInterfaceList(hTL, ...)
→ Rafraîchit la liste des interfaces réseau/USB/etc.

TLGetNumInterfaces(hTL, &numIfaces)
→ Combien d'interfaces sont dispo ? (GigE, USB3, etc.)

TLGetInterfaceID(hTL, i, ...)
→ Récupère l’ID d’une interface

TLOpenInterface(hTL, ifaceID, &hIF)
→ Ouvre une interface (IF_HANDLE)

IFUpdateDeviceList(hIF, ...)
→ Scanne les caméras connectées

IFGetNumDevices(hIF, &numDevices)
→ Combien de caméras trouvées ?

IFGetDeviceID(hIF, i, ...)
→ ID de la caméra (tu peux l’ouvrir ensuite)

*/
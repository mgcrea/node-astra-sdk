#include <nan.h>

#ifdef __APPLE__
  #include "astra/astra.h"
  #include "astra/stream_reader.h"
#endif

#ifdef WIN32
#else
  #include "openni/openni.h"
  #include "openni/stream_reader.h"
#endif

void InitAll(v8::Local<v8::Object> exports) {
  StreamReader::Init();

  exports->Set(Nan::New("initialize").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(Initialize)->GetFunction());
  exports->Set(Nan::New("terminate").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(Terminate)->GetFunction());
}

NODE_MODULE(addon, InitAll)

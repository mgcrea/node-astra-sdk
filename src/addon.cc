#include <nan.h>
#include "addon/astra.h"
#include "addon/stream_reader.h"

void InitAll(v8::Local<v8::Object> exports) {
  StreamReader::Init();

  exports->Set(Nan::New("initialize").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(Initialize)->GetFunction());
  exports->Set(Nan::New("terminate").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(Terminate)->GetFunction());
}

NODE_MODULE(addon, InitAll)

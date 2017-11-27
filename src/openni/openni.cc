#include <cstdlib>

#include "openni.h"  // NOLINT(build/include)
#include "stream_reader.h"

using namespace openni;

void Initialize(const Nan::FunctionCallbackInfo<v8::Value>& info) {

  v8::Local<v8::Function> callback = info[0].As<v8::Function>();

  Status rc = OpenNI::initialize();
  if (rc != STATUS_OK) {
    printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
    return;
  }

  v8::Local<v8::Object> streamReader = StreamReader::NewInstance(info[0]);

  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = { Nan::Undefined(), streamReader };

  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);
}

void Terminate(const Nan::FunctionCallbackInfo<v8::Value>& info) {

  v8::Local<v8::Function> callback = info[0].As<v8::Function>();

	OpenNI::shutdown();

  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = { Nan::Undefined(), Nan::New(true) };

  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);

  return;
}

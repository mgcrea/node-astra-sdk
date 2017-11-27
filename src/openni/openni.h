#ifndef INITIALIZE_H
#define INITIALIZE_H

#include <nan.h>
#include <OpenNI.h>

void Initialize(const Nan::FunctionCallbackInfo<v8::Value>& info);
void Terminate(const Nan::FunctionCallbackInfo<v8::Value>& info);

#endif

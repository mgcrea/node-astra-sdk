#ifndef STREAM_READER_H
#define STREAM_READER_H

#include <nan.h>
#include <stdio.h>
#include <OpenNI.h>

class StreamReader : public Nan::ObjectWrap {
 public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(v8::Local<v8::Value> arg);
  double Val() const { return val_; }

 private:
  StreamReader();
  ~StreamReader();

  static Nan::Persistent<v8::Function> constructor;
  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Close(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StartDepthStream(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GetDepthFrame(const Nan::FunctionCallbackInfo<v8::Value>& info);

  double val_;
  openni::Device device_;
  openni::VideoStream depth_;
};

#endif

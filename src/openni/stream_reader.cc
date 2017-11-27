#include <node.h>
#include <node_buffer.h>

#include "stream_reader.h"
#include "common/helpers.h"

StreamReader::StreamReader() {};
StreamReader::~StreamReader() {};

Nan::Persistent<v8::Function> StreamReader::constructor;

void StreamReader::Init() {
  Nan::HandleScope scope;

  // Prepare constructor template (contructor)
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("StreamReader").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "startDepthStream", StartDepthStream);
  Nan::SetPrototypeMethod(tpl, "getDepthFrame", GetDepthFrame);
  Nan::SetPrototypeMethod(tpl, "close", Close);

  constructor.Reset(tpl->GetFunction());
}

void StreamReader::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {

  StreamReader* obj = new StreamReader();

  openni::Status rc = obj->device_.open(openni::ANY_DEVICE);
  if (rc != openni::STATUS_OK) {
    printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
    return;
  }

  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object> StreamReader::NewInstance(v8::Local<v8::Value> arg) {
  Nan::EscapableHandleScope scope;

  const unsigned argc = 0;
  v8::Local<v8::Value> argv[argc] = {};
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  return scope.Escape(instance);
}

// void StreamReader::Close(const Nan::FunctionCallbackInfo<v8::Value>& info) {
NAN_METHOD(StreamReader::Close) {
  Nan::HandleScope scope;

  StreamReader* obj = ObjectWrap::Unwrap<StreamReader>(info.This());
  v8::Local<v8::Function> callback = info[0].As<v8::Function>();
  // printf("StreamReader::Close\n");

  obj->depth_.stop();
  obj->depth_.destroy();
  obj->device_.close();

  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = { Nan::Undefined(), Nan::New(true) };

  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);
}

// void StreamReader::StartDepthStream(const Nan::FunctionCallbackInfo<v8::Value>& info) {
NAN_METHOD(StreamReader::StartDepthStream) {
  Nan::HandleScope scope;

  StreamReader* obj = ObjectWrap::Unwrap<StreamReader>(info.This());
  v8::Local<v8::Function> callback = info[0].As<v8::Function>();
  // printf("StreamReader::StartDepthStream\n");

  if (obj->device_.getSensorInfo(openni::SENSOR_DEPTH) == NULL) {
    printf("Couldn't get depth sensor\n%s\n", openni::OpenNI::getExtendedError());
    return;
  }

  openni::Status rc;
  rc = obj->depth_.create(obj->device_, openni::SENSOR_DEPTH);
  if (rc != openni::STATUS_OK) {
    printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
    return;
  }

  rc = obj->depth_.start();
  if (rc != openni::STATUS_OK) {
    printf("Couldn't start depth stream\n%s\n", openni::OpenNI::getExtendedError());
    return;
  }

  if (!obj->depth_.isValid()) {
		printf("No valid streams. Exiting\n");
		return;
	}

  /*
  openni::VideoMode videoMode1 = m_depth1.getVideoMode();
  openni::VideoMode videoMode2 = m_depth2.getVideoMode();

  if (videoMode1.getResolutionX() != videoMode2.getResolutionX() ||
    videoMode1.getResolutionY() != videoMode2.getResolutionY())
  {
    printf("Streams need to match resolution.\n");
    return openni::STATUS_ERROR;
  }

  m_width = videoMode1.getResolutionX();
  m_height = videoMode1.getResolutionY();
  */

  v8::Local<v8::Object> res = Nan::New<v8::Object>();
  // res->Set(Nan::New("hFov").ToLocalChecked(), Nan::New<v8::Number>(hFov));
  // res->Set(Nan::New("vFov").ToLocalChecked(), Nan::New<v8::Number>(vFov));

  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = { Nan::Undefined(), res };

  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);
}

// void StreamReader::GetDepthFrame(const Nan::FunctionCallbackInfo<v8::Value>& info) {
NAN_METHOD(StreamReader::GetDepthFrame) {
  Nan::HandleScope scope;

  StreamReader* obj = ObjectWrap::Unwrap<StreamReader>(info.This());

  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  v8::Local<v8::Object> options = info[0].As<v8::Object>();
  v8::Local<v8::Function> callback = info[1].As<v8::Function>();
  // v8::Local<v8::String> v8Str = Nan::To<v8::String>(options->Get(Nan::New("x").ToLocalChecked())).ToLocalChecked();

  // Get timeout
  int timeout = (int)v8::Local<v8::Number>::Cast(options->Get(Nan::New("timeout").ToLocalChecked()))->Value();

  int changedIndex = -1;
  clock_t t1 = clock();
  openni::VideoStream** streams = new openni::VideoStream*[2];
	streams[0] = &obj->depth_;
  openni::Status rc = openni::OpenNI::waitForAnyStream(streams, 1, &changedIndex, timeout);

  if (rc != openni::STATUS_OK) {
		printf("Wait failed\n");
		return;
	}

  openni::VideoFrameRef frame;
	switch (changedIndex) {
	case 0:
		obj->depth_.readFrame(&frame);
    break;
	default:
		printf("Error in wait\n");
	}

  const openni::DepthPixel* depthData = (const openni::DepthPixel*)frame.getData();
  int depthLength = frame.getDataSize();
  int frameIndex = frame.getFrameIndex();
  int width = frame.getWidth();
  int height = frame.getHeight();
  // uint64_t timestamp = frame.getTimestamp();
  float elapsed = ((float)(clock() - t1) / CLOCKS_PER_SEC) * 1000;

  // Compute extremes
  int maxX = 0;
  int maxY = 0;
  short maxZ = SHRT_MIN;

  int minX = 0;
  int minY = 0;
  short minZ = SHRT_MAX;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = y * width + x;
      if (depthData[i] > maxZ) {
        maxZ = depthData[i];
        maxX = x;
        maxY = y;
      }
      if (depthData[i] > 0 && depthData[i] < minZ) {
        minZ = depthData[i];
        minX = x;
        minY = y;
      }
    }
  }
  if (maxZ == 0) {
    minZ = 0;
  }

  // Prepare response
  v8::Local<v8::Object> res = Nan::New<v8::Object>();
  res->Set(Nan::New("index").ToLocalChecked(), Nan::New<v8::Number>(frameIndex));
  res->Set(Nan::New("delay").ToLocalChecked(), Nan::New<v8::Number>(elapsed));
  res->Set(Nan::New("width").ToLocalChecked(), Nan::New<v8::Number>(width));
  res->Set(Nan::New("height").ToLocalChecked(), Nan::New<v8::Number>(height));
  v8::Local<v8::Object> data = Nan::NewBuffer(depthLength).ToLocalChecked();
  memcpy(node::Buffer::Data(data), depthData, depthLength);
  res->Set(Nan::New("data").ToLocalChecked(), data);

  // Return extremes
  v8::Local<v8::Object> min = Nan::New<v8::Object>();
  min->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(minX));
  min->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(minY));
  min->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(minZ));
  res->Set(Nan::New("min").ToLocalChecked(), min);
  v8::Local<v8::Object> max = Nan::New<v8::Object>();
  max->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(maxX));
  max->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(maxY));
  max->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(maxZ));
  res->Set(Nan::New("max").ToLocalChecked(), max);


  const unsigned argc = 2;
  v8::Local<v8::Value> argv[argc] = { Nan::Undefined(), res };

  Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);
}

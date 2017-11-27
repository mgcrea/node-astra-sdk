#include <node.h>
#include <node_buffer.h>

#include "stream_reader.h"
#include "common/helpers.h"

StreamReader::StreamReader(astra_streamsetconnection_t sensor, astra_reader_t reader) : sensor_(sensor), reader_(reader) {};
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

  astra_streamsetconnection_t sensor;
  astra_streamset_open("device/default", &sensor);

  astra_reader_t reader;
  astra_reader_create(sensor, &reader);

  StreamReader* obj = new StreamReader(sensor, reader);

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

  astra_reader_destroy(&obj->reader_);
  astra_streamset_close(&obj->sensor_);

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

  astra_reader_get_depthstream(obj->reader_, &obj->depthStream_);

  /*

  astra_result_token_t token;
  size_t count = 0;
  astra_imagestream_request_modes(obj->depthStream_, &token, &count);

  astra_imagestream_mode_t allmodes[100];
  memset(allmodes, 0, sizeof(astra_imagestream_mode_t) * 100);

  if (count > 100)
      count = 100;

  astra_imagestream_get_modes_result(obj->depthStream_, token, allmodes, count);

  unsigned int i;
  for (i = 0; i < count; i++)
  {
      astra_imagestream_mode_t* t = &allmodes[i];
      if (allmodes[i].width == desired_width
              && allmodes[i].height == desired_height
              && allmodes[i].fps == desired_fps
              && allmodes[i].pixelFormat == desired_pixelformat)
      {
          selected_mode_index = i;
      }

      fprintf(stderr, "fps: %d, id: %d, width: %d, height: %d, format: %d\n", (int)t->fps, (unsigned int)t->id, t->width, t->height, t->pixelFormat);
  }

  astra_imagestream_mode_t used_videomode = allmodes[0];
  int rc = astra_imagestream_set_mode(obj->depthStream_, &used_videomode);
  if (rc != ASTRA_STATUS_SUCCESS) {
      fprintf(stderr, "Failed setting video mode %d\n", rc);
  }
  */

  float hFov, vFov;
  astra_depthstream_get_hfov(obj->depthStream_, &hFov);
  astra_depthstream_get_vfov(obj->depthStream_, &vFov);

  astra_stream_start(obj->depthStream_);

  v8::Local<v8::Object> res = Nan::New<v8::Object>();
  res->Set(Nan::New("hFov").ToLocalChecked(), Nan::New<v8::Number>(hFov));
  res->Set(Nan::New("vFov").ToLocalChecked(), Nan::New<v8::Number>(vFov));

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

  // astra_reader_callback_id_t callbackId;
  // astra_reader_register_frame_ready_callback(obj->reader_, &frame_ready, NULL, &callbackId);
  astra_reader_frame_t frame;
  astra_status_t rc;
  clock_t t1, t2;
  float elapsed;
  t1 = clock();

  do {
    astra_temp_update();
    rc = astra_reader_open_frame(obj->reader_, 0, &frame);
    t2 = clock();
    elapsed = ((float)(t2 - t1) / CLOCKS_PER_SEC) * 1000;
  } while (rc == ASTRA_STATUS_TIMEOUT && elapsed < timeout);

  if (rc != ASTRA_STATUS_SUCCESS) {
    const unsigned argc = 2;
    v8::Local<v8::Value> error;
    if (rc == ASTRA_STATUS_TIMEOUT) {
      error = Nan::Error(format("Failed to open camera stream within given timeout: %d", timeout));
    } else {
      error = Nan::Error(format("Unexpected camera error: %d", rc));
    }

    v8::Local<v8::Value> argv[argc] = { error, Nan::Undefined() };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, argc, argv);
    return;
  }

  astra_depthframe_t depthFrame;
  astra_frame_get_depthframe(frame, &depthFrame);

  astra_frame_index_t frameIndex;
  astra_depthframe_get_frameindex(depthFrame, &frameIndex);

  astra_image_metadata_t metadata;

  int16_t* depthData;
  size_t depthLength;

  astra_depthframe_get_data_byte_length(depthFrame, &depthLength);
  astra_depthframe_get_metadata(depthFrame, &metadata);

  depthData = (int16_t*)malloc(depthLength);
  astra_depthframe_copy_data(depthFrame, depthData);
  int width = metadata.width;
  int height = metadata.height;

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

  free(depthData);
  astra_reader_close_frame(&frame);
}

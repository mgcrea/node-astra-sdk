#include <nan.h>

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

#define FORMAT_MAX_BUFFER 256
char buffer[FORMAT_MAX_BUFFER] = {'\0'};

const char* format(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  // vsnprintf(buffer, FORMAT_MAX_BUFFER * 5, fmt, args);
  va_end(args);
  return buffer;
}

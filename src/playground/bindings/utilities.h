// Copyright 2015 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the MIT license, a copy of which can
// be found in the LICENSE file.

#ifndef PLAYGROUND_BINDINGS_UTILITIES_H_
#define PLAYGROUND_BINDINGS_UTILITIES_H_

#include <string>

#include <include/v8.h>

namespace bindings {

inline v8::Local<v8::String> v8String(const char* string) {
  return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), string);
}

inline v8::Local<v8::String> v8String(const std::string& string) {
  return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), string.c_str());
}

inline std::string toString(v8::Local<v8::Value> string) {
  v8::String::Utf8Value value(string);
  if (value.length())
    return std::string(*value, value.length());

  return std::string();
}

inline void ThrowException(const std::string& message) {
  v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8String(message)));
}

}  // namespace bindings

#endif  // PLAYGROUND_BINDINGS_UTILITIES_H_
#pragma once
#include <string>
#include <typeinfo>
#include <dlfcn.h>

std::string demangle(const char * name);

template <class T> std::string type(const T & t) {
  return demangle(typeid(t).name());
}

// convenient for dlopening the current program
struct dl {
  dl(const char * filename = nullptr) : handle(dlopen(filename,0)) {}
  ~dl() { dlclose(handle); }
  void * handle;
};

// identify a pointer to some built-in thing described at compile time.
template <typename T>
std::string wat(T * t) {
  Dl_info info;
  if (dladdr(reinterpret_cast<const void *>(t), &info)) {
    return info.dli_sname;
  } else {
    return std::to_string(reinterpret_cast<intptr_t>(t));
  }
}

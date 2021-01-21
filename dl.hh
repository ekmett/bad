#pragma once
#include <dlfcn.h>
#include "attrib.hh"

namespace bad {

  // convenient RAII wrapper for dlopening, usually used for the current program
  struct dl {
    dl(const char * filename = nullptr) : handle(dlopen(filename,0)) {}
    ~dl() { dlclose(handle); }
    void * handle;
  };

  // identify a pointer to some built-in thing described at compile time.
  template <class T>
  BAD_HD std::string wat(T * t) {
    Dl_info info;
    if (dladdr(reinterpret_cast<const void *>(t), &info)) {
      return info.dli_sname;
    } else {
      return std::to_string(reinterpret_cast<intptr_t>(t));
    }
  }
}

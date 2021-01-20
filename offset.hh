#pragma once

#include <cstdint>

namespace bad {
  template <class T>
  inline T * byte_offset(T *ptr, std::ptrdiff_t delta) {
    return reinterpret_cast<T*>(reinterpret_cast<std::byte *>(ptr) + delta);
  }
}

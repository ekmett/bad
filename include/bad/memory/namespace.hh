#pragma once

/// \file
/// \brief memory allocation namespace
/// \author Edward Kmett
///
/// \defgroup memory_group memory
/// \brief memory allocation

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::memory
  /// \ref memory_group "memory" internals, import bad::memory::api
  /// \ingroup memory_group
  namespace memory {
    /// \namespace bad::memory::common
    /// \ingroup memory_group
    /// re-exported by \ref bad and bad::memory::api
    namespace common {}
    /// \namespace bad::memory::api
    /// \ingroup memory_group
    /// See \ref memory_group "memory" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace memory::common;
}

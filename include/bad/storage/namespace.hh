#pragma once
#include <array>
#include "bad/attributes.hh"
#include "bad/sequences.hh"

/// \file storage.hh
/// \brief tensor storage
/// \author Edward Kmett
///
/// \defgroup storage_group storage
/// \brief tensor storage and expressions

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::storage
  /// \ref storage_group "storage" internals, import bad::storage::api
  /// \ingroup storage_group
  namespace storage {
    /// \namespace bad::storage::common
    /// \ingroup storage_group
    /// re-exported by \ref bad and bad::storage::api
    namespace common {}
    /// \namespace bad::storage::api
    /// \ingroup storage_group
    /// See \ref storage_group "storage" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace storage::common;
}

/// \{

// explicitly allow for fp shenanigans like FMA?
#pragma STDC FP_CONTRACT ON

/// tensor storage
namespace bad::storage {
  using namespace sequences::api;
  using namespace storage::api;
}

/// \}

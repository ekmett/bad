#ifndef BAD_TYPES_HH
#define BAD_TYPES_HH

#include <string>
#include <typeinfo>
#include "bad/common.hh"
#include "bad/attributes.hh"

/// \file
/// \brief type names
/// \author Edward Kmett
///
/// \defgroup types_group types
/// \brief type names

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::types
  /// \ref types_group "types" internals, import bad::types::api
  /// \ingroup types_group
  namespace types {
    /// \namespace bad::types::common
    /// \ingroup types_group
    /// re-exported by \ref bad and bad::types::api
    namespace common {}
    /// \namespace bad::types::api
    /// \ingroup types_group
    /// See \ref types_group "types" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace types::common;
}

/// \{
namespace bad::types {
  /// demangle a type name with the C++ ABI if available
  /// \ingroup types_group
  /// \private
  BAD(hd)
  std::string demangle(char const * name);
}

namespace bad::types::common {
  /// return the name of a given type given a reference
  /// \ingroup types_group
  template <class T>
  BAD(hd)
  std::string type(T const & t) noexcept {
    return demangle(typeid(t).name());
  }

  /// return the name of a given type without passing a reference
  /// \ingroup types_group
  template <class T>
  BAD(hd)
  std::string type_name() noexcept {
    return demangle(typeid(T).name());
  }
}

/// \}

#endif

#ifndef BAD_TYPES_HH
#define BAD_TYPES_HH

#include <string>
#include <typeinfo>
#include "bad/common.hh"
#include "bad/attributes.hh"

/// \file
/// \brief type names
/// \author Edward Kmett

/// \defgroup types_group types
/// \brief type names
/// \ingroup internals_group

namespace bad::types {
  /// demangle a type name with the C++ ABI if available
  /// \ingroup types_group
  /// \private
  BAD(hd)
  std::string demangle(char const * name);

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

namespace bad {
  using namespace bad::types;
}

#endif

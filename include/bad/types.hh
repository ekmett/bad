#ifndef BAD_TYPES_HH
#define BAD_TYPES_HH

#include <string>
#include <typeinfo>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

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
  BAD(hd,inline)
  static std::string demangle(char const * name) noexcept {
#ifdef __GNUG__
    int status = -4;
    std::unique_ptr<char, void(*)(void*)> res {
      abi::__cxa_demangle(name, NULL, NULL, &status),
      std::free
    };
    return (status==0) ? res.get() : name ;
#else
    return name;
#endif
  }

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

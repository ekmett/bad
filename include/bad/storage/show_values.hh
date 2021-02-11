#ifndef BAD_STORAGE_SHOW_VALUES_HH
#define BAD_STORAGE_SHOW_VALUES_HH

#include "bad/storage/store_expr.hh"

/// \file
/// \author Edward Kmett

namespace bad::storage::api {

  /// used to show the values in an expr or fixed array
  /// \ingroup storage_group
  template <size_t d, class T>
  struct BAD(nodiscard) show_values {
    T const & data;

    BAD(hd) constexpr
    show_values(BAD(lifetimebound) const T & data) noexcept
    : data(data) {};

    BAD(hd) constexpr
    show_values(BAD(lifetimebound) const T (&data)[d]) noexcept
    : data(data) {}
  };

  /// \ingroup storage_group
  template <size_t d, class T>
  BAD(hd)
  std::ostream & operator << (std::ostream &os, const show_values<d,T> & rhs) {
    os << "{";
    for (size_t i=0;i<d;++i) {
      if (i) os << ",";
      os << rhs.data[i];
    }
    return os << "}";
  }

  /// infer \ref show_values size from store_expr dimension
  /// \ingroup storage_group
  template <size_t d, size_t... ds, class B>
  show_values(store_expr<B,d,ds...> const & data)
    -> show_values<d, store_expr<B,d,ds...>>;

  /// infer \ref show_values size from array size
  /// \ingroup storage_group
  template <size_t d, class T>
  show_values(T(&)[d])
    -> show_values<d,T*>;
}

#endif

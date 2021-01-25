#pragma once

#include <cstdint>
#include <utility>
#include "attributes.hh"
#include "errors.hh"
#include "sequences.hh"

/// @file lists.hh
/// @brief type level heterogeneous lists
/// @author Edward Kmett
///
/// @defgroup lists lists 
/// @brief type level heterogeneous lists
///
/// @{

namespace bad {
  /// type level lists
  namespace lists {
    /// re-exported by \ref bad and \ref bad::lists::api "api"
    namespace common {}
    /// public components
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace lists::common;
}

namespace bad::lists {
  using namespace sequences::api;
  using namespace errors::api;

  using std::size_t;
  using std::ptrdiff_t;

  namespace common {
    /// heterogeneous lists
    /// @ingroup lists
    template <class...>
    struct BAD(empty_bases) list {};
  }

  /// @private
  template <class, class L>
  struct list_cons_ {
    static_assert(no<L>,"list_cons: not a list");
  };

  /// @private
  template <class x, class... xs>
  struct list_cons_<x,list<xs...>> {
    using type = list<x, xs...>;
  };

  namespace api {
    /// prepend a type onto a heterogeneous list
    /// @ingroup lists
    template <class x, class xs>
    using list_cons = typename list_cons_<x,xs>::type;
  }

  /// @private
  template <class L>
  struct list_head_ {
    static_assert(no<L>,"list_head: not a list");
  };

  /// @private
  template <class x, class... xs>
  struct list_head_<list<x,xs...>> {
    using type = x;
  };

  namespace api {
    /// extract the head of a heterogeneous list
    /// @ingroup lists
    template <class L>
    using list_head = typename list_head_<L>::type;
  }

  namespace common {
    /// inferrable `std::integral_constant`, used to encode lists as heterogenous lists
    /// @ingroup lists
    template <auto x>
    using int_t = std::integral_constant<decltype(x), x>;
  }

  /// @private
  template <class>
  struct seq_list_;

  /// @private
  template <class T, T... is>
  struct seq_list_<seq_t<T,is...>> {
    using type = list<int_t<is>...>;
  };

  namespace api {
    /// convert a sequence to a heteregeneous list of integral constants
    /// @ingroup lists
    template <class S>
    using seq_list = typename seq_list_<S>::type;
  }

  /// @private
  template <class L>
  struct list_zip_ {
    static_assert(no<L>,"list_zip: first arg is not a list");
  };

  /// @private
  template <>
  struct list_zip_<list<>> {
    /// @private
    template <class>
    struct at {
      using type = list<>;
    };
  };

  /// @private
  template <class x, class... xs>
  struct list_zip_<list<x,xs...>> {
    /// @private
    template <class R>
    struct at {
      static_assert(no<R>,"list_zip: second arg is not a list");
    };

    /// @private
    template <>
    struct at<list<>> {
      using type = list<>;
    };

    /// @private
    template <class y, class... ys>
    struct at<list<y,ys...>> {
      using type = list_cons<std::tuple<x,y>,typename list_zip_<xs...>::template at<ys...>::type>;
    };
  };

  namespace api {
    /// zip two heterogenous lists together to form a heterogeneous list of tuples
    /// @ingroup lists
    template <class X, class Y>
    using list_zip = typename list_zip_<X>::template at<Y>::type;
  }

  /// @private
  template <class, class L>
  struct list_seq_ {
    static_assert(no<L>,"list_seq: not a homogeneous list of integral constants");
  };

  /// @private
  template <class T>
  struct list_seq_<T,list<>> {
    using type = seq_t<T>;
  };

  /// @private
  template <class T, T i, class... xs>
  struct list_seq_<T,list<int_t<i>,xs...>> {
    using type = seq_cons<i,typename list_seq_<T,list<xs...>>::type>;
  };

  namespace api {
    /// convert a list of integral constants, each of type T to a sequence
    /// @ingroup lists
    template <class T, class S>
    using list_seq = typename list_seq_<T,S>::type;
  }

  /// @private
  template <template <class...> class, class L>
  struct list_apply_ {
    static_assert(no<L>, "list_apply: not a list");
  };

  /// @private
  template <template <class...> class F, class... xs>
  struct list_apply_<F,list<xs...>> {
    using type = F<xs...>;
  };

  namespace api {
    /// pass each type in a heterogeneous list to a given template
    /// @ingroup lists
    template <template <class...> class F, class L>
    using list_apply = typename list_apply_<F,L>::type;
  }
}

/// @}

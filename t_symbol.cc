#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "symbol.hh"

using namespace bad;
using namespace std;

constexpr size_t well_formed(symbol s) {
  constexpr symbol arr = "->";
  std::size_t p = s.index(arr);
  if (p == symbol::npos) return false;
  if (s.drop(p+2).count(',') != 0) return false;
  return true;
  //static_assert(p != symbol::npos, "missing -> in einsum description");
  //static_assert(s.drop(p + 2).count(',') != 0,"tuple results unsupported");
  //return s.take(p).count(',') + 1;
}

TEST_CASE( "symbol works", "[symbol]" ) {
  { 
    constexpr symbol x = "hello";
    constexpr char o = x[4];
    REQUIRE(o == 'o');
    {
      constexpr size_t n = x.size();
      REQUIRE(n == 5);
    }
    { 
      constexpr symbol y = "llo";
      REQUIRE(x != y);
      REQUIRE(x.drop(2) == y);
      REQUIRE(x.take(2) == "he");
      { 
        constexpr size_t loc = x.index(y);
        REQUIRE(loc == 2);
      }
      { 
        constexpr bool cmp_is_constexpr = x.take(2) == "he";
        REQUIRE(cmp_is_constexpr);
      }
    }
  }

  { 
    constexpr symbol t = "ij,jk->ik";
    constexpr size_t n = t.count(',');
    REQUIRE(n == 1);
    REQUIRE(1 == t.count(','));
  }
  // we can judge at compile time if an einsum string is well formed.
  constexpr bool wfe = well_formed("ij,jk->ik");
  REQUIRE(wfe);

  // goal: well formed, compile time checked.
  // auto z = einsum("ij,jk->ik")(x,y);
}


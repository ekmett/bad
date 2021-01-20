#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "abi.hh"
#include "str.hh"
#include "map.hh"

using namespace std;
using namespace bad;

TEST_CASE("reify works","[str]") {
   REQUIRE(sizeof(reify<str<'c','a','t'>>) == 3);
   REQUIRE(reify<str<'c','a','t'>>[1] == 'a');
   REQUIRE(&reify<str<'c','a','t'>> == &reify<str<'c','a','t'>>);
}

TEST_CASE("_str", "[str]") {
  auto x = "wat"_str; // gives me back a thing of that type
  REQUIRE(sizeof(reify<decltype("wat"_str)>) == 3);
}

#define S(str) decltype(#str ## _str)
TEST_CASE("macro works", "[str]") {
  REQUIRE(reify<S(wat)>[2] == 't');
  REQUIRE(sizeof(reify<S(wat)>) == 3);
}

#define op(...) list<BAD_MAP_LIST(S,__VA_ARGS__)>

TEST_CASE("einsum sketch", "[str]") {
  op(ij,jk,ik) foo;
  cout << "matrix-matrix: " << type(foo) << endl;
  op(i,i,) dot;
  cout << "dot-product: " << type(dot) << endl;
  op(,i,i) sv;
  cout << "scalar-vector: " << type(sv) << endl;
  op(ij,i,j) mv;
  cout << "matrix-vector: " << type(mv) << endl;
  op(i,) sum;
  cout << "sum: " << type(sum) << endl;
  op(ij,ji) transpose;
  cout << "transpose: " << type(transpose) << endl;
  op(ij,ij,ij) hadamard;
  cout << "hadamard product: " << type(hadamard) << endl;
  op(ii,) trace;
  cout << "trace: " << type(trace) << endl;
  op(i,j,ij) outer;
  cout << "outer product: " << type(outer) << endl;
}

// auto z = einsum(i,i,)(x,y) // dot
// auto z = einsum(ij,jk,il)(x,y) // mmul
// auto z = einsum(ij,i,j)(m,v) // matrix vector multiplication
// auto z = einsum(,i,i)(s,v) // scalar vector multiplication

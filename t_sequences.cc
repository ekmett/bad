#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "types.hh"
#include "sequences.hh"
#include "lists.hh"

using namespace std;
using namespace bad;
using namespace bad::sequences::api;
using namespace bad::lists::api;

TEST_CASE("seq_head works", "[sequences]") {
  REQUIRE(seq_head<seq<1,2,3>> == 1 );
  REQUIRE(seq_head<iseq<size_t,4,2,1> > == 4 );
}

TEST_CASE("stride works","[sequences]") {
  REQUIRE(stride<0,7,5,3> == 15);
  REQUIRE(stride<1,7,5,3> == 3);
  REQUIRE(stride<2,7,5,3> == 1);
}

TEST_CASE("prod works","[sequences]") {
  REQUIRE(prod<4,2,3> == 24 );
}

TEST_CASE("nth works","[sequences]") {
  REQUIRE(nth<4,0,10,20,30,40,50,60> == 40);
}

TEST_CASE("seq_nth works","[sequences]") {
  REQUIRE(seq_nth<4,seq<0,11,21,31,41,51>> == 41);
}
TEST_CASE("seq_length works","[sequences]") {
  REQUIRE(seq_length<seq<3,2,1>> == 3);
  // FAIL(seq_length<int> == 123)
}

TEST_CASE("reify works","[sequences]") {
   REQUIRE(sizeof(reify<str<'c','a','t'>>) == 3);
   REQUIRE(reify<str<'c','a','t'>>[1] == 'a');
   REQUIRE(&reify<str<'c','a','t'>> == &reify<str<'c','a','t'>>);
}

TEST_CASE("_str", "[sequences]") {
  BAD(maybe_unused) auto x = "wat"_str; // gives me back a thing of that type
  REQUIRE(sizeof(reify<decltype("wat"_str)>) == 3);
}

#define S(str) decltype(#str ## _str)
TEST_CASE("macro works", "[sequences]") {
  REQUIRE(reify<S(wat)>[2] == 't');
  REQUIRE(sizeof(reify<S(wat)>) == 3);
}

#define M(x) #x ## s
TEST_CASE("map works","[sequences]") {
  std::string strings[] = { BAD_MAP_LIST(M,hello,world) }; // testing empty strings
  REQUIRE(sizeof(strings) / sizeof(string) == 2);
  REQUIRE(strings[1][1] == 'o');
  std::string empty[] = { BAD_MAP_LIST(M,,hello,,world,) }; // testing empty strings
  REQUIRE(sizeof(empty) / sizeof(string) == 5);
}

#define op(...) BAD(maybe_unused) list<BAD_MAP_LIST(S,__VA_ARGS__)>

TEST_CASE("einsum sketch", "[sequences]") {
  op(ij,jk,ik) mul;
  //cout << "matrix-matrix: " << type(mul) << endl;
  op(i,i,) dot;
  //cout << "dot-product: " << type(dot) << endl;
  op(,i,i) sv;
  //cout << "scalar-vector: " << type(sv) << endl;
  op(ij,i,j) mv;
  //cout << "matrix-vector: " << type(mv) << endl;
  op(i,) sum;
  //cout << "sum: " << type(sum) << endl;
  op(ij,ji) transpose;
  //cout << "transpose: " << type(transpose) << endl;
  op(ij,ij,ij) hadamard;
  //cout << "hadamard product: " << type(hadamard) << endl;
  op(ii,) trace;
  //cout << "trace: " << type(trace) << endl;
  op(i,j,ij) outer;
  //cout << "outer product: " << type(outer) << endl;
}

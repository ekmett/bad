#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "abi.hh"
#include "seq.hh"

using namespace std;
using namespace bad;

TEST_CASE("seq_head works", "[shape]") {
  REQUIRE(seq_head<seq<1,2,3>> == 1 );
  REQUIRE(seq_head<seq_t<size_t,4,2,1> > == 4 );
}

TEST_CASE("stride works","[shape]") {
  REQUIRE(stride<0,7,5,3> == 15);
  REQUIRE(stride<1,7,5,3> == 3);
  REQUIRE(stride<2,7,5,3> == 1);
}

TEST_CASE("prod works","[shape]") {
  REQUIRE(prod<4,2,3> == 24 );
}

TEST_CASE("nth works","[shape]") {
  REQUIRE(nth<4,0,10,20,30,40,50,60> == 40);
}

TEST_CASE("seq_nth works","[shape]") {
  REQUIRE(seq_nth<4,seq<0,11,21,31,41,51>> == 41);
}
TEST_CASE("seq_length works","[shape]") {
  REQUIRE(seq_length<seq<3,2,1>> == 3);
}

TEST_CASE("reify works","[str]") {
   REQUIRE(sizeof(reify<str<'c','a','t'>>) == 3);
   REQUIRE(reify<str<'c','a','t'>>[1] == 'a');
   REQUIRE(&reify<str<'c','a','t'>> == &reify<str<'c','a','t'>>);
}

TEST_CASE("_str", "[str]") {
  BAD(maybe_unused) auto x = "wat"_str; // gives me back a thing of that type
  REQUIRE(sizeof(reify<decltype("wat"_str)>) == 3);
}

#define S(str) decltype(#str ## _str)
TEST_CASE("macro works", "[str]") {
  REQUIRE(reify<S(wat)>[2] == 't');
  REQUIRE(sizeof(reify<S(wat)>) == 3);
}

#define M(x) #x ## s
TEST_CASE("map works","[str]") {
  std::string strings[] = { BAD_MAP_LIST(M,hello,world) }; // testing empty strings
  REQUIRE(sizeof(strings) / sizeof(string) == 2);
  REQUIRE(strings[1][1] == 'o');
  std::string empty[] = { BAD_MAP_LIST(M,,hello,,world,) }; // testing empty strings
  REQUIRE(sizeof(empty) / sizeof(string) == 5);
}

#define op(...) BAD(maybe_unused) list<BAD_MAP_LIST(S,__VA_ARGS__)>

TEST_CASE("einsum sketch", "[str]") {
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

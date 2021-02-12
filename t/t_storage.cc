#include <iostream>
#include "catch.hh"
#include "bad/storage.hh"
#include "bad/types.hh"

using namespace bad;
using namespace std;

TEST_CASE( "store works", "[storage]" ) {
  REQUIRE(std::is_standard_layout_v<store<int,seq<4,5>>>);

  store<int,seq<4,5>> s;



  cout << s << endl;
  s = { 40,30,20,10}; // set rows
  cout << s << endl;
  s[1][1] = 2; // set individual elements
  cout << s << endl;
  s[2] = 3; // set an entire row
  cout << s << endl;
  s[3] = {1,2,3}; // set part of a row, element by element, this clobbers the remaining elements in the row/column to match assignment from a { } initialized store
  cout << s << endl;

  // various expected failures
  // store<int,seq<43,21>,int> y;
  // store<int,seq<43,21>,seq<2>> y; 

  REQUIRE(sizeof(s) == 4*5*sizeof(int));
  REQUIRE(s[1][1] == 2);
  REQUIRE(s[3][4] == 0);
  REQUIRE(static_cast<void*>(&s) == static_cast<void*>(&s[0]));
  REQUIRE(static_cast<void*>(&s) == static_cast<void*>(&s[0][0]));
  REQUIRE(reinterpret_cast<byte*>(&s[1]) - reinterpret_cast<byte*>(&s[0]) == 5*sizeof(int));
  REQUIRE(reinterpret_cast<byte*>(&s[1][1]) - reinterpret_cast<byte*>(&s[1]) == sizeof(int));
  // REQUIRE(type(s[1][1]) == "int");

  store h(1,2,3,4);
  cout << h << endl;

  vec<int,4> v;
  REQUIRE(type(h) == type(v));

  store<int,seq<4,5>> t = s;
  //store t = s; // would be better, CTAD for using statements?
  cout << t << endl;

  store tprime = s;
  cout << tprime << endl;

  REQUIRE(type(t) == type(tprime));

  store j(1,2,3), k(4,5,6);
  cout << type(std::move(j) + std::move(k)) << endl;

  // column major
  store<int,seq<4,5>,sseq<1,4>> r = s + t;
  r.pull<1>(1) = 200;
  cout << r << endl;

  store w = s + t;
  cout << w << endl;
  REQUIRE(type(w) != type(r));

  store<int,seq<5,5>> x;
  x.pull<1>() = {1,2,3,4,5};
  x.pull<1>(3)[2] = 100;
  REQUIRE(x[2][3] == 100);
  cout << x << endl;

  auto xd = x.tie<1,1,1>(3);
  cout << type(xd) << endl;

  store<int,seq<4>> z = {1,2,3,4};
  //store zdz = einsum<str<>,str<'i'>,str<'i'>>(z,z);
  //cout << zdz << endl;
}

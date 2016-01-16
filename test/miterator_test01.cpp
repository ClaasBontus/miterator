// MITERATOR
//
//  Copyright Claas Bontus
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ClaasBontus/miterator
//

#include "../miterator.hpp"
#include "../stream_wrapper.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <string>
#include <sstream>


// Tests without 'using namespace miterator'
void f1()
{
  std::string   s1{ "abcd" };
  int           v2[]= { -2, -3, -4, -5 };

  for( auto  &p : miterator::make_miterator( s1, v2 ) )
  {
    auto & r1= *miterator::get<0>( p );
    auto & r2= *miterator::get<1>( p );
    std::cout << r1 << "  " << r2 << "\n";
  }
  std::cout << "\n";

  std::istringstream  str2( "0.5 0.6 0.7 0.8" );
  std::istringstream  str3( "1.5 1.6 1.7 1.8" );
  auto  sc2= miterator::make_ostream_cont<double>( std::cout, ", " );
  auto  sc3= miterator::make_istream_cont<double>( str2 );
  auto  sc4= miterator::make_ostream_cont<double>( std::cout );
  auto  sc5= miterator::make_istream_cont<double>( str3 );
  for( auto &p : miterator::make_miterator( sc2, sc3 ) )
    *miterator::get<0>( p )= *miterator::get<1>( p );
  std::cout << "\n";
  for( auto &p : miterator::make_miterator( sc4, sc5 ) )
    *miterator::get<0>( p )= *miterator::get<1>( p );
  std::cout << "\n\n";

  std::vector<int>   v1a{ 1, 2, 3 };
  int                v2a[]= { -2, -3, -4, -5 };
  for( auto const &p : miterator::make_miterator( v1a, v2a ) )
       miterator::apply( p, []( int i1, int const & i2 )
                            { std::cout << i1 << "  " << i2 << "\n"; } );
  std::cout << "\n";
} // f1




void f2()
{
  using namespace miterator;

  std::vector<int>   v1{ 1, -1,  2, -2, 3, -3 };
  std::vector<int>   v2{ 4,  5, -6,  7, 8 };
  int res= 0;
  for( auto  p : make_miterator( v1, v2 ) )
  {
    auto  i1= *get<0>( p );
    auto  i2= *get<1>( p );
    if( i1 <= 0 || i2 <= 0 ) continue;
    res += i1 * i2;
  }
  std::cout << "res= " << res << "\n\n"; // 1*4 + 3*8 = 28
} // f2


void f3()
{
  using namespace miterator;

  std::vector<int>   v1{ 1, 2, 3 };
  int                v2[]= { -2, -3, -4, -5 };
  std::cout << "Apply test 1\n";
  for( auto const &p : make_miterator( v1, v2 ) )
  {
    apply( p, []( int i1, int const & i2 )
              { std::cout << i1 << "  " << i2 << "\n"; } );

    // Must not compile since p is const
    // apply( p, []( int i1, int &i2 ){ std::cout << i1 << "  " << i2 << "\n"; } );
  }
  std::cout << "\n";

  std::array<double,3> const  a1{ 1.1, 2.2, 3.3 };
  std::array<double,3> const  a2{ 4.1, 5.2, 6.3 };
  double scal_prod= 0.0;
  for( auto &p : make_miterator( a1, a2) )
  {
    scal_prod += apply( p, []( double const & d1, double d2 )
                           { return d1 * d2; } );

    // Must not compile since a1 is const
    // scal_prod += apply( p, []( double &d1, double d2 ){ return d1 * d2; } );
  }
  std::cout << "Apply test 2: scal_prod= " << scal_prod << "\n\n";
  // 1.1 * 4.1 + 2.2 * 5.2 + 3.3 * 6.3 = 36.74


  std::array<int,3>        a3{ 7, 8, 9 };
  std::array<int,3> const  a4{ 12, 11, 10 };
  std::cout << "Apply test 3\n";
  for( auto &p : make_miterator( a3, a4 ) )
    apply( p, []( int &i1, int i2 ) { i1 += i2; } );
  for( auto i : a3 ) std::cout << i << "  ";
  std::cout << "\n\n";
} // f3


void f4()
{
  using namespace miterator;

  std::vector<int>  v1{ 1, -1, 2, -2, 3, -3, 4, -4 };
  std::vector<int>  v2{ 5,  5, 6,  6 };

  int result1= 0;
  for( auto const &p : make_miterator( v1, v2 ) )
  {
    auto i1= *get<0>( p );
    auto i2= *get<1>( p );
    if( i1 <= 0 || i2 <= 0 ) continue;
    result1 += i1 * i2;
  }
  std::cout << "(f4) result1= " << result1 << "\n"; // 1 * 5 + 2 * 6 = 17

  int result2= 0;
  for( auto const &p : make_miterator( v1, v2 ) )
  {
    apply( p, [&result2]( int i1, int i2 )
    { if( i1 > 0 && i2 > 0 ) result2 += i1 * i2; } );
  }
  std::cout << "(f4) result2= " << result2 << "\n\n";
} // f4



int main()
{
  using namespace miterator;

  std::vector<int>   v1{ 1, 2, 3 };
  int                v2[]= { -2, -3, -4, -5 };

  std::cout << "\nTest 1 (Vector and C array)\n";
  for( auto  &p : make_miterator( v1, v2 ) )
  {
    auto & r1= *get<0>( p );
    auto & r2= *get<1>( p );
    r1 += r2;
    std::cout << r1 << "  " << r2 << "\n";
  }
  std::cout << "\n";


  std::array<int,3> const  a1{ 4, 5, 6 };
  int const                v3[]= { -6, -7, -8, -9 };
  int                      v4[]= { 0, 0, 0, 0 };

  std::cout << "Test 2 (C and C++ arrays)\n";
  for( auto  &p : make_miterator( a1, v2, v3, v4 ) )
  {
    auto & r1= *get<0>( p );
    auto & r2= *get<1>( p );
    auto & r3= *get<2>( p );
    auto & r4= *get<3>( p );

    // r1 += r2; // Must not compile since a1 is const
    // r3= 42;   // Must not compile since v3 is const
    r4= r3;

    std::cout << r1 << "  " << r2 << "  " << r3 << "\n";
  }
  for( auto const &i : v4 ) std::cout << i << "  "; // -6  -7  -8  0
  std::cout << "\n\n";


  std::array<int,3>   a2{ 7, 8, 9 };

  std::cout << "Test 3\n";
  for( auto const &p : make_miterator( a2, v2 ) )
  {
    auto & r1= *get<0>( p );
    auto & r2= *get<1>( p );

    // r1 += r2; // Must not compile since p is const

    std::cout << r1 << "  " << r2 << "\n";
  }
  std::cout << "\n";



  std::istringstream  str1( "0.1 0.2 0.3 0.4" );
  std::list<double>   l1{ 2.1, 3.2, 4.3, 5.4 };

  auto  sc1= make_istream_cont<double>( str1 );

  std::cout << "Test 4 (stream iterator)\n";
  for( auto &p : make_miterator( sc1, l1 ) ) *get<1>( p ) += *get<0>( p );
  for( auto &d : l1 ) std::cout << d << "  ";
  std::cout << "\n\n";



  std::cout << "Test 5 (stream iterators)\n";
  std::istringstream  str2( "0.5 0.6 0.7 0.8" );
  std::istringstream  str3( "1.5 1.6 1.7 1.8" );
  auto  sc2= make_ostream_cont<double>( std::cout, ", " );
  auto  sc3= make_istream_cont<double>( str2 );
  auto  sc4= make_ostream_cont<double>( std::cout );
  auto  sc5= make_istream_cont<double>( str3 );
  for( auto &p : make_miterator( sc2, sc3 ) )
    *get<0>( p )= *get<1>( p ); // 0.5, 0.6, 0.7, 0.8,
  std::cout << "\n";
  for( auto &p : make_miterator( sc4, sc5 ) )
    *get<0>( p )= *get<1>( p ); // 1.51.61.71.8
  std::cout << "\n\n";

  f1();
  f2();
  f3();
  f4();
} // main

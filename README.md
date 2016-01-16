# miterator: Iterate over groups of containers in range based for loops

The following code is supposed to iterate over two vectors and sum over the
product of their elements if each element is larger than zero. Can you spot
the two bugs?

```.cpp
vector<int>  v1{ 1, -1, 2, -2, 3, -3, 4, -4 };
vector<int>  v2{ 5,  5, 6,  6 };
int result= 0;
auto it= v2.begin();
for( auto i1 : v1 )
{
  auto i2= *it;
  if( i1 <= 0 || i2 <= 0 ) continue;
  result += i1 * i2;
  ++it;
}
```
1. Iterator *it* is not increased in case the if statement triggers.
2. Vector *v2* has less elements than *v1*. Therefore *it* will eventually point
past the end of *v2*.

## miterator
miterator is a header only library built for easing iteration over an
arbitrary number of containers in range based for loops. With miterator we
solve the task above as follows.

```.cpp
#include "miterator.hpp"
#include <vector>

using namespace miterator;

std::vector<int>  v1{ 1, -1, 2, -2, 3, -3, 4, -4 };
std::vector<int>  v2{ 5,  5, 6,  6 };

int result= 0;
for( auto const &p : make_miterator( v1, v2 ) )
{
  auto i1= *get<0>( p );
  auto i2= *get<1>( p );
  if( i1 <= 0 || i2 <= 0 ) continue;
  result += i1 * i2;
}
```
This can further be simplified using *miterator::apply*

```.cpp
int result= 0;
for( auto const &p : make_miterator( v1, v2 ) )
{
  apply( p, [&result]( int i1, int i2 )
            { if( i1 > 0 && i2 > 0 ) result += i1 * i2; } );
}
```
*make_miterator* returns an object of a proxy type which wraps the iterators
of the containers. Each of these iterators is incremented automatically in
each step of the for loop. Condition *it != end* is true only if for each of
the underlying iterators *it != end* is true. This ensures that iteration ends
as soon as one iterator points behind the last element.

*get* returns the wrapped iterator indexed by the template parameter.

*apply* takes the bunch of iterators, dereferences each of them and
submits the resulting values to the function, i.e. the second
argument of *apply*. *apply* returns the result of the function.

See the code in the *test* directory for further examples.

## Stream iterators
Stream iterators are supported via surrogate containers defined in
*stream_wrapper.hpp*. The following example shows how they can be used.

```.cpp
#include "miterator.hpp"
#include "stream_wrapper.hpp"
#include <iostream>
#include <sstream>
using namespace miterator;
std::istringstream  str( "0.5 0.6 0.7 0.8" );
auto  sc1= make_ostream_cont<double>( std::cout, ", " );
auto  sc2= make_istream_cont<double>( str );
for( auto &p : make_miterator( sc1, sc2 ) )
     *get<0>( p )= *get<1>( p ); // 0.5, 0.6, 0.7, 0.8,
```
ostream iterators are supported for symmetry reasons only. Since increment
of an ostream iterator is a noop, there is actually no need for wrapping them.

## Const correctness
miterator is written such that const correctness is ensured. Therefore none
of the following assignments compiles.

```.cpp
std::vector<int>   v1{ 1, 2, 3 };
int const          v2[]= { -2, -3, -4, -5 };
for( auto const &p : make_miterator( v1, v2 ) )
{
  *get<0>( p )= *get<1>( p ); // Must not compile since p is const
}
for( auto &p : make_miterator( v1, v2 ) )
{
  *get<1>( p )= *get<0>( p ); // Must not compile since v2 is const
}
```

## Caveats
* miterator requires a C++14 compliant compiler.
* Tested with gcc 4.9 and 5.2.
* A miterator does not fulfill the requirements of an
[iterator](http://en.cppreference.com/w/cpp/concept/Iterator). It might
not be used anywhere else than in range based for loops.

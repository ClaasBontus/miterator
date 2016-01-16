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

#pragma once

#ifndef MITERATORHPP
#define MITERATORHPP


#include <iterator>
#include <utility>
#include <functional>
#include <type_traits>
#include <tuple>

namespace miterator
{
  namespace detail
  {

    /// Select iterator or const_iterator depending on boolean
    template<class Cont,bool isconst>
    struct cc_it
    { using type= typename Cont::const_iterator; };

    template<class Cont>
    struct cc_it<Cont,false>
    { using type= typename Cont::iterator; };




    /// Traits for choosing correct iterator
    template<class Cont>
    struct it_h_t
    {
      using iterator=       typename cc_it<Cont,std::is_const<Cont>::value>::type;
      using const_iterator= typename Cont::const_iterator;
    };

    template<class T>
    struct it_h_t<T*>
    {
      using iterator=       T *;
      using const_iterator= T const *;
    };

    template<class T,size_t N>
    struct it_h_t<T[N]>
    {
      using iterator=       T*;
      using const_iterator= T const *;
    };




    template<typename Tup, size_t... index>
    inline
    void
    incr_helper(Tup& tup, std::index_sequence<index...>)
    {
      (void)std::initializer_list<int>{(++(std::get<index>(tup)), 0)...};
    }

    /// Increment each member of the tuple
    template<typename Tup>
    inline
    void
    incr_tuple( Tup & tup )
    {
      constexpr auto s= std::tuple_size<typename std::decay<Tup>::type>::value;
      incr_helper( tup, std::make_index_sequence<s>{} );
    }



    template <class T>
    constexpr std::add_const_t<T>& asconst(T& t) noexcept   { return t; }

    /// Derived from
    /// http://www.cppsamples.com/common-tasks/apply-tuple-to-function.html
    template<typename F, typename Tup, size_t ...S >
    inline
    decltype(auto)
    apply_tuple_impl( F && fn, Tup & t, std::index_sequence<S...> )
    {
      return std::forward<F>(fn)( *( std::get<S>(t) )... );
    }

    template<typename F, typename Tup, size_t ...S >
    inline
    decltype(auto)
    apply_tuple_impl( F && fn, Tup const & t, std::index_sequence<S...> )
    {
      return std::forward<F>(fn)( asconst(*( std::get<S>(t) ))... );
    }

    template<typename F, typename Tup>
    inline
    decltype(auto)
    apply_from_tuple( F && fn, Tup & t )
    {
      constexpr auto s= std::tuple_size<typename std::decay<Tup>::type>::value;

      return apply_tuple_impl( std::forward<F>( fn ), t,
                               std::make_index_sequence<s>() );
    }



    /// \brief Wraps a tuple with iterators and provides functions for
    /// increment. it_wrapper is returned when an miterator_proxy
    /// gets dereferenced.
    template<class ... Cont>
    struct it_wrapper
    {
      using it_tuple_t=   std::tuple<typename it_h_t<Cont>::iterator...>;
      using c_it_tuple_t= std::tuple<typename it_h_t<Cont>::const_iterator...>;

      explicit
      it_wrapper( it_tuple_t its )
      : m_iterators( its )
      {}

      it_wrapper &
      operator++()
      {
        incr_tuple( m_iterators );
        return *this;
      }

      it_wrapper
      operator++(int)
      {
        it_wrapper tmp(*this);
        operator++();
        return tmp;
      }

      it_tuple_t  &
      get_iterators()
      { return m_iterators; }

      it_tuple_t const &
      get_iterators() const
      { return m_iterators; }

    private:
      it_tuple_t  m_iterators;
    }; // struct it_wrapper




    /// \brief miterator_proxy is the struct that is used as a
    /// surrogate iterator within range based for loops.
    /// It provides functions for dereferencing and increment.
    template<class ... Cont>
    struct miterator_proxy
    {
      using wrapper_t=   it_wrapper<Cont...>;
      using it_tuple_t=  typename wrapper_t::it_tuple_t;

      wrapper_t   iterators;

      explicit
      miterator_proxy( it_tuple_t its )
      : iterators( its )
      {}

      wrapper_t const &
      operator*() const { return iterators; }

      wrapper_t &
      operator*() { return iterators; }

      miterator_proxy &
      operator++()
      {
        ++iterators;
        return *this;
      }

      miterator_proxy
      operator++(int)
      {
        miterator_proxy tmp(*this);
        operator++();
        return tmp;
      }
    }; // struct miterator_proxy





    /// \brief miterator internally stores the various begin and end
    /// iterators. It provides functions begin and end which return
    /// objects of struct miterator_proxy.
    template<class ... Cont>
    struct miterator
    {
      using proxy_t= miterator_proxy<Cont...>;

      explicit
      miterator( Cont& ... containers )
      : b_it( std::make_tuple( std::begin(containers)... ) )
      , e_it( std::make_tuple( std::end(  containers)... ) )
      {}

      proxy_t
      begin() const { return b_it; }

      proxy_t
      end() const { return e_it; }

      proxy_t   b_it;
      proxy_t   e_it;
    }; // struct miterator




    /// Helper struct used when evaluating operator!= on tuples of iterators.
    template<size_t I,class T1, class T2>
    struct compare_tuples_h
    {
      inline
      bool
      operator()( const T1 &t1, const T2 &t2 )
      {
        return ( std::get<I>( t1 ) != std::get<I>( t2 ) )
            && compare_tuples_h<I-1,T1,T2>{}( t1, t2 );
      }
    };

    template<class T1, class T2>
    struct compare_tuples_h<0,T1,T2>
    {
      inline
      bool
      operator()( const T1 &t1, const T2 &t2 )
      {
        return ( std::get<0>( t1 ) != std::get<0>( t2 ) );
      }
    };



    /// Helper function used when evaluating operator!= on tuples of iterators.
    template<class T1, class T2>
    inline
    bool
    compare_tuples( const T1 &t1, const T2 &t2 )
    {
      constexpr auto s= std::tuple_size<typename std::decay<T1>::type>::value;
      return compare_tuples_h<s-1,T1,T2>{}( t1, t2 );
    }

  } // namespace detail



  /// Returns the I'th iterator within an it_wrapper.
  template<size_t I,class ... T>
  inline
  decltype(auto)
  get( detail::it_wrapper<T...> & mit )
  {
    return std::get<I>( mit.get_iterators() );
  }

  /// Returns the I'th iterator within an it_wrapper. Ensures const correctness.
  template<size_t I,class ... T>
  inline
  decltype(auto)
  get( detail::it_wrapper<T...> const & mit )
  {
    using type= typename std::tuple_element<I,typename detail::it_wrapper<T...>
                                                         ::c_it_tuple_t>::type;
    return type( std::get<I>( mit.get_iterators() ) );
  }




  template<typename F, typename ... T>
  inline
  decltype(auto)
  apply( detail::it_wrapper<T...> & mit, F && fn )
  {
    return detail::apply_from_tuple( std::forward<F>( fn ),
                                     mit.get_iterators() );
  }

  template<typename F, typename ... T>
  inline
  decltype(auto)
  apply( detail::it_wrapper<T...> const & mit, F && fn )
  {
    return detail::apply_from_tuple( std::forward<F>( fn ),
                                     mit.get_iterators() );
  }




  /// \brief Helper function which relieves the user from specifying the
  /// template parameters.
  template<class ... Cont>
  inline
  detail::miterator<Cont...>
  make_miterator( Cont& ... containers )
  { return detail::miterator<Cont...>( containers... ); }

} // namespace miterator




/// Compare two surrogate iterators of type miterator_proxy.
template<class ... T>
inline
bool
operator!=( const miterator::detail::miterator_proxy<T...> &p1,
            const miterator::detail::miterator_proxy<T...> &p2 )
{
  auto &t1= p1.iterators.get_iterators();
  auto &t2= p2.iterators.get_iterators();

  return miterator::detail::compare_tuples( t1, t2 );
}


#endif // MITERATORHPP

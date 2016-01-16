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

#ifndef MITERATOR_STREAM_WRAPPER_HPP
#define MITERATOR_STREAM_WRAPPER_HPP


#include <iterator>
#include <string>


namespace miterator
{
  namespace detail
  {

    /// \brief Wraps a reference to an input stream and provides begin/end
    /// functions which return an istream_iterator.
    template<class T,class S>
    struct istream_cont
    {
      using iterator=       std::istream_iterator<T>;
      using const_iterator= std::istream_iterator<T>;

      explicit
      istream_cont( S &stream )
      : m_stream( stream )
      {}

      iterator
      begin()
      { return iterator( m_stream.get() ); }

      iterator
      cbegin()
      { return const_iterator( m_stream.get() ); }

      iterator
      end()
      { return iterator(); }

      iterator
      cend()
      { return iterator(); }
    private:
      std::reference_wrapper<S> m_stream;
    }; // struct istream_cont



    /// Wraps an ostream_iterator which is required since we need to
    /// overload operator!=
    template<class T,class S>
    struct ostream_it_wrapper
    {
      using char_type=      typename S::char_type;

      explicit
      ostream_it_wrapper( S &stream )
      : m_iterator( stream )
      {}

      explicit
      ostream_it_wrapper( S &stream, char_type const * c )
      : m_iterator( stream, c )
      {}

      T &
      operator*()
      { return m_iterator; }

      // T const &
      // operator*() const
      // { return m_iterator; }

      ostream_it_wrapper &
      operator++()
      { return *this; }

      ostream_it_wrapper &
      operator++(int)
      { return *this; }
    private:
      T   m_iterator;
    }; // struct ostream_it_wrapper




    /// \brief Wraps a reference to an output stream and provides begin/end
    /// functions which return an ostream_it_wrapper.
    template<class T,class S>
    struct ostream_cont
    {
      using char_type=      typename S::char_type;
      using str_t=          std::basic_string<char_type>;
      using iterator=       ostream_it_wrapper<std::ostream_iterator<T>,S>;
      using const_iterator= ostream_it_wrapper<std::ostream_iterator<T>,S>;

      explicit
      ostream_cont( S &stream, char_type const * c= nullptr )
      : m_stream( stream )
      , m_sep( c ? str_t(c) : str_t() )
      {}

      iterator
      begin()
      { return gen_it(); }

      const_iterator
      cbegin()
      { return const_iterator( gen_it() ); }

      iterator
      end()
      { return gen_it(); }

      const_iterator
      cend()
      { return const_iterator( gen_it() ); }
    private:
      std::reference_wrapper<S>     m_stream;
      str_t const                   m_sep;

      iterator
      gen_it()
      {
        return m_sep.empty() ? iterator( m_stream.get() )
                             : iterator( m_stream.get(), m_sep.c_str() );
      }
    }; // struct ostream_cont


  } // namespace detail


  /// \brief Helper function which relieves the user from specifying the
  /// template parameters.
  template<class T,class S>
  inline
  detail::istream_cont<T,S>
  make_istream_cont( S &stream )
  {
    return detail::istream_cont<T,S>( stream );
  }


  /// \brief Helper function which relieves the user from specifying the
  /// template parameters.
  template<class T,class S,class Char=char>
  inline
  detail::ostream_cont<T,S>
  make_ostream_cont( S &stream, Char const *c= nullptr )
  {
    return detail::ostream_cont<T,S>( stream, c );
  }


} // namespace miterator


template<class T1,class T2,class S1, class S2>
inline
bool
operator!=( const miterator::detail::ostream_it_wrapper<T1,S1>& lhs,
            const miterator::detail::ostream_it_wrapper<T2,S2>& rhs )
{ return true; }


#endif // MITERATOR_STREAM_WRAPPER_HPP

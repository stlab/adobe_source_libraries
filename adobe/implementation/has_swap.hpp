/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

/*
    This file is borrowed from the boost repository with Dave Abrahams permission. Namespaces have
    been changed to protect the innocent.
*/

// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef ADOBE_IMPLEMENTATION_HAS_SWAP_HPP
#define ADOBE_IMPLEMENTATION_HAS_SWAP_HPP

#include <boost/mpl/bool.hpp>
#include <boost/detail/workaround.hpp>

namespace adobe {


namespace has_swap_
{
  // any soaks up implicit conversions and makes the following
  // operator++ less-preferred than any other such operator which
  // might be found via ADL.
  struct anything { template <class T> anything(T const&); };
  struct no_swap
  {
      char (& operator,(char) )[2];
  };
  no_swap swap(anything,anything);
    
#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4675) // function found through argument dependent lookup -- duh!
#endif 
  template <class T>
  struct has_swap_impl
  {
      static T& x;
        
      BOOST_STATIC_CONSTANT(bool, value = sizeof(swap(x,x),'x') == 1);
      
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
      typedef boost::mpl::bool_<has_swap_impl<T>::value> type;
#else
      typedef boost::mpl::bool_<value> type;
#endif      
  };
}
template <class T>
struct has_swap
  : has_swap_::has_swap_impl<T>::type
{};
#if defined(BOOST_MSVC)
# pragma warning(pop)
#endif 

} // namespace adobe

#endif

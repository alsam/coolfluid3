// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

/// @file
/// @brief This file contains compile-time predicates for element types
///
/// @author Bart Janssens

#ifndef CF_Mesh_ElementTypePredicates_hpp
#define CF_Mesh_ElementTypePredicates_hpp

////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {

////////////////////////////////////////////////////////////////////////////////

/// Compile-time predicate to determine if the given shape function represents a volume element, i.e. dimensions == dimensionality
struct IsCellType
{
  template<typename ETYPE>
  struct apply
  {
    typedef typename boost::mpl::equal_to<boost::mpl::int_<ETYPE::dimension>,boost::mpl::int_<ETYPE::dimensionality> >::type type;
  };
};

/// Compile-time predicate to determine if the given shape function represents a volume element, i.e. dimensions == dimensionality
struct IsFaceType
{
  template<typename ETYPE>
  struct apply
  {
    typedef typename boost::mpl::equal_to<boost::mpl::int_<ETYPE::dimension>,boost::mpl::int_<ETYPE::dimensionality> >::type type;
  };
};

/// Compile-time predicate to determine if the given shape function represents a volume element, i.e. dimensions == dimensionality
struct IsEdgeType
{
  template<typename ETYPE>
  struct apply
  {
    typedef typename boost::mpl::equal_to<boost::mpl::int_<ETYPE::dimension>,boost::mpl::int_<ETYPE::dimensionality> >::type type;
  };
};

/// Compile-time predicate to determine if the given shape function is compatible with the shape function given as template argument
/// i.e. when dimension, shape and number of nodes are equal
template<typename ETYPE>
struct IsCompatibleWith
{
  template<typename OTHER_ETYPE>
  struct apply
  {
    typedef typename boost::mpl::and_
    <
      boost::mpl::equal_to
      <
        boost::mpl::int_<ETYPE::dimension>,
        boost::mpl::int_<OTHER_ETYPE::dimension>
      >,
      boost::mpl::equal_to
      <
        boost::mpl::int_<ETYPE::shape>,
        boost::mpl::int_<OTHER_ETYPE::shape>
      >
    >::type type;
  };
};

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_ElementTypePredicates_hpp

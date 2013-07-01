#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/closest_points.hpp"

/** @file distance.hpp
    @brief Computes the (Cartesian) distance between different elements.
*/


namespace viennagrid
{
  namespace detail
  {
    
    //
    // Distance between points
    //
    
    // Distance between two points:
    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord_type<PointType1>::type
    distance_impl(PointType1 const & p1,
                  PointType2 const & p2)
    {
      return norm_2(p1 - p2);
    }
    
    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord_type<PointType1>::type
    distance_impl( std::pair<PointType1,PointType2> const & pts)
    {
      return distance_impl(pts.first, pts.second);
    }
    
    template <typename PointAccessorType, typename CoordType1, typename CoordinateSystem1, typename CoordType2, typename CoordinateSystem2>
    CoordType1
    distance_impl(PointAccessorType const accessor,
                  point_t<CoordType1, CoordinateSystem1> const & p1,
                  point_t<CoordType2, CoordinateSystem2> const & p2)
    {
      return distance_impl(p1, p2);
    }

    template <typename PointAccessorType, typename PointType, typename EB, typename EC, typename ED>
    typename viennagrid::result_of::coord_type<PointType>::type
    distance_impl(PointAccessorType const accessor,
                  PointType const & p1,
                  element_t<viennagrid::vertex_tag, EB,EC,ED> const & v2)
    {
      return distance_impl(p1, accessor(v2));
    }

    template <typename PointAccessorType, typename PointType, typename EB, typename EC, typename ED>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, EB,EC,ED> const & v1,
                  PointType const & p2)
    {
      return distance_impl(accessor(v1), p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename PointAccessorType, typename PointType, typename EB1, typename EC1, typename ED1, typename EB2, typename EC2, typename ED2>
    typename result_of::coord_type<PointAccessorType>::type
    distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, EB1,EC1,ED1> const & v1,
                  element_t<viennagrid::vertex_tag, EB2,EC2,ED2> const & v2)
    {
      return distance_impl(accessor(v1), accessor(v2));
    }

    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename PointAccessorType, typename Something1, typename Something2>
    typename result_of::coord_type<PointAccessorType>::type
    distance_impl(PointAccessorType const accessor,
                  Something1 const & el1,
                  Something2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type   PointType;
      typedef typename result_of::point_type<PointAccessorType>::type PointType;
      
      std::pair<PointType, PointType> points = closest_points(accessor, el1, el2);
      
      return distance_impl( points );
    }
    
    
    
    ////////////////////////////////// boundary distance //////////////////////////////////////

    
    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord_type<PointType1>::type
    boundary_distance_impl(PointType1 const & p1,
                  PointType2 const & p2)
    {
      return norm_2(p1 - p2);
    }
    
    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord_type<PointType1>::type
    boundary_distance_impl( std::pair<PointType1,PointType2> const & pts)
    {
      return boundary_distance_impl(pts.first, pts.second);
    }
    
    template <typename PointAccessorType, typename CoordType1, typename CoordinateSystem1, typename CoordType2, typename CoordinateSystem2>
    CoordType1
    boundary_distance_impl(PointAccessorType const accessor,
                  point_t<CoordType1, CoordinateSystem1> const & p1,
                  point_t<CoordType2, CoordinateSystem2> const & p2)
    {
      return boundary_distance_impl(p1, p2);
    }

    template <typename PointAccessorType, typename PointType, typename EB, typename EC, typename ED>
    typename viennagrid::result_of::coord_type<PointType>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  PointType const & p1,
                  element_t<viennagrid::vertex_tag, EB,EC,ED> const & v2)
    {
      return boundary_distance_impl(p1, accessor(v2));
    }

    template <typename PointAccessorType, typename PointType, typename EB, typename EC, typename ED>
    typename viennagrid::result_of::coord_type<PointAccessorType>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, EB,EC,ED> const & v1,
                  PointType const & p2)
    {
      return boundary_distance_impl(accessor(v1), p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename PointAccessorType, typename PointType, typename EB1, typename EC1, typename ED1, typename EB2, typename EC2, typename ED2>
    typename result_of::coord_type<PointAccessorType>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, EB1,EC1,ED1> const & v1,
                  element_t<viennagrid::vertex_tag, EB2,EC2,ED2> const & v2)
    {
      return boundary_distance_impl(accessor(v1), accessor(v2));
    }
    
    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename PointAccessorType, typename Something1, typename Something2>
    typename result_of::coord_type<PointAccessorType>::type
    boundary_distance_impl(PointAccessorType const accessor,
                           Something1 const & el1,
                           Something2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type      PointType;
      typedef typename result_of::point_type<PointAccessorType>::type PointType;
      
      std::pair<PointType, PointType> points = closest_points_on_boundary(accessor, el1, el2);
      
      return boundary_distance_impl(points);
    }
    
//     template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
//     typename result_of::coord_type<DomainPointType>::type
//     boundary_distance_impl(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
//                            ElementType const & element)
//     {
//       //typedef typename result_of::point<ElementType1>::type      PointType;
//       typedef DomainPointType PointType;
//       
//       std::pair<PointType, PointType> points = closest_points_on_boundary<ElementTypeOrTag>(domain, element);
//       
//       return norm_2(points.first - points.second);
//     }
//     
//     template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
//     typename result_of::coord_type<DomainPointType>::type
//     boundary_distance_impl(ElementType const & element,
//                            domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain)
//     {
//       //typedef typename result_of::point<ElementType1>::type      PointType;
//       typedef DomainPointType PointType;
//       
//       std::pair<PointType, PointType> points = closest_points_on_boundary<ElementTypeOrTag>(domain, element);
//       
//       return norm_2(points.first - points.second);
//     }
    
    
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the distance between n-cells, segments and/or domains */
  template <typename PointAccessorType, typename Something1, typename Something2>
  typename result_of::coord_type<Something1>::type
  distance(PointAccessorType const accessor,
           Something1 const & el1,
           Something2 const & el2)
  {
    return detail::distance_impl(accessor, el1, el2);
  }
  
  
  template <typename Something1, typename Something2>
  typename result_of::coord_type<Something1>::type
  distance(Something1 const & el1,
           Something2 const & el2)
  {
    return detail::distance_impl( accessor::default_point_accessor(el1), el1, el2 );
  }
  
  
  
  
  /** @brief Returns the distance between n-cells, segments and/or domains */
  template <typename PointAccessorType, typename Something1, typename Something2>
  typename result_of::coord_type<Something1>::type
  boundary_distance(PointAccessorType const accessor,
           Something1 const & el1,
           Something2 const & el2)
  {
    return detail::boundary_distance_impl(accessor, el1, el2);
  }
  
  /** @brief Returns the distance between n-cells, segments and/or domains */
  template <typename Something1, typename Something2>
  typename result_of::coord_type<Something1>::type
  boundary_distance(Something1 const & el1,
                    Something2 const & el2)
  {
    return detail::boundary_distance_impl( accessor::default_point_accessor(el1), el1, el2 );
  }
  
  
  
  
  
//   /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
//   template <typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType,
//             typename ElementType1, typename ElementType2>
//   typename result_of::coord_type<DomainPointType>::type
//   boundary_distance(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
//                     ElementType1 const & el1, ElementType2 const & el2)
//   {
//     return detail::boundary_distance_impl(domain, el1, el2);
//   }
//   
//   /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
//   template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
//   typename result_of::coord_type<DomainPointType>::type
//   boundary_distance(domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain,
//                     ElementType const & element)
//   {
//     return detail::boundary_distance_impl<ElementTypeOrTag>(domain, element);
//   }
//   
//   /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
//   template <typename ElementTypeOrTag, typename DomainPointType, typename TopologicDomainType, typename MetainfoCollectionType, typename ElementType>
//   typename result_of::coord_type<DomainPointType>::type
//   boundary_distance(ElementType const & element,
//                     domain_t<DomainPointType, TopologicDomainType, MetainfoCollectionType> const & domain)
//   {
//     return detail::boundary_distance_impl<ElementTypeOrTag>(domain, element);
//   }
  
} //namespace viennagrid
#endif

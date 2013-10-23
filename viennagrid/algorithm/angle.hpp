#ifndef VIENNAGRID_ALGORITHM_ANGLE_HPP
#define VIENNAGRID_ALGORITHM_ANGLE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/geometry.hpp"

/** @file viennagrid/algorithm/angle.hpp
    @brief Contains functions for calculating angles between vectors given by points
*/

namespace viennagrid
{

  /** @brief Implementation of the spanned angle between two vectors */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type spanned_angle( PointT const & v0, PointT const & v1 )
  {
    return std::acos( viennagrid::inner_prod(v0, v1) / (viennagrid::norm_2(v0) * viennagrid::norm_2(v1)) );
  }

  /** @brief Implementation of the spanned (solid) angle by three points */
  // http://en.wikipedia.org/wiki/Solid_angle#Tetrahedron
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type spanned_angle( PointT const & a, PointT const & b, PointT const & c )
  {
    typedef typename viennagrid::result_of::coord<PointT>::type coord_type;

    coord_type det = viennagrid::geometry::determinant(a, b, c);

    coord_type al = viennagrid::norm_2(a);
    coord_type bl = viennagrid::norm_2(b);
    coord_type cl = viennagrid::norm_2(c);

    coord_type div = al*bl*cl + viennagrid::inner_prod(a,b)*cl + viennagrid::inner_prod(a,c)*bl + viennagrid::inner_prod(b,c)*al;
    coord_type at = std::atan2(det, div);

    return std::abs(2*at);
  }


  /** @brief Implementation of the dihedral angle between two planes each defined by 3 points */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type dihedral_angle( PointT const & p0, PointT const & p1, PointT const & p2,
                                                                      PointT const & q0, PointT const & q1, PointT const & q2)
  {
    PointT np = viennagrid::cross_prod( p1-p0, p2-p0 );
    PointT nq = viennagrid::cross_prod( q1-q0, q2-q0 );

    return spanned_angle(np, nq);
  }



  /** @brief Implementation of the angle between two lines directing from origin to p0 and p1  */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type angle( PointT const & origin, PointT const & p0, PointT const & p1 )
  {
    PointT v0 = p0 - origin;
    PointT v1 = p1 - origin;

    return spanned_angle(v0, v1);
  }

  /** @brief Implementation of the solid angle from an origin to p0, p1 and p2 */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type angle( PointT const & origin, PointT const & p0, PointT const & p1, PointT const & p2 )
  {
    PointT v0 = p0 - origin;
    PointT v1 = p1 - origin;
    PointT v2 = p2 - origin;

    return spanned_angle(v0, v1, v2);
  }

  /** @brief Implementation of the solid angle from an origin to p0, p1 and p2, same as angle(origin, p0, p1, p2) */
  // same as above, http://en.wikipedia.org/wiki/Solid_angle#Tetrahedron
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type angle2( PointT const & origin, PointT const & a, PointT const & b, PointT const & c )
  {
    double da_ab = dihedral_angle( origin, a, c, origin, b, c );
    double da_ac = dihedral_angle( origin, a, b, origin, c, b );
    double da_bc = dihedral_angle( origin, b, a, origin, c, a );

    return da_ab + da_ac + da_bc - M_PI;
  }

}

#endif

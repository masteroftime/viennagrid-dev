#ifndef VIENNAGRID_MESH_OPERATIONS_HPP
#define VIENNAGRID_MESH_OPERATIONS_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/functors.hpp"

/** @file viennagrid/mesh/mesh_operations.hpp
    @brief Helper routines for meshes
*/

namespace viennagrid
{

  /** @brief A helper class for element copy operation between two differen meshes.
    *
    * @tparam SrcMeshT      The mesh type of the source mesh
    * @tparam DstMeshT      The mesh type of the destination mesh
    */
  template<typename SrcMeshT, typename DstMeshT>
  class vertex_copy_map
  {
  public:

    typedef typename viennagrid::result_of::coord<DstMeshT>::type DstNumericType;

    typedef typename viennagrid::result_of::vertex<SrcMeshT>::type SrcVertexType;
    typedef typename viennagrid::result_of::vertex_id<SrcMeshT>::type SrcVertexIDType;

    typedef typename viennagrid::result_of::vertex<DstMeshT>::type DstVertexType;
    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstVertexHandleType;

    /** @brief The constructor, requires the destination mesh where the elements are copied to.
      *
      * @param  dst_mesh_                The destination mesh
      */
    vertex_copy_map( DstMeshT & dst_mesh_ ) : dst_mesh(dst_mesh_), tolerance(-1.0) {}
    vertex_copy_map( DstMeshT & dst_mesh_, DstNumericType tolerance_ ) : dst_mesh(dst_mesh_), tolerance(tolerance_) {}

    /** @brief Copies one vertex to the destination mesh. If the vertex is already present in the destination mesh, the vertex handle of this vertex is return, otherwise a new vertex is created in the destination mesh.
      *
      * @param  src_vertex              The vertex to be copied
      */
    DstVertexHandleType operator()( SrcVertexType const & src_vertex )
    {
      typename std::map<SrcVertexIDType, DstVertexHandleType>::iterator vit = vertex_map.find( src_vertex.id() );
      if (vit != vertex_map.end())
        return vit->second;
      else
      {
        DstVertexHandleType vh = viennagrid::make_unique_vertex( dst_mesh,
                                                                 viennagrid::point(src_vertex),
                                                                 viennagrid::norm_2(viennagrid::point(src_vertex)) * tolerance );
        vertex_map[src_vertex.id()] = vh;
        return vh;
      }
    }

    /** @brief Copies one vertex to the destination mesh. If the vertex is already present in the destination mesh, the vertex handle of this vertex is return, otherwise a new vertex is created in the destination mesh.
      *
      * @param  src_vertex              The vertex to be copied
      * @param  tol                     A geometric tolerance for vertex creation
      */
    DstVertexHandleType operator()( SrcVertexType const & src_vertex, DstNumericType tol )
    {
      typename std::map<SrcVertexIDType, DstVertexHandleType>::iterator vit = vertex_map.find( src_vertex.id() );
      if (vit != vertex_map.end())
        return vit->second;
      else
      {
        DstVertexHandleType vh = viennagrid::make_unique_vertex( dst_mesh,
                                                                 viennagrid::point(src_vertex),
                                                                 viennagrid::norm_2(viennagrid::point(src_vertex)) * tol );
        vertex_map[src_vertex.id()] = vh;
        return vh;
      }
    }



    /** @brief Copies a whole element including its vertices to the destination mesh.
      *
      * @param  el                      The element to be copied
      */
    template<typename ElementTagT, typename WrappedConfigT>
    typename viennagrid::result_of::handle<DstMeshT, ElementTagT>::type copy_element( element<ElementTagT, WrappedConfigT> const & el )
    {
      typedef element<ElementTagT, WrappedConfigT> ElementType;
      typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type ConstVerticesOnElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVerticesOnElementRangeType>::type ConstVerticesOnElementIteratorType;

      std::vector<DstVertexHandleType> vertex_handles;

      ConstVerticesOnElementRangeType vertices(el);
      for (ConstVerticesOnElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        vertex_handles.push_back( (*this)(*vit) );

      return viennagrid::make_element<ElementTagT>( dst_mesh, vertex_handles.begin(), vertex_handles.end() );
    }

    /** @brief Copies a whole element including its vertices to the destination mesh.
      *
      * @param  el                      The element to be copied
      * @param  tol                     A geometric tolerance for vertex creation
      */
    template<typename ElementTagT, typename WrappedConfigT>
    typename viennagrid::result_of::handle<DstMeshT, ElementTagT>::type copy_element( element<ElementTagT, WrappedConfigT> const & el, DstNumericType tol )
    {
      typedef element<ElementTagT, WrappedConfigT> ElementType;
      typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type ConstVerticesOnElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVerticesOnElementRangeType>::type ConstVerticesOnElementIteratorType;

      std::vector<DstVertexHandleType> vertex_handles;

      ConstVerticesOnElementRangeType vertices(el);
      for (ConstVerticesOnElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        vertex_handles.push_back( (*this)(*vit, tol) );

      return viennagrid::make_element<ElementTagT>( dst_mesh, vertex_handles.begin(), vertex_handles.end() );
    }



  private:

    DstMeshT & dst_mesh;
    std::map<SrcVertexIDType, DstVertexHandleType> vertex_map;

    DstNumericType tolerance;
  };





  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  vertex_map              A vertex copy map used for copying
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename DstMeshT, typename ToCopyFunctorT>
  void copy(vertex_copy_map<SrcMeshT, DstMeshT> & vertex_map,
            SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
            ToCopyFunctorT functor)
  {
    dst_mesh.clear();

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

    ConstCellRangeType cells(src_mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( functor(*cit) )
        vertex_map.copy_element(*cit );
    }
  }

  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename DstMeshT, typename ToCopyFunctorT>
  void copy(SrcMeshT const & src_mesh, DstMeshT & dst_mesh, ToCopyFunctorT functor)
  {
    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);
    copy(vertex_map, src_mesh, dst_mesh, functor);
  }

  /** @brief Copies the cells of a mesh
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    */
  template<typename SrcMeshT, typename DstMeshT>
  void copy(SrcMeshT const & src_mesh, DstMeshT & dst_mesh)
  {
    copy(src_mesh, dst_mesh, viennagrid::true_functor());
  }





  /** @brief Copies the cells of a mesh and a segmentation if a boolean functor is true.
    *
    * @param  vertex_map              A vertex copy map used for copying
    * @param  src_mesh                The source mesh
    * @param  src_segmentation        The source segmentation
    * @param  dst_mesh                The destination mesh
    * @param  dst_segmentation        The destination segmentation
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename ToCopyFunctorT>
  void copy(vertex_copy_map<SrcMeshT, DstMeshT> & vertex_map,
            SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
            DstMeshT       & dst_mesh, DstSegmentationT       & dst_segmentation,
            ToCopyFunctorT functor)
  {
    dst_mesh.clear();
    dst_segmentation.clear();

    typedef typename viennagrid::result_of::segment_handle<DstSegmentationT>::type DstSegmentHandleType;

    for (typename SrcSegmentationT::const_iterator sit = src_segmentation.begin(); sit != src_segmentation.end(); ++sit)
    {
      DstSegmentHandleType seg = dst_segmentation( (*sit).id() );
      seg.set_name( (*sit).name() );
    }

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

    typedef typename viennagrid::result_of::cell_handle<DstMeshT>::type CellHandleType;

    ConstCellRangeType cells(src_mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( functor(*cit) )
      {
        CellHandleType cell_handle = vertex_map.copy_element(*cit );
        viennagrid::add( dst_segmentation, viennagrid::segment_ids( src_segmentation, *cit ).begin(), viennagrid::segment_ids( src_segmentation, *cit ).end(), cell_handle );
      }
    }
  }



  /** @brief Copies the cells of a mesh and a segmentation if a boolean functor is true.
    *
    * @param  src_mesh                The source mesh
    * @param  src_segmentation        The source segmentation
    * @param  dst_mesh                The destination mesh
    * @param  dst_segmentation        The destination segmentation
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename ToCopyFunctorT>
  void copy(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
            DstMeshT       & dst_mesh, DstSegmentationT       & dst_segmentation,
            ToCopyFunctorT functor)
  {
    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);
    copy(vertex_map, src_mesh, src_segmentation, dst_mesh, dst_segmentation, functor);
  }

  /** @brief Copies the cells of a mesh and a segmentation if a boolean functor is true.
    *
    * @param  src_mesh                The source mesh
    * @param  src_segmentation        The source segmentation
    * @param  dst_mesh                The destination mesh
    * @param  dst_segmentation        The destination segmentation
    */
  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT>
  void copy(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
            DstMeshT       & dst_mesh, DstSegmentationT       & dst_segmentation)
  {
    copy(src_mesh, src_segmentation, dst_mesh, dst_segmentation, viennagrid::true_functor());
  }





  namespace detail
  {
    /** @brief For internal use only */
    template<typename MeshT, typename ToEraseViewT, typename HandleT, typename ReferencingElementTypelist =
        typename viennagrid::result_of::referencing_element_typelist<MeshT, typename viennagrid::detail::result_of::value_type<HandleT>::type >::type >
    struct mark_referencing_elements_impl;

    template<typename MeshT, typename ToEraseViewT, typename HandleT, typename CoboundaryElementT, typename TailT>
    struct mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::typelist<CoboundaryElementT, TailT> >
    {
      static void mark(MeshT & mesh_obj, ToEraseViewT & mesh_view, HandleT host_element)
      {
        //typedef viennagrid::typelist<CoboundaryElementT, TailT> ReferencingElementTypelist;
        typedef typename viennagrid::detail::result_of::value_type<HandleT>::type HostElementType;

        //typedef typename viennagrid::result_of::handle<MeshT, CoboundaryElementT>::type CoboundaryElementHandle;
        typedef typename viennagrid::result_of::coboundary_range<MeshT, HostElementType, CoboundaryElementT>::type CoboundaryElementRangeType;
        typedef typename viennagrid::result_of::iterator<CoboundaryElementRangeType>::type CoboundaryElementRangeIterator;

        typedef typename viennagrid::result_of::element_range<ToEraseViewT, CoboundaryElementT>::type CoboundaryElementViewRangeType;

        CoboundaryElementRangeType coboundary_elements = viennagrid::coboundary_elements<HostElementType, CoboundaryElementT>(mesh_obj, host_element);
        for (CoboundaryElementRangeIterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
        {
          CoboundaryElementViewRangeType view_elements( mesh_view );
          if ( viennagrid::find_by_handle(mesh_view, it.handle()) == view_elements.end() )
          {
            view_elements.insert_unique_handle( it.handle() );
          }
        }

        mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, TailT>::mark(mesh_obj, mesh_view, host_element);
      }
    };

    /** @brief For internal use only */
    template<typename MeshT, typename ToEraseViewT, typename HandleT>
    struct mark_referencing_elements_impl<MeshT, ToEraseViewT, HandleT, viennagrid::null_type >
    {
      static void mark(MeshT &, ToEraseViewT &, HandleT) {}
    };

  } //namespace detail

  /** @brief Marks elements which reference a given host element
    *
    * @tparam MeshT                   The mesh type in which the element to erase lives
    * @tparam MeshViewT               The mesh view type for all elements to erase
    * @tparam HandleT                 The handle type of the element to delete
    * @param  mesh_obj                The host mesh object
    * @param  element_view            A mesh view which stores all elements to be marked
    * @param  host_element            A handle object of the host element
    */
  template<typename MeshT, typename MeshViewT, typename HandleT>
  void mark_referencing_elements( MeshT & mesh_obj, MeshViewT & element_view, HandleT host_element )
  {
    detail::mark_referencing_elements_impl<MeshT, MeshViewT, HandleT>::mark(mesh_obj, element_view, host_element);
  }

}

#endif

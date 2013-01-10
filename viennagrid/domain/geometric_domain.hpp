#ifndef VIENNAGRID_GEOMETRIC_DOMAIN_HPP
#define VIENNAGRID_GEOMETRIC_DOMAIN_HPP

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

#include "viennagrid/config/generic_config.hpp"
#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"

namespace viennagrid
{
    struct topologic_config_tag;
    struct vector_type_tag;
    struct metainfo_typelist_tag;
    struct metainfo_container_config_tag;
    
    namespace result_of
    {
        template<typename domain_config>
        struct topologic_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, topologic_config_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct vector_type
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, vector_type_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_typelist
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_typelist_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_container_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_container_config_tag>::type::second type;
        };
        
    }

    
    template<typename vector_type_, typename topologic_domain_type_, typename metainfo_collection_type_>
    class geometric_domain_t
    {
        typedef geometric_domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_> self_type;
        
    public:
        
        typedef vector_type_ vector_type;
        typedef topologic_domain_type_ topologic_domain_type;       
        typedef metainfo_collection_type_ metainfo_collection_type;
        
        geometric_domain_t() : topologic_domain(create_topologic_domain<topologic_domain_type>()) {}
//         geometric_domain_t(topologic_domain_type topologic_domain_, metainfo_collection_type metainfo_collection_) :
//             topologic_domain(topologic_domain_), metainfo_collection(metainfo_collection_) {}



      /** @brief Constructor triggering the refinement of the domain */
      template <typename CellTag, typename OtherDomainType, typename RefinementTag>
      geometric_domain_t(refinement_proxy<CellTag, OtherDomainType, RefinementTag> const & proxy) : topologic_domain(create_topologic_domain<topologic_domain_type>())
      {
        detail::refine_impl<CellTag>(proxy.get(), *this, proxy.tag());
      }

        
        typedef typename result_of::element< self_type, viennagrid::vertex_tag >::type vertex_type;
        typedef typename result_of::element_hook< self_type, viennagrid::vertex_tag >::type vertex_hook_type;
        typedef typename result_of::const_element_hook< self_type, viennagrid::vertex_tag >::type const_vertex_hook_type;
        
        topologic_domain_type & get_topological_domain() { return topologic_domain; }
        const topologic_domain_type & get_topological_domain() const { return topologic_domain; }
        
        metainfo_collection_type & get_metainfo_collection() { return metainfo_collection; }
        const metainfo_collection_type & get_metainfo_collection() const { return metainfo_collection; }
        
    private:
        
        topologic_domain_type topologic_domain;
        metainfo_collection_type metainfo_collection;
    };
    
    template<typename vector_type_, typename topologic_domain_type_, typename metainfo_collection_type_>
    class geometric_domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_ *>
    {
        typedef geometric_domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_*> self_type;
        
    public:
        
        typedef vector_type_ vector_type;
        typedef topologic_domain_type_ topologic_domain_type;
        typedef metainfo_collection_type_ metainfo_collection_type;
        
        geometric_domain_t() : metainfo_collection(0) {}
        geometric_domain_t(topologic_domain_type topologic_domain_, metainfo_collection_type & metainfo_collection_) :
            topologic_domain(topologic_domain_), metainfo_collection(&metainfo_collection_) {}
        
        typedef typename result_of::element< self_type, viennagrid::vertex_tag >::type vertex_type;
        typedef typename result_of::element_hook< self_type, viennagrid::vertex_tag >::type vertex_hook_type;
        typedef typename result_of::const_element_hook< self_type, viennagrid::vertex_tag >::type const_vertex_hook_type;
        
        topologic_domain_type & get_topological_domain() { return topologic_domain; }
        const topologic_domain_type & get_topological_domain() const { return topologic_domain; }
        
        metainfo_collection_type & get_metainfo_collection() { return *metainfo_collection; }
        const metainfo_collection_type & get_metainfo_collection() const { return *metainfo_collection; }
        
    private:
        
        topologic_domain_type topologic_domain;
        metainfo_collection_type * metainfo_collection;
    };
    
    
    
    
    
    namespace result_of
    {
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct element< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename element<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct element_hook< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename element_hook<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct const_element_hook< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename const_element_hook<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct element_range< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename element_range< topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct const_element_range< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename const_element_range< topologic_domain_type, element_type_or_tag>::type type;
        };
        
        
        
        
//         template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, long dim>
//         struct ncell< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim >
//         {
//             typedef typename ncell<topologic_domain_type, dim>::type type;
//         };
//         
//         template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, long dim>
//         struct ncell_hook< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim >
//         {
//             typedef typename ncell_hook<topologic_domain_type, dim>::type type;
//         };
//         
//         template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, long dim>
//         struct const_ncell_hook< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim >
//         {
//             typedef typename const_ncell_hook<topologic_domain_type, dim>::type type;
//         };
//         
//         
//         template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, long dim>
//         struct ncell_range< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim >
//         {
//             typedef typename ncell_range< topologic_domain_type, dim>::type type;
//         };
//         
//         template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, long dim>
//         struct const_ncell_range< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim >
//         {
//             typedef typename const_ncell_range< topologic_domain_type, dim>::type type;
//         };
    }
    
    
    
    template<typename element_type_or_tag, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::element_range<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag>::type elements(geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return elements<element_type_or_tag>( domain.get_topological_domain() );
    }
    
    template<typename element_type_or_tag, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::const_element_range<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag>::type elements(const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return elements<element_type_or_tag>( domain.get_topological_domain() );
    }
    
    

    template<long dim, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::ncell_range<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim>::type ncells(geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return ncells<dim>( domain.get_topological_domain() );
    }
    
    template<long dim, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::const_ncell_range<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, dim>::type ncells(const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return ncells<dim>( domain.get_topological_domain() );
    }
    
    
    
    
    
    namespace result_of
    {
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct topologic_domain< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef topologic_domain_type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct container_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef typename topologic_domain_type::container_collection_type type;
        };
        
        
        
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct metainfo_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef metainfo_collection_type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct metainfo_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type *> >
        {
            typedef metainfo_collection_type type;
        };
        

        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct point_type< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >
        {
            typedef vector_type type;
        };
        
    }
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::topologic_domain< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & topologic_domain( geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::topologic_domain< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & topologic_domain( const geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_collection< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & metainfo_collection( geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::metainfo_collection< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & metainfo_collection( const geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_collection< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > >::type & metainfo_collection( geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::metainfo_collection< geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > >::type & metainfo_collection( const geometric_domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename metainfo_collection_type>
    metainfo_collection_type & metainfo_collection( metainfo_collection_type & collection )
    { return collection; }
    
    template<typename metainfo_collection_type>
    const metainfo_collection_type & metainfo_collection( const metainfo_collection_type & collection )
    { return collection; }
    
    
    
    
    
    
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    metainfo_type & look_up( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), element );
    }
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    const metainfo_type & look_up( const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), element );
    }
    
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_hook_type>
    metainfo_type & look_up( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_hook_type & element_hook )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), dereference_hook(domain, element_hook) );
    }
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_hook_type>
    const metainfo_type & look_up( const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_hook_type & element_hook )
    {
        return look_up<metainfo_type>( domain.get_metainfo_collection(), dereference_hook(domain, element_hook) );
    }
    
    
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type, typename metainfo_type>
    void set( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element, const metainfo_type & meta_info )
    {
        metainfo::set( get_info<element_type, metainfo_type>(domain.get_metainfo_collection()), element, meta_info );
    }
    
    
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & geometric_domain, const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( geometric_domain, vertex );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & geometric_domain, const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( geometric_domain, vertex );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & geometric_domain, const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_hook_type & vertex_hook )
    {
        return viennagrid::look_up<typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( geometric_domain, dereference_hook(geometric_domain, vertex_hook) );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( const geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & geometric_domain, const typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_hook_type & vertex_hook )
    {
        return viennagrid::look_up<typename geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( geometric_domain, dereference_hook(geometric_domain, vertex_hook) );
    }
    
    
    
    
    
    

    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
                >::type::hook_type,
                bool
            >
        push_element( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        metainfo::increment_size< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> >(metainfo_collection(domain));
        return inserter(domain)(element);
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    element_type
                >::type::hook_type,
                bool
            >
        push_element( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element)
    {
        metainfo::increment_size< element_type >(metainfo_collection(domain));
        return inserter(domain)(element);
    }
    
//     template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
//     std::pair<
//                 typename viennagrid::storage::result_of::container_of<
//                     typename result_of::container_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
//                     viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
//                 >::type::hook_type,
//                 bool
//             >
//         push_element_noid( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
//     {
//         metainfo::increment_size< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> >(metainfo_collection(domain));
//         return inserter(domain).insert_noid(element);
//     }
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    element_type
                >::type::hook_type,
                bool
            >
        push_element_noid( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element)
    {
        metainfo::increment_size< element_type >(metainfo_collection(domain));
        return inserter(domain).insert_noid(element);
    }
    
    
    
    template<typename element_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::element_hook<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type>::type
        create_element( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const vector_type & point )
    {
        //typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        //element_type element;
        typename result_of::element_hook<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type>::type ret = push_element(domain, element_type() ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }
    
    template<typename element_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::element_hook<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type>::type
        create_element( geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, typename element_type::id_type id, const vector_type & point )
    {
        //typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        
        element_type element;
        element.id( id );
        
        typename result_of::element_hook<geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type>::type ret = push_element_noid(domain, element_type() ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }
    
    
    
    template<typename plc_type, typename domain_type, typename polygon_hook_type, typename polygon_hook_array_iterator_type,
                typename line_hook_array_iterator_type, typename vertex_hook_array_iterator_type>
    typename result_of::element_hook<domain_type, plc_type>::type create_element( domain_type & domain, polygon_hook_type bounding_polygon_hook, 
                             polygon_hook_array_iterator_type hole_polygons_start, const polygon_hook_array_iterator_type & hole_polygons_end,
                             line_hook_array_iterator_type lines_start, const line_hook_array_iterator_type & lines_end,
                             vertex_hook_array_iterator_type vertices_start, const vertex_hook_array_iterator_type & vertices_end)
    {
        plc_type plc;
        plc.set_container_collection( viennagrid::container_collection(domain) );
        
        plc.set_bounding_polygon( bounding_polygon_hook );
        plc.add_hole_polygons( hole_polygons_start, hole_polygons_end );
        plc.add_lines( lines_start, lines_end );
        plc.add_vertices( vertices_start, vertices_end );
        
        return viennagrid::push_element(domain, plc).first;
    }
    
    
    

    

    
//     template<typename domain_config>
//     typename geometric_domain_t<domain_config>::vector_type & point( geometric_domain_t<domain_config> & geometric_domain, std::size_t pos )
//     {
//         return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, viennagrid::elements<viennagrid::vertex_tag>( geometric_domain )[pos] );
//     }
//     
//     template<typename domain_config>
//     const typename geometric_domain_t<domain_config>::vector_type & point( const geometric_domain_t<domain_config> & geometric_domain, std::size_t pos )
//     {
//         return viennagrid::look_up<typename geometric_domain_t<domain_config>::vector_type>( geometric_domain, viennagrid::elements<viennagrid::vertex_tag>( geometric_domain )[pos] );
//     }
    
    
    
    namespace result_of
    {
        
        template<typename element_tag, typename vector_type, typename hook_tag = viennagrid::storage::pointer_hook_tag,
            typename metainfo_typelist = viennameta::null_type, typename metainfo_container_config = viennagrid::storage::default_container_config>
        struct geometric_domain_config
        {
            typedef typename viennagrid::result_of::default_topologic_config<element_tag, hook_tag>::type toplologic_config;
            typedef typename viennameta::make_typemap<
                viennagrid::topologic_config_tag, toplologic_config,
                viennagrid::vector_type_tag, vector_type,
                viennagrid::metainfo_typelist_tag, metainfo_typelist,
                viennagrid::metainfo_container_config_tag, metainfo_container_config
            >::type type;
        };
        
        
        template<typename geometric_domain_config>
        struct geometric_domain
        {
            typedef typename viennagrid::result_of::topologic_domain<
                    typename result_of::topologic_config<geometric_domain_config>::type
                >::type topologic_domain_type;
                
             typedef typename result_of::vector_type<geometric_domain_config>::type vector_type;
                
            typedef typename viennagrid::storage::collection_t<
                    typename viennagrid::result_of::metainfo_container_typemap<
                        typename viennameta::typelist::result_of::push_back<
                            typename result_of::metainfo_typelist< geometric_domain_config >::type,
                            viennameta::static_pair<
                                viennagrid::vertex_tag,
                                vector_type
                            >
                        >::type,
                        typename result_of::metainfo_container_config<geometric_domain_config>::type,
                        typename result_of::topologic_config<geometric_domain_config>::type
                    >::type
                > metainfo_collection_type;  
            
            typedef geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> type;
        };
        
        
        
        
        
        template<
            typename geometric_domain_type,
            typename element_typelist = 
                typename storage::container_collection::result_of::value_typelist<
                    typename container_collection<geometric_domain_type>::type
                >::type,
            typename container_config = 
                storage::default_container_config>
        struct geometric_view
        {
            typedef typename viennagrid::result_of::topologic_view<typename geometric_domain_type::topologic_domain_type, element_typelist>::type topologic_view_type;
            typedef typename geometric_domain_type::metainfo_collection_type metainfo_collection_type;
            typedef typename viennagrid::geometric_domain_t<typename geometric_domain_type::vector_type, topologic_view_type, metainfo_collection_type*> type;
        };
    }
    
    

    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    struct create_view_helper< geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
    {
        typedef geometric_domain_t<vector_type, topologic_domain_type, metainfo_collection_type> geomatric_view_type;
        
        template<typename domain_type>
        static geomatric_view_type create( domain_type & domain )
        {
            typedef typename geomatric_view_type::topologic_domain_type topologic_view_type;
            return geomatric_view_type(
                viennagrid::create_view<topologic_view_type>( viennagrid::topologic_domain(domain) ),
                domain.get_metainfo_collection()
            );
        }
    };
    
    
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::hook_iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_hook(domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::hook_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.hook_begin(); it != range.hook_end(); ++it)
        {
            if ( viennagrid::dereference_hook(domain, *it).id() == id )
                return it;
        }
        
        return range.hook_end();
        //return typename result_of::element_hook<domain_type, element_tag>::type();
        //return storage::hook::invalid_hook( range );
    }
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::const_hook_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_hook(const domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_hook_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.hook_begin(); it != range.hook_end(); ++it)
        {
            if ( viennagrid::dereference_hook(domain, *it).id() == id )
                return it;
        }
        
        return range.hook_end();
        //return typename result_of::const_element_hook<domain_type, element_tag>::type();
        //return storage::hook::invalid_hook( range );
    }

    
}

#endif

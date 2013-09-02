#ifndef VIENNAGRID_DOMAIN_HPP
#define VIENNAGRID_DOMAIN_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <typeinfo>
#include "viennagrid/forwards.hpp"

#include "viennagrid/meta/algorithm.hpp"

#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/algorithm.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"

#include "viennagrid/element/element_view.hpp"

/** @file domain.hpp
    @brief Contains definition and implementation of domain and domain views
*/


namespace viennagrid
{
  /** @brief For internal use only */
  template<typename container_collection_type>
  class view_domain_setter
  {
  public:

    view_domain_setter(container_collection_type & domain_) : domain(domain_) {}

    template<typename container_type>
    void operator()( container_type & container )
    {
        typedef typename container_type::value_type value_type;
        container.set_base_container( viennagrid::storage::collection::get<value_type>(domain) );
    }

  private:
    container_collection_type & domain;
  };


  /** @brief Proxy object for a domain. This is used for wrapping a domain/domain view when creating a view
    *
    * @tparam DomainT    The domain/domain view type
    */
  template<typename DomainT>
  struct domain_proxy
  {
    domain_proxy( DomainT & domain_ ) : domain(&domain_){}
    DomainT * domain;
  };


  /** @brief For internal use only */
  template<typename container_type_, typename change_counter_type>
  struct coboundary_container_wrapper
  {
      typedef container_type_ container_type;
      coboundary_container_wrapper() : change_counter(0) {}

      change_counter_type change_counter;
      container_type container;
  };

  /** @brief For internal use only */
  template<typename container_type_, typename change_counter_type>
  struct neighbour_container_wrapper
  {
      typedef container_type_ container_type;
      neighbour_container_wrapper() : change_counter(0) {}

      change_counter_type change_counter;
      container_type container;
  };

  /** @brief For internal use only */
  template<typename container_type_, typename change_counter_type>
  struct boundary_information_wrapper
  {
      typedef container_type_ container_type;
      boundary_information_wrapper() : change_counter(0) {}

      change_counter_type change_counter;
      container_type container;
  };

  namespace result_of
  {
    template<typename ElementTypelistT, typename ContainerTypemapT>
    struct filter_element_container;

    template<typename ContainerTypemapT>
    struct filter_element_container<viennagrid::meta::null_type, ContainerTypemapT>
    {
        typedef viennagrid::meta::null_type type;
    };

    template<typename ElementT, typename TailT, typename ContainerTypemapT>
    struct filter_element_container<viennagrid::meta::typelist_t< ElementT, TailT> , ContainerTypemapT>
    {
        typedef typename viennagrid::meta::typemap::result_of::find<ContainerTypemapT, ElementT>::type result_type;

        typedef typename filter_element_container<TailT, ContainerTypemapT>::type new_tail;

        typedef typename viennagrid::meta::IF<
            viennagrid::meta::EQUAL< result_type, viennagrid::meta::not_found >::value,
            new_tail,
            viennagrid::meta::typelist_t<
                result_type,
                new_tail
            >
        >::type type;
    };


    // domain_element_collection_type
    template <typename WrappedConfigT>
    struct domain_element_collection_type
    {
      typedef typename config::result_of::element_collection< WrappedConfigT >::type   type;
    };

    template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig>
    struct domain_element_collection_type< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >
    {
      typedef typename WrappedDomainConfigType::type   DomainConfigType;

      typedef typename domain_element_collection_type<WrappedDomainConfigType>::type   full_domain_element_collection_type;

      typedef typename filter_element_container<ElementTypeList, typename full_domain_element_collection_type::typemap>::type      view_container_collection_typemap;
      typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type   type;
    };

    // domain_appendix_type
    template <typename WrappedConfigT>
    struct domain_appendix_type
    {
      typedef typename WrappedConfigT::type   ConfigType;

      typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::coboundary_container_collection_typemap<WrappedConfigT>::type >::type   coboundary_collection_type;
      typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::neighbour_container_collection_typemap< WrappedConfigT>::type >::type   neighbour_collection_type;
      typedef typename viennagrid::storage::result_of::collection< typename viennagrid::result_of::boundary_information_collection_typemap<WrappedConfigT>::type >::type   boundary_information_type;

      typedef typename viennagrid::storage::collection_t<
            typename viennagrid::meta::make_typemap<

                coboundary_collection_tag,
                coboundary_collection_type,

                neighbour_collection_tag,
                neighbour_collection_type,

                boundary_information_collection_tag,
                boundary_information_type

            >::type
      > type;
    };

    template <typename WrappedDomainConfigT, typename ElementTypeListT, typename ContainerConfigT>
    struct domain_appendix_type< decorated_domain_view_config<WrappedDomainConfigT, ElementTypeListT, ContainerConfigT> >
    {
      typedef typename domain_appendix_type<WrappedDomainConfigT>::type    type;
    };


    // domain_change_counter_type
    template <typename WrappedConfigT>
    struct domain_change_counter_type
    {
      typedef typename config::result_of::query_config<typename WrappedConfigT::type, config::domain_change_counter_tag>::type  type;
    };

    template <typename WrappedDomainConfigT, typename ElementTypeList, typename ContainerConfig>
    struct domain_change_counter_type< decorated_domain_view_config<WrappedDomainConfigT, ElementTypeList, ContainerConfig> >
    {
      typedef typename config::result_of::query_config<typename WrappedDomainConfigT::type, config::domain_change_counter_tag>::type  type;
    };


    // domain_inserter_type
    template <typename WrappedConfigType>
    struct domain_inserter_type
    {
      typedef typename config::result_of::element_collection<WrappedConfigType>::type                                                       element_collection_type;
      typedef typename viennagrid::result_of::id_generator<WrappedConfigType>::type                                                 id_generator_type;
      typedef typename domain_change_counter_type<WrappedConfigType>::type change_counter_type;

      typedef typename viennagrid::storage::result_of::physical_inserter<element_collection_type, change_counter_type, id_generator_type>::type   type;
    };

    template <typename WrappedDomainConfigT, typename ElementTypeList, typename ContainerConfig>
    struct domain_inserter_type< decorated_domain_view_config<WrappedDomainConfigT, ElementTypeList, ContainerConfig> >
    {
      typedef decorated_domain_view_config<WrappedDomainConfigT, ElementTypeList, ContainerConfig>  argument_type;
      typedef WrappedDomainConfigT ConfigType;

      typedef typename domain_element_collection_type<argument_type>::type                             view_container_collection_type;
      typedef typename domain_inserter_type<WrappedDomainConfigT>::type                             full_domain_inserter_type;
      typedef typename domain_change_counter_type<WrappedDomainConfigT>::type change_counter_type;

      typedef typename viennagrid::storage::result_of::recursive_inserter<view_container_collection_type, change_counter_type, full_domain_inserter_type>::type      type;
    };
  }

  template <typename WrappedConfigType>
  class domain_t
  {
    typedef domain_t<WrappedConfigType> self_type;

  public:
    typedef WrappedConfigType                    wrapped_config_type;
    typedef typename WrappedConfigType::type     config_type;


    typedef typename result_of::domain_element_collection_type<WrappedConfigType>::type     element_collection_type;
    typedef typename result_of::domain_appendix_type<WrappedConfigType>::type               appendix_type;
    typedef typename result_of::domain_change_counter_type<WrappedConfigType>::type         change_counter_type;
    typedef typename result_of::domain_inserter_type<WrappedConfigType>::type               inserter_type;


    /** @brief Default constructor */
    domain_t() : inserter( element_container_collection, change_counter_ ), change_counter_(0) {}

    /** @brief Constructor for creating a view from another domain/domain view
      *
      * @tparam OtherWrappedConfigT     Wrapped config type of the domain type from which the view is created
      * @tparam proxy                   Proxy object wrapping the domain object from which the view is created
      */
    template<typename OtherWrappedConfigT>
    domain_t( domain_proxy<domain_t<OtherWrappedConfigT> > proxy ) : change_counter_(0)
    {
        typedef typename domain_t<OtherWrappedConfigT>::element_collection_type   other_element_collection_type;

        view_domain_setter< other_element_collection_type > functor(proxy.domain->element_collection());
        viennagrid::storage::collection::for_each(element_container_collection, functor);

        inserter = inserter_type( element_container_collection, change_counter_,proxy.domain->get_inserter() );
    }

    ~domain_t() {}


    /** @brief Copy constructor, remember that copying a domain might have performance impact due to handle fixing: handles for boundary element will point to other locations.
      *
      * @param  other                   The domain which is copied to *this
      */
    domain_t(const domain_t & other) : element_container_collection(other.element_container_collection), appendix_(other.appendix_), inserter(other.inserter), change_counter_(other.change_counter_)
    {
      inserter.set_domain_info( element_container_collection, change_counter_ );
      increment_change_counter();

      fix_handles(other, *this);
    }

    /** @brief Assignement operator, remember that assigning a domain to another might have performance impact due to handle fixing: handles for boundary element will point to other locations.
      *
      * @param  other                   The domain which is assigned to *this
      * @return                         reference to *this
      */
    domain_t & operator=( domain_t const & other)
    {
//           element_container_collection = element_collection_type();
      element_container_collection = other.element_container_collection;
      
      appendix_ = other.appendix_;
      inserter = other.inserter;
      change_counter_ = other.change_counter_;
      
      inserter.set_domain_info( element_container_collection, change_counter_ );
      increment_change_counter();
      
      fix_handles(other, *this);
      return *this;
    }

    /** @brief Completely clear a domain
      *
      */
    void clear()
    {
      *this = domain_t();
    }
    
  public:

    // TODO no direct access to collection!
    
    /** @brief For internal use only */
    element_collection_type & element_collection() { return element_container_collection; }
    element_collection_type const & element_collection() const { return element_container_collection; }

    /** @brief For internal use only */
    appendix_type & appendix() { return appendix_; }
    appendix_type const & appendix() const { return appendix_; }

    /** @brief For internal use only */
    inserter_type & get_inserter() { return inserter; }
    inserter_type const & get_inserter() const { return inserter; }

    /** @brief For internal use only */
    bool is_obsolete( change_counter_type change_counter_to_check ) const { return change_counter_to_check != change_counter_; }
    void update_change_counter( change_counter_type & change_counter_to_update ) const { change_counter_to_update = change_counter_; }
    void increment_change_counter() { ++change_counter_; }

  protected:
    element_collection_type element_container_collection;

    appendix_type appendix_;
    inserter_type inserter;

    change_counter_type change_counter_;
  };


  namespace result_of
  {
    // doxygen doku in forwards.hpp
    template<typename WrappedConfigType>
    struct domain
    {
      typedef domain_t<WrappedConfigType> type;
    };
  }



  /** @brief For internal use only */
  template<typename WrappedConfigType>
  bool is_obsolete( domain_t<WrappedConfigType> const & domain, typename domain_t<WrappedConfigType>::change_counter_type change_counter_to_check )
  { return domain.is_obsolete( change_counter_to_check ); }

  /** @brief For internal use only */
  template<typename WrappedConfigType>
  void update_change_counter( domain_t<WrappedConfigType> & domain, typename domain_t<WrappedConfigType>::change_counter_type & change_counter_to_update )
  { domain.update_change_counter( change_counter_to_update ); }

  /** @brief For internal use only */
  template<typename WrappedConfigType>
  void increment_change_counter( domain_t<WrappedConfigType> & domain )
  { domain.increment_change_counter(); }









  /** @brief For internal use only */
  template<typename element_tag, typename coboundary_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  coboundary_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type & coboundary_collection( domain_type & domain )
  { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> >( viennagrid::storage::collection::get<coboundary_collection_tag>( domain.appendix() ) );}

  /** @brief For internal use only */
  template<typename element_tag, typename coboundary_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  coboundary_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type const & coboundary_collection( domain_type const & domain )
  { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> >( viennagrid::storage::collection::get<coboundary_collection_tag>( domain.appendix() ) );}


  /** @brief For internal use only */
  template<typename element_tag, typename connector_element_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  neighbour_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, connector_element_tag>
              >::type & neighbour_collection( domain_type & domain )
  { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, connector_element_tag> >( viennagrid::storage::collection::get<neighbour_collection_tag>( domain.appendix() ) ); }

  /** @brief For internal use only */
  template<typename element_tag, typename connector_element_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  neighbour_collection_tag
              >::type,
              viennagrid::meta::static_pair<element_tag, connector_element_tag>
              >::type const & neighbour_collection( domain_type const & domain )
  { return viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, connector_element_tag> >( viennagrid::storage::collection::get<neighbour_collection_tag>( domain.appendix() ) ); }


  /** @brief For internal use only */
  template<typename element_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
      typename viennagrid::storage::result_of::value_type<
          typename domain_type::appendix_type,
          boundary_information_collection_tag
      >::type,
      element_tag
  >::type & boundary_information_collection( domain_type & domain )
  { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<boundary_information_collection_tag>( domain.appendix() ) ); }

  /** @brief For internal use only */
  template<typename element_tag, typename domain_type>
  typename viennagrid::storage::result_of::value_type<
      typename viennagrid::storage::result_of::value_type<
          typename domain_type::appendix_type,
          boundary_information_collection_tag
      >::type,
      element_tag
  >::type const & boundary_information_collection( domain_type const & domain )
  { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<boundary_information_collection_tag>( domain.appendix() ) ); }

}


namespace viennagrid
{

  namespace result_of
  {
    // doxygen doku in forwards.hpp
    template<typename SomethingT>
    struct container_collection_typemap;

    template<typename TypemapT>
    struct container_collection_typemap< storage::collection_t<TypemapT> >
    {
        typedef TypemapT type;
    };

    template<typename TypemapT>
    struct container_collection_typemap< const storage::collection_t<TypemapT> >
    {
        typedef TypemapT type;
    };

    template<typename KeyT, typename ValueT, typename TailT>
    struct container_collection_typemap< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<KeyT, ValueT>, TailT > >
    {
        typedef viennagrid::meta::typelist_t< viennagrid::meta::static_pair<KeyT, ValueT>, TailT > type;
    };


    // doxygen doku in forwards.hpp
    template<typename TypemapT>
    struct element_collection< storage::collection_t<TypemapT> >
    {
        typedef storage::collection_t<TypemapT> type;
    };

    template<typename WrappedConfigT>
    struct element_collection< domain_t<WrappedConfigT> >
    {
        typedef typename domain_t<WrappedConfigT>::element_collection_type type;
    };

    template<typename WrappedConfigT>
    struct element_collection< const domain_t<WrappedConfigT> >
    {
        typedef typename domain_t<WrappedConfigT>::element_collection_type type;
    };

    
    /** @brief Metafunction for obtaining the element typelist of something
     *
     * @tparam SomethingT     The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct element_typelist
    {
        typedef typename element_collection<SomethingT>::type container_collection;
        typedef typename viennagrid::meta::typemap::result_of::key_typelist<typename container_collection::typemap >::type type;
    };
    
    template<typename HostElementT>
    struct element_typelist_for_element;

    template<>
    struct element_typelist_for_element<viennagrid::meta::null_type>
    {
      typedef viennagrid::meta::null_type type;
    };

    template<typename HeadT, typename TailT>
    struct element_typelist_for_element< viennagrid::meta::typelist_t<HeadT, TailT> >
    {
      typedef typename HeadT::first BoundaryContainerType;
      typedef typename BoundaryContainerType::value_type CurrentElementType;
      
      typedef viennagrid::meta::typelist_t<
            CurrentElementType,
            typename element_typelist_for_element<TailT>::type
          > type;
    };
    
    template<typename ElementTagT, typename WrappedConfigT>
    struct element_typelist< element_t<ElementTagT, WrappedConfigT> >
    {
      typedef typename element_t<ElementTagT, WrappedConfigT>::bnd_cell_container_typelist bnd_cell_container_typelist;
      typedef typename element_typelist_for_element<bnd_cell_container_typelist>::type type;
    };

    /** @brief Metafunction for obtaining the element typelist of something
     *
     * @tparam SomethingT     The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct element_taglist
    {
        typedef typename viennagrid::meta::typelist::TRANSFORM<
            result_of::element_tag,
            typename element_typelist<SomethingT>::type
        >::type type;
    };

    
    /** @brief Metafunction for query if an element type/tag is boundary element of another host element
     *
     * @tparam HostElementT                 The host element type
     * @tparam BoundaryElementTypeOrTagT    The element type/tag to be queried if it is a boundary element
     */
    template<typename HostElementT, typename BoundaryElementTypeOrTagT>
    struct is_boundary
    {
      typedef typename result_of::element_tag<BoundaryElementTypeOrTagT>::type BoundaryElementTag;
      static const bool value =
        meta::typelist::result_of::index_of<
          typename element_taglist<HostElementT>::type,
          BoundaryElementTag
        >::value != -1;
    };

    
    /** @brief For internal use only */
    template<typename element_typelist, typename element_type>
    struct referencing_element_typelist_impl;

    template<typename element_type>
    struct referencing_element_typelist_impl<viennagrid::meta::null_type, element_type>
    {
        typedef viennagrid::meta::null_type type;
    };

    template<typename head, typename tail, typename element_type>
    struct referencing_element_typelist_impl< viennagrid::meta::typelist_t<head, tail>, element_type >
    {
        typedef typename referencing_element_typelist_impl<tail, element_type>::type tail_typelist;

        typedef typename viennagrid::meta::IF<
            is_boundary<head, element_type>::value,
            viennagrid::meta::typelist_t< head, tail_typelist>,
            tail_typelist
        >::type type;
    };

    /** @brief Metafunction for obtaining the an element typelist including all element which references a given element. e.g. in a default triangle domain the referencing element typelist based on the domain for a vertex contains a line type and a triangle type. The host type can also be an element in which case all boundary elements referening the given element are returned (in a default tetrahedron domain the referencing elements for a vertex based on a triangle contains only a line type)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The element type/tag which the resulting elements references
     */
    template<typename SomethingT, typename ElementTypeOrTagT>
    struct referencing_element_typelist
    {
        typedef typename element<SomethingT, ElementTypeOrTagT>::type element_type;
        typedef typename element_typelist<SomethingT>::type elements;
        typedef typename referencing_element_typelist_impl<elements, element_type>::type type;
    };
  }

  /** @brief For internal use only */
  template <typename WrappedConfigType, typename ElementTypeList, typename ContainerConfig>
  class decorated_domain_view_config
  {
    private:
      typedef typename config::result_of::element_collection< WrappedConfigType >::type                                                   element_collection_type;
      typedef typename viennagrid::result_of::id_generator<WrappedConfigType>::type                                               id_generator_type;

      typedef typename result_of::filter_element_container<ElementTypeList, typename element_collection_type::typemap>::type        view_container_collection_typemap;
      typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type    view_container_collection_type;


    public:
      typedef view_container_collection_typemap    type;

      typedef WrappedConfigType                     domain_config_type;
  };

  /** @brief For internal use only */
  template <typename WrappedConfigType, typename E, typename C, typename ElementTypeList, typename ContainerConfig>
  class decorated_domain_view_config< decorated_domain_view_config<WrappedConfigType, E, C>, ElementTypeList, ContainerConfig>
  {
    private:
      typedef typename config::result_of::element_collection< WrappedConfigType >::type                                                   element_collection_type;
      typedef typename viennagrid::result_of::id_generator<WrappedConfigType>::type                                               id_generator_type;

      typedef typename result_of::filter_element_container<ElementTypeList, typename element_collection_type::typemap>::type        view_container_collection_typemap;
      typedef typename viennagrid::storage::result_of::view_collection<view_container_collection_typemap, ContainerConfig>::type    view_container_collection_type;


    public:
      typedef view_container_collection_typemap    type;

      typedef typename decorated_domain_view_config<WrappedConfigType, E, C>::domain_config_type      domain_config_type;
  };


  namespace result_of
  {
    /** @brief Metafunction for obtaining a domain view from a domain using an element type list and a container configuration
     *
     * @tparam DomainT            The host domain type
     * @tparam ElementTypelistT   A typelist of all element types which will be referenced by the domain view. All element from the domain are used as default.
     * @tparam ContainerConfigT   A configuration typemap which container should be used for storing the handles within the domain view. The default view container config is used as default (std::set is used).
     */
    template<
        typename DomainT,
        typename ElementTypelistT =
            typename viennagrid::meta::typemap::result_of::key_typelist<
                typename element_collection<DomainT>::type::typemap
            >::type,
        typename ContainerConfigT =
            storage::default_view_container_config
        >
    struct domain_view_from_typelist
    {
        typedef domain_t< decorated_domain_view_config<typename DomainT::wrapped_config_type, ElementTypelistT, ContainerConfigT> >  type;
    };


    /** @brief Metafunction for obtaining a domain view from a domain. Elements can be directly given. If viennagrid::meta::null_type is specified somewhere all following types will be ignored. The default type for all element types is viennagrid::meta::null_type.
     *
     * @tparam DomainT              The host domain type
     * @tparam Element0TypeOrTagT   The first element type which is present in the domain view
     * @tparam Element1TypeOrTagT   The second element type which is present in the domain view
     * @tparam Element2TypeOrTagT   The third element type which is present in the domain view
     * @tparam Element3TypeOrTagT   The forth element type which is present in the domain view
     * @tparam Element4TypeOrTagT   The fifth element type which is present in the domain view
     * @tparam Element5TypeOrTagT   The sixth element type which is present in the domain view
     * @tparam Element6TypeOrTagT   The seventh element type which is present in the domain view
     * @tparam Element7TypeOrTagT   The eighth element type which is present in the domain view
     * @tparam Element8TypeOrTagT   The nineth element type which is present in the domain view
     * @tparam Element9TypeOrTagT   The tenth element type which is present in the domain view
     */
    template<typename DomainT,
             typename Element0TypeOrTagT = viennagrid::meta::null_type, typename Element1TypeOrTagT = viennagrid::meta::null_type,
             typename Element2TypeOrTagT = viennagrid::meta::null_type, typename Element3TypeOrTagT = viennagrid::meta::null_type,
             typename Element4TypeOrTagT = viennagrid::meta::null_type, typename Element5TypeOrTagT = viennagrid::meta::null_type,
             typename Element6TypeOrTagT = viennagrid::meta::null_type, typename Element7TypeOrTagT = viennagrid::meta::null_type,
             typename Element8TypeOrTagT = viennagrid::meta::null_type, typename Element9TypeOrTagT = viennagrid::meta::null_type>
    struct domain_view
    {
        typedef typename domain_view_from_typelist<
            DomainT,
            typename viennagrid::meta::make_typelist<
              typename element<DomainT, Element0TypeOrTagT>::type,
              typename element<DomainT, Element1TypeOrTagT>::type,
              typename element<DomainT, Element2TypeOrTagT>::type,
              typename element<DomainT, Element3TypeOrTagT>::type,
              typename element<DomainT, Element4TypeOrTagT>::type,
              typename element<DomainT, Element5TypeOrTagT>::type,
              typename element<DomainT, Element6TypeOrTagT>::type,
              typename element<DomainT, Element7TypeOrTagT>::type,
              typename element<DomainT, Element8TypeOrTagT>::type,
              typename element<DomainT, Element9TypeOrTagT>::type
            >::type
        >::type type;
    };


    template<typename DomainT>
    struct domain_view<DomainT,
                       viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type,
                       viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type, viennagrid::meta::null_type>
    {
        typedef typename domain_view_from_typelist<DomainT>::type type;
    };
  }

  
  /** @brief Creates a view out of a domain using the domain_proxy object
    *
    * @tparam DomainOrSegmentT    The domain or segment type from which the domain view is created
    * @param  domain              The domain or segment object from which the domain view is created
    * @return                     a domain_proxy object holding the host domain/segment object, can be assigned to a domain_t object
    */
  template<typename DomainOrSegmentT>
  domain_proxy<DomainOrSegmentT> make_view(DomainOrSegmentT & domain)
  {
      return domain_proxy<DomainOrSegmentT>( domain );
  }



  /** @brief For internal use only */
  template<typename container_collection_type>
  class handle_domain_functor
  {
  public:

      handle_domain_functor(container_collection_type & collection_) : collection(collection_) {}

      template<typename container_type>
      void operator()( container_type & container )
      {
          typedef typename container_type::value_type value_type;
          storage::view::handle( viennagrid::storage::collection::get<value_type>(collection), container);
      }


  private:
      container_collection_type & collection;
  };

  
  /** @brief Function for inserting handles to all element from a domain in a domain view
    *
    * @tparam DomainT     The domain or segment type
    * @tparam ViewT       The domain view type
    * @param  domain      The input domain/segment object
    * @param  view        The output view object
    */
  template<typename DomainT, typename ViewT>
  void handle_domain(DomainT & domain, ViewT & view)
  {
      handle_domain_functor< typename result_of::element_collection<DomainT>::type > functor( element_collection(domain) );
      viennagrid::storage::collection::for_each( element_collection(view), functor);
  }




  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::element_collection_type & element_collection( domain_t<ConfigType> & domain)
  { return domain.element_collection(); }

  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::element_collection_type const & element_collection( domain_t<ConfigType> const & domain)
  { return domain.element_collection(); }


  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::inserter_type & inserter(domain_t<ConfigType> & domain)
  { return domain.get_inserter(); }

  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::inserter_type const & inserter(domain_t<ConfigType> const & domain)
  { return domain.get_inserter(); }


  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::inserter_type::id_generator_type & id_generator(domain_t<ConfigType> & domain)
  { return inserter(domain).get_id_generator(); }

  /** @brief For internal use only */
  template<typename ConfigType>
  typename domain_t<ConfigType>::inserter_type::id_generator_type const & id_generator(domain_t<ConfigType> const & domain)
  { return inserter(domain).get_id_generator(); }





  /** @brief Function for obtaining the maximum ID for a specifig element type/tag in a domain/segment
    *
    * @tparam ElementTypeOrTag     The element type/tag from which the maximum ID is queried
    * @tparam DomainOrSegmentT     The domain/segment type
    * @param  domain_or_segment    The domain/segment object
    * @return                      The maximum ID for specified element type/tag
    */
//   template<typename ElementTypeOrTag, typename DomainOrSegmentT>
//   typename viennagrid::result_of::id<
//     typename viennagrid::result_of::element<DomainOrSegmentT, ElementTypeOrTag>::type
//   >::type max_id(DomainOrSegmentT const & domain_or_segment)
//   { return id_generator(domain_or_segment).max_id( viennagrid::meta::tag< typename viennagrid::result_of::element<DomainOrSegmentT, ElementTypeOrTag>::type >() ); }
  
  template<typename element_type_or_tag, typename domain_type>
  typename viennagrid::result_of::id< typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type >::type id_upper_bound( domain_type const & domain )
  {
      typedef typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type element_type;
      return id_generator(domain).max_id( viennagrid::meta::tag<element_type>() );
  }



  /** @brief Function for dereferencing a handle using a domain object
    *
    * @tparam WrappedConfigT     The wrapped config of the domain type
    * @tparam HandleT            A handle type
    * @param  domain             The host domain object
    * @param  handle             The handle to be dereferenced
    * @return                    A C++ reference to an element which is referenced by handle
    */
  template<typename WrappedConfigT, typename HandleT>
  typename storage::handle::result_of::value_type<HandleT>::type & dereference_handle(domain_t<WrappedConfigT> & domain, HandleT const & handle)
  {
      typedef typename storage::handle::result_of::value_type<HandleT>::type value_type;
      return storage::collection::get<value_type>(element_collection(domain)).dereference_handle( handle );
  }

  /** @brief Function for dereferencing a handle using a domain object, const version
    *
    * @tparam WrappedConfigT     The wrapped config of the domain type
    * @tparam HandleT            A handle type
    * @param  domain             The host domain object
    * @param  handle             The handle to be dereferenced
    * @return                    A C++ const reference to an element which is referenced by handle
    */
  template<typename WrappedConfigT, typename HandleT>
  typename storage::handle::result_of::value_type<HandleT>::type const & dereference_handle(domain_t<WrappedConfigT> const & domain, HandleT const & handle)
  {
      typedef typename storage::handle::result_of::value_type<HandleT>::type value_type;
      return storage::collection::get<value_type>(element_collection(domain)).dereference_handle( handle );
  }


  /** @brief Function for creating a handle for a given element using a domain/segment object
    *
    * @tparam DomainOrSegmentT      The host domain/segment type
    * @tparam ElementTagT           The tag of the element of which a handle is created
    * @tparam WrappedConfigT        The wrapped config of the element of which a handle is created
    * @param  domain_or_segment     The host domain/segment object
    * @param  element               The element of which a handle is created
    * @return                       A handle referencing the given element
    */
  template<typename DomainOrSegmentT, typename ElementTagT, typename WrappedConfigT>
  typename result_of::handle<DomainOrSegmentT, element_t<ElementTagT, WrappedConfigT> >::type handle(DomainOrSegmentT & domain_or_segment, element_t<ElementTagT, WrappedConfigT> & element)
  { return storage::collection::get< element_t<ElementTagT, WrappedConfigT> >(element_collection(domain_or_segment)).handle( element ); }

  /** @brief Function for creating a handle for a given element using a domain/segment object, const version
    *
    * @tparam DomainOrSegmentT      The host domain/segment type
    * @tparam ElementTagT           The tag of the element of which a handle is created
    * @tparam WrappedConfigT        The wrapped config of the element of which a handle is created
    * @param  domain_or_segment     The host domain/segment object
    * @param  element               The element of which a handle is created
    * @return                       A const handle referencing the given element
    */
  template<typename DomainOrSegmentT, typename ElementTagT, typename WrappedConfigT>
  typename result_of::const_handle<DomainOrSegmentT, element_t<ElementTagT, WrappedConfigT> >::type handle(DomainOrSegmentT const & domain_or_segment, element_t<ElementTagT, WrappedConfigT> const & element)
  { return storage::collection::get< element_t<ElementTagT, WrappedConfigT> >(element_collection(domain_or_segment)).handle( element ); }






  /** @brief Function for querying a local vertex on a boundary element within a host element
    *
    * @tparam ElementTagT               The element tag of the host element type
    * @tparam WrappedConfigT            The wrapped config of the host element type
    * @tparam BoundaryHandleT           The handle type of the boundary element
    * @param  host_element              The host element object
    * @param  boundary_element_handle   The boundary element handle
    * @param  index                     The index defining which vertex within the boundary element is queried
    * @return                           A handle referencing the given element
    */
  template<typename ElementTagT, typename WrappedConfigT, typename BoundaryHandleT>
  typename viennagrid::result_of::vertex_handle< element_t<ElementTagT, WrappedConfigT> >::type local_vertex(
      element_t<ElementTagT, WrappedConfigT> & host_element,
      BoundaryHandleT const & boundary_element_handle,
      std::size_t index)
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<BoundaryHandleT>::type element_type_2;
    element_type_2 & bnd_kcell = viennagrid::dereference_handle(host_element, boundary_element_handle);
    return viennagrid::vertices(bnd_kcell).handle_at(host_element.global_to_local_orientation(boundary_element_handle, index));
  }
  
  /** @brief Function for querying a local vertex on a boundary element within a host element, const version
    *
    * @tparam ElementTagT               The element tag of the host element type
    * @tparam WrappedConfigT            The wrapped config of the host element type
    * @tparam BoundaryHandleT           The handle type of the boundary element
    * @param  host_element              The host element object
    * @param  boundary_element_handle   The boundary element handle
    * @param  index                     The index defining which vertex within the boundary element is queried
    * @return                           A const handle referencing the given element
    */
  template<typename ElementTagT, typename WrappedConfigT, typename BoundaryHandleT>
  typename viennagrid::result_of::const_vertex_handle< element_t<ElementTagT, WrappedConfigT> >::type local_vertex(
      element_t<ElementTagT, WrappedConfigT> const & host_element,
      BoundaryHandleT const & boundary_element_handle,
      std::size_t index)
  {
    typedef typename viennagrid::storage::handle::result_of::value_type<BoundaryHandleT>::type element_type_2;
    element_type_2 const & bnd_kcell = viennagrid::dereference_handle(host_element, boundary_element_handle);
    return viennagrid::vertices(bnd_kcell).handle_at(host_element.global_to_local_orientation(boundary_element_handle, index));
  }



  /** @brief For internal use only */
  template<typename ContainerT>
  class dereference_handle_comperator
  {
  public:

      dereference_handle_comperator(ContainerT const & container_) : container(container_) {}

      template<typename HandleT>
      bool operator() ( HandleT h1, HandleT h2 )
      {
          return &viennagrid::dereference_handle( container, h1 ) < &viennagrid::dereference_handle( container, h2 );
      }

  private:
      ContainerT const & container;
  };

}



namespace viennagrid
{

  namespace result_of
  {
    /** @brief Metafunction for querying if an element is present within a domain, domain view or segment
     *
     * @tparam DomainViewOrSegmentT       The host domain/domain view/segment type
     * @tparam ElementTypeOrTagT          The requested element type/tag
     */
    template<typename DomainViewOrSegmentT, typename ElementTypeOrTagT>
    struct is_element_present;

    template<typename WrappedConfigType, typename element_type_or_tag>
    struct is_element_present< domain_t<WrappedConfigType>, element_type_or_tag >
    {
      typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type tag;
      static const bool value = config::result_of::is_element_present<WrappedConfigType, tag>::value;
    };

    template<typename WrappedConfigType, typename element_type_or_tag>
    struct is_element_present< const domain_t<WrappedConfigType>, element_type_or_tag >
    {
      static const bool value = is_element_present<domain_t<WrappedConfigType>, element_type_or_tag>::value;
    };




    /** @brief For internal use only */
    template<typename typelist, typename element_tag>
    struct is_element_present_helper;

    template<typename element_tag>
    struct is_element_present_helper<meta::null_type, element_tag>
    {
        static const bool value = false;
    };

    template<typename element_type, typename tail, typename element_tag>
    struct is_element_present_helper< meta::typelist_t<element_type, tail>, element_tag>
    {
        static const bool value =
          viennagrid::meta::EQUAL<
            typename element_type::tag,
            element_tag
          >::value ||
          is_element_present_helper<tail, element_tag>::value;
    };


    template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig, typename element_type_or_tag>
    struct is_element_present< domain_t< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >, element_type_or_tag >
    {
      typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type tag;
      static const bool value = is_element_present_helper<ElementTypeList, tag>::value;
    };

    template <typename WrappedDomainConfigType, typename ElementTypeList, typename ContainerConfig, typename element_type_or_tag>
    struct is_element_present< const domain_t< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >, element_type_or_tag >
    {
      static const bool value = is_element_present<domain_t< decorated_domain_view_config<WrappedDomainConfigType, ElementTypeList, ContainerConfig> >, element_type_or_tag>::value;
    };




    // doxygen doku in forwards.hpp
    template<typename DomainSegmentType, typename element_type_or_tag>
    struct element
    {
        typedef typename meta::STATIC_ASSERT< is_element_present<DomainSegmentType, element_type_or_tag>::value >::type ERROR_ELEMENT_IS_NOT_PRESENT_IN_DOMAIN;
        typedef typename element< typename element_collection< DomainSegmentType >::type, element_type_or_tag >::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename DomainSegmentType, typename element_type_or_tag>
    struct handle
    {
        typedef typename meta::STATIC_ASSERT< is_element_present<DomainSegmentType, element_type_or_tag>::value >::type ERROR_ELEMENT_IS_NOT_PRESENT_IN_DOMAIN;
        typedef typename handle< typename element_collection< DomainSegmentType >::type, element_type_or_tag >::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename DomainSegmentType, typename element_type_or_tag>
    struct const_handle
    {
        typedef typename meta::STATIC_ASSERT< is_element_present<DomainSegmentType, element_type_or_tag>::value >::type ERROR_ELEMENT_IS_NOT_PRESENT_IN_DOMAIN;
        typedef typename const_handle< typename element_collection< DomainSegmentType >::type, element_type_or_tag >::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename DomainSegmentType, typename element_type_or_tag>
    struct element_range
    {
        typedef typename meta::STATIC_ASSERT< is_element_present<DomainSegmentType, element_type_or_tag>::value >::type ERROR_ELEMENT_IS_NOT_PRESENT_IN_DOMAIN;
        typedef typename element_range< typename element_collection< DomainSegmentType >::type, element_type_or_tag >::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename DomainSegmentType, typename element_type_or_tag>
    struct const_element_range
    {
        typedef typename meta::STATIC_ASSERT< is_element_present<DomainSegmentType, element_type_or_tag>::value >::type ERROR_ELEMENT_IS_NOT_PRESENT_IN_DOMAIN;
        typedef typename const_element_range< typename element_collection< DomainSegmentType >::type, element_type_or_tag >::type type;
    };


    /** @brief For internal use only */
    template<typename typemap>
    struct topologic_cell_dimension_impl;

    template<>
    struct topologic_cell_dimension_impl<viennagrid::meta::null_type>
    {
        static const int value = -1;
    };

    template<typename element_type, typename element_container_type, typename tail>
    struct topologic_cell_dimension_impl< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, element_container_type>, tail > >
    {
        static const int tail_cell_dimension = topologic_cell_dimension_impl<tail>::value;
        static const int current_element_dimension = topologic_dimension<element_type>::value;

        static const int value = (tail_cell_dimension > current_element_dimension) ? tail_cell_dimension : current_element_dimension;
    };

    // doxygen doku in forwards.hpp
    template<typename something>
    struct topologic_cell_dimension
    {
        static const int value = topologic_cell_dimension_impl<
                typename container_collection_typemap<
                    typename element_collection<something>::type
                >::type
            >::value;
    };





    /** @brief For internal use only */
    template<typename typemap, int topologic_dimension>
    struct elements_of_topologic_dim_impl;

    template<int topologic_dimension>
    struct elements_of_topologic_dim_impl< viennagrid::meta::null_type, topologic_dimension >
    {
        typedef viennagrid::meta::null_type type;
    };

    template<typename element_type, typename element_container_type, typename tail, int topologic_dimension>
    struct elements_of_topologic_dim_impl< viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_type, element_container_type>, tail >, topologic_dimension >
    {
        typedef typename elements_of_topologic_dim_impl<tail, topologic_dimension>::type tail_typelist;

        typedef typename viennagrid::meta::IF<
            viennagrid::result_of::topologic_dimension<element_type>::value == topologic_dimension,
            typename viennagrid::meta::typelist::result_of::push_back<tail_typelist, element_type>::type,
            tail_typelist
        >::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename something, int topologic_dimension>
    struct elements_of_topologic_dim
    {
        typedef typename elements_of_topologic_dim_impl<
            typename container_collection_typemap<
                typename element_collection<something>::type
            >::type,
            topologic_dimension>::type type;
    };



    // doxygen doku in forwards.hpp
    template<typename something>
    struct cells
    {
        typedef typename elements_of_topologic_dim<
            something,
            topologic_cell_dimension<something>::value
        >::type type;
    };


    // doxygen doku in forwards.hpp
    template<typename something>
    struct cell
    {
        typedef typename cells<something>::type all_cell_types;
        typedef typename viennagrid::meta::STATIC_ASSERT< viennagrid::meta::typelist::result_of::size<all_cell_types>::value == 1 >::type static_assert_typedef;

        typedef typename viennagrid::meta::typelist::result_of::at<all_cell_types,0>::type type;
    };

    // doxygen doku in forwards.hpp
    template<typename something>
    struct cell_tag
    {
        typedef typename element_tag< typename cell<something>::type >::type type;
    };
  }



  /** @brief For internal use only */
  template<typename DomainSegmentT, typename FunctorT>
  struct for_each_element_functor
  {
    for_each_element_functor( DomainSegmentT & d, FunctorT f ) : f_(f), domain_(d) {}

    template<typename element_type>
    void operator() ( viennagrid::meta::tag<element_type> )
    {
      typedef typename viennagrid::result_of::element_range<DomainSegmentT, element_type>::type element_range_type;
      typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

      element_range_type range = viennagrid::elements(domain_);
      for (element_range_iterator it = range.begin(); it != range.end(); ++it)
        f_(*it);
    }

    template<typename element_type>
    void operator() ( viennagrid::meta::tag<element_type> ) const
    {
      typedef typename viennagrid::result_of::const_element_range<DomainSegmentT, element_type>::type element_range_type;
      typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

      element_range_type range = viennagrid::elements(domain_);
      for (element_range_iterator it = range.begin(); it != range.end(); ++it)
        f_(*it);
    }

    FunctorT f_;
    DomainSegmentT & domain_;
  };


  /** @brief Function which executes functor an each element with specific topologic dimension
    *
    * @tparam TopologicDimensionV     Topologic dimension of the elements on which the functor is executed
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam FunctorT                Functor type, needs to provide void operator(ElementType &/const &) for each element type with topologic dimension equal to TopologicDimensionV
    * @param  domain_or_segment       Host domain/segment object
    * @param  f                       Functor object
    */
  template<int TopologicDimensionV, typename DomainSegmentT, typename FunctorT>
  void for_each( DomainSegmentT & domain_or_segment, FunctorT f )
  {
    for_each_element_functor<DomainSegmentT, FunctorT> for_each_functor(domain_or_segment, f);
    typedef typename viennagrid::result_of::elements_of_topologic_dim<DomainSegmentT, TopologicDimensionV>::type element_typelist;

    viennagrid::meta::typelist::for_each<element_typelist>( for_each_functor );
  }

  /** @brief Function which executes functor an each element with specific topologic dimension, const version
    *
    * @tparam TopologicDimensionV     Topologic dimension of the elements on which the functor is executed
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam FunctorT                Functor type, needs to provide void operator(ElementType &/const &) for each element type with topologic dimension equal to TopologicDimensionV
    * @param  domain_or_segment       Host domain/segment object
    * @param  f                       Functor object
    */
  template<int TopologicDimensionV, typename DomainSegmentT, typename FunctorT>
  void for_each( DomainSegmentT const & domain_or_segment, FunctorT f )
  {
    for_each_element_functor<const DomainSegmentT, FunctorT> for_each_functor(domain_or_segment, f);
    typedef typename viennagrid::result_of::elements_of_topologic_dim<DomainSegmentT, TopologicDimensionV>::type element_typelist;

    viennagrid::meta::typelist::for_each<element_typelist>( for_each_functor );
  }




  // doxygen doku in forwards.hpp
  template<typename element_type_or_tag, typename WrappedConfigType>
  typename result_of::element_range<domain_t<WrappedConfigType>, element_type_or_tag>::type elements(domain_t<WrappedConfigType> & domain)
  { return elements<element_type_or_tag>( element_collection(domain) ); }

  // doxygen doku in forwards.hpp
  template<typename element_type_or_tag, typename WrappedConfigType>
  typename result_of::const_element_range<domain_t<WrappedConfigType>, element_type_or_tag>::type elements(domain_t<WrappedConfigType> const & domain)
  { return elements<element_type_or_tag>( element_collection(domain) ); }





  /** @brief Function which finds an element based on an ID. The runtime of this function is linear in the number of elements of the requested type in the domain.
    *
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam IDT                     ID type of the object to be found
    * @param  domain_or_segment       Host domain/segment object
    * @param  id                      id of the object to be found
    * @return                         An iterator pointing to the found element. If no element was found it points to viennagrid::elements<ElementType>(domain_or_segment).end()
    */
  template<typename DomainSegmentT, typename IDT>
  typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<DomainSegmentT, typename IDT::value_type::tag>::type >::type
          find(DomainSegmentT & domain_or_segment, IDT id )
  {
      typedef typename IDT::value_type element_type;
      typedef typename element_type::tag element_tag;
      typedef typename viennagrid::result_of::element_range<DomainSegmentT, element_tag>::type RangeType;
      RangeType range = viennagrid::elements<element_tag>(domain_or_segment);
      return std::find_if(
                  range.begin(),
                  range.end(),
                  viennagrid::storage::id_compare<IDT>(id)
          );
  }

  /** @brief Function which finds an element based on an ID, const version. The runtime of this function is linear in the number of elements of the requested type in the domain.
    *
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam IDT                     ID type of the object to be found
    * @param  domain_or_segment       Host domain/segment object
    * @param  id                      id of the object to be found
    * @return                         A const iterator pointing to the found element. If no element was found it points to viennagrid::elements<ElementType>(domain_or_segment).end()
    */
  template<typename DomainSegmentT, typename IDT>
  typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::element_range<DomainSegmentT, typename IDT::value_type::tag>::type >::type
          find(DomainSegmentT const & domain_or_segment, IDT id )
  {
      typedef typename IDT::value_type element_type;
      typedef typename element_type::tag element_tag;
      typedef typename viennagrid::result_of::const_element_range<DomainSegmentT, element_tag>::type RangeType;
      RangeType range = viennagrid::elements<element_tag>(domain_or_segment);
      return std::find_if(
                  range.begin(),
                  range.end(),
                  viennagrid::storage::id_compare<IDT>(id)
          );
  }
  
  /** @brief Function which finds an element based on a handle. The runtime of this function is linear in the number of elements of the requested type in the domain.
    *
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam HandleT                 The handle type of the object to be found
    * @param  domain_or_segment       Host domain/segment object
    * @param  id                      id of the object to be found
    * @return                         An iterator pointing to the found element. If no element was found it points to viennagrid::elements<ElementType>(domain_or_segment).end()
    */
  template<typename DomainSegmentT, typename HandleT>
  typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<DomainSegmentT, typename storage::handle::result_of::value_type<HandleT>::type >::type >::type
          find_by_handle(DomainSegmentT & domain_or_segment, HandleT handle)
  {
      typedef typename storage::handle::result_of::value_type<HandleT>::type element_type;
      typedef typename element_type::tag element_tag;
      typedef typename viennagrid::result_of::element_range<DomainSegmentT, element_tag>::type RangeType;
      typedef typename viennagrid::result_of::iterator<RangeType>::type RangeIterator;

      RangeType range = viennagrid::elements<element_tag>(domain_or_segment);
      for (RangeIterator it = range.begin(); it != range.end(); ++it)
      {
          if ( it.handle() == handle )
              return it;
      }

      return range.end();
  }

  /** @brief Function which finds an element based on a handle, const version. The runtime of this function is linear in the number of elements of the requested type in the domain.
    *
    * @tparam DomainSegmentT          Host domain/segment type
    * @tparam HandleT                 The handle type of the object to be found
    * @param  domain_or_segment       Host domain/segment object
    * @param  id                      id of the object to be found
    * @return                         A const iterator pointing to the found element. If no element was found it points to viennagrid::elements<ElementType>(domain_or_segment).end()
    */
  template<typename DomainSegmentT, typename HandleT>
  typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::const_element_range<DomainSegmentT, typename storage::handle::result_of::value_type<HandleT>::type >::type  >::type
          find_by_handle(DomainSegmentT const & domain_or_segment, HandleT handle)
  {
      typedef typename storage::handle::result_of::value_type<HandleT>::type element_type;
      typedef typename element_type::tag element_tag;
      typedef typename viennagrid::result_of::const_element_range<DomainSegmentT, element_tag>::type RangeType;
      typedef typename viennagrid::result_of::const_iterator<RangeType>::type RangeIterator;

      RangeType range = viennagrid::elements<element_tag>(domain_or_segment);
      for (RangeIterator it = range.begin(); it != range.end(); ++it)
      {
          if ( it.handle() == handle )
              return it;
      }

      return range.end();
  }



  /** @brief Function for setting a vertex within an element. Don't use this unless you know what you are doing!
    *
    * @tparam ElementTag          Element tag of the host element type
    * @tparam WrappedConfigType   Wrapped config of the host element type
    * @tparam VertexHandleType    Vertex handle type          
    * @param  element             The host element object where the vertex is to be set
    * @param  vertex_handle       A vertex handle which is to be used in the host element
    * @param  pos                 The position of the vertex to be set within the host element
    */
  template<typename ElementTag, typename WrappedConfigType, typename VertexHandleType>
  void set_vertex(
          viennagrid::element_t<ElementTag, WrappedConfigType> & element,
          VertexHandleType vertex_handle,
          unsigned int pos)
  {
      element.container( viennagrid::dimension_tag<0>() ).set_handle( vertex_handle, pos );
  }



  /** @brief For internal use only */
  template<bool generate_id, bool call_callback, typename domain_type, typename ElementTag, typename WrappedConfigType>
  std::pair<
              typename viennagrid::storage::result_of::container_of<
                  typename result_of::element_collection<domain_type>::type,
                  viennagrid::element_t<ElementTag, WrappedConfigType>
              >::type::handle_type,
              bool
          >
      push_element( domain_type & domain, viennagrid::element_t<ElementTag, WrappedConfigType> const & element)
  {
      return inserter(domain).template insert<generate_id, call_callback>(element);
  }


  namespace result_of
  {
    // doxygen doku in forwards.hpp
    template<typename point_container_type>
    struct point {};

    template<typename ConfigType>
    struct point< domain_t<ConfigType> >
    {
        typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
    };

    template<typename ConfigType>
    struct point< const domain_t<ConfigType> >
    {
        typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
    };

    template<typename ElementTag, typename WrappedConfigType>
    struct point< element_t<ElementTag, WrappedConfigType> >
    {
        typedef typename viennagrid::result_of::vertex< element_t<ElementTag, WrappedConfigType> >::type::appendix_type type;
    };

    template<typename ElementTag, typename WrappedConfigType>
    struct point< const element_t<ElementTag, WrappedConfigType> >
    {
        typedef typename viennagrid::result_of::vertex< element_t<ElementTag, WrappedConfigType> >::type::appendix_type type;
    };
  }


  /** @brief Function for obtaining the point from a vertex
    *
    * @tparam WrappedConfigT      Wrapped config of the host domain type
    * @param  domain              The host domain object
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A reference to the point
    */
  template<typename WrappedConfigT>
  typename result_of::point< domain_t<WrappedConfigT> >::type & point(domain_t<WrappedConfigT> & /*domain*/, typename result_of::vertex< domain_t<WrappedConfigT> >::type & vertex)
  { return vertex.appendix(); }

  /** @brief Function for obtaining the point from a vertex, const version
    *
    * @tparam WrappedConfigT      Wrapped config of the host domain type
    * @param  domain              The host domain object
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A const reference to the point
    */
  template<typename WrappedConfigT>
  typename result_of::point< domain_t<WrappedConfigT> >::type const & point(domain_t<WrappedConfigT> const & /*domain*/, typename result_of::vertex< domain_t<WrappedConfigT> >::type const & vertex)
  { return vertex.appendix(); }


  /** @brief Function for obtaining the point from a vertex
    *
    * @tparam WrappedConfigT      Wrapped config of the host domain type
    * @param  domain              The host domain object
    * @param  vertex_handle       A handle to the vertex of which the point is obtained
    * @return                     A reference to the point
    */
  template<typename WrappedConfigT>
  typename result_of::point< domain_t<WrappedConfigT> >::type & point(domain_t<WrappedConfigT> & domain, typename result_of::vertex_handle< domain_t<WrappedConfigT> >::type vertex_handle)
  { return dereference_handle(domain, vertex_handle).appendix(); }

  /** @brief Function for obtaining the point from a vertex, const version
    *
    * @tparam WrappedConfigT      Wrapped config of the host domain type
    * @param  domain              The host domain object
    * @param  vertex_handle       A handle to the vertex of which the point is obtained
    * @return                     A const reference to the point
    */
  template<typename WrappedConfigT>
  typename result_of::point< domain_t<WrappedConfigT> >::type const & point(domain_t<WrappedConfigT> const & domain, typename result_of::const_vertex_handle< domain_t<WrappedConfigT> >::type vertex_handle)
  { return dereference_handle(domain, vertex_handle).appendix(); }
  

  /** @brief Function for obtaining the point from a vertex, no domain needed
    *
    * @tparam VertexT             The vertex type
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A reference to the point
    */
  template<typename VertexT>
  typename result_of::point< VertexT >::type & point(VertexT & vertex)
  { return default_point_accessor(vertex)(vertex); }

  /** @brief Function for obtaining the point from a vertex, no domain needed, const version
    *
    * @tparam VertexT             The vertex type
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A const reference to the point
    */
  template<typename VertexT>
  typename result_of::point< VertexT >::type const & point(VertexT const & vertex)
  { return default_point_accessor(vertex)(vertex); }




  /** @brief For internal use only */
  template<typename DomainT, typename SourceElementT, typename DestinationElementT>
  struct copy_element_setters
  {
    copy_element_setters(DomainT & domain_, SourceElementT const & source_element_, DestinationElementT & destination_element_) :
      domain(domain_), source_element(source_element_), destination_element(destination_element_) {}

    template<typename BoundaryElementT>
    void operator() ( viennagrid::meta::tag<BoundaryElementT> )
    {
      typedef typename result_of::const_element_range<SourceElementT, BoundaryElementT>::type     SourceBoundaryElementRangeType;
      typedef typename result_of::iterator<SourceBoundaryElementRangeType>::type                  SourceBoundaryElementRangeIterator;

      typedef typename result_of::element_range<SourceElementT, BoundaryElementT>::type           DestinationBoundaryElementRangeType;
      typedef typename result_of::iterator<DestinationBoundaryElementRangeType>::type             DestinationBoundaryElementRangeIterator;

      SourceBoundaryElementRangeType      source_boundary_elements      = viennagrid::elements(source_element);
      DestinationBoundaryElementRangeType destination_boundary_elements = viennagrid::elements(destination_element);

      SourceBoundaryElementRangeIterator      sit = source_boundary_elements.begin();
      DestinationBoundaryElementRangeIterator dit = destination_boundary_elements.begin();

      for (; sit != source_boundary_elements.end(); ++sit, ++dit)
      {
        dit.handle() = viennagrid::find( domain, sit->id() ).handle();
      }
    }

    DomainT & domain;
    SourceElementT const & source_element;
    DestinationElementT & destination_element;
  };


  /** @brief For internal use only */
  template<typename ElementTypelistT>
  struct fix_handle_helper;

  template<>
  struct fix_handle_helper< meta::null_type >
  {
    template<typename SourceWrappedConfigT, typename DestinationWrappedConfigT>
    static void fix_handles( domain_t<SourceWrappedConfigT> const &, domain_t<DestinationWrappedConfigT> & )
    {}
  };


  template<typename SourceElementT, typename TailT>
  struct fix_handle_helper< meta::typelist_t<SourceElementT, TailT> >
  {
    template<typename SourceWrappedConfigT, typename DestinationWrappedConfigT>
    static void fix_handles( domain_t<SourceWrappedConfigT> const & source_domain, domain_t<DestinationWrappedConfigT> & destination_domain )
    {
      typedef typename viennagrid::result_of::element_tag<SourceElementT>::type ElementTag;

      typedef domain_t<SourceWrappedConfigT>          SourceDomainType;
      typedef domain_t<DestinationWrappedConfigT>     DestinationDomainType;

      typedef typename viennagrid::result_of::const_element_range<SourceDomainType, SourceElementT>::type     SourceElementRangeType;
      typedef typename viennagrid::result_of::iterator<SourceElementRangeType>::type                          SourceElementRangeIterator;

      typedef typename viennagrid::result_of::element_range<DestinationDomainType, SourceElementT>::type      DestinationElementRangeType;
      typedef typename viennagrid::result_of::iterator<DestinationElementRangeType>::type                     DestinationElementRangeIterator;

      
      typedef typename viennagrid::result_of::element<SourceDomainType, ElementTag>::type SourceElementType;
      typedef typename viennagrid::result_of::element<DestinationDomainType, ElementTag>::type DestinationElementType;
      typedef typename viennagrid::result_of::handle<DestinationDomainType, ElementTag>::type DestinationElementHandleType;

      SourceElementRangeType source_elements = viennagrid::elements( source_domain );
      DestinationElementRangeType destination_elements = viennagrid::elements( destination_domain );
      
      DestinationElementRangeIterator dit = destination_elements.begin();
      for (SourceElementRangeIterator sit = source_elements.begin(); sit != source_elements.end(); ++sit, ++dit)
      {
        SourceElementType const & source_element = *sit;
        DestinationElementType & destination_element = *dit;
        
        if (source_element.id() != destination_element.id())
        {
          std::cout << "ERROR in fix_handles: destination element id != source element id" << std::endl;
          continue;
        }
//           DestinationElementHandleType handle = viennagrid::push_element<false, false>(destination_domain, destination_element).first;

        typedef typename viennagrid::result_of::boundary_element_typelist<SourceElementT>::type BoundaryElementTypelist;
        typedef typename viennagrid::result_of::element_typelist<DestinationDomainType>::type DestinationElementTypelist;

        typedef typename viennagrid::meta::typelist::result_of::intersection<
                  BoundaryElementTypelist,
                  DestinationElementTypelist
              >::type ElementTypelist;

        copy_element_setters<DestinationDomainType, SourceElementT, DestinationElementType> setter( destination_domain, source_element, destination_element );
        viennagrid::meta::typelist::for_each<ElementTypelist>(setter);
      }

      fix_handle_helper<TailT>::fix_handles( source_domain, destination_domain );
    }
  };

  /** @brief For internal use only */
  template<typename SourceWrappedConfigT, typename DestinationWrappedConfigT>
  void fix_handles( domain_t<SourceWrappedConfigT> const & source_domain, domain_t<DestinationWrappedConfigT> & destination_domain )
  {
    typedef domain_t<SourceWrappedConfigT>          SourceDomainType;
    typedef domain_t<DestinationWrappedConfigT>     DestinationDomainType;

    typedef typename viennagrid::result_of::element_typelist<SourceDomainType>::type      SourceTypelist;
    typedef typename viennagrid::result_of::element_typelist<DestinationDomainType>::type DestinationTypelist;

        typedef typename viennagrid::meta::typelist::result_of::intersection<
                  SourceTypelist,
                  DestinationTypelist
              >::type ElementTypelist;

    fix_handle_helper<SourceTypelist>::fix_handles( source_domain, destination_domain );
  }

}

#endif

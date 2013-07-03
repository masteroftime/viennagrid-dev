#ifndef VIENNAGRID_STORAGE_ID_GENERATOR_HPP
#define VIENNAGRID_STORAGE_ID_GENERATOR_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/storage/id.hpp"
#include "viennagrid/config/config.hpp"


namespace viennagrid
{
    namespace storage
    {
        namespace result_of
        {
            template<typename typelist, typename id_type>
            struct continuous_id_generator_config_helper;
            
            template<typename head, typename tail, typename id_type>
            struct continuous_id_generator_config_helper< viennagrid::meta::typelist_t<head, tail>, id_type >
            {
                typedef typename viennagrid::meta::typemap::result_of::insert<
                    typename continuous_id_generator_config_helper<tail, id_type>::type,
                    viennagrid::meta::static_pair<
                        head,
                        id_type
                    >
                >::type type;
            };
            
            template<typename id_type>
            struct continuous_id_generator_config_helper<viennagrid::meta::null_type, id_type>
            {
                typedef viennagrid::meta::null_type type;
            };
            
            
            template<typename domain_config, typename typemap>
            struct continuous_id_generator_config_from_domain_config_helper;
            
            template<typename domain_config, typename element_tag, typename element_config, typename tail>
            struct continuous_id_generator_config_from_domain_config_helper< domain_config, viennagrid::meta::typelist_t<viennagrid::meta::static_pair<element_tag, element_config>, tail> >
            {
                typedef typename viennagrid::meta::typemap::result_of::insert<
                    typename continuous_id_generator_config_from_domain_config_helper<domain_config, tail>::type,
                    viennagrid::meta::static_pair<
                        typename viennagrid::result_of::element<domain_config, element_tag>::type,
                        typename config::result_of::query_config<element_config, config::element_id_tag>::type
                    >
                >::type type;
            };
            
            template<typename domain_config>
            struct continuous_id_generator_config_from_domain_config_helper<domain_config, viennagrid::meta::null_type>
            {
                typedef viennagrid::meta::null_type type;
            };
            
            template<typename typelist, typename id_type>
            struct continuous_id_generator_config
            {
                typedef typename continuous_id_generator_config_helper<
                    typename viennagrid::meta::typelist::result_of::no_duplicates<typelist>::type,
                    id_type
                >::type type;
            };
            
            template<typename domain_config>
            struct continuous_id_generator_config_from_domain_config
            {
                typedef typename config::result_of::query_config<domain_config, config::topology_config_tag>::type topology_config;
                typedef typename continuous_id_generator_config_from_domain_config_helper<domain_config, topology_config>::type type;
            };
        }

        
        

        template<typename id_generator_config>
        class continuous_id_generator_layer_t;
        
        template<typename value_type, typename id_tag, typename tail>
        class continuous_id_generator_layer_t<viennagrid::meta::typelist_t< viennagrid::meta::static_pair<value_type, id_tag>, tail> > : public continuous_id_generator_layer_t<tail>
        {
            typedef continuous_id_generator_layer_t<tail> base;
        public:
            
            typedef viennagrid::meta::typelist_t< viennagrid::meta::static_pair<value_type, id_tag>, tail> config_type;
            
            //typedef smart_id<value_type, base_id_type> id_type;
            typedef typename result_of::id<value_type, id_tag>::type id_type;
            
            continuous_id_generator_layer_t() : base(), last_id(0) {}
            
            using base::operator();
            id_type operator()( viennagrid::meta::tag<value_type> )
            {
                return last_id++;
            }
            
            using base::max_id;
            id_type max_id( viennagrid::meta::tag<value_type> ) const
            {
                return last_id;
            }
            
        private:
            id_type last_id;
        };
        
        template<>
        class continuous_id_generator_layer_t<viennagrid::meta::null_type>
        {
        public:
            void operator()();
            void max_id() const;
        };
        
        
        template<typename typemap>
        class continuous_id_generator : public continuous_id_generator_layer_t< typemap >
        {};
        
        namespace result_of
        {
            template<typename typemap>
            struct continuous_id_generator
            {
                typedef viennagrid::storage::continuous_id_generator< typemap > type;
            };
        }
        
    }
}

#endif

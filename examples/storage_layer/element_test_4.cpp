#include <iostream>
#include <typeinfo>

#include <map>
#include <boost/concept_check.hpp>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/quadrilateral.hpp"
#include "viennagrid/topology/polygon.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"



#include "viennagrid/domain/topologic_domain.hpp"


int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
    //
    // A full config for tetrahedrons
    //
    
    typedef viennameta::make_typemap<    
    
        viennagrid::vertex_tag,
        viennameta::make_typemap<
            viennagrid::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::std_deque_tag,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
            >::type
        >::type,
        
        viennagrid::line_tag,
        viennameta::make_typemap<
            viennagrid::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag >,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag
            >::type
        >::type,
        
        viennagrid::polygon_tag,
        viennameta::make_typemap<
            viennagrid::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag >,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag,
                viennagrid::line_tag, viennagrid::full_handling_tag
            >::type
        >::type,
        
        viennagrid::triangle_tag,
        viennameta::make_typemap<
            viennagrid::element_id_tag,
            viennagrid::storage::smart_id_tag<int>,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag >,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag,
                viennagrid::line_tag, viennagrid::full_handling_tag
            >::type
        >::type

    >::type config;
    
    
    
    //
    // Generates the same config as show above in comment
    //
    
    //typedef viennagrid::result_of::default_topologic_config<viennagrid::tetrahedron_tag, hook_tag>::type config;
       
    
        
    //
    // typedefs for the element types
    //
    typedef viennagrid::result_of::element<config, viennagrid::vertex_tag>::type vertex_type;        
    typedef viennagrid::result_of::element<config, viennagrid::line_tag>::type line_type;
//     typedef viennagrid::result_of::element<config, viennagrid::triangle_tag>::type triangle_type;
    
    
    std::cout << typeid( viennagrid::result_of::element<config, viennagrid::polygon_tag>::type ).name() << std::endl;
    std::cout << typeid( viennagrid::result_of::element<config, viennagrid::triangle_tag>::type ).name() << std::endl;
    
    
    
    
    //typedef viennagrid::storage::result_of::continuous_id_generator< id_generator_config >::type id_generator_type;
    
    
    
    
//     cout << "The Vertex Type:" << endl;
//     vertex_type::print_class();
//     cout << endl;
//     
//     cout << "The Line Type:" << endl;
//     line_type::print_class();
//     cout << endl;
//     
// //     cout << "The Triangle Type:" << endl;
// //     triangle_type::print_class();
// //     cout << endl;
//     
//     
//     cout << endl;
//     cout << endl;
//         
// 
//     //
//     // setting up the domain
//     //
//     
//     typedef viennagrid::result_of::element_container_typemap<config>::type element_container_typemap;
//     
//     //cout << typeid(element_container_typelist).name() << endl;
//     
//     typedef viennagrid::storage::result_of::collection< element_container_typemap >::type domain_container_collection_type;
//     domain_container_collection_type domain_container_collection;
//     
// 
// //     typedef viennagrid::storage::result_of::continuous_id_generator_config<
// //         viennagrid::storage::container_collection::result_of::value_typelist<domain_container_collection_type>::type,
// //         viennagrid::storage::smart_id_tag<int>
// //     >::type id_generator_config;
//     
//     typedef viennagrid::result_of::continuous_id_generator_config< config >::type id_generator_config;
//     
//     typedef viennagrid::storage::result_of::continuous_id_generator< id_generator_config >::type id_generator_type;
//     id_generator_type id_generator;
//     
//     typedef viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type&>::type inserter_type;
//     inserter_type inserter(domain_container_collection, id_generator);
//     
// 
//     
//     // Adding a tetrahedron
//     //
// 
//     vertex_type v0;
//     vertex_type v1;
//     vertex_type v2;
//     vertex_type v3;
//     
//     inserter(v0);
//     inserter(v1);
//     inserter(v2);
//     inserter(v3);
    
    
//     tetrahedron_type tet(domain_container_collection);
//     
//     tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::collection::get<vertex_type>(domain_container_collection).hook_at(0), 0);
//     tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::collection::get<vertex_type>(domain_container_collection).hook_at(1), 1);
//     tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::collection::get<vertex_type>(domain_container_collection).hook_at(2), 2);
//     tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::collection::get<vertex_type>(domain_container_collection).hook_at(3), 3);
//     
//     inserter(tet);
//     
    
    
    
    

    
    
    
    
    //
    // display the domain content
    //
    
//     cout << "All vertices of the domain" << endl;
//     std::copy( viennagrid::storage::collection::get<vertex_type>(domain_container_collection).begin(), viennagrid::storage::collection::get<vertex_type>(domain_container_collection).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
//     cout << endl;
//     
//     cout << "All triangles of the domain" << endl;
//     std::copy( viennagrid::storage::collection::get<line_type>(domain_container_collection).begin(), viennagrid::storage::collection::get<line_type>(domain_container_collection).end(), std::ostream_iterator<line_type>(cout, "\n") );
//     cout << endl;
//     
//     cout << "All lines of the domain" << endl;
//     std::copy( viennagrid::storage::collection::get<triangle_type>(domain_container_collection).begin(), viennagrid::storage::collection::get<triangle_type>(domain_container_collection).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
//     cout << endl;


    
    
    

    return 0;
}


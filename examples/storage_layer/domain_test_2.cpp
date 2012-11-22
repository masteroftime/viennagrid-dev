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
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/domain/geometric_domain.hpp"


#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;




int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    //typedef viennagrid::storage::id_hook_tag hook_tag;
    

    typedef viennameta::make_typelist< viennameta::static_pair<viennagrid::tetrahedron_tag, double> >::type metainfo_typelist;
    

    typedef ublas::vector<double> vector_type;
    typedef viennagrid::result_of::geometric_domain_config< viennagrid::tetrahedron_tag, vector_type, hook_tag, metainfo_typelist >::type domain_config;
    
    
    //
    // typedefing and setting up the geometric domain
    //
    
    typedef viennagrid::result_of::geometric_domain< domain_config >::type domain_type;
    domain_type domain;
        
    //
    // typedefs for the element types
    //
    
    typedef viennagrid::result_of::ncell<domain_type, 0>::type vertex_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 0>::type vertex_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 1>::type line_type;
    typedef viennagrid::result_of::ncell<domain_type, 2>::type triangle_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 3>::type tetrahedron_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 3>::type tetrahedron_hook_type;
    

    //
    // Adding a tetrahedron
    //
    
    // creates four vertices within the domain, vh is short vor vertex hook
    // create_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_hook_type vh0 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh1 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh2 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh3 = viennagrid::create_element<vertex_type>( domain );
    
    
    // create geometric information for the vertices
    vector_type p0(3);
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    vector_type p1(3);
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    vector_type p2(3);
    p2[0] = 0.0; p2[1] = 1.0; p2[2] = 0.0;
    vector_type p3(3);
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;    
    
    
    // set the geometric information for the vertices
    // is equivalent to viennagrid::look_up<vector_type>(domain, vhX)
    viennagrid::point(domain, vh0) = p0;
    viennagrid::point(domain, vh1) = p1;
    viennagrid::point(domain, vh2) = p2;
    viennagrid::point(domain, vh3) = p3;
    
    
    // creates a hook buffer for the vertex hooks of the tetdrahedron
    std::vector<vertex_hook_type> hooks(4);
    hooks[0] = vh0; hooks[1] = vh1; hooks[2] = vh2; hooks[3] = vh3;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    tetrahedron_hook_type tet = viennagrid::create_element<tetrahedron_type>( domain, hooks );
    
    // set a double value to a tetdrahedron
    viennagrid::look_up<double>(domain, tet) = 1.0;
    
    //
    // display the domain content
    //
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::ncells<0>(domain).begin(), viennagrid::ncells<0>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::ncells<1>(domain).begin(), viennagrid::ncells<1>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::ncells<2>(domain).begin(), viennagrid::ncells<2>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::ncells<3>(domain).begin(), viennagrid::ncells<3>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    //
    // doing some boundary cell iteration
    //
    
    const tetrahedron_type & test_tet = *viennagrid::ncells<3>(domain).begin();
    const triangle_type & test_tri = *viennagrid::ncells<2>(domain).begin();
    
    
    
    typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 2>::type tetrahedron_triangle_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range>::type tetrahedron_triangle_iterator;
    
    cout << "All triangles of the first tetdrahedron in the domain" << endl;
    tetrahedron_triangle_range tri_range = viennagrid::ncells<2>(test_tet);
    for (tetrahedron_triangle_iterator it = tri_range.begin(); it != tri_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( tri_range.begin(), tri_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    typedef viennagrid::result_of::const_ncell_range<triangle_type, 1>::type triangle_line_range;
    typedef viennagrid::result_of::const_iterator<triangle_line_range>::type triangle_line_iterator;

    cout << "All lines of the first triangle in the domain" << endl;
    triangle_line_range lin_range = viennagrid::ncells<1>(test_tri);
    for (triangle_line_iterator it = lin_range.begin(); it != lin_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( lin_range.begin(), lin_range.end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    //
    // geometric iteration
    //
    
    // iterating over all vertices and piping out the point information
    typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 0>::type tetrahedron_vertex_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_vertex_range>::type tetrahedron_vertex_iterator;
    
    cout << "All vertices of the first tetdrahedron in the domain" << endl;
    tetrahedron_vertex_range vtx_range = viennagrid::ncells<0>(test_tet);
    for (tetrahedron_vertex_iterator it = vtx_range.begin(); it != vtx_range.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::look_up<vector_type>( domain, *it ) << endl;
    cout << endl;
    
    
    // iterating over all tetrahedrons and piping out the double meta-information
    typedef viennagrid::result_of::const_ncell_range<domain_type, 3>::type tetrahedron_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_range>::type tetrahedron_iterator;
    
    cout << "All tetdrahedrons in the domain" << endl;
    tetrahedron_range tet_range = viennagrid::ncells<3>(domain);
    for (tetrahedron_iterator it = tet_range.begin(); it != tet_range.end(); ++it)
    {
        cout << *it << endl;
        cout << "   geometric information: " << viennagrid::look_up<double>( domain, *it ) << endl;
    }
    cout << endl;
    
    return 0;
}
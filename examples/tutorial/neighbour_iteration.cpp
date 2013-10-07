#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"



#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/mesh_config.hpp"
#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"
#include "viennagrid/mesh/element_creation.hpp"


int main()
{

    //
    // typedefing and setting up the mesh
    //

    typedef viennagrid::triangular_2d_mesh MeshType;
    MeshType mesh;

    //
    // typedefs for the element types
    //

    typedef viennagrid::result_of::vertex_handle<MeshType>::type        VertexHandleType;

    typedef viennagrid::result_of::triangle<MeshType>::type             TriangleType;
    typedef viennagrid::result_of::triangle_handle<MeshType>::type      TriangleHandleType;


    //
    // Adding a tetrahedron
    //

    // Creating nine vertices within the mesh, vh is short vor vertex handle
    // In this example we don't need geometric information -> no points
    //
    //  0 --- 1 --- 2
    //  |   / | \   |
    //  |  /  |  \  |
    //  | /   |   \ |
    //  3 --- 4 --- 5
    //  | \   |   / |
    //  |  \  |  /  |
    //  |   | | /   |
    //  6 --- 7 --- 8
    //

    VertexHandleType vh0 = viennagrid::make_vertex( mesh );
    VertexHandleType vh1 = viennagrid::make_vertex( mesh );
    VertexHandleType vh2 = viennagrid::make_vertex( mesh );
    VertexHandleType vh3 = viennagrid::make_vertex( mesh );
    VertexHandleType vh4 = viennagrid::make_vertex( mesh );
    VertexHandleType vh5 = viennagrid::make_vertex( mesh );
    VertexHandleType vh6 = viennagrid::make_vertex( mesh );
    VertexHandleType vh7 = viennagrid::make_vertex( mesh );
    VertexHandleType vh8 = viennagrid::make_vertex( mesh );

    // Creating the triangles within the mesh, all boundary cell generation is done here implicit
    //
    //  X --- X --- X
    //  | 0 / | \ 3 |
    //  |  /  |  \  |
    //  | / 1 | 2 \ |
    //  X --- X --- X
    //  | \ 5 | 6 / |
    //  |  \  |  /  |
    //  | 4 | | / 7 |
    //  X --- X --- X
    //

    TriangleHandleType th0 = viennagrid::make_triangle( mesh, vh0, vh1, vh3 );
    TriangleHandleType th1 = viennagrid::make_triangle( mesh, vh1, vh4, vh3 );
    viennagrid::make_triangle( mesh, vh1, vh5, vh4 );
    viennagrid::make_triangle( mesh, vh1, vh2, vh5 );
    viennagrid::make_triangle( mesh, vh3, vh7, vh6 );
    viennagrid::make_triangle( mesh, vh3, vh4, vh7 );
    viennagrid::make_triangle( mesh, vh4, vh5, vh7 );
    viennagrid::make_triangle( mesh, vh5, vh8, vh7 );






    {

      typedef viennagrid::result_of::neighbour_range<MeshType, viennagrid::triangle_tag, viennagrid::vertex_tag>::type neighbour_range_type;

      // we now want to iterate over all neighbour triangles of triangle 1 which share a common vertex. These should be the triangles 0, 2, 3, 4, 5 and 6
      neighbour_range_type neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(mesh, th1);
      cout << "All triangles neighbouring th1 using vertices (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
      std::copy( neighbour_range.begin(), neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
      cout << endl;

      // we now want to iterate over all neighbour triangles of triangle 0 which share a common vertex. These should be the triangles 1, 2, 3, 4 and 5
      neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(mesh, th0);
      cout << "All triangles neighbouring th0 using vertices (should be triangle 1, 2, 3, 4 and 5)" << endl;
      std::copy( neighbour_range.begin(), neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
      cout << endl;
    }


    {
        // now we do the same but with a constant mesh

        const MeshType & cmesh = mesh;
        typedef viennagrid::result_of::const_neighbour_range<MeshType, viennagrid::triangle_tag, viennagrid::vertex_tag>::type const_neighbour_range_type;

        const_neighbour_range_type const_neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th1);
        cout << "All triangles neighbouring th1 using vertices and const ranges (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
        std::copy( const_neighbour_range.begin(), const_neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
        cout << endl;

        const_neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th0);
        cout << "All triangles neighbouring th0 using vertices and const ranges  (should be triangle 1, 2, 3, 4 and 5)" << endl;
        std::copy( const_neighbour_range.begin(), const_neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
        cout << endl;
    }


    {
        typedef viennagrid::result_of::neighbour_range<MeshType, viennagrid::triangle_tag, viennagrid::line_tag>::type neighbour_range_type;

        // we now want to iterate over all neighbour triangles of triangle 1 which share a common edge. These should be the triangles 0, 2 and 5
        neighbour_range_type neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::line_tag>(mesh, th1);
        cout << "All triangles neighbouring th1 using lines (should be triangle 0, 2 and 5)" << endl;
        std::copy( neighbour_range.begin(), neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
        cout << endl;

        // we now want to iterate over all neighbour triangles of triangle 0 which share a common edge. This should only be the triangle 1
        neighbour_range = viennagrid::neighbour_elements<viennagrid::triangle_tag, viennagrid::line_tag>(mesh, th0);
        cout << "All triangles neighbouring th0 using lines (should only be triangle 1)" << endl;
        std::copy( neighbour_range.begin(), neighbour_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
        cout << endl;
    }




    return 0;
}



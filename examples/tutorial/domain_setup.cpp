/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif


#include "viennagrid/forwards.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/point.hpp"


//
//    Let us construct the following input domain:
//
//    5---------4---------3
//    | \       | \       |
//    |   \     |   \     |    y
//    |     \   |     \   |     ^
//    |       \ |       \ |     |
//    0---------1---------2     *--> x
//
//    Segment 1 | Segment 2
//
int main()
{
  //
  // Define the necessary types:
  //
  
  typedef viennagrid::triangular_2d_domain                  DomainType;
  typedef viennagrid::triangular_2d_segmentation            SegmentationType;
  typedef viennagrid::triangular_2d_segment                 SegmentType;
  
  typedef viennagrid::result_of::point<DomainType>::type            PointType;
  typedef viennagrid::result_of::vertex<DomainType>::type           VertexType;
  typedef viennagrid::result_of::vertex_handle<DomainType>::type    VertexHandleType;
  
  typedef viennagrid::result_of::cell<DomainType>::type             CellType;
  typedef viennagrid::result_of::cell_range<SegmentType>::type      CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a domain with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the domain and the segmentation and create 2 segments:
  //
  DomainType domain;
  SegmentationType segmentation(domain);

  SegmentType seg0 = segmentation.make_segment();
  SegmentType seg1 = segmentation.make_segment();
  
  //
  // Step 2: Add vertices to the domain. 
  //         Note that vertices with IDs are enumerated in the order they are pushed to the domain.
  //
  VertexHandleType vh0 = viennagrid::make_vertex(domain, PointType(0,0)); // id = 0
  VertexHandleType vh1 = viennagrid::make_vertex(domain, PointType(1,0)); // id = 1
  VertexHandleType vh2 = viennagrid::make_vertex(domain, PointType(2,0));
  VertexHandleType vh3 = viennagrid::make_vertex(domain, PointType(2,1));
  VertexHandleType vh4 = viennagrid::make_vertex(domain, PointType(1,1));
  VertexHandleType vh5 = viennagrid::make_vertex(domain, PointType(0,1)); // id = 5

  //
  // Step 3: Fill the two segments with cells. 
  //         To do so, each cell must be linked with the defining vertex handles from the domain
  //
  
  // First triangle, use vertex handles
  viennagrid::storage::static_array<VertexHandleType, 3> vertices;
  vertices[0] = vh0;
  vertices[1] = vh1;
  vertices[2] = vh5;
  // Note that vertices are rearranged internally if they are not supplied in mathematically positive order.
  
  viennagrid::make_element<CellType>(seg0, vertices.begin(), vertices.end()); // creates an element with vertex handles
  
  
  // Second triangle:
  viennagrid::make_triangle(seg0, vh1, vh4, vh5);  //use the shortcut function

  // Third triangle:
  viennagrid::make_triangle(seg1, vh1, vh2, vh4); // Note that we create in 'seg1' now.

  // Fourth triangle:
  viennagrid::make_triangle(seg1, vh2, vh3, vh4 );

  //
  // That's it. The domain consisting of two segments is now set up.
  // If no segments are required, one can also directly write viennagrid::make_triangle(domain, ...);
  //
  
  //
  // Step 4: Output the cells for each segment:
  //
  
  std::cout << "Cells in segment 0:" << std::endl;
  CellRange cells_seg0 = viennagrid::elements(seg0);
  for (CellIterator cit0 = cells_seg0.begin();
                    cit0 != cells_seg0.end();
                  ++cit0)
  {
    std::cout << *cit0 << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "Cells in segment 1:" << std::endl;
  CellRange cells_seg1 = viennagrid::elements(seg1);
  for (CellIterator cit1 = cells_seg1.begin();
                    cit1 != cells_seg1.end();
                  ++cit1)
  {
    std::cout << *cit1 << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  
  return EXIT_SUCCESS;
}

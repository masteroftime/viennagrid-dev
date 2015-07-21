/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagridpp/core.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/algorithm/refine.hpp"


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_hierarchy MeshHierarchyType;
  typedef viennagrid::result_of::mesh<MeshHierarchyType>::type MeshType;


  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "--       ViennaGrid tutorial: Read an STL File              -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshType mesh;
  viennagrid_error error;

  viennagrid_mesh_io mesh_io;
  viennagrid_mesh_io_create(&mesh_io);
  viennagrid_mesh_io_mesh_set(mesh_io, mesh.internal());

  error = viennagrid_mesh_io_read( mesh_io, "../data/porsche.stl" );

  viennagrid_mesh_io_release(mesh_io);
  
  if(error == VIENNAGRID_SUCCESS)
  {
    viennagrid::io::vtk_writer<MeshType> writer;
    writer(mesh, "stl_c_reader");

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
  }
  else
  {
    switch(error)
    {
      case VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE:
        std::cout << "Error: Cannot open file!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_FILE_EMPTY:
        std::cout << "Error: File is empty!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED:
        std::cout << "Error: End of file encountered!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_EOF_WHILE_READING_VERTICES:
        std::cout << "Error: End of file encountered while reading vertices!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_VERTEX_DIMENSION_MISMATCH:
        std::cout << "Error: Vertex dimension mismatch!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_UNKNOWN_FILETYPE:
        std::cout << "Error: Unknown filetype!" << std::endl;
        break;
      case VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT:
        std::cout << "Error: Invalid vertex count!" << std::endl;
        break;
      default:
        std::cout << "Error: " << error << std::endl;
    }
  }

  return EXIT_SUCCESS;
}

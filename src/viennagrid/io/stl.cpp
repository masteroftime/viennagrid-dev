#include "viennagrid/viennagrid.h"

#include <iostream>

viennagrid_error viennagrid_mesh_io_read_stl(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  std::cout << "Reading STL file " << filename << std::endl;

  return VIENNAGRID_SUCCESS;
}


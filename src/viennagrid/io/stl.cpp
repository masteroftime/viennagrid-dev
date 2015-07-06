#include "viennagrid/viennagrid.h"

#include <iostream>
#include <fstream>
#include <algorithm>

viennagrid_error viennagrid_mesh_io_read_stl_binary(viennagrid_mesh_io mesh_io,
                                                        const char * filename);

viennagrid_error viennagrid_mesh_io_read_stl(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  //TODO: Implement ASCII Reader
  
  return viennagrid_mesh_io_read_stl_binary(mesh_io, filename);
}

viennagrid_error viennagrid_mesh_io_read_stl_binary(viennagrid_mesh_io mesh_io,
                                                        const char * filename)
{
  std::ifstream reader(filename, std::ios::in | std::ios::binary);
  
  if (!reader)
    return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

  if (!reader.good())
    return VIENNAGRID_ERROR_IO_FILE_EMPTY;

  viennagrid_mesh mesh;
  viennagrid_mesh_io_mesh_get(mesh_io, &mesh);
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);
  
  viennagrid_mesh_geometric_dimension_set(mesh, 3);

  uint32_t facet_count;
  viennagrid_int vertex_count = 0;
  float vertex_buf[3];  // buffer for reading vertex data
  viennagrid_numeric vertex_data[3];  // vertex data converted to viennagrid_numeric type
  viennagrid_int vertex_ids[3];  //stores the vertex ids making up a triangle
  
  // first 80 bytes are header
  reader.ignore(80);
  
  reader.read((char*)&facet_count, 4);
  
  std::cout << "Reading " << facet_count << " triangles" << std::endl;
  
  for(uint32_t i = 0; i < facet_count; ++i)
  {
    //ignore normal vector
    reader.ignore(sizeof(float)*3);
    
    for(uint32_t j = 0; j < 3; ++j)
    {
      //read vertex data (as float)
      if( !reader.read((char*)vertex_buf, sizeof(float)*3) )
      {
        return VIENNAGRID_ERROR_IO_EOF_WHILE_READING_VERTICES;
      }
      
      //convert read data to viennagrid_numeric
      std::copy(vertex_buf, vertex_buf+3, vertex_data);
          
      viennagrid_numeric * vertex_coords;
      viennagrid_mesh_hierarchy_vertex_coords_pointer(mesh_hierarchy, &vertex_coords);
      
      vertex_ids[j] = -1;
      
      // every vertex is saved multiple times, once for every facet it appears in.
      // check if the read vertex has already appeared before, and if yes obtain its id.
      for(viennagrid_int k = vertex_count-1; k >= 0; --k)
      {
        if(std::equal(vertex_data, vertex_data+3, vertex_coords+(k*3)))
        {
          vertex_ids[j] = k;
          break;
        }
      }
      
      // if it didn't appear, create a new vertex.
      if(vertex_ids[j] < 0)
      {        
        viennagrid_mesh_vertex_create(mesh,vertex_data, &vertex_ids[j]);
        ++vertex_count;
      }
    }

    viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, vertex_ids, NULL);
    
    //ignore attribute byte count
    reader.ignore(2);
  }
  
  return VIENNAGRID_SUCCESS;
}

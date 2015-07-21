#include "viennagrid/viennagrid.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include <sstream>
#include <cstring>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>

typedef boost::array<viennagrid_numeric, 3>  VertexType;
typedef std::map<VertexType, viennagrid_int> IdMapType;


viennagrid_error viennagrid_mesh_io_read_stl_binary(viennagrid_mesh_io mesh_io,
                                                        const char * filename);

viennagrid_error viennagrid_mesh_io_read_stl_ascii(viennagrid_mesh_io mesh_io,
                                                        const char * filename);


viennagrid_error viennagrid_mesh_io_read_stl(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  std::ifstream file(filename);
  
  //skip whitespace at the beginning of the file
  char c = file.get();
  while(c == ' ' || c == '\t')
    c = file.get();
  
  file.unget();
  
  // check if the file starts with "solid". In this case it is probably an ascii file
  char header[6];
  file.read(header, 5);
  header[5] = '\0';
  
  file.close();
  
  if (strcmp(header, "solid") == 0)
    return viennagrid_mesh_io_read_stl_ascii(mesh_io, filename);
  else
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
  
  viennagrid_mesh_geometric_dimension_set(mesh, 3);

  uint32_t facet_count; // number of facets in the file
  float vertex_buf[3];  // buffer for reading vertex data
  viennagrid_int vertex_ids[3];  //stores the vertex ids making up a triangle
  IdMapType id_map; //used to lookup if a vertex already exists
  
  // first 80 bytes are header
  reader.ignore(80);
  
  reader.read((char*)&facet_count, 4);
  
  for(uint32_t i = 0; i < facet_count; ++i)
  {
    // ignore normal vector
    reader.ignore(sizeof(float)*3);
    
    // now read 3 vertices
    for(uint32_t j = 0; j < 3; ++j)
    {
      //read vertex data (as float)
      if( !reader.read((char*)vertex_buf, sizeof(float)*3) )
      {
        return VIENNAGRID_ERROR_IO_EOF_WHILE_READING_VERTICES;
      }
      
      // store the read data in a boost array so that it can be used as a key for id_map
      VertexType v;
      std::copy(vertex_buf, vertex_buf+3, v.begin());
      
      // check if the given vertex has already been read before
      IdMapType::const_iterator it = id_map.find(v);
      
      if(it == id_map.end())
      {
        // if this is a new vertex, add it to the mesh and the map
        viennagrid_mesh_vertex_create(mesh, &v[0], &vertex_ids[j]);
        id_map[v] = vertex_ids[j];
      }
      else
      {
        // if the vertex alreay exists retrieve its id from the map
        vertex_ids[j] = it->second;
      }
    }

    viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, vertex_ids, NULL);
    
    //ignore attribute byte count
    reader.ignore(2);
  }
  
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_read_stl_ascii(viennagrid_mesh_io mesh_io,
                                                        const char * filename)
{
  std::ifstream reader(filename);
  
  if (!reader)
    return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

  if (!reader.good())
    return VIENNAGRID_ERROR_IO_FILE_EMPTY;
  
  viennagrid_mesh mesh;
  viennagrid_mesh_io_mesh_get(mesh_io, &mesh);
  
  viennagrid_mesh_geometric_dimension_set(mesh, 3);
  
  std::string tmp;
  std::string command;
  
  // store the vertex ids that form a triangle
  viennagrid_int vertex_ids[3];
  // used to keep track of how many vetices have been read for the current triangle
  viennagrid_int vertex_count = 0;
  // used to lookup if a vertex already exists
  IdMapType id_map;
  
  //skip the first line "solid ..."
  getline(reader, tmp);
  
  while(reader.good())
  {
    getline(reader, tmp);
    std::stringstream line(tmp);
    
    line >> command;
    
    if(command == "vertex")
    {
      if(vertex_count > 2)
        return VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT;
      
      VertexType v;
      line >> v[0] >> v[1] >> v[2];
      
      //check if all 3 vertex coordinates have been successfully read
      if(!line)
        return VIENNAGRID_ERROR_IO_VERTEX_DIMENSION_MISMATCH;
      
      // check if the given vertex has already been read before
      IdMapType::const_iterator it = id_map.find(v);
      
      if(it == id_map.end())
      {
        // if this is a new vertex, add it to the mesh and the map
        viennagrid_mesh_vertex_create(mesh, &v[0], &vertex_ids[vertex_count]);
        id_map[v] = vertex_ids[vertex_count];
      }
      else
      {
        // if the vertex alreay exists retrieve its id from the map
        vertex_ids[vertex_count] = it->second;
      }
      
      ++vertex_count;
    }
    else if(command == "endloop")
    {
        if(vertex_count != 3)
          return VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT;
        
        viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, vertex_ids, NULL);
        
        vertex_count = 0;
    }
    else if(command == "endsolid")
    {
      return VIENNAGRID_SUCCESS;
    }
  }
  
  return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;
}
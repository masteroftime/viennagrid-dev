#include "viennagrid/viennagrid.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/unordered_map.hpp>

#include "viennagridpp/point.hpp"
#include "viennagridpp/algorithm/cross_prod.hpp"

namespace {
  typedef boost::array<viennagrid_numeric, 3>  VertexType;
  typedef boost::unordered_map<VertexType, viennagrid_int> IdMapType;

  viennagrid_int create_unique_vertex(viennagrid_mesh mesh, IdMapType &id_map, const VertexType &v)
  {
    // check if the given vertex has already been read before
    IdMapType::const_iterator it = id_map.find(v);
    
    if(it == id_map.end())
    {
      // if this is a new vertex, add it to the mesh and the map
      viennagrid_int id;
      viennagrid_mesh_vertex_create(mesh, &v[0], &id);
      id_map[v] = id;
      return id;
    }
    else
    {
      // if the vertex alreay exists retrieve its id from the map
      return it->second;
    }
  }

  viennagrid_error viennagrid_mesh_io_read_stl_binary(viennagrid_mesh mesh,
                                                          const char * filename)
  {
    std::ifstream reader(filename, std::ios::in | std::ios::binary);
    
    if (!reader)
      return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

    if (!reader.good())
      return VIENNAGRID_ERROR_IO_FILE_EMPTY;

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
        
        vertex_ids[j] = create_unique_vertex(mesh, id_map, v);
      }

      viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, vertex_ids, NULL);
      
      //ignore attribute byte count
      reader.ignore(2);
    }
    
    return VIENNAGRID_SUCCESS;
  }

  viennagrid_error viennagrid_mesh_io_read_stl_ascii(viennagrid_mesh mesh,
                                                          const char * filename)
  {
    std::ifstream reader(filename);
    
    if (!reader)
      return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;

    if (!reader.good())
      return VIENNAGRID_ERROR_IO_FILE_EMPTY;
    
    std::string tmp;
    std::string command;
    
    // store the vertex ids that form a facet
    std::vector<viennagrid_int> vertex_ids;
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
        VertexType v;
        line >> v[0] >> v[1] >> v[2];
        
        //check if all 3 vertex coordinates have been successfully read
        if(!line)
          return VIENNAGRID_ERROR_IO_VERTEX_DIMENSION_MISMATCH;
        
        vertex_ids.push_back(create_unique_vertex(mesh, id_map, v));
      }
      else if(command == "endloop")
      {
          if(vertex_ids.size() != 3)
            return VIENNAGRID_ERROR_IO_INVALID_VERTEX_COUNT;
          
          viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, &vertex_ids[0], NULL);
          
          vertex_ids.clear();
      }
      else if(command == "endsolid")
      {
        return VIENNAGRID_SUCCESS;
      }
    }
    
    return VIENNAGRID_ERROR_IO_EOF_ENCOUNTERED;
  }

}

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
  
  viennagrid_mesh mesh;
  viennagrid_mesh_io_mesh_get(mesh_io, &mesh);
  
  viennagrid_mesh_geometric_dimension_set(mesh, 3);
  
  if (strcmp(header, "solid") == 0)
    return viennagrid_mesh_io_read_stl_ascii(mesh, filename);
  else
    return viennagrid_mesh_io_read_stl_binary(mesh, filename);
}

namespace {
  class STLBinaryWriter {
    std::ofstream file_;
    uint32_t facet_count_;
    
  public:
    STLBinaryWriter(const char* filename) 
      : file_(filename, std::ios::out | std::ios::binary), facet_count_(0) {}
      
    void file_start()
    {
      for(viennagrid_int i = 0; i < 84; ++i)
        file_.put(' ');
      
      /* the 4 last bytes should be the facet count but there
      * is not yet a reliable way to get the element count of a mesh.
      * So we leave this field blank for now, count the elements while
      * writing them and then write the field afterwards.
      */
      //file_.write((char*)&facet_count, 4);
    }
    
    void file_end()
    {
      // retrospectively write facet count header field, see comment in file_start()
      file_.seekp(80);
      file_.write((char*)&facet_count_, 4);
    }
    
    void facet_start(const std::vector<viennagrid_numeric> &normal) {
      // convert normal vector coordinates to 32bit floats and write them to file
      float normal_data[3];
      std::copy(normal.begin(), normal.begin()+3, normal_data);
      file_.write((char*)normal_data, sizeof(float) * 3);
      ++facet_count_;
    }
    
    void facet_end()
    {
      // we dont use the attribut byte count field so just write zeros
      file_.write("\0\0", 2);
    }
    
    void vertex(const std::vector<viennagrid_numeric> &vertex_coords)
    {
      // convert vertex coordinates to 32bit floats and write them to file
      float vertex_data[3];
      std::copy(vertex_coords.begin(), vertex_coords.begin()+3, vertex_data);
      file_.write((char*)vertex_data, sizeof(float)*3);
    }
    
    operator void*()
    {
      // to check for io errors we simply redirect to operator void* of our ofstream
      return file_;
    }
  };


  class STLAsciiWriter {
    std::ofstream file_;
    
  public:
    STLAsciiWriter(const char* filename)
      : file_(filename)
    {
      file_ << std::scientific;
    }
    
    void file_start()
    {
      file_ << "solid" << std::endl;
    }
    
    void file_end()
    {
      file_ << "endsolid" << std::endl;
    }
    
    void facet_start(const std::vector<viennagrid_numeric> &normal)
    {
      file_ << "facet normal "
        << normal[0] << " "
        << normal[1] << " "
        << normal[2] << std::endl
        << "outer loop" << std::endl;
    }
    
    void facet_end()
    {
      file_ << "endloop" << std::endl << "endfacet" << std::endl;
    }
    
    void vertex(const std::vector<viennagrid_numeric> &vertex_coords)
    {
      file_ << "vertex " 
        << vertex_coords[0] << " "
        << vertex_coords[1] << " "
        << vertex_coords[2] << std::endl;
    }
    
    operator void*()
    {
      // to check for io errors we simply redirect to operator void* of our ofstream
      return file_;
    }
  };

  bool stl_check_mesh(viennagrid_mesh mesh, viennagrid_mesh_hierarchy mesh_hierarchy)
  {
    viennagrid_dimension topo_dimension, geo_dimension;
    viennagrid_mesh_hierarchy_cell_dimension_get(mesh_hierarchy, &topo_dimension);
    viennagrid_mesh_hierarchy_geometric_dimension_get(mesh_hierarchy, &geo_dimension);
    
    if(topo_dimension != 2 || geo_dimension != 3)
      return false;
    
    viennagrid_int *id, *id_end;
    viennagrid_element_type type;
    
    viennagrid_mesh_elements_get(mesh, 2, &id, &id_end);
    
    for(; id != id_end; ++id)
    {
      viennagrid_element_type_get(mesh_hierarchy, 2, *id, &type);
      
      if(type != VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
        return false;
    }
    
    return true;
  }

  template <class Writer>
  viennagrid_error stl_write(viennagrid_mesh_io mesh_io, const char* filename)
  {
    viennagrid_int *id, *id_end, *boundary_id, *boundary_id_end;
    viennagrid_element_type type;
    viennagrid_numeric *vertex_coords;
    std::vector<viennagrid::point> facet;
    
    viennagrid_mesh mesh;
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_io_mesh_get(mesh_io, &mesh);
    viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);
    
    if(!stl_check_mesh(mesh, mesh_hierarchy))
      return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
    
    Writer writer(filename);
    
    if (!writer)
      return VIENNAGRID_ERROR_IO_CANNOT_OPEN_FILE;
    
    writer.file_start();
    
    // iterate over all elements in the mesh
    viennagrid_mesh_elements_get(mesh, 2, &id, &id_end);
    
    for(; id != id_end; ++id)
    {
      // get the vertex ids. The vertices of an element are the boundary elements with topological dimension 0.
      viennagrid_element_boundary_elements(mesh_hierarchy, 2, *id,  0, &boundary_id, &boundary_id_end);
      
      // iterate over the vertices and get their coordinates
      for(; boundary_id != boundary_id_end; ++boundary_id)
      {
        viennagrid_mesh_hierarchy_vertex_coords_get(mesh_hierarchy, *boundary_id, &vertex_coords);
        facet.push_back(viennagrid::make_point(vertex_coords[0], vertex_coords[1], vertex_coords[2]));
      }
      
      viennagrid::point n = viennagrid::cross_prod<viennagrid::point, viennagrid::point>(facet[1]-facet[0], facet[2]-facet[0]);
      n.normalize();
      
      //use the given writer to write the facet and vertex data to file
      writer.facet_start(n);
      
      for(std::vector<viennagrid::point>::const_iterator it = facet.begin(); it != facet.end(); ++it)
      {
        writer.vertex(*it);
      }
      
      writer.facet_end();
      facet.clear();
    }
    
    writer.file_end();
    
    if(!writer)
      return VIENNAGRID_ERROR_IO_WRITE_ERROR;
    
    return VIENNAGRID_SUCCESS;
  }

}

viennagrid_error viennagrid_mesh_io_write_stl_binary(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  return stl_write<STLBinaryWriter>(mesh_io, filename);
}

viennagrid_error viennagrid_mesh_io_write_stl_ascii(viennagrid_mesh_io mesh_io,
                                                const char * filename)
{
  return stl_write<STLAsciiWriter>(mesh_io, filename);
}
#include "viennagrid/viennagrid.h"
#include "io.hpp"
#include "common.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iterator>
#include <algorithm>



viennagrid_error viennagrid_mesh_io_create(viennagrid_mesh_io * mesh_io)
{
  if (mesh_io)
    *mesh_io = new viennagrid_mesh_io_();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_retain(viennagrid_mesh_io mesh_io)
{
  if (mesh_io)
    retain(mesh_io);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_io_release(viennagrid_mesh_io mesh_io)
{
  if (mesh_io)
    release(mesh_io);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_clear(viennagrid_mesh_io mesh_io)
{
  if (mesh_io)
    mesh_io->clear();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_mesh_set(viennagrid_mesh_io mesh_io, viennagrid_mesh mesh)
{
  if (mesh_io)
    mesh_io->set_mesh(mesh);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_mesh_get(viennagrid_mesh_io mesh_io, viennagrid_mesh * mesh)
{
  if (mesh_io && mesh)
    *mesh = mesh_io->get_mesh();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_io_quantity_count(viennagrid_mesh_io mesh_io,
                                                   viennagrid_int * count)
{
  if (count)
    *count = mesh_io->quantity_field_count();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_quantity_field_get_by_index(viennagrid_mesh_io mesh_io,
                                                                viennagrid_int index,
                                                                viennagrid_quantity_field * quantity_field)
{
  if (quantity_field)
    *quantity_field = mesh_io->get_quantity_field(index);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_quantity_field_get(viennagrid_mesh_io mesh_io,
                                                       const char * quantity_name,
                                                       viennagrid_quantity_field * quantity_field)
{
  if (quantity_field)
    *quantity_field = mesh_io->get_quantity_field(quantity_name);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_quantity_field_set(viennagrid_mesh_io mesh_io,
                                                       viennagrid_quantity_field quantity_field)
{
  if (mesh_io && quantity_field)
  {
    const char * quantity_name;
    viennagrid_quantity_field_name_get(quantity_field, &quantity_name);
    mesh_io->set_quantity_field( quantity_name, quantity_field );
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_io_quantity_field_unset(viennagrid_mesh_io mesh_io,
                                                         const char * quantity_name)
{
  if (mesh_io)
  {
    mesh_io->set_quantity_field( quantity_name, 0 );
  }

  return VIENNAGRID_SUCCESS;
}
















bool extension_found( std::string const & filename, std::string const & extension )
{
  std::string ext_w_pt = ".";
  ext_w_pt += extension;

  if (filename.length() < ext_w_pt.length())
    return false;

  std::transform( ext_w_pt.begin(), ext_w_pt.end(), ext_w_pt.begin(), ::toupper );

  return filename.rfind(ext_w_pt) == filename.length()-ext_w_pt.length();
}

viennagrid_error viennagrid_mesh_io_filetype_from_filename(const char * filename,
                                                           viennagrid_int * filetype)
{
  std::string fn = filename;
  std::transform( fn.begin(), fn.end(), fn.begin(), ::toupper );

  if ( extension_found(fn.c_str(), "mesh") )
  {
    *filetype = VIENNAGRID_FILETYPE_NETGEN_MESH;
    return VIENNAGRID_SUCCESS;
  }
  if ( extension_found(fn.c_str(), "stl") )
  {
    *filetype = VIENNAGRID_FILETYPE_STL;
    return VIENNAGRID_SUCCESS;
  }

  *filetype = VIENNAGRID_FILETYPE_UNKNOWN;
  return VIENNAGRID_ERROR_IO_UNKNOWN_FILETYPE;
}




viennagrid_error viennagrid_mesh_io_read_with_filetype(viennagrid_mesh_io mesh_io,
                                                       const char * filename,
                                                       viennagrid_int filetype)
{
  viennagrid_mesh mesh;
  viennagrid_mesh_io_mesh_get(mesh_io, &mesh);
  if (!mesh)
  {
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_hierarchy_create(&mesh_hierarchy);

    viennagrid_mesh_hierarchy_root_mesh_get(mesh_hierarchy, &mesh);
    viennagrid_mesh_io_mesh_set(mesh_io, mesh);
  }
  else
  {
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);

    viennagrid_mesh root;
    viennagrid_mesh_hierarchy_root_mesh_get(mesh_hierarchy, &root);
    if (mesh != root)
      return VIENNAGRID_ERROR_IO_MESH_IS_NOT_ROOT;

    viennagrid_mesh_hierarchy_clear(mesh_hierarchy);
  }

  switch (filetype)
  {
    case VIENNAGRID_FILETYPE_NETGEN_MESH:
      return viennagrid_mesh_io_read_netgen(mesh_io, filename);
    case VIENNAGRID_FILETYPE_STL:
      return viennagrid_mesh_io_read_stl(mesh_io, filename);
  }

  return VIENNAGRID_ERROR_IO_UNKNOWN_FILETYPE;
}






viennagrid_error viennagrid_mesh_io_read(viennagrid_mesh_io mesh_io,
                                         const char * filename)
{
  viennagrid_int filetype;

  viennagrid_error error = viennagrid_mesh_io_filetype_from_filename(filename, &filetype);
  if (error != VIENNAGRID_SUCCESS)
    return error;

  return viennagrid_mesh_io_read_with_filetype(mesh_io, filename, filetype);
}



viennagrid_error viennagrid_mesh_io_write_with_filetype(viennagrid_mesh_io mesh_io,
                                                          const char * filename,
                                                          viennagrid_int filetype)
{
  switch(filetype)
  {
    case VIENNAGRID_FILETYPE_STL:
    case VIENNAGRID_FILETYPE_STL_ASCII:
      return viennagrid_mesh_io_write_stl_ascii(mesh_io, filename);
    case VIENNAGRID_FILETYPE_STL_BINARY:
      return viennagrid_mesh_io_write_stl_binary(mesh_io, filename);
  }
  
  return VIENNAGRID_ERROR_IO_UNKNOWN_FILETYPE;
}


viennagrid_error viennagrid_mesh_io_write(viennagrid_mesh_io mesh_io,
                                            const char * filename)
{
  viennagrid_int filetype;
  
  viennagrid_error error = viennagrid_mesh_io_filetype_from_filename(filename, &filetype);
  if(error != VIENNAGRID_SUCCESS)
    return error;
  
  return viennagrid_mesh_io_write_with_filetype(mesh_io, filename, filetype);
}


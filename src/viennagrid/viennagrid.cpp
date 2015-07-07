#include <cstring>
#include <cmath>

#include "viennagrid/viennagrid.h"
#include "mesh_hierarchy.hpp"
#include "plc.hpp"
#include "common.hpp"




viennagrid_error viennagrid_new(viennagrid_int size, void ** ptr)
{
  *ptr = malloc(size);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_delete(void ** ptr)
{
  free(*ptr);
  *ptr = NULL;
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_mesh_hierarchy_create(viennagrid_mesh_hierarchy * mesh_hierarchy)
{
  *mesh_hierarchy = new viennagrid_mesh_hierarchy_();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_retain(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  if (mesh_hierarchy)
    retain(mesh_hierarchy);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_release(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  if (mesh_hierarchy)
    release(mesh_hierarchy);

  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_mesh_hierarchy_property_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                        viennagrid_int property,
                                                        viennagrid_int value)
{
  switch (property)
  {
    case VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT:
      mesh_hierarchy->set_boundary_layout(value);

    default:
      return VIENNAGRID_ERROR_UNKNOWN_FLAG;
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_property_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                        viennagrid_int property,
                                                        viennagrid_int * value)
{
  switch (property)
  {
    case VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT:
      if (value)
        *value = mesh_hierarchy->boundary_layout();
      break;

    default:
      return VIENNAGRID_ERROR_UNKNOWN_FLAG;
  }

  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_hierarchy_cell_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                              viennagrid_dimension * cell_dimension)
{
  *cell_dimension = mesh_hierarchy->cell_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_dimension * geometric_dimension)
{
  *geometric_dimension = mesh_hierarchy->geometric_dimension();
  return VIENNAGRID_SUCCESS;
}
viennagrid_error viennagrid_mesh_geometric_dimension_get(viennagrid_mesh mesh,
                                                         viennagrid_dimension * geometric_dimension)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);
  viennagrid_mesh_hierarchy_geometric_dimension_get(mesh_hierarchy, geometric_dimension);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_hierarchy_geometric_dimension_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                   viennagrid_int geometric_dimension)
{
  mesh_hierarchy->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}
viennagrid_error viennagrid_mesh_geometric_dimension_set(viennagrid_mesh mesh,
                                                         viennagrid_int geometric_dimension)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);
  viennagrid_mesh_hierarchy_geometric_dimension_set(mesh_hierarchy, geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_clear(viennagrid_mesh_hierarchy mesh_hierarchy)
{
  mesh_hierarchy->clear();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_mesh_hierarchy_root_mesh_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_mesh * mesh)
{
  if (mesh)
    *mesh = mesh_hierarchy->root();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_mesh_hierarchy_get(viennagrid_mesh mesh,
                                                    viennagrid_mesh_hierarchy * mesh_hierarchy)
{
  if (mesh_hierarchy)
    *mesh_hierarchy = mesh->mesh_hierarchy();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_create(viennagrid_mesh parent,
                                        viennagrid_mesh * mesh)
{
  if (mesh)
    *mesh = parent->make_child();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_children_count(viennagrid_mesh mesh,
                                                viennagrid_int * children_count)
{
  if (children_count)
    *children_count = mesh->children_count();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_child_get(viennagrid_mesh mesh,
                                           viennagrid_int id,
                                           viennagrid_mesh * child)
{
  if (child)
    *child = mesh->get_child(id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_parent_get(viennagrid_mesh mesh,
                                            viennagrid_mesh * parent)
{
  if (parent)
    *parent = mesh->parent();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_name_get(viennagrid_mesh mesh,
                                          const char ** mesh_name)
{
  if (mesh_name)
  {
    if (!mesh->name().empty())
      *mesh_name = mesh->name().c_str();
    else
      *mesh_name = NULL;
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_name_set(viennagrid_mesh mesh,
                                          const char * mesh_name)
{
  if (mesh_name)
    mesh->set_name(mesh_name);
  else
    mesh->set_name( std::string() );

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_mesh_hierarchy_element_count(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_element_type element_type,
                                                         viennagrid_int * count)
{
  if (count)
    *count = mesh_hierarchy->element_count(element_type);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_vertex_create(viennagrid_mesh_hierarchy hierarchy,
                                                         const viennagrid_numeric * coords,
                                                         viennagrid_int * vertex_id)
{
  viennagrid_int tmp = hierarchy->make_vertex(coords);

  if (vertex_id)
    *vertex_id = tmp;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_vertex_create(viennagrid_mesh mesh,
                                               const viennagrid_numeric * coords,
                                               viennagrid_int * vertex_id)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);

  viennagrid_int vid;
  viennagrid_mesh_hierarchy_vertex_create(mesh_hierarchy, coords, &vid);

  viennagrid_mesh_element_add(mesh, 0, vid);

  if (vertex_id)
    *vertex_id = vid;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_pointer(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                                 viennagrid_numeric ** coords)
{
  if (coords)
    *coords = mesh_hierarchy->get_vertex_pointer();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_hierarchy_vertex_coords_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                             viennagrid_int id,
                                                             viennagrid_numeric ** coords)
{
  if (coords)
    *coords = mesh_hierarchy->get_vertex(id);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_hierarchy_element_create(viennagrid_mesh_hierarchy hierarchy,
                                                          viennagrid_element_type element_type,
                                                          viennagrid_int vertex_count,
                                                          viennagrid_int * vertex_ids,
                                                          viennagrid_int * element_id)
{
  viennagrid_mesh root = hierarchy->root();

  std::pair<viennagrid_int, bool> tmp = hierarchy->get_make_element(element_type, vertex_ids, vertex_count, root);

  if (element_id)
    *element_id = tmp.first;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_element_create(viennagrid_mesh mesh,
                                                viennagrid_element_type element_type,
                                                viennagrid_int vertex_count,
                                                viennagrid_int * vertex_ids,
                                                viennagrid_int * element_id)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);

  viennagrid_int eid;
  viennagrid_mesh_hierarchy_element_create(mesh_hierarchy, element_type, vertex_count, vertex_ids, &eid);

  viennagrid_mesh_element_add(mesh, viennagrid_topological_dimension(element_type), eid);

  if (element_id)
    *element_id = eid;

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_mesh_refined_element_create(viennagrid_mesh mesh,
                                                        viennagrid_dimension element_topo_dim,
                                                        viennagrid_int element_id,
                                                        viennagrid_element_type    refined_element_type,
                                                        viennagrid_int            refined_element_vertex_count,
                                                        viennagrid_int *        refined_element_vertex_ids,
                                                        viennagrid_int            intersects_count,
                                                        viennagrid_int *        intersect_vertices_ids,
                                                        viennagrid_int *        intersects_ids,
                                                        viennagrid_dimension *    intersects_topo_dims,
                                                        viennagrid_int *        id)
{
  *id = mesh->make_refined_element(element_topo_dim, element_id,
                                   refined_element_type, refined_element_vertex_count,
                                   refined_element_vertex_ids,
                                   intersects_count, intersect_vertices_ids,
                                   intersects_ids, intersects_topo_dims);
  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_type_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                             viennagrid_dimension element_topo_dim,
                                             viennagrid_int element_id,
                                             viennagrid_element_type * element_type)
{
  if (element_type)
    *element_type = mesh_hierarchy->element_buffer(element_topo_dim).element_type(element_id);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_element_boundary_element_pointers(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_dimension boundary_topo_dim,
                                                      viennagrid_int ** boundary_offsets,
                                                      viennagrid_int ** boundary_ids)
{
  if (boundary_offsets)
    *boundary_offsets = hierarchy->element_buffer(element_topo_dim).boundary_buffer(boundary_topo_dim).offset_pointer();

  if (boundary_ids)
    *boundary_ids = hierarchy->element_buffer(element_topo_dim).boundary_buffer(boundary_topo_dim).values_pointer();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_element_boundary_elements(viennagrid_mesh_hierarchy hierarchy,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_int element_id,
                                                      viennagrid_dimension boundary_topo_dim,
                                                      viennagrid_int ** boundary_element_ids_begin,
                                                      viennagrid_int ** boundary_element_ids_end)
{
  if (boundary_element_ids_begin)
    *boundary_element_ids_begin = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);

  if (boundary_element_ids_end)
    *boundary_element_ids_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_coboundary_elements(viennagrid_mesh mesh,
                                                        viennagrid_dimension element_topo_dim,
                                                        viennagrid_int element_id,
                                                        viennagrid_dimension coboundary_topo_dim,
                                                        viennagrid_int ** coboundary_element_ids_begin,
                                                        viennagrid_int ** coboundary_element_ids_end)
{
  if (coboundary_element_ids_begin)
    *coboundary_element_ids_begin = mesh->coboundary_begin(element_topo_dim, element_id, coboundary_topo_dim);

  if (coboundary_element_ids_end)
    *coboundary_element_ids_end = mesh->coboundary_end(element_topo_dim, element_id, coboundary_topo_dim);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_element_neighbor_elements(viennagrid_mesh mesh,
                                                      viennagrid_dimension element_topo_dim,
                                                      viennagrid_int element_id,
                                                      viennagrid_dimension connector_topo_dim,
                                                      viennagrid_dimension neighbor_topo_dim,
                                                      viennagrid_int ** neighbor_element_ids_begin,
                                                      viennagrid_int ** neighbor_element_ids_end)
{
  if (neighbor_element_ids_begin)
    *neighbor_element_ids_begin = mesh->neighbor_begin(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);

  if (neighbor_element_ids_end)
    *neighbor_element_ids_end = mesh->neighbor_end(element_topo_dim, element_id, connector_topo_dim, neighbor_topo_dim);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_element_is_mesh_boundary(viennagrid_mesh mesh,
                                                     viennagrid_dimension element_topo_dim,
                                                     viennagrid_int element_id,
                                                     viennagrid_bool * result)
{
  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags();

  if (result)
    *result = mesh->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_is_region_boundary(viennagrid_region region,
                                                       viennagrid_mesh mesh,
                                                       viennagrid_dimension element_topo_dim,
                                                       viennagrid_int element_id,
                                                       viennagrid_bool * result)
{
  const_cast<viennagrid_mesh>(mesh)->make_boundary_flags( const_cast<viennagrid_region>(region) );

  if (result)
    *result = region->is_boundary(element_topo_dim, element_id) ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_mesh_elements_get(viennagrid_mesh mesh,
                                              viennagrid_dimension element_topo_dim,
                                              viennagrid_int ** element_ids_begin,
                                              viennagrid_int ** element_ids_end)
{
  if (element_ids_begin)
    *element_ids_begin = mesh->elements_begin(element_topo_dim);

  if (element_ids_end)
    *element_ids_end = mesh->elements_end(element_topo_dim);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_element_add(viennagrid_mesh mesh,
                                             viennagrid_dimension element_topo_dim,
                                             viennagrid_int element_id)
{
  mesh->add_element(element_topo_dim, element_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_get(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_int element_id,
                                               viennagrid_int * element_parent_id)
{
  if (element_parent_id)
    *element_parent_id = mesh_hierarchy->element_buffer(element_topo_dim).parent_id(element_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_parent_set(viennagrid_mesh_hierarchy mesh_hierarchy,
                                               viennagrid_dimension element_topo_dim,
                                               viennagrid_int element_id,
                                               viennagrid_int element_parent_id)
{
  mesh_hierarchy->element_buffer(element_topo_dim).set_parent_id(element_id, element_parent_id);
  return VIENNAGRID_SUCCESS;
}






viennagrid_error viennagrid_region_create(viennagrid_mesh_hierarchy hierarchy,
                                          viennagrid_region * region)
{
  viennagrid_region tmp = hierarchy->create_region();

  if (region)
    *region = tmp;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get_or_create(viennagrid_mesh_hierarchy hierarchy,
                                                 viennagrid_int region_id,
                                                 viennagrid_region * region)
{
  viennagrid_region tmp = hierarchy->get_or_create_region(region_id);;

  if (region)
    *region = tmp;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_get(viennagrid_mesh_hierarchy hierarchy,
                                       viennagrid_int region_id,
                                       viennagrid_region * region)
{
  if (region)
    *region = hierarchy->get_region(region_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_mesh_hierarchy_get(viennagrid_region region,
                                                      viennagrid_mesh_hierarchy * hierarchy)
{
  if (hierarchy)
    *hierarchy = region->mesh_hierarchy();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_region_name_get(viennagrid_region region,
                                            const char ** region_name)
{
  if (region_name)
    *region_name = region->name().c_str();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_name_set(viennagrid_region region,
                                       const char * region_name)
{
  region->set_name( std::string(region_name) );
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_region_id_get(viennagrid_region region,
                                          viennagrid_int * id)
{
  if (id)
    *id = region->id();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_mesh_hierarchy_regions_get(viennagrid_mesh_hierarchy hierarchy,
                                                       viennagrid_region ** region_begin,
                                                       viennagrid_region ** region_end)
{
  if (region_begin)
    *region_begin = hierarchy->regions_begin();

  if (region_end)
    *region_end = hierarchy->regions_end();

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_element_regions_get(viennagrid_mesh_hierarchy hierarchy,
                                                viennagrid_dimension element_topo_dim,
                                                viennagrid_int element_id,
                                                viennagrid_region ** region_begin,
                                                viennagrid_region ** region_end)
{
  if (region_begin)
    *region_begin = hierarchy->element_buffer(element_topo_dim).regions_begin(element_id);

  if (region_end)
    *region_end = hierarchy->element_buffer(element_topo_dim).regions_end(element_id);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_element_add_to_region(viennagrid_mesh_hierarchy hierarchy,
                                                  viennagrid_dimension element_topo_dim,
                                                  viennagrid_int element_id,
                                                  viennagrid_region region)
{
  hierarchy->element_buffer(element_topo_dim).add_to_region(element_id, region);

  for (viennagrid_dimension boundary_topo_dim = 0; boundary_topo_dim != element_topo_dim; ++boundary_topo_dim)
  {
    viennagrid_int * boundary = hierarchy->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);
    viennagrid_int * boundary_end = hierarchy->boundary_end(element_topo_dim, element_id, boundary_topo_dim);

    for (; boundary != boundary_end; ++boundary)
      hierarchy->element_buffer(boundary_topo_dim).add_to_region(*boundary, region);
  }

  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_plc_create(viennagrid_plc * plc)
{
  *plc = new viennagrid_plc_;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_retain(viennagrid_plc plc)
{
  if (plc)
    retain(plc);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_release(viennagrid_plc plc)
{
  if (plc)
    release(plc);

  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_plc_clear(viennagrid_plc plc)
{
  plc->clear();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_init_from_plc(viennagrid_plc src_plc,
                                              viennagrid_plc dst_plc,
                                              viennagrid_int copy_hole_points,
                                              viennagrid_int copy_seed_points)
{
  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(src_plc, &geometric_dimension);

  viennagrid_plc_clear(dst_plc);
  viennagrid_plc_geometric_dimension_set(dst_plc, geometric_dimension);

  if (copy_hole_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * hole_points;
    viennagrid_int hole_point_count;
    viennagrid_plc_volumetric_hole_points_get(src_plc, &hole_points, &hole_point_count);
    for (viennagrid_int i = 0; i != hole_point_count; ++i)
      viennagrid_plc_volumetric_hole_point_add(dst_plc, hole_points + i*geometric_dimension);
  }

  if (copy_seed_points == VIENNAGRID_TRUE)
  {
    viennagrid_numeric * seed_points;
    viennagrid_int * seed_point_regions;
    viennagrid_int seed_point_count;
    viennagrid_plc_seed_points_get(src_plc, &seed_points, &seed_point_regions, &seed_point_count);
    for (viennagrid_int i = 0; i != seed_point_count; ++i)
      viennagrid_plc_seed_point_add(dst_plc, seed_points + i*geometric_dimension, seed_point_regions[i]);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_get(viennagrid_plc plc,
                                                        viennagrid_dimension * geometric_dimension)
{
  *geometric_dimension = plc->geometric_dimension();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_geometric_dimension_set(viennagrid_plc plc,
                                                        viennagrid_int geometric_dimension)
{
  plc->set_geometric_dimension(geometric_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_element_count_get(viennagrid_plc plc,
                                                  viennagrid_dimension topologic_dimension,
                                                  viennagrid_int * element_count)
{
  *element_count = plc->element_count(topologic_dimension);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_vertex_create(viennagrid_plc plc,
                                              const viennagrid_numeric * coords,
                                              viennagrid_int * vertex_id)
{
  viennagrid_int id = plc->make_vertex(coords);

  if (vertex_id)
    *vertex_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_vertex_coords_pointer(viennagrid_plc plc,
                                               viennagrid_numeric ** coords)
{
  *coords = plc->vertex_pointer();
  return VIENNAGRID_SUCCESS;
}
viennagrid_error viennagrid_plc_vertex_coords_get(viennagrid_plc plc,
                                           viennagrid_int id,
                                           viennagrid_numeric ** coords)
{
  *coords = plc->get_vertex(id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_line_create(viennagrid_plc plc,
                                            viennagrid_int vertex_id0,
                                            viennagrid_int vertex_id1,
                                            viennagrid_int * line_id)
{
  viennagrid_int id = plc->get_make_line(vertex_id0, vertex_id1);

  if (line_id)
    *line_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_create(viennagrid_plc plc,
                                             viennagrid_int * line_ids,
                                             viennagrid_int line_count,
                                             viennagrid_int * facet_id)
{
  viennagrid_int id = plc->get_make_facet(line_ids, line_count);

  if (facet_id)
    *facet_id = id;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_boundary_elements(viennagrid_plc plc,
                                                  viennagrid_dimension element_topo_dim,
                                                  viennagrid_int element_id,
                                                  viennagrid_dimension boundary_topo_dim,
                                                  viennagrid_int ** boundary_element_ids_begin,
                                                  viennagrid_int ** boundary_element_ids_end)
{
  if (boundary_element_ids_begin)
    *boundary_element_ids_begin = plc->boundary_begin(element_topo_dim, element_id, boundary_topo_dim);

  if (boundary_element_ids_end)
    *boundary_element_ids_end = plc->boundary_end(element_topo_dim, element_id, boundary_topo_dim);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_add(viennagrid_plc plc,
                                                     viennagrid_int facet_id,
                                                     const viennagrid_numeric * coords)
{
  plc->add_facet_hole_point(facet_id, coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_point_delete(viennagrid_plc plc,
                                                        viennagrid_int facet_id,
                                                        viennagrid_int point_id)
{
  plc->delete_facet_hole_point(facet_id, point_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_facet_hole_points_get(viennagrid_plc plc,
                                                      viennagrid_int facet_id,
                                                      viennagrid_numeric ** coords,
                                                      viennagrid_int * hole_point_count)
{
  *coords = plc->facet_hole_points(facet_id);
  *hole_point_count = plc->facet_hole_point_count(facet_id);
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_plc_volumetric_hole_point_add(viennagrid_plc plc,
                                                          const viennagrid_numeric * coords)
{
  plc->add_hole_point(coords);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_volumetric_hole_points_get(viennagrid_plc plc,
                                                           viennagrid_numeric ** coords,
                                                           viennagrid_int * count)
{
  *coords = plc->get_hole_points();
  *count = plc->hole_point_count();
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_plc_seed_point_add(viennagrid_plc plc,
                                               const viennagrid_numeric * coords,
                                               viennagrid_int region_id)
{
  plc->add_seed_point(coords, region_id);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_seed_points_get(viennagrid_plc plc,
                                                viennagrid_numeric ** coords,
                                                viennagrid_int ** region_ids,
                                                viennagrid_int * count)
{
  *coords = plc->get_seed_points();
  *region_ids = plc->get_seed_point_regions();
  *count = plc->seed_point_count();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_plc_read_tetgen_poly(viennagrid_plc plc,
                                                 const char * filename)
{
  return plc->read_tetgen_poly(filename);
}

viennagrid_error viennagrid_plc_line_refine(viennagrid_plc plc,
                                            viennagrid_plc output_plc,
                                            viennagrid_numeric line_size)
{
  viennagrid_dimension geometric_dimension;
  viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

  viennagrid_plc_init_from_plc(plc, output_plc, VIENNAGRID_TRUE, VIENNAGRID_TRUE);

  viennagrid_int line_count;
  viennagrid_plc_element_count_get(plc, 1, &line_count);

  viennagrid_int facet_count;
  viennagrid_plc_element_count_get(plc, 2, &facet_count);

  std::map<viennagrid_int, viennagrid_int> vertex_map;
  std::vector< std::vector<viennagrid_int> > line_to_lines_map(line_count);

  for (viennagrid_int line_id = 0; line_id != line_count; ++line_id)
  {
    viennagrid_int * vertices_begin;
    viennagrid_int * vertices_end;
    viennagrid_plc_boundary_elements(plc, 1, line_id, 0, &vertices_begin, &vertices_end);

    viennagrid_int v0 = *(vertices_begin+0);
    viennagrid_int v1 = *(vertices_begin+1);


    viennagrid_numeric * p0;
    viennagrid_plc_vertex_coords_get(plc, v0, &p0);

    viennagrid_numeric * p1;
    viennagrid_plc_vertex_coords_get(plc, v1, &p1);


    viennagrid_int nv0;
    std::map<viennagrid_int, viennagrid_int>::iterator it0 = vertex_map.find(v0);
    if (it0 == vertex_map.end())
    {
      viennagrid_plc_vertex_create(output_plc, p0, &nv0);
      vertex_map[v0] = nv0;
    }
    else
      nv0 = it0->second;

    viennagrid_int nv1;
    std::map<viennagrid_int, viennagrid_int>::iterator it1 = vertex_map.find(v1);
    if (it1 == vertex_map.end())
    {
      viennagrid_plc_vertex_create(output_plc, p1, &nv1);
      vertex_map[v1] = nv1;
    }
    else
      nv1 = it1->second;


    std::vector<viennagrid_int> new_lines;

    double current_line_size = 0.0;
    for (viennagrid_dimension i = 0; i != geometric_dimension; ++i)
      current_line_size += (p0[i]-p1[i])*(p0[i]-p1[i]);
    current_line_size = std::sqrt(current_line_size);

    if (current_line_size <= line_size)
    {
      viennagrid_int line_id;
      viennagrid_plc_line_create( output_plc, nv0, nv1, &line_id );
      new_lines.push_back(line_id);
    }
    else
    {
      int number_of_new_lines = std::ceil(current_line_size / line_size);
      assert(number_of_new_lines > 1);

      std::vector<viennagrid_numeric> offset(geometric_dimension);
      for (viennagrid_dimension i = 0; i != geometric_dimension; ++i)
        offset[i] = (p1[i]-p0[i]) / number_of_new_lines;


      std::vector<viennagrid_int> vertices;
      vertices.push_back(nv0);
      for (int i = 1; i != number_of_new_lines; ++i)
      {
        std::vector<viennagrid_numeric> tmp(geometric_dimension);
        for (viennagrid_dimension j = 0; j != geometric_dimension; ++j)
          tmp[j] = p0[j] + i*offset[j];

        viennagrid_int vertex_id;
        viennagrid_plc_vertex_create( output_plc, &tmp[0], &vertex_id);
        vertices.push_back(vertex_id);
      }
      vertices.push_back(nv1);

      for (std::size_t i = 0; i != vertices.size()-1; ++i)
      {
        viennagrid_int line_id;
        viennagrid_plc_line_create( output_plc, vertices[i], vertices[i+1], &line_id );
        new_lines.push_back(line_id);
      }
    }

    line_to_lines_map[line_id] = new_lines;
  }



  for (viennagrid_int facet_id = 0; facet_id != facet_count; ++facet_id)
  {
    std::vector<viennagrid_int> new_lines;

    viennagrid_int * lines_begin;
    viennagrid_int * lines_end;
    viennagrid_plc_boundary_elements(plc, 2, facet_id, 1, &lines_begin, &lines_end);

    for (viennagrid_int * line_it = lines_begin; line_it != lines_end; ++line_it)
    {
      std::vector<viennagrid_int> const & current_new_lines = line_to_lines_map[*line_it];
      std::copy( current_new_lines.begin(), current_new_lines.end(), std::back_inserter(new_lines) );
    }

    viennagrid_int new_facet_id;
    viennagrid_plc_facet_create(output_plc, &new_lines[0], new_lines.size(), &new_facet_id);

    viennagrid_numeric * facet_hole_points;
    viennagrid_int facet_hole_point_count;
    viennagrid_plc_facet_hole_points_get(plc, facet_id, &facet_hole_points, &facet_hole_point_count);
    for (viennagrid_int i = 0; i != facet_hole_point_count; ++i)
      viennagrid_plc_facet_hole_point_add(plc, new_facet_id, facet_hole_points + i*geometric_dimension);
  }

  return VIENNAGRID_SUCCESS;
}


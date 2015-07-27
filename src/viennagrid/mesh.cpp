#include "mesh.hpp"
#include "mesh_hierarchy.hpp"




viennagrid_mesh_::viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in) : hierarchy_(hierarchy_in), parent_(0)
{
  hierarchy_in->add_mesh(this);
  clear();
}
viennagrid_mesh_::viennagrid_mesh_(viennagrid_mesh_hierarchy hierarchy_in, viennagrid_mesh parent_in) : hierarchy_(hierarchy_in), parent_(parent_in)
{
  hierarchy_in->add_mesh(this);
  clear();
}

viennagrid_mesh viennagrid_mesh_::make_child()
{
  // switching to full boundary layout, sparse boundary layout is support for root-only meshes only
  mesh_hierarchy()->set_boundary_layout(VIENNAGRID_BOUNDARY_LAYOUT_FULL);

  viennagrid_mesh mesh = new viennagrid_mesh_( mesh_hierarchy(), this );
  children.push_back( mesh );
  element_children.push_back( viennagrid_element_children_() );
  mesh_children_map[ children.back() ] = children.size()-1;

  return mesh;
}


void viennagrid_mesh_::add_elements(viennagrid_dimension element_topo_dim,
                                    viennagrid_int first_id, viennagrid_int count)
{
  assert( (element_topo_dim >= 0) && (element_topo_dim < VIENNAGRID_TOPOLOGIC_DIMENSION_END) );
  assert( first_id >= 0 );

  element_handle_buffer(element_topo_dim).add_elements( first_id, count );
}



void viennagrid_mesh_::add_element(viennagrid_dimension element_topo_dim,
                                   viennagrid_int element_id)
{
  assert( (element_topo_dim >= 0) && (element_topo_dim < VIENNAGRID_TOPOLOGIC_DIMENSION_END) );
  assert( element_id >= 0 );

  viennagrid_int parent_id = mesh_hierarchy()->element_buffer(element_topo_dim).parent_id(element_id);

  if (element_topo_dim == cell_dimension())
  {
    if (parent())
    {
      if (parent_id >= 0)
      {
        assert( parent()->element_handle_buffer(element_topo_dim).element_present(parent_id) ||
                parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );
      }
      else
      {
        assert( parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );
      }
    }
    else
    {
      assert( parent_id < 0 );
    }
  }

  element_handle_buffer(element_topo_dim).add_element(element_id);
}




template<typename SortedContainerT1, typename SortedContainerT2>
bool is_subset(SortedContainerT1 const & cont1, SortedContainerT2 const & cont2)
{
  typename SortedContainerT1::const_iterator it1 = cont1.begin();
  typename SortedContainerT1::const_iterator it2 = cont2.begin();

  while ((it1 != cont1.end()) && (it2 != cont2.end()))
  {
    if (*it1 == *it2)
      ++it1;
    ++it2;
  }

  return it1 == cont1.end();
}







viennagrid_int viennagrid_mesh_::make_refined_element(
  viennagrid_dimension      element_topo_dim,
  viennagrid_int          element_id,
  viennagrid_element_type    refined_element_type,
  viennagrid_int            refined_element_vertex_count,
  viennagrid_int *        refined_element_vertex_ids,
  viennagrid_int            intersects_count,
  viennagrid_int *        intersect_vertices_ids,
  viennagrid_int *        intersects_ids,
  viennagrid_dimension *    intersects_topo_dims)
{
  assert( !is_root() );
  assert( parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );

  std::vector<viennagrid_int> refined_elements;


  std::map< viennagrid_dimension, std::map<viennagrid_int, std::vector<viennagrid_int> > > intersects;

  for (viennagrid_int i = 0; i != intersects_count; ++i)
  {
    intersects[intersects_topo_dims[i]][intersects_ids[i]].push_back( intersect_vertices_ids[i] );
  }


  viennagrid_int child_element_id = mesh_hierarchy()->get_make_element(
          refined_element_type,
          refined_element_vertex_ids,
          refined_element_vertex_count,
          NULL ).first;

  mesh_hierarchy()->element_buffer(element_topo_dim).set_parent_id( child_element_id, element_id );


  add_element(element_topo_dim, child_element_id);
  for (viennagrid_dimension boundary_dim = 0; boundary_dim != element_topo_dim; ++boundary_dim)
  {
    viennagrid_int * boundary = boundary_begin(element_topo_dim, child_element_id, boundary_dim);
    viennagrid_int * end = boundary_end(element_topo_dim, child_element_id, boundary_dim);

    for (; boundary != end; ++boundary)
    {
      viennagrid_int boundary_id = *boundary;
      add_element(boundary_dim, boundary_id);

      // no parents for vertices
      if (boundary_dim == 0)
        continue;


      viennagrid_int * bv = boundary_begin(boundary_dim, boundary_id, 0);
      viennagrid_int * bv_end = boundary_end(boundary_dim, boundary_id, 0);

      std::vector<viennagrid_int> boundary_vertices(bv_end-bv);

      std::copy(bv, bv_end, boundary_vertices.begin());
      std::sort(boundary_vertices.begin(), boundary_vertices.end());

      if (mesh_hierarchy()->element_buffer(boundary_dim).parent_id(boundary_id) < 0)
      {
        std::map<viennagrid_int, std::vector<viennagrid_int> > & intersected_elements = intersects[boundary_dim];

        for (std::map<viennagrid_int, std::vector<viennagrid_int> >::iterator kt = intersected_elements.begin(); kt != intersected_elements.end(); ++kt)
        {
          viennagrid_int * iv = boundary_begin(boundary_dim, kt->first, 0);
          viennagrid_int * iv_end = boundary_end(boundary_dim, kt->first, 0);

          std::vector<viennagrid_int> intersected_element_ids(iv_end-iv + kt->second.size());

          std::copy(iv, iv_end, intersected_element_ids.begin());
          std::copy(kt->second.begin(), kt->second.end(), &intersected_element_ids[iv_end-iv]);
          std::sort(intersected_element_ids.begin(), intersected_element_ids.end());

          if ( is_subset(boundary_vertices, intersected_element_ids) )
            mesh_hierarchy()->element_buffer(boundary_dim).set_parent_id(boundary_id, kt->first);
        }
      }
    }
  }

  return child_element_id;
}






bool viennagrid_mesh_::is_root() const { return hierarchy_->root() == this; }

viennagrid_dimension viennagrid_mesh_::geometric_dimension() { return hierarchy_->geometric_dimension(); }
viennagrid_dimension viennagrid_mesh_::cell_dimension() { return hierarchy_->cell_dimension(); }

viennagrid_int * viennagrid_mesh_::boundary_begin(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim)
{ return mesh_hierarchy()->boundary_begin(element_topo_dim, element_id, boundary_topo_dim); }
viennagrid_int * viennagrid_mesh_::boundary_end(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim)
{ return mesh_hierarchy()->boundary_end(element_topo_dim, element_id, boundary_topo_dim); }


bool viennagrid_mesh_::is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{ return mesh_hierarchy()->is_obsolete( coboundary_change_counter(element_topo_dim, coboundary_topo_dim) ); }
void viennagrid_mesh_::set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{ mesh_hierarchy()->update_change_counter( coboundary_change_counter(element_topo_dim, coboundary_topo_dim) ); }

bool viennagrid_mesh_::is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{ return mesh_hierarchy()->is_obsolete( neighbor_change_counter(element_topo_dim, connector_topo_dim, neighbor_topo_dim) ); }
void viennagrid_mesh_::set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{ mesh_hierarchy()->update_change_counter( neighbor_change_counter(element_topo_dim, connector_topo_dim, neighbor_topo_dim) ); }


bool viennagrid_mesh_::are_boundary_flags_obsolete()
{ return hierarchy_->is_obsolete( boundary_elements_change_counter ); }
void viennagrid_mesh_::set_boundary_flags_uptodate()
{ hierarchy_->update_change_counter( boundary_elements_change_counter ); }





bool viennagrid_mesh_::is_element_children_obsolete(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->is_obsolete( element_children_change_counter(child_mesh_id_, element_topo_dim) ); }
void viennagrid_mesh_::set_element_children_uptodate(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->update_change_counter( element_children_change_counter(child_mesh_id_, element_topo_dim) ); }


void viennagrid_mesh_::make_element_children(viennagrid_mesh child, viennagrid_int element_topo_dim)
{
  viennagrid_int child_mesh_id_ = child_mesh_id(child);

  if (!is_element_children_obsolete(child_mesh_id_, element_topo_dim))
    return;


  std::map<viennagrid_int, std::vector<viennagrid_int> > children;

  viennagrid_int * it       = child->elements_begin(element_topo_dim);
  viennagrid_int * it_end   = child->elements_end(element_topo_dim);

  for (; it != it_end; ++it)
  {
    viennagrid_int parent_id = mesh_hierarchy()->element_buffer(element_topo_dim).parent_id(*it);
    assert( element_handle_buffer(element_topo_dim).element_present(parent_id) );
    children[parent_id].push_back(*it);
  }



  ViennaGridElementChildrenBufferType & element_children_buffer = element_children[child_mesh_id_].children_ids_buffer(element_topo_dim);
  element_children_buffer.clear();



  set_element_children_uptodate(child_mesh_id_, element_topo_dim);
}





void viennagrid_mesh_::make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{
  if (!is_coboundary_obsolete(element_topo_dim, coboundary_topo_dim))
    return;

  viennagrid_int * coboundary_ids = element_handle_buffer(coboundary_topo_dim).ids();
  viennagrid_int coboundary_count = element_handle_buffer(coboundary_topo_dim).count();

  std::map< viennagrid_int, std::vector<viennagrid_int> > tmp;

  for (viennagrid_int i = 0; i < coboundary_count; ++i)
  {
    viennagrid_int coboundary_id= coboundary_ids[i];
    viennagrid_int * element_it = boundary_begin(coboundary_topo_dim, coboundary_id, element_topo_dim);
    viennagrid_int * element_end = boundary_end(coboundary_topo_dim, coboundary_id, element_topo_dim);

    for (; element_it != element_end; ++element_it)
      tmp[*element_it].push_back(coboundary_id);
  }


  ViennaGridCoBoundaryBufferType & coboundary_buffer = element_handle_buffer(element_topo_dim).coboundary_buffer(coboundary_topo_dim);

  coboundary_buffer.clear();
  for (std::map< viennagrid_int, std::vector<viennagrid_int> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    viennagrid_int * ids = coboundary_buffer.insert( it->first, it->second.size() );
    std::copy( it->second.begin(), it->second.end(), ids );
  }

  set_coboundary_uptodate(element_topo_dim, coboundary_topo_dim);
}




void viennagrid_mesh_::make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{
  if (!is_neighbor_obsolete(element_topo_dim, connector_topo_dim, neighbor_topo_dim))
    return;

  if ((element_topo_dim > connector_topo_dim) && (connector_topo_dim > neighbor_topo_dim))
    return;

  if ((element_topo_dim == connector_topo_dim) || (connector_topo_dim == neighbor_topo_dim))
    return;

  viennagrid_int * element_ids = element_handle_buffer(element_topo_dim).ids();
  viennagrid_int element_count = element_handle_buffer(element_topo_dim).count();


  ViennaGridNeighborBufferType & neighbor_buffer = element_handle_buffer(element_topo_dim).neighbor_buffer(connector_topo_dim, neighbor_topo_dim);
  neighbor_buffer.clear();

  for (viennagrid_int i = 0; i < element_count; ++i)
  {
    viennagrid_int element_id= element_ids[i];
    std::set<viennagrid_int> tmp;

    viennagrid_int * connector_it = connected_begin(element_topo_dim, element_id, connector_topo_dim);
    viennagrid_int * connector_end = connected_end(element_topo_dim, element_id, connector_topo_dim);

    for (; connector_it != connector_end; ++connector_it)
    {
      viennagrid_int connector_id= *connector_it;

      viennagrid_int * neighbor_it = connected_begin(connector_topo_dim, connector_id, neighbor_topo_dim);
      viennagrid_int * neighbor_end = connected_end(connector_topo_dim, connector_id, neighbor_topo_dim);


      for (; neighbor_it != neighbor_end; ++neighbor_it)
      {
        viennagrid_int neighbor_id = *neighbor_it;
        if ((element_topo_dim == neighbor_topo_dim) && (element_id == neighbor_id))
          continue;

        tmp.insert( neighbor_id );
      }
    }

    if (element_topo_dim > neighbor_topo_dim)
    {
      viennagrid_int * it = boundary_begin(element_topo_dim, element_id, neighbor_topo_dim);
      viennagrid_int * end = boundary_begin(element_topo_dim, element_id, neighbor_topo_dim);

      for (; it != end; ++it)
        tmp.erase(*it);
    }



    viennagrid_int * ids = neighbor_buffer.insert( element_id, tmp.size() );
    std::copy( tmp.begin(), tmp.end(), ids );
  }

  set_neighbor_uptodate(element_topo_dim, connector_topo_dim, neighbor_topo_dim);
}




void viennagrid_mesh_::make_boundary_flags(viennagrid_region region)
{
  const viennagrid_int cell_topo_dim = cell_dimension();
  const viennagrid_int facet_topo_dim = cell_topo_dim-1;

  if (!region->are_boundary_flags_obsolete())
    return;

  region->clear_boundary(facet_topo_dim);
  make_coboundary(facet_topo_dim, cell_topo_dim);

  for (viennagrid_int * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_int * it = coboundary_begin(facet_topo_dim, *facet, cell_topo_dim);
    viennagrid_int * cells_end = coboundary_end(facet_topo_dim, *facet, cell_topo_dim);

    viennagrid_int size = 0;
    for (; it != cells_end; ++it)
    {
      if ( mesh_hierarchy()->is_in_region(cell_topo_dim, *it, region->id()) )
        ++size;
    }

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_type(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_int * it = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).begin(*facet);
          viennagrid_int * boundary_end = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).end(*facet);
          for (; it != boundary_end; ++it)
          {
            region->set_boundary(boundary_topo_dim, *it);
          }
        }
      }

      region->set_boundary(facet_topo_dim, *facet);
    }
  }

  region->set_boundary_flags_uptodate();
}


void viennagrid_mesh_::make_boundary_flags()
{
  const viennagrid_int cell_topo_dim = cell_dimension();
  const viennagrid_int facet_topo_dim = cell_topo_dim-1;

  if (!are_boundary_flags_obsolete())
    return;


  clear_boundary(facet_topo_dim);
  make_coboundary(facet_topo_dim, cell_topo_dim);

  for (viennagrid_int * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_int * cells_begin = coboundary_begin(facet_topo_dim, *facet, cell_topo_dim);
    viennagrid_int * cells_end = coboundary_end(facet_topo_dim, *facet, cell_topo_dim);

    viennagrid_int size = cells_end - cells_begin;

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_type(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_int * it = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).begin(*facet);
          viennagrid_int * boundary_end = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).end(*facet);
          for (; it != boundary_end; ++it)
          {
            set_boundary(boundary_topo_dim, *it);
          }
        }
      }

      set_boundary(facet_topo_dim, *facet);
    }
  }

  set_boundary_flags_uptodate();
}






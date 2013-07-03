#include <iostream>

#include <vector>
#include <deque>


using std::cout;
using std::endl;


#include "viennagrid/storage/collection.hpp"


int main()
{
    typedef viennagrid::meta::make_typemap<int, int, float, float, std::vector<int>, std::vector<int> >::type config;

    typedef viennagrid::storage::result_of::collection<config>::type my_collection_type;
    my_collection_type my_collection;

    // accessing the objects stored in the collection
    int & the_int = viennagrid::storage::collection::get<int>( my_collection );
    float & the_float = viennagrid::storage::collection::get<float>( my_collection );
    viennagrid::storage::collection::get< std::vector<int> >( my_collection ).resize(10);

    
    return 0;
}

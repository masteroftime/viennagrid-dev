#ifndef VIENNAGRID_STORAGE_VIEW_HPP
#define VIENNAGRID_STORAGE_VIEW_HPP

#include <iterator>
#include <algorithm>

#include "viennagrid/meta/typemap.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/hook.hpp"
#include "viennagrid/storage/id.hpp"

namespace viennagrid
{
    
    namespace storage
    {
        
        template<typename base_container_type__, typename hook_tag__, typename container_tag>
        class view_t
        {
        public:
            
            typedef base_container_type__ base_container_type;
            
            typedef hook_tag__ hook_tag;
            typedef typename hook::hook_type<base_container_type, hook_tag>::type hook_type;
            typedef typename hook::const_hook_type<base_container_type, hook_tag>::type const_hook_type;
            
            
        private:
            typedef typename viennagrid::storage::result_of::container<hook_type, container_tag>::type hook_container_type;
            
            
        public:
            typedef typename hook_container_type::size_type size_type;
            typedef typename base_container_type::value_type value_type;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;
            
            
            
            class iterator : public hook_container_type::iterator
            {
                typedef typename hook_container_type::iterator base;
            public:
                iterator(const base & foo) : base(foo) {}
                iterator(const iterator & it) : base(it) {}
                
                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;
                
                reference operator* () { return *(base::operator*()); }
                const reference operator* () const { return *(base::operator*()); }
                
                pointer operator->() const { return &(operator* ()); }
            };
            
            
            class const_iterator : public hook_container_type::const_iterator
            {
                typedef typename hook_container_type::const_iterator base;
            public:
                const_iterator(const base & foo) : base(foo) {}
                const_iterator(const const_iterator & it) : base(it) {}
                const_iterator(const iterator & it) : base(it) {}
                
                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;
                
                reference operator* () { return *(base::operator*()); }
                const reference operator* () const { return *(base::operator*()); }
                
                pointer operator->() const { return &(operator* ()); }
            };
            
            
            
            typedef typename hook_container_type::iterator hook_iterator;
            typedef typename hook_container_type::const_iterator const_hook_iterator;
            

            
            view_t() {}
            void set_base_container( base_container_type & base_container ) {}
            void set_base_container( view_t<base_container_type, hook_tag, container_tag> & base_view ) {}

            
            
            iterator begin() { return iterator(hook_container.begin()); }
            iterator end() { return iterator(hook_container.end()); }

            const_iterator begin() const { return const_iterator(hook_container.begin()); }
            const_iterator end() const { return const_iterator(hook_container.end()); }

            
            
            
            hook_iterator hook_begin() { return hook_container.begin(); }
            hook_iterator hook_end() { return hook_container.end(); }
            
            const_hook_iterator hook_begin() const { return hook_container.begin(); }
            const_hook_iterator hook_end() const { return hook_container.end(); }
            
            reference dereference_hook( hook_type hook ) { return *hook; }
            const_reference dereference_hook( hook_type hook ) const { return *hook; }

            
            
            
            
            
            reference front() { return *(hook_container.front()); }
            const_reference front() const { return *(hook_container.front()); }
            
            reference back() { return *(hook_container.back()); }
            const_reference back() const { return *(hook_container.back()); }
            
            reference operator[]( size_type index ) { return *(hook_container[index]); }
            const_reference operator[]( size_type index ) const { return *(hook_container[index]); }
            
            
            
            
            
            
            
            
            
            size_type size() const { return hook_container.size(); }
            bool empty() const { return hook_container.empty(); }
            void clear() { hook_container.clear(); }
            
            
            void insert_hook(hook_type hook)
            {
                hook_container.insert( hook );
            }
            
            void set_hook( hook_type element, size_type pos )
            {
                hook_container[pos] = element;                
            }
            
            hook_type hook_at(std::size_t pos)
            {
                hook_iterator it = hook_begin();
                std::advance( it, pos );
                return *it;
            }
            
            const_hook_type hook_at(std::size_t pos) const
            {
                hook_iterator it = hook_begin();
                std::advance( it, pos );
                return *it;
            }

            
            
        private:
            
            hook_container_type hook_container;
        };
        
        
        
        
        template<typename base_container_type__, typename id_type, typename container_tag>
        class view_t< base_container_type__, id_hook_tag<id_type>, container_tag >
        {
            
        public:
            
            typedef base_container_type__ base_container_type;
            
            typedef id_hook_tag<id_type> hook_tag;
            typedef typename hook::hook_type<base_container_type, hook_tag>::type hook_type;
            typedef typename hook::const_hook_type<base_container_type, hook_tag>::type const_hook_type;
            
        private:
            //typedef typename viennagrid::storage::result_of::container_from_tag<hook_type, hook_container_tag>::type hook_container_type;
            typedef typename viennagrid::storage::result_of::container<hook_type, container_tag>::type hook_container_type;
            
            
        public:
            typedef typename hook_container_type::size_type size_type;
            typedef typename base_container_type::value_type value_type;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;
            
            
            class const_iterator;
            
            class iterator : public hook_container_type::iterator
            {
                typedef typename hook_container_type::iterator base;
                friend class const_iterator;
            public:
                iterator(view_t & view__, const base & foo) : base(foo), view(view__) {}
                iterator(const iterator & it) : base(it), view(it.view) {}
                
                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::reference reference;
                typedef view_t::pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;
                
                reference operator* () { return view.dereference_hook( base::operator*() ); }
                const reference operator* () const { return view.dereference_hook( base::operator*() ); }
                
                pointer operator->() const { return &view.dereference_hook( base::operator*() ); }
                
            private:
                view_t & view;
            };
            
            
            class const_iterator : public hook_container_type::const_iterator
            {
                typedef typename hook_container_type::const_iterator base;
            public:
                const_iterator(const view_t & view__, const base & foo) : base(foo), view(view__) {}
                const_iterator(const const_iterator & it) : base(it), view(it.view) {}
                const_iterator(const iterator & it) : base(it), view(it.view) {}
                
                typedef typename std::iterator_traits<base>::difference_type difference_type;
                typedef view_t::value_type value_type;
                typedef view_t::const_reference reference;
                typedef view_t::const_pointer pointer;
                typedef typename std::iterator_traits<base>::iterator_category iterator_category;
                
                reference operator* () { return view.dereference_hook( base::operator*() ); }
                const reference operator* () const { return view.dereference_hook( base::operator*() ); }
                
                pointer operator->() const { return &view.dereference_hook( base::operator*() ); }
                
            private:
                const view_t & view;
            };
            
            
            

            
            
            view_t() : base_container(0) {}
            
            void set_base_container( base_container_type & base_container_ )
            {
                base_container = &base_container_;
            }
            
            void set_base_container( view_t<base_container_type, hook_tag, container_tag> & base_view )
            {
                base_container = base_view.base_container;
            }

            
            
            iterator begin() { return iterator(*this, hook_container.begin()); }
            iterator end() { return iterator(*this, hook_container.end()); }

            const_iterator begin() const { return const_iterator(*this, hook_container.begin()); }
            const_iterator end() const { return const_iterator(*this, hook_container.end()); }

            
            
            
            typedef typename hook_container_type::iterator hook_iterator;
            typedef typename hook_container_type::const_iterator const_hook_iterator;
            
            hook_iterator hook_begin() { return hook_container.begin(); }
            hook_iterator hook_end() { return hook_container.end(); }
            
            const_hook_iterator hook_begin() const { return hook_container.begin(); }
            const_hook_iterator hook_end() const { return hook_container.end(); }
            
            reference dereference_hook( hook_type hook )
            {
                return *std::find_if(
                    base_container->begin(),
                    base_container->end(),
                    viennagrid::storage::id_compare<id_type>(hook)
                );
            }
            
            const_reference dereference_hook( hook_type hook ) const
            {
                return *std::find_if(
                    base_container->begin(),
                    base_container->end(),
                    viennagrid::storage::id_compare<id_type>(hook)
                );
            }
            
            
            
            
            
            reference front() { return dereference_hook( hook_container.front() ); }
            const_reference front() const { return dereference_hook( hook_container.front() ); }
            
            reference back() { return dereference_hook( hook_container.back() ); }
            const_reference back() const { return dereference_hook( hook_container.back() ); }
            
            reference operator[]( size_type index ) { return dereference_hook(hook_container[index]); }
            const_reference operator[]( size_type index ) const { return dereference_hook(hook_container[index]); }
            
            
            
            size_type size() const { return hook_container.size(); }
            bool empty() const { return hook_container.empty(); }
            void clear() { hook_container.clear(); }
            
            
            void insert_hook(hook_type hook)
            {
                hook_container.insert( hook );
            }
            
            void set_hook( hook_type element, size_type pos )
            {
                hook_container[pos] = element;                
            }
            
            hook_type hook_at(std::size_t pos)
            {
                hook_iterator it = hook_begin();
                std::advance( it, pos );
                return *it;
            }
            
            const_hook_type hook_at(std::size_t pos) const
            {
                hook_iterator it = hook_begin();
                std::advance( it, pos );
                return *it;
            }


            
            
        private:
            
            hook_container_type hook_container;
            base_container_type * base_container;
            
        };
        
        
        
        
        
        namespace result_of
        {
            template<typename base_container_type, typename view_container_tag>
            struct view
            {
                typedef view_t<base_container_type, typename base_container_type::hook_tag, view_container_tag> type;
            };
            
            template<typename base_container_type, typename base_view_hook_tag, typename base_view_container_tag, typename view_container_tag>
            struct view< view_t<base_container_type, base_view_hook_tag, base_view_container_tag>, view_container_tag>
            {
                typedef view_t<base_container_type, typename base_container_type::hook_tag, view_container_tag> type;
            };
            
            template<typename base_container_type, typename view_container_config>
            typename view<base_container_type, view_container_config>::type view_of( base_container_type & base_container )
            {
                typename view<base_container_type, view_container_config>::type tmp;
                tmp.set_base_container(base_container);
                return tmp;
            }
        }

        
        
        

        
        namespace view
        {
            //typedef viennameta::make_typemap<viennagrid::storage::default_tag, viennagrid::storage::std_deque_tag<> >::type default_view_container_config;
            typedef viennameta::make_typemap<
                        viennagrid::storage::default_tag,   viennagrid::storage::hooked_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>
                    >::type default_view_container_config;
            
            

            
            template<typename base_container_type, typename view_base_container_type, typename hook_tag, typename hook_container_tag, typename predicate>
            void hook_if(base_container_type & src_container, view_t<view_base_container_type, hook_tag, hook_container_tag> & dst_view, predicate pred)
            {
                for (typename base_container_type::hook_iterator it = src_container.hook_begin(); it != src_container.hook_end(); ++it)
                    if (pred(*it))
                        dst_view.insert_hook( *it );
            }
            
            template<typename base_container_type, typename view_base_container_type, typename hook_tag, typename view_container_tag>
            void hook(base_container_type & src_container, view_t<view_base_container_type, hook_tag, view_container_tag> & dst_view)
            {
                hook_if(src_container, dst_view, viennameta::true_predicate());
            }
            
            
            
            
            
            
            
            template<typename container_type, typename view_container_config>
            struct view_container_tag
            {
                typedef typename container_type::value_type value_type;
                typedef typename viennameta::typemap::result_of::find<view_container_config, value_type>::type search_result;
                typedef typename viennameta::typemap::result_of::find<view_container_config, viennagrid::storage::default_tag>::type default_container;
                
                typedef typename viennameta::_if<
                    !viennameta::_equal<search_result, viennameta::not_found>::value,
                    search_result,
                    default_container
                >::type::second type;
            };
        
            

            
            
            template<typename container_typelist, typename view_container_config>
            struct view_container_typelist {};
            
            template<typename view_container_config>
            struct view_container_typelist<viennameta::null_type, view_container_config>
            {
                typedef viennameta::null_type type;
            };
            
            template<typename head, typename tail, typename view_container_config>
            struct view_container_typelist<viennameta::typelist_t<head, tail>, view_container_config>
            {
                typedef viennameta::typelist_t<
                    typename viennagrid::storage::result_of::view<
                        head,
                        typename view_container_tag<head, view_container_config>::type
                        >::type,
                    
                    typename view_container_typelist<tail, view_container_config>::type
                > type;
            };
        }
        

        namespace result_of
        {
            
            template<typename container_typelist, typename view_container_config>
            struct view_collection
            {
                typedef typename viennagrid::storage::result_of::collection<
                    typename viennagrid::storage::view::view_container_typelist<
                        container_typelist,
                        view_container_config
                    >::type
                >::type type;
            };
            
            template<typename container_typelist, typename view_container_config>
            struct view_collection< collection_t<container_typelist>, view_container_config>
            {
                typedef viennagrid::storage::collection_t<container_typelist> container_collection_type;
                
                typedef typename viennagrid::storage::result_of::collection<
                    typename viennagrid::storage::view::view_container_typelist<
                        //typename viennagrid::storage::container_collection::result_of::container_typelist<container_collection_type>::type,
                        container_typelist,
                        view_container_config
                    >::type
                >::type type;
            };
            
        }


    }
    
}

#endif
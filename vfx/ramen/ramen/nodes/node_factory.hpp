// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_FACTORY_HPP
#define RAMEN_NODE_FACTORY_HPP

#include<list>
#include<map>
#include<memory>

#include<boost/noncopyable.hpp>
#include<loki/Singleton.h>

#include<ramen/nodes/node_info.hpp>

namespace ramen
{

class node_factory_impl : boost::noncopyable
{
public:

    bool register_node( const node_info_t& i);

    std::auto_ptr<node_t> create_by_id( const std::string& id, bool ui = false);
    std::auto_ptr<node_t> create_by_name( const std::string& name);

    typedef std::map<std::string, node_info_t>::const_iterator	const_iterator;
    typedef std::map<std::string, node_info_t>::iterator	iterator;
	
    const_iterator begin() const    { return node_infos_.begin();}
    const_iterator end() const	    { return node_infos_.end();}

    iterator begin()	{ return node_infos_.begin();}
    iterator end()	{ return node_infos_.end();}

    const std::map<std::string, node_info_t>& entries() const { return node_infos_;}
	
private:

    friend struct Loki::CreateUsingNew<node_factory_impl>;

    node_factory_impl();
    ~node_factory_impl();

    std::map<std::string, node_info_t> node_infos_;
};

typedef Loki::SingletonHolder<node_factory_impl> node_factory_t;

}

#endif

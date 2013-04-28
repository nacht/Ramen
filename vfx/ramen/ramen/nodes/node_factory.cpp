// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node_factory.hpp>
#include<ramen/nodes/node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ofx/plugin_manager.hpp>
#include<ramen/ofx/ofx_node.hpp>

namespace ramen
{

node_factory_impl::node_factory_impl()  {}

node_factory_impl::~node_factory_impl()
{
    for( std::map<std::string, node_info_t>::iterator it( begin()); it != end(); ++it)
    {
	if( !it->second.first_time && it->second.cleanup)
		it->second.cleanup();
    }
}

bool node_factory_impl::register_node( const node_info_t& i)
{
    std::map<std::string, node_info_t>::iterator it( node_infos_.find( i.id));

    if( it != end())
        assert( 0 && "duplicate id in node factory" && i.id.c_str());

    node_infos_[i.id] = i;
    return true;
}

std::auto_ptr<node_t> node_factory_impl::create_by_id( const std::string& id, bool ui)
{
    std::map<std::string, node_info_t>::iterator it( node_infos_.find( id));

    if( it == end())
	return application_t::Instance().ofx_plugin_manager().create_node_by_id( id);

    if( it->second.first_time && it->second.init)
    {
	it->second.init();
	it->second.first_time = false;
    }

    std::auto_ptr<node_t> n;

    if( it->second.create_gui && ui)
	n.reset( it->second.create_gui());
    else
	n.reset( it->second.create());

    return n;
}

std::auto_ptr<node_t> node_factory_impl::create_by_name( const std::string& name)
{
    std::map<std::string, node_info_t>::iterator it( begin());

    for( ; it != end(); ++it)
    {
	if( it->second.menu_item == name)
	    break;
    }

    if( it == end())
	return std::auto_ptr<node_t>();

    if( it->second.first_time && it->second.init)
    {
	it->second.init();
	it->second.first_time = false;
    }

    std::auto_ptr<node_t> n( it->second.create());
    return n;
}

} // namespace

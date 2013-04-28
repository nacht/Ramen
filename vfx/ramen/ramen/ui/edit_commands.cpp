// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/edit_commands.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/dataflow/algorithm.hpp>

namespace ramen
{
namespace undo
{

delete_command_t::delete_command_t() : command_t( "Delete") {}

void delete_command_t::add_node( node_t *n) { nodes.push_back( n);}

void delete_command_t::add_edge( const composition_t::edge_type& e)	{ edges.push_back( e);}

void delete_command_t::add_dependent_node( node_t *n) { dependents.insert( n);}

void delete_command_t::undo()
{
    while( !node_storage.empty())
    {
    	boost::ptr_vector<node_t>::auto_type ptr( node_storage.pop_back());
    	node_t *nptr = ptr.release();
	document_t::Instance().add_node( std::auto_ptr<node_t>( nptr));
    }

    adobe::for_each( edges, boost::bind( &document_impl::add_edge, &document_t::Instance(), _1, true));
    dataflow::breadth_first_multiple_outputs_search( dependents, boost::bind( &node_t::notify, _1));
    command_t::undo();
}

void delete_command_t::redo()
{
    for( std::vector<node_t*>::const_iterator it( nodes.begin()); it != nodes.end(); ++it)
    {
    	std::auto_ptr<node_t> ptr = document_t::Instance().release_node( *it);
	node_storage.push_back( ptr);
    }
	
    adobe::for_each( edges, boost::bind( &document_impl::remove_edge, &document_t::Instance(), _1, true));
    dataflow::breadth_first_multiple_outputs_search( dependents, boost::bind( &node_t::notify, _1));
    command_t::redo();
}

duplicate_command_t::duplicate_command_t() : command_t( "Duplicate") {}

void duplicate_command_t::add_node( node_t *n)
{
    node_storage.push_back( n);
    nodes.push_back( n);
}

void duplicate_command_t::add_edge( const composition_t::edge_type& e) { edges.push_back( e);}

void duplicate_command_t::undo()
{
    for( std::vector<node_t*>::const_iterator it( nodes.begin()); it != nodes.end(); ++it)
    {
    	std::auto_ptr<node_t> ptr = document_t::Instance().release_node( *it);
	node_storage.push_back( ptr);
    }

    adobe::for_each( edges, boost::bind( &document_impl::remove_edge, &document_t::Instance(), _1, true));
    command_t::undo();
}

void duplicate_command_t::redo()
{
    while( !node_storage.empty())
    {
    	boost::ptr_vector<node_t>::auto_type ptr( node_storage.pop_back());
    	node_t *nptr = ptr.release();
	document_t::Instance().add_node( std::auto_ptr<node_t>( nptr));
    }

    adobe::for_each( edges, boost::bind( &document_impl::add_edge, &document_t::Instance(), _1, true));
    command_t::redo();
}

ignore_nodes_command_t::ignore_nodes_command_t() : command_t( "Ignore") {}

void ignore_nodes_command_t::add_node( node_t *n) { nodes.push_back( n);}

void ignore_nodes_command_t::undo()
{
    for( std::vector<node_t*>::const_iterator it( nodes.begin()); it != nodes.end(); ++it)
        (*it)->set_ignored( !(*it)->ignored());

    dataflow::breadth_first_multiple_outputs_search( nodes, boost::bind( &node_t::notify, _1));
    command_t::undo();
}

void ignore_nodes_command_t::redo()
{
    for( std::vector<node_t*>::const_iterator it( nodes.begin()); it != nodes.end(); ++it)
        (*it)->set_ignored( !(*it)->ignored());

    dataflow::breadth_first_multiple_outputs_search( nodes, boost::bind( &node_t::notify, _1));
    command_t::redo();
}

} // namespace
} // namespace

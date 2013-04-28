// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/add_node_command.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace undo
{

add_node_command_t::add_node_command_t( std::auto_ptr<node_t> n, node_t *src) : command_t( "Add Node"), node_(n.release()), src_( src)
{
}

add_node_command_t::~add_node_command_t()
{
    if( !done())
        delete node_;
}

void add_node_command_t::undo()
{
    if( src_)
        document_t::Instance().composition().disconnect( src_, node_, 0);

    std::auto_ptr<node_t> p = document_t::Instance().release_node( node_);
    p.release();
    command_t::undo();
}

void add_node_command_t::redo()
{
    document_t::Instance().add_node( std::auto_ptr<node_t>( node_));

    if( src_)
        document_t::Instance().composition().connect( src_, node_, 0);

    command_t::redo();
}

add_nodes_command_t::add_nodes_command_t() : command_t( "Add Nodes") {}

void add_nodes_command_t::add_node( node_t *n) { node_storage.push_back( n);}

void add_nodes_command_t::undo()
{
    for( std::vector<node_t*>::const_iterator it( nodes.begin()); it != nodes.end(); ++it)
    {
    	std::auto_ptr<node_t> ptr = document_t::Instance().release_node( *it);
	node_storage.push_back( ptr);
    }

    command_t::undo();
}

void add_nodes_command_t::redo()
{
    while( !node_storage.empty())
    {
    	boost::ptr_vector<node_t>::auto_type ptr( node_storage.pop_back());
    	node_t *nptr = ptr.release();
	document_t::Instance().add_node( std::auto_ptr<node_t>( nptr));
    }

    command_t::redo();
}

} // namespace
} // namespace

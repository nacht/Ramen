// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EDIT_MENU_COMMANDS_HPP
#define RAMEN_EDIT_MENU_COMMANDS_HPP

#include<set>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/undo/command.hpp>
#include<ramen/app/document.hpp>

namespace ramen
{
namespace undo
{

struct delete_command_t : public command_t
{
    delete_command_t();
	
    void add_node( node_t *n);
    void add_edge( const composition_t::edge_type& e);
    void add_dependent_node( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes;
    std::vector<composition_t::edge_type> edges;
    boost::ptr_vector<node_t> node_storage;
    std::set<node_t*> dependents;
};

struct duplicate_command_t : public command_t
{
    duplicate_command_t();

    void add_node( node_t *n);
    void add_edge( const composition_t::edge_type& e);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes;
    std::vector<composition_t::edge_type> edges;
    boost::ptr_vector<node_t> node_storage;
};

struct ignore_nodes_command_t : public command_t
{
    ignore_nodes_command_t();

    void add_node( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes;
};

} // namespace
} // namespace

#endif

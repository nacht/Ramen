// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UNDO_ADD_NODE_COMMAND_HPP
#define	RAMEN_UNDO_ADD_NODE_COMMAND_HPP

#include<memory>
#include<set>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/undo/command.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{
namespace undo
{

class add_node_command_t : public command_t
{
public:

    add_node_command_t( std::auto_ptr<node_t> n, node_t *src = 0);
    virtual ~add_node_command_t();
  
    virtual void undo();
    virtual void redo();

private:

    node_t *node_;
    node_t *src_;
};

class add_nodes_command_t : public command_t
{
public:

    add_nodes_command_t();

    void add_node( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes;
    boost::ptr_vector<node_t> node_storage;
};

} // namespace
} // namespace

#endif

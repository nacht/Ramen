// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/reset_node_command.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{
namespace undo
{

reset_node_command_t::reset_node_command_t( node_t *n) : composite_command_t( "Reset Node")
{
    assert( n && "Null node in reset node command");
    n_ = n;
}

void reset_node_command_t::undo()
{
    composite_command_t::undo();

    n_->notify();
    n_->notify_dependents();
}

void reset_node_command_t::redo()
{
    composite_command_t::redo();

    n_->notify();
    n_->notify_dependents();
}
	
} // namespace
} // namespace

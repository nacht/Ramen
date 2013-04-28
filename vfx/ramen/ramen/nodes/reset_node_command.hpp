// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RESET_NODE_COMMAND_HPP
#define RAMEN_RESET_NODE_COMMAND_HPP

#include<ramen/undo/command.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{
namespace undo
{

class reset_node_command_t : public composite_command_t
{
public:

    reset_node_command_t( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    node_t *n_;
};

} // namespace
} // namespace

#endif

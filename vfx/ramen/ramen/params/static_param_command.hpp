// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_STATIC_PARAM_COMMAND_HPP
#define RAMEN_STATIC_PARAM_COMMAND_HPP

#include<ramen/undo/command.hpp>

#include<adobe/any_regular.hpp>

#include<ramen/params/param_fwd.hpp>
#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

class static_param_command_t : public undo::command_t
{
public:

    static_param_command_t( param_t *p);

    virtual void undo();
    virtual void redo();

private:

    param_t *p_;
    adobe::any_regular_t old_value_, new_value_;
};

class static_param_reset_command_t : public undo::command_t
{
public:

    static_param_reset_command_t( param_t *p);

    virtual void undo();
    virtual void redo();

private:

    param_t *p_;
    adobe::any_regular_t old_value_;
};

} // namespace

#endif

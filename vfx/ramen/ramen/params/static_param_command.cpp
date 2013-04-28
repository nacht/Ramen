// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/static_param_command.hpp>

#include<ramen/params/static_param.hpp>
#include<ramen/nodes/node.hpp>

namespace ramen
{

static_param_command_t::static_param_command_t( param_t *p) : undo::command_t( "Param Changed"), p_( p)
{
    set_done( true);
    old_value_ = p_->value();
}

void static_param_command_t::undo()
{
    new_value_ = p_->value();
    p_->value() = old_value_;
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::undo();
}
	
void static_param_command_t::redo()
{
    p_->value() = new_value_;
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::redo();
}

static_param_reset_command_t::static_param_reset_command_t( param_t *p) : undo::command_t( "Param Reset"), p_( p)
{
    old_value_ = p_->value();
}

void static_param_reset_command_t::undo()
{
    p_->value() = old_value_;
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::undo();
}

void static_param_reset_command_t::redo()
{
    p_->value() = p_->default_value();
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::redo();
}

} // namespace

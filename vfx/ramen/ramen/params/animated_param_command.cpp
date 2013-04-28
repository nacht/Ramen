// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/animated_param_command.hpp>

#include<boost/foreach.hpp>

#include<ramen/params/animated_param.hpp>
#include<ramen/nodes/node.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

animated_param_command_t::animated_param_command_t( animated_param_t *p) : undo::command_t( "Param Changed"), p_(p)
{
    set_done( true);
    previous_value_ = p_->value();

    BOOST_FOREACH( anim::curve_t& c, p_->curves())
    {
	old_.push_back( c);
	new_.push_back( &c);
    }
}

void animated_param_command_t::swap_curves()
{
    using std::swap;

    for( int i = 0; i < old_.size(); ++i)
	swap( old_[i], *new_[i]);
}

void animated_param_command_t::undo()
{
    new_value_ = p_->value();
    swap_curves();
    p_->value() =  previous_value_;
    p_->evaluate( document_t::Instance().composition().frame());
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::undo();
}

void animated_param_command_t::redo()
{
    assert( !adobe::empty( new_value_) && "Empty any_regular_t in animated param command");
    
    p_->value() =  new_value_;
    swap_curves();
    p_->evaluate( document_t::Instance().composition().frame());
    p_->update_widgets();
    p_->param_set()->param_changed( p_);
    undo::command_t::redo();
}

animated_param_reset_command_t::animated_param_reset_command_t( animated_param_t *p) : undo::command_t( "Param Changed")
{
    // TODO: implement
}

void animated_param_reset_command_t::undo()
{
    // TODO: implement
}

void animated_param_reset_command_t::redo()
{
    // TODO: implement
}

} // namespace

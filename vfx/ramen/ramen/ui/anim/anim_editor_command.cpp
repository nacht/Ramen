// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor_command.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{
namespace undo
{

anim_editor_command_t::anim_editor_command_t() : command_t( "Edit Curves")
{
    n_ = ui::user_interface_t::Instance().active_node();
    assert(n_);
    
    track_tree_ = ui::anim_editor_t::Instance().tracks_for_node( n_);
    assert( track_tree_);

    set_done( true);
}

void anim_editor_command_t::add_track( anim::track_t *t)
{
    old_curves_.push_back( *(t->curve()));
    tracks_.push_back( t);
}

void anim_editor_command_t::undo()
{
    swap_curves();
    adobe::for_each( tracks_, boost::bind( &anim::track_t::notify, _1));

    n_->notify();
    n_->notify_dependents();
    command_t::undo();
}

void anim_editor_command_t::redo()
{
    swap_curves();
    adobe::for_each( tracks_, boost::bind( &anim::track_t::notify, _1));

    n_->notify();
    n_->notify_dependents();
    command_t::redo();
}

void anim_editor_command_t::swap_curves()
{
    using std::swap;

    for( int i = 0; i < old_curves_.size(); ++i)
	swap( old_curves_[i], *(tracks_[i]->curve()));
}

} // namespace
} // namespace

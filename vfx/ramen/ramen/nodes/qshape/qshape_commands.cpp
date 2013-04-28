// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_commands.hpp>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/nodes/qshape/shape.hpp>
#include<ramen/nodes/qshape/qshape_toolbar.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/viewer.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{
namespace undo
{

add_qshape_command_t::add_qshape_command_t( qshape_node_t *node, qshape::shape_t *shape) : command_t( "Add Shape")
{
    node_ = node;
    shape_ = shape;
}

add_qshape_command_t::~add_qshape_command_t()
{
    if( !done())
	delete shape_;
}

void add_qshape_command_t::undo()
{
    node_->deselect_all();
    node_->pop_back_shape();
    node_->notify();
    node_->notify_dependents();
    ui::anim_editor_t::Instance().recreate_tracks( node_);
    ui::anim_editor_t::Instance().update();
    ui::anim_editor_t::Instance().update_tree();
    undo::command_t::undo();
}

void add_qshape_command_t::redo()
{
    node_->add_shape( shape_);
    node_->select( shape_);
    node_->notify();
    node_->notify_dependents();
    ui::anim_editor_t::Instance().recreate_tracks( node_);
    ui::anim_editor_t::Instance().update();
    ui::anim_editor_t::Instance().update_tree();
    undo::command_t::redo();
}

modify_qshape_command_t::modify_qshape_command_t( qshape_node_t *node, bool recreate_curves) : command_t( "Modify Shape")
{
    node_ = node;
    shape_ = node_->selected();
    new_ = &( shape_->triples());
    old_ = *new_;
    recreate_curves_ = recreate_curves;
    set_done( true);
}

void modify_qshape_command_t::undo()
{
    swap_control_points();
    shape_->evaluate( document_t::Instance().composition().frame());

    if( recreate_curves_)
    {
	ui::anim_editor_t::Instance().recreate_tracks( node_);
	ui::anim_editor_t::Instance().update();
	ui::anim_editor_t::Instance().update_tree();
    }

    if( shape_->active())
    {
	node_->notify();
	node_->notify_dependents();
    }
    else
	ui::viewer_t::Instance().image_view()->update();

    undo::command_t::undo();
}

void modify_qshape_command_t::redo()
{
    swap_control_points();
    shape_->evaluate( document_t::Instance().composition().frame());

    if( recreate_curves_)
    {
	ui::anim_editor_t::Instance().recreate_tracks( node_);
	ui::anim_editor_t::Instance().update();
	ui::anim_editor_t::Instance().update_tree();
    }

    if( shape_->active())
    {
	node_->notify();
	node_->notify_dependents();
    }
    else
	ui::viewer_t::Instance().image_view()->update();
    
    undo::command_t::redo();
}

void modify_qshape_command_t::swap_control_points()
{
    using std::swap;

    // I'm not really happy with this.
    if( new_->size() == old_.size())
    {
	for( int i = 0; i < old_.size(); ++i)
	    swap( (*new_)[i], old_[i]);
    }
    else
        new_->swap( old_);
}

qshape_toggle_active_command_t::qshape_toggle_active_command_t( qshape_node_t *node) : command_t( "Toggle Active")
{
    node_ = node;
    shape_ = node_->selected();
    prev_state_ = shape_->active();
}

void qshape_toggle_active_command_t::undo()
{
    prev_state_ = !prev_state_;
    shape_->set_active( prev_state_);
    node_->notify();
    node_->notify_dependents();
    node_->toolbar().update_widgets();
    undo::command_t::undo();
}

void qshape_toggle_active_command_t::redo()
{
    prev_state_ = !prev_state_;
    shape_->set_active( prev_state_);
    node_->notify();
    node_->notify_dependents();
    node_->toolbar().update_widgets();
    undo::command_t::redo();
}

qshape_order_command_t::qshape_order_command_t( qshape_node_t *node, bool up) : command_t( "Change shape layer order")
{
    node_ = node;
    move_up_ = up;
    index_ = node_->index_for_shape( node_->selected());
}

void qshape_order_command_t::undo()
{
    if( move_up_)
    {
	qshape::shape_t *upper = node_->release_shape( index_ + 1);
	qshape::shape_t *lower = node_->release_shape( index_);

	node_->insert_shape( upper, index_);
	node_->insert_shape( lower, index_ + 1);
    }
    else
    {
	qshape::shape_t *upper = node_->release_shape( index_);
	qshape::shape_t *lower = node_->release_shape( index_ - 1);

	node_->insert_shape( upper, index_ - 1);
	node_->insert_shape( lower, index_);
    }

    node_->notify();
    node_->notify_dependents();
    node_->toolbar().update_widgets();
    undo::command_t::undo();
}

void qshape_order_command_t::redo()
{
    if( move_up_)
    {
	qshape::shape_t *upper = node_->release_shape( index_ + 1);
	qshape::shape_t *lower = node_->release_shape( index_);

	node_->insert_shape( upper, index_);
	node_->insert_shape( lower, index_ + 1);
    }
    else
    {
	qshape::shape_t *upper = node_->release_shape( index_);
	qshape::shape_t *lower = node_->release_shape( index_ - 1);

	node_->insert_shape( upper, index_ - 1);
	node_->insert_shape( lower, index_);
    }

    node_->notify();
    node_->notify_dependents();
    node_->toolbar().update_widgets();
    undo::command_t::redo();
}

qshape_delete_command_t::qshape_delete_command_t( qshape_node_t *node, qshape::shape_t *shape) : command_t( "Delete shape")
{
    node_ = node;
    shape_ = shape;
    index_ = node_->index_for_shape( shape_);

    assert( index_ >= 0);
}

qshape_delete_command_t::~qshape_delete_command_t()
{
    if( done())
	delete shape_;
}

void qshape_delete_command_t::undo()
{
    node_->insert_shape( shape_, index_);

    node_->notify();
    node_->notify_dependents();
    ui::anim_editor_t::Instance().recreate_tracks( node_);
    ui::anim_editor_t::Instance().update();
    ui::anim_editor_t::Instance().update_tree();
    undo::command_t::undo();
}

void qshape_delete_command_t::redo()
{
    shape_ = node_->release_shape( index_);

    node_->notify();
    node_->notify_dependents();
    ui::anim_editor_t::Instance().recreate_tracks( node_);
    ui::anim_editor_t::Instance().update();
    ui::anim_editor_t::Instance().update_tree();
    undo::command_t::redo();
}

} // namespace
} // namespace

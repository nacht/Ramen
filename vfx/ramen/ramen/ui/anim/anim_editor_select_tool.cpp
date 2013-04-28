// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor_select_tool.hpp>

#include<boost/foreach.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathVec.h>

#include<QKeyEvent>
#include<QMouseEvent>
#include<QPainter>

#include<ramen/nodes/node.hpp>

#include<ramen/anim/keyframe.hpp>
#include<ramen/anim/track.hpp>
#include<ramen/anim/curve_algorithm.hpp>

#include<ramen/undo/undo_stack.hpp>

#include<ramen/ui/anim/anim_curves_view.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/draw_pick_curves.hpp>
#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace ui
{

anim_editor_select_tool_t::anim_editor_select_tool_t()
{
    box_pick_mode_ = false;
}

void anim_editor_select_tool_t::draw_overlay( anim_curves_view_t& view) const
{
    if( box_pick_mode_)
    {
	QPen pen;
	pen.setColor( QColor( 255, 0, 0, 255));
	view.painter()->setPen( pen);
	view.painter()->drawLine( push_x_, push_y_, last_x_, push_y_);
	view.painter()->drawLine( last_x_, push_y_, last_x_, last_y_);
	view.painter()->drawLine( last_x_, last_y_, push_x_, last_y_);
	view.painter()->drawLine( push_x_, last_y_, push_x_, push_y_);
    }
}

void anim_editor_select_tool_t::key_press_event( anim_curves_view_t& view, QKeyEvent *event)
{ 
    event->ignore();
}

void anim_editor_select_tool_t::key_release_event( anim_curves_view_t& view, QKeyEvent *event)
{ 
    event->ignore();
}

void anim_editor_select_tool_t::mouse_press_event( anim_curves_view_t& view, QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();

    dragged_ = false;
    box_pick_mode_ = false;
    curve_ = 0;
    key_ = 0;
    drag_tangents_mode_ = false;
    left_ = false;
    drag_keyframes_mode_ = false;
    insert_keyframe_ = false;

    Imath::V2i p( push_x_, push_y_);

    key_ = pick_tangent( view, p, left_, curve_);
    if( key_)
    {
	drag_tangents_mode_ = true;
	key_iter_ = curve_->iterator_from_keyframe_ptr( key_);
	event->accept();
	return;
    }

    // test select point here
    key_ = pick_keyframe( view, p);

    if( !key_ && ( event->modifiers() & Qt::ControlModifier))
    {
	BOOST_FOREACH( anim::curve_t *c, anim_editor_t::Instance().curves())
	{
	    if( pick_curve( view, *c, Imath::V2i( push_x_, push_y_)))
	    {
		insert_keyframe_ = true;
		command_ = anim_editor_t::Instance().create_command();
		Imath::V2f pw( view.screen_to_world( Imath::V2i( push_x_, push_y_)));
		float time = pw.x;
		key_ = &( *c->insert( time));
		key_->select( true);
		view.update();
		break;
	    }
	}
    }

    if( key_)
    {
	if( !(event->modifiers() & Qt::ShiftModifier))
	{
	    if( !key_->selected())
	    {
		anim_editor_t::Instance().deselect_all_keyframes();
		key_->select( true);
	    }

	    drag_keyframes_mode_ = true;
	}
	else
	{
	    key_->toggle_selection();

	    if( key_->selected())
		drag_keyframes_mode_ = true;
	}
	
	view.update();
	event->accept();
	return;
    }

    box_pick_mode_ = true;

    if( !( event->modifiers() & Qt::ShiftModifier))
	anim_editor_t::Instance().deselect_all_keyframes();

    event->accept();
}

void anim_editor_select_tool_t::mouse_move_event( anim_curves_view_t& view, QMouseEvent *event) { event->accept();}

void anim_editor_select_tool_t::mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event)
{
    if( !dragged_)
    {
	// first time dragged init
	if( drag_tangents_mode_)
	{
	    assert( !command_.get());

	    // find the keyframe index
	    std::size_t indx = std::distance( curve_->begin(), key_iter_);
	    command_ = anim_editor_t::Instance().create_command( curve_);

	    // command copies the curve, so we loose the pointer and iter
	    // find the new ones
	    key_iter_ = curve_->begin() + indx;
	    key_ = &(*key_iter_);
	}
	else
	{
	    if( drag_keyframes_mode_)
	    {
		if( !insert_keyframe_) // command is already created
		    command_ = anim_editor_t::Instance().create_command();
	    }
	}

	dragged_ = true;
    }

    if( drag_tangents_mode_)
    {
	if( left_)
	    drag_left_tangent( view, event);
	else
	    drag_right_tangent( view, event);

	// recalc coeffs
	if( key_iter_ != curve_->begin())
	    (key_iter_-1)->calc_cubic_coefficients( *key_iter_);

	if( key_iter_ + 1 != curve_->end())
	    key_iter_->calc_cubic_coefficients( *(key_iter_ + 1));

	command_->track()->notify();
	user_interface_t::Instance().active_node()->notify();
	user_interface_t::Instance().active_node()->notify_dependents();
    }
    else
    {
	if( drag_keyframes_mode_)
	{
	    Imath::V2f d( ( event->x() - last_x_) / view.time_scale(),
			 -( event->y() - last_y_) / view.value_scale());

	    BOOST_FOREACH( anim::track_t *t, command_->tracks())
	    {
		if( t->curve()->any_keyframe_selected())
		{
		    anim::move_selected_keyframes( *(t->curve()), d);
		    t->notify();
		}
	    }

	    user_interface_t::Instance().active_node()->notify();
	    user_interface_t::Instance().active_node()->notify_dependents();
	}
    }

    last_x_ = event->x();
    last_y_ = event->y();
    event->accept();
    view.update();
}

void anim_editor_select_tool_t::mouse_release_event( anim_curves_view_t& view, QMouseEvent *event)
{
    if( drag_tangents_mode_)
    {
	if( command_.get())
	{
	    if( command_.get())
	    {
		undo::undo_stack_t::Instance().push_back( command_);
		user_interface_t::Instance().update();
	    }

	    drag_tangents_mode_ = false;
	}
    }
    else
    {
	if( drag_keyframes_mode_)
	{
	    if( command_.get())
	    {
		undo::undo_stack_t::Instance().push_back( command_);
		user_interface_t::Instance().update();
	    }

	    drag_keyframes_mode_ = false;
	}
	else
	{
	    if( box_pick_mode_)
	    {
		box_pick_mode_ = false;

		Imath::Box2i area;
                area.extendBy( Imath::V2i( push_x_, push_y_));
                area.extendBy( Imath::V2i( last_x_, last_y_));

		BOOST_FOREACH( anim::curve_t *c, anim_editor_t::Instance().curves())
		{
		    pick_keyframes( view, *c, area);
		}
	    }
	}
    }

    insert_keyframe_ = false;
    event->accept();
    view.update();
}

void anim_editor_select_tool_t::drag_left_tangent( anim_curves_view_t& view, QMouseEvent *event)
{
    Imath::V2i  p( view.world_to_screen( Imath::V2f( key_->time(), key_->value())));
    Imath::V2f  q( event->x(), event->y());
    float slope;

    if( q.x >= p.x)
    {
	if( q.y >= p.y)
	    slope = anim::keyframe_t::max_slope();
	else
	    slope = anim::keyframe_t::min_slope();
    }
    else
    {
	Imath::V2f v( q.x - p.x, q.y - p.y);
	v.x /= -view.time_scale();
	v.y /=  view.value_scale();
        v.normalize();
	slope = v.y / v.x;
    }

    key_->set_v0_tangent( slope);
}

void anim_editor_select_tool_t::drag_right_tangent( anim_curves_view_t& view, QMouseEvent *event)
{
    Imath::V2i  p( view.world_to_screen( Imath::V2f( key_->time(), key_->value())));
    Imath::V2f  q( event->x(), event->y());
    float slope;

    if( q.x <= p.x)
    {
	if( q.y >= p.y)
	    slope = anim::keyframe_t::min_slope();
	else
	    slope = anim::keyframe_t::max_slope();
    }
    else
    {
	Imath::V2f v( q.x - p.x, q.y - p.y);
	v.x /=   view.time_scale();
	v.y /=  -view.value_scale();
	v.normalize();
	slope = v.y / v.x;
    }

    key_->set_v1_tangent( slope);
}

} // namespace
} // namespace

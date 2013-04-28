// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_select_tool.hpp>

#include<vector>

#include<boost/foreach.hpp>
#include<boost/range/adaptor/reversed.hpp>

#include<QKeyEvent>
#include<QMouseEvent>

#include<ramen/GL/gl.hpp>

#include<ramen/IECoreExt/LineSegmentAlgo.hpp>

#include<ramen/bezier/curve.hpp>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/ui/image_view.hpp>

#include<ramen/nodes/qshape/shape.hpp>
#include<ramen/nodes/qshape/qshape_commands.hpp>

#include<ramen/app/document.hpp>
#include<ramen/undo/undo_stack.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{
namespace qshape
{

qshape_select_tool_t::qshape_select_tool_t( qshape_node_t *parent) : qshape_tool_t( parent), command_(0)
{
    assert( parent);

    dragged_ = false;
    box_pick_mode_ = false;
    drag_curve_mode_ = false;
    drag_points_mode_ = false;
    drag_tangents_mode_ = false;
    left_tangent_ = false;
    active_point_ = 0;
}

void qshape_select_tool_t::draw_overlay( ui::image_view_t& view) const
{
    if( box_pick_mode_)
    {
	glColor3f( 1, 0, 0);
	view.save_projection();
	view.set_screen_projection();
	view.draw_selection_box( push_x_, push_y_, last_x_, last_y_);
	view.restore_projection();
    }
}

bool qshape_select_tool_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)
{
    if( event->key() == Qt::Key_Backspace)
    {
	qshape::shape_t *s = parent()->selected();

	if( s)
	{
	    if( s->any_point_selected())
	    {
		// if the shape becomes invalid after deleting the points,
		// then remove it all.
		if( ( s->size() - s->num_selected()) < 3)
		    delete_shape();
		else
		    delete_points();
	    }
	    else
		delete_shape();
	}


	if( parent()->selected()->active())
	{
	    parent()->notify();
	    parent()->notify_dependents();
	}

	ui::anim_editor_t::Instance().update();
	ui::user_interface_t::Instance().update();
	return true;
    }
    else
	return false;
}

bool qshape_select_tool_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)
{
    if( event->key() == Qt::Key_Backspace)
	return true;
    else
	return false;
}

void qshape_select_tool_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();
    dragged_ = false;

    box_pick_mode_ = false;
    drag_curve_mode_ = false;
    drag_points_mode_ = false;
    drag_tangents_mode_ = false;

    left_tangent_ = false;

    if( parent()->selected())
    {
	if( event->modifiers() & Qt::ControlModifier)
	{
	    box_pick_mode_ = true;

	    if( !(event->modifiers() & Qt::ShiftModifier))
		parent()->selected()->deselect_all_points();

	    view.update();
            return;
	}

	// pick points
	Imath::V2i p( push_x_, push_y_);

	active_point_ = 0;
	active_point_ = pick_point( *(parent()->selected()), view, p);

	if( active_point_)
	{
	    if( !active_point_->selected())
	    {
		if( !(event->modifiers() & Qt::ShiftModifier))
		    parent()->selected()->deselect_all_points();

		active_point_->toggle_selection();
	    }

	    if( active_point_->selected())
		drag_points_mode_ = true;

	    view.update();
            return;
	}

	// pick tangents
	active_point_ = 0;
	active_point_ = pick_tangent( *(parent()->selected()), view, p, left_tangent_);

	if( active_point_)
	{
	    drag_tangents_mode_ = true;
	    view.update();
            return;
	}
    }

    Imath::Box2i screen_pick_box( Imath::V2i( event->x() - 2, event->y() - 2), Imath::V2i( event->x() + 2, event->y() + 2));
    Imath::Box2f pick_area = view.screen_to_world( screen_pick_box);

    parent()->deselect_all();
    shape_t *picked = pick_shape( pick_area);

    if( picked)
    {
	parent()->select( picked);
	drag_curve_mode_ = true;
    }
}

void qshape_select_tool_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    Imath::V2f off = view.screen_to_world_dir( Imath::V2i( event->x() - last_x_, event->y() - last_y_));

    if( !dragged_)
    {
	// first time drag, create an undo::command
	if( drag_curve_mode_ || drag_tangents_mode_ || drag_points_mode_)
            command_.reset( new undo::modify_qshape_command_t( parent()));

	dragged_ = true;
    }

    if( drag_curve_mode_)
    {
	parent()->selected()->move( off);
	parent()->selected()->set_shape_keyframe( document_t::Instance().composition().frame());

	if( parent()->selected()->active())
	{
	    parent()->notify();
	    parent()->notify_dependents();
	}

	ui::anim_editor_t::Instance().update();
    }

    if( drag_tangents_mode_)
    {
	if( event->modifiers() & Qt::ShiftModifier)
	    active_point_->set_broken( true);

	if( left_tangent_)
	{
	    active_point_->move_left_tangent( off);
	    active_point_->set_p0_keyframe( document_t::Instance().composition().frame());

	    if( !active_point_->broken())
	    {
		active_point_->adjust_right_tangent();
		active_point_->set_p2_keyframe( document_t::Instance().composition().frame());
	    }
	}
	else
	{
	    active_point_->move_right_tangent( off);
	    active_point_->set_p2_keyframe( document_t::Instance().composition().frame());

	    if( !active_point_->broken())
	    {
		active_point_->adjust_left_tangent();
		active_point_->set_p0_keyframe( document_t::Instance().composition().frame());
	    }
	}

	if( parent()->selected()->active())
	{
	    parent()->notify();
	    parent()->notify_dependents();
	}

	ui::anim_editor_t::Instance().update();
    }

    if( drag_points_mode_)
    {
	parent()->selected()->move_selection( off);
	float time = document_t::Instance().composition().frame();

	BOOST_FOREACH( triple_t& t, parent()->selected()->triples())
	{
	    if( t.selected())
		t.set_keyframe( time);
	}

	if( parent()->selected()->active())
	{
	    parent()->notify();
	    parent()->notify_dependents();
	}

	ui::anim_editor_t::Instance().update();
    }

    view.update();

    last_x_ = event->x();
    last_y_ = event->y();
}

void qshape_select_tool_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    if( box_pick_mode_)
    {
	Imath::Box2f world_area = view.screen_to_world( Imath::Box2i( Imath::V2i( std::min( push_x_, last_x_), std::min( push_y_, last_y_)),
									Imath::V2i( std::max( push_x_, last_x_), std::max( push_y_, last_y_))));

	select_triples_in_rect( *(parent()->selected()), world_area);
	box_pick_mode_ = false;
    }
    else
    {
	// push the undo step
	if( drag_curve_mode_ || drag_tangents_mode_ || drag_points_mode_)
	{
	    if( command_.get() != 0)
	    {
		undo::undo_stack_t::Instance().push_back( command_);
		ui::user_interface_t::Instance().update();
	    }
	}
    }

    dragged_ = false;
    active_point_ = 0;
    view.update();
}

shape_t *qshape_select_tool_t::pick_shape( const Imath::Box2f& area)
{
    BOOST_FOREACH( shape_t& s, boost::adaptors::reverse( parent()->shapes()))
    {
	if( pick_shape( s, area))
	    return &s;
    }

    return 0;
}

bool qshape_select_tool_t::pick_shape( const shape_t& s, const Imath::Box2f& area) const
{
    Imath::Box2f bbox( s.control_points_bounding_box());

    if( !area.intersects( bbox))
        return false;

    Imath::V2f a, b, c, d;

    for( shape_t::const_iterator it( s.begin()); it != s.end()-1; ++it)
    {
	a = it->p1(); b = it->p2(); c = (it+1)->p0(); d = (it+1)->p1();

        if( curve_intersects_box( a, b, c, d, area))
            return true;
    }

    // test last segment
    a = s.back().p1(); b = s.back().p2(); c = s.front().p0(); d = s.front().p1();

    if( curve_intersects_box( a, b, c, d, area))
        return true;

    return false;
}

void qshape_select_tool_t::select_triples_in_rect( shape_t& s, const Imath::Box2f& area) const
{
    BOOST_FOREACH( triple_t& t, s.triples())
    {
        if( area.intersects( t.p1()))
	    t.toggle_selection();
    }
}

triple_t *qshape_select_tool_t::pick_point( shape_t& s, ui::image_view_t& view, const Imath::V2i& p)
{
    BOOST_FOREACH( triple_t& t, s.triples())
    {
	Imath::V2i q( view.world_to_screen( t.p1()));
	float d = ( p - q).length();

	if( d < pick_distance)
	    return &t;
    }

    return 0;
}

triple_t *qshape_select_tool_t::pick_tangent( shape_t& s, ui::image_view_t& view, const Imath::V2i& p, bool& left)
{
    BOOST_FOREACH( triple_t& t, s.triples())
    {
	Imath::V2i q( view.world_to_screen( t.p0()));

	if( ( p - q).length() < pick_distance)
	{
	    left = true;
	    return &t;
	}

	q = view.world_to_screen( t.p2());

	if( ( p - q).length() < pick_distance)
	{
	    left = false;
	    return &t;
	}
    }

    return 0;
}

bool qshape_select_tool_t::curve_intersects_box( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c,
                                const Imath::V2f& d, const Imath::Box2f& area) const
{
    bezier::curve_t<Imath::V2f> curve( a, b, c, d);

    if( area.intersects( curve.bounding_box()))
    {
        const int num_steps = 20;
        const float step_size = 1.0f / 20.0f;

        Imath::V2f p0( a);
        Imath::V2f p1;

        float t = step_size;

        for( int i = 1; i < num_steps; ++i)
        {
            p1 = curve( t);

            if( intersects( IECore::LineSegment2f( p0, p1), area))
                return true;

            p0 = p1;
            t += step_size;
        }
    }

    return false;
}

void qshape_select_tool_t::delete_shape()
{
    command_.reset( new undo::qshape_delete_command_t( parent(), parent()->selected()));
    command_->redo();
    undo::undo_stack_t::Instance().push_back( command_);
}

void qshape_select_tool_t::delete_points()
{
    command_.reset( new undo::modify_qshape_command_t( parent(), true));
    parent()->selected()->delete_selected_points();

    ui::anim_editor_t::Instance().recreate_tracks( parent());
    ui::anim_editor_t::Instance().update();
    ui::anim_editor_t::Instance().update_tree();

    undo::undo_stack_t::Instance().push_back( command_);
}

} // namespace
} // namespace

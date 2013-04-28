// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_convert_tool.hpp>

#include<boost/foreach.hpp>

#include<QKeyEvent>
#include<QMouseEvent>

#include<ramen/GL/gl.hpp>
#include<ramen/ui/image_view.hpp>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/nodes/qshape/qshape_commands.hpp>

#include<ramen/undo/undo_stack.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/app/composition.hpp>

namespace ramen
{
namespace qshape
{

qshape_convert_tool_t::qshape_convert_tool_t( qshape_node_t *parent) : qshape_tool_t( parent)
{
    assert( parent);
}

void qshape_convert_tool_t::draw_overlay( ui::image_view_t& view) const
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

bool qshape_convert_tool_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)	    { return false;}
bool qshape_convert_tool_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)    { return false;}

void qshape_convert_tool_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();
    dragged_ = false;

    box_pick_mode_ = false;

    if( !parent()->selected())
	return;

    if( event->modifiers() & Qt::ControlModifier)
    {
	box_pick_mode_ = true;

	if( !(event->modifiers() & Qt::ShiftModifier))
	    parent()->selected()->deselect_all_points();

	view.update();
	return;
    }

    Imath::V2i p( push_x_, push_y_);
    triple_t *active_point = pick_point( *(parent()->selected()), view, p);

    if( active_point)
    {
	if( !active_point->selected())
	{
	    if( !(event->modifiers() & Qt::ShiftModifier))
		parent()->selected()->deselect_all_points();

	    active_point->select( true);
	}

	std::auto_ptr<undo::command_t> c( new undo::modify_qshape_command_t( parent()));

	float time = parent()->composition()->frame();
	parent()->selected()->toggle_corner_curve_selected_points( time);

	if( parent()->selected()->active())
	{
	    parent()->notify();
	    parent()->notify_dependents();
	}

	undo::undo_stack_t::Instance().push_back( c);
	ui::user_interface_t::Instance().update();
    }
}

void qshape_convert_tool_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    view.update();
    last_x_ = event->x();
    last_y_ = event->y();
}

void qshape_convert_tool_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    if( box_pick_mode_)
    {
	Imath::Box2f world_area = view.screen_to_world( Imath::Box2i( Imath::V2i( std::min( push_x_, last_x_), std::min( push_y_, last_y_)),
									Imath::V2i( std::max( push_x_, last_x_), std::max( push_y_, last_y_))));

	select_triples_in_rect( *(parent()->selected()), world_area);
	box_pick_mode_ = false;
    }

    view.update();
}

triple_t *qshape_convert_tool_t::pick_point( shape_t& s, ui::image_view_t& view, const Imath::V2i& p)
{
    BOOST_FOREACH( triple_t& t, s.triples())
    {
	Imath::V2i q( view.world_to_screen( t.p1()));
	float d = ( p - q).length2();

	if( d < pick_distance * pick_distance)
	    return &t;
    }

    return 0;
}

void qshape_convert_tool_t::select_triples_in_rect( shape_t& s, const Imath::Box2f& area) const
{
    BOOST_FOREACH( triple_t& t, s.triples())
    {
        if( area.intersects( t.p1()))
	    t.toggle_selection();
    }
}

} // namespace
} // namespace

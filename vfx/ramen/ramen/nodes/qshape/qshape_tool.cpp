// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_tool.hpp>

#include<boost/foreach.hpp>

#include<QKeyEvent>
#include<QMouseEvent>

#include<ramen/nodes/qshape/qshape_node.hpp>
#include<ramen/nodes/qshape/shape.hpp>
#include<ramen/ui/image_view.hpp>

namespace ramen
{
namespace qshape
{

qshape_tool_t::~qshape_tool_t() {}

void qshape_tool_t::draw_overlay( ui::image_view_t& view) const {}
bool qshape_tool_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)	    { return false;}
bool qshape_tool_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)    { return false;}
void qshape_tool_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)  {}
void qshape_tool_t::mouse_move_event( ui::image_view_t& view, QMouseEvent *event)   {}
void qshape_tool_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)   {}
void qshape_tool_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event){}

void qshape_tool_t::deselect() {}

triple_t *qshape_tool_t::pick_shape_points( const ui::image_view_t& view, shape_t& s, const Imath::V2i& p, qshape_tool_t::pick_result& result) const
{
    Imath::V2i q;

    BOOST_FOREACH( triple_t& t, s.triples())
    {
	q = view.world_to_screen( t.p1());

	if( ( p - q).length2() < 25)
	{
	    result = qshape_tool_t::pick_central_point;
	    return &t;
	}

	q = view.world_to_screen( t.p0());

	if( ( p - q).length2() < 25)
	{
	    result = qshape_tool_t::pick_left_tangent;
	    return &t;
	}

	q = view.world_to_screen( t.p2());

	if( ( p - q).length2() < 25)
	{
	    result = qshape_tool_t::pick_right_tangent;
	    return &t;
	}
    }

    result = qshape_tool_t::pick_nothing;
    return 0;
}

} // namespace
} // namespace

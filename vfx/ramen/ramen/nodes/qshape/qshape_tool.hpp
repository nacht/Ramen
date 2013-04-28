// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_TOOLS_HPP
#define	RAMEN_QSHAPE_TOOLS_HPP

#include<OpenEXR/ImathVec.h>

class QKeyEvent;
class QMouseEvent;

namespace ramen
{

class qshape_node_t;

namespace ui
{
class image_view_t;
}

namespace qshape
{

class shape_t;
class triple_t;

class qshape_tool_t
{
public:

    qshape_tool_t( qshape_node_t *parent) : parent_( parent) {}
    virtual ~qshape_tool_t();

    qshape_node_t *parent()		{ return parent_;}
    const qshape_node_t *parent() const	{ return parent_;}

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

    virtual void deselect();

protected:

    enum pick_result
    {
        pick_nothing = 0,
        pick_curve,
        pick_central_point,
        pick_left_tangent,
        pick_right_tangent
    };

    triple_t *pick_shape_points( const ui::image_view_t& view, shape_t& s, const Imath::V2i& p, pick_result& result) const;

    qshape_node_t *parent_;
};

} // namespace
} // namespace

#endif	/* _qshape_TOOLS_HPP */

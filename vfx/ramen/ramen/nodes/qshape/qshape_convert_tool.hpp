// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_CONVERT_TOOL_HPP
#define	RAMEN_QSHAPE_CONVERT_TOOL_HPP

#include<ramen/nodes/qshape/qshape_tool.hpp>
#include<ramen/nodes/qshape/shape.hpp>

namespace ramen
{
namespace qshape
{

class qshape_convert_tool_t : public qshape_tool_t
{
public:

    qshape_convert_tool_t( qshape_node_t *parent);

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

private:

    triple_t *pick_point( shape_t& s, ui::image_view_t& view, const Imath::V2i& p);
    void select_triples_in_rect( shape_t& s, const Imath::Box2f& area) const;

    int push_x_, push_y_;
    int last_x_, last_y_;
    bool dragged_;

    // modes
    bool box_pick_mode_;

    static const int pick_distance = 5;
};

} // namespace
} // namespace

#endif	/* _qshape_CONVERT_TOOL_HPP */

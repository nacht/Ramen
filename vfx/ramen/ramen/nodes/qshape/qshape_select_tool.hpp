// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_SELECT_TOOL_HPP
#define	RAMEN_QSHAPE_SELECT_TOOL_HPP

#include<ramen/nodes/qshape/qshape_tool.hpp>

#include<memory>
#include<vector>

#include<OpenEXR/ImathBox.h>

#include<ramen/undo/command.hpp>

namespace ramen
{
namespace qshape
{

class shape_t;

class qshape_select_tool_t : public qshape_tool_t
{
public:

    qshape_select_tool_t( qshape_node_t *parent);

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

private:

    shape_t *pick_shape( const Imath::Box2f& area);

    bool pick_shape( const shape_t& s, const Imath::Box2f& area) const;

    void select_triples_in_rect( shape_t& s, const Imath::Box2f& area) const;

    triple_t *pick_point( shape_t& s, ui::image_view_t& view, const Imath::V2i& p);
    triple_t *pick_tangent( shape_t& s, ui::image_view_t& view, const Imath::V2i& p, bool& left);

    bool curve_intersects_box( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c,
                                const Imath::V2f& d, const Imath::Box2f& area) const;

    void delete_shape();
    void delete_points();
    
    int push_x_, push_y_;
    int last_x_, last_y_;
    bool dragged_;

    // modes
    bool box_pick_mode_;
    bool drag_curve_mode_;
    bool drag_points_mode_;
    bool drag_tangents_mode_;
    bool left_tangent_;

    triple_t *active_point_;

    static const int pick_distance = 5;

    std::auto_ptr<undo::command_t> command_;
};

} // namespace
} // namespace

#endif	/* _qshape_SELECT_TOOL_HPP */

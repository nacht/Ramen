// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_CREATE_TOOL_HPP
#define	RAMEN_QSHAPE_CREATE_TOOL_HPP

#include<ramen/nodes/qshape/qshape_tool.hpp>

namespace ramen
{
namespace qshape
{

class triple_t;
class shape_t;

class qshape_create_tool_t : public qshape_tool_t
{
public:

    qshape_create_tool_t( qshape_node_t *parent);

    virtual void draw_overlay( ui::image_view_t& view) const;

    virtual bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

    virtual void deselect();

private:

    float param_closest_to_point( qshape::shape_t *s, const Imath::V2f& p, float max_distance) const;
    void insert_point( float param) const;

    void subdivide_segment( float param, qshape::triple_t *prev, qshape::triple_t *cur, qshape::triple_t *next) const;

    void close_shape( bool success = true);

    int push_x_, push_y_;
    int last_x_, last_y_;
    qshape::shape_t *new_shape_;
    qshape::triple_t *triple_p_;

    bool insert_point_mode_;
};

} // namespace
} // namespace

#endif


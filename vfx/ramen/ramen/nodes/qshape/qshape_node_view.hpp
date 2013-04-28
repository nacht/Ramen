// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_NODE_VIEW_HPP
#define	RAMEN_QSHAPE_NODE_VIEW_HPP

#include<ramen/ui/image_view_fwd.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathColor.h>

namespace ramen
{

class qshape_node_t;

namespace qshape
{

class shape_t;
class triple_t;

class node_view_t
{
public:

    node_view_t( qshape_node_t *parent);

    void draw_overlay( ui::image_view_t& view) const;

    void draw_shape( const shape_t& s) const;

private:

    void draw_bezier_curve( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c, const Imath::V2f& d) const;

    void draw_control_polygon( const shape_t& s) const;
    void draw_triple( const triple_t& t, const Imath::Color3f& col) const;

    qshape_node_t *parent_;
};

} // namespace
} // namespace

#endif

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/point2_manipulator.hpp>

#include<cassert>
#include<cmath>

#include<ramen/manipulators/draw_pick_controls.hpp>

#include<ramen/params/parameterised.hpp>
#include<ramen/app/composition.hpp>
#include<ramen/nodes/image_node.hpp>
#include<ramen/params/float2_param.hpp>

#include<ramen/ui/image_view.hpp>

#include<ramen/GL/gl.hpp>

namespace ramen
{

point2_manipulator_t::point2_manipulator_t( float2_param_t *point) : manipulator_t()
{
    assert( point != 0 && "point2 manipulator: null float2_param_t");
    point_ = point;
}

void point2_manipulator_t::draw_overlay( ui::image_view_t& view) const
{
    const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
    assert( node);

    int subsample = node->composition()->subsample();
    float time = node->composition()->frame();

    // draw the center
    Imath::V2f p = get_absolute_value<Imath::V2f>( *point_) * subsample;

    glLineWidth( 2);
    glColor3f( 1, 1, 0);
    draw_point_control( p, 10 / view.pixel_scale());
}

} // namespace

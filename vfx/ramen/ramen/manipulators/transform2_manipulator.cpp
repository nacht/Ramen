// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/transform2_manipulator.hpp>

#include<cassert>
#include<cmath>

#include<ramen/manipulators/draw_pick_controls.hpp>

#include<ramen/params/parameterised.hpp>
#include<ramen/app/composition.hpp>
#include<ramen/nodes/image_node.hpp>
#include<ramen/params/transform2_param.hpp>

#include<ramen/ui/image_view.hpp>

#include<ramen/GL/gl.hpp>

namespace ramen
{

transform2_manipulator_t::transform2_manipulator_t( transform2_param_t *xform) : manipulator_t()
{
    assert( xform != 0 && "Transform2 manipulator: null transform2_param_t");
    xform_ = xform;
}

void transform2_manipulator_t::draw_overlay( ui::image_view_t& view) const
{
    const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
    assert( node);

    int subsample = node->composition()->subsample();
    float time = node->composition()->frame();

    boost::optional<Imath::M33d> m( xform_->matrix_at_time( time, subsample));

    if( !m)
        return;

    // adjust for the subsample factor
    Imath::M33d mm = m.get() * Imath::M33d().setScale( Imath::V2d( subsample, subsample));

    // draw the center
    glLineWidth( 2);
    Imath::V2f p = get_absolute_value<Imath::V2f>( xform_->center_param());
    p = p * mm;

    draw_xy_axes( p, 70 / view.pixel_scale(), get_value<float>( xform_->rotate_param()));

    glColor3f( 1, 1, 0);
    draw_point_control( p, 10 / view.pixel_scale());

    // draw the boundary
    glLineWidth( 1);
    Imath::Box2i bbox( node->domain());

    glColor3f( 1, 1, 0);
    glBegin( GL_LINE_LOOP);
        gl_transformed_box( bbox, mm);
    glEnd();

    // draw the corner handles
    glColor3f( 1, 1, 0);
    glPointSize( 5);
    glBegin( GL_POINTS);
        gl_transformed_box( bbox, mm);
    glEnd();
}

} // namespace

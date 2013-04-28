// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DRAW_PICK_CONTROLS_HPP
#define RAMEN_DRAW_PICK_CONTROLS_HPP

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathColor.h>
#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/ui/image_view_fwd.hpp>

namespace ramen
{

void gl_transformed_box( const Imath::Box2i& box, const Imath::M33d& m);

void draw_point_control( const Imath::V2f& p, float size);

void draw_arrow( const Imath::V2f& p, const Imath::V2f& q);

void draw_xy_axes( const Imath::V2f& c, float size, float angle = 0,
                    const Imath::Color3f& xcol = Imath::Color3f( 1, 0, 0),
                    const Imath::Color3f& ycol = Imath::Color3f( 0, 1, 0));


} // namespace

#endif

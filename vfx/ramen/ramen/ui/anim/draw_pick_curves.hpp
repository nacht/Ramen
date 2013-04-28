// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_DRAW_PICK_CURVES_HPP
#define RAMEN_UI_ANIM_DRAW_PICK_CURVES_HPP

#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/curve_fwd.hpp>

#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace ui
{

class anim_curves_view_t;

// draw
void draw_curve( const anim_curves_view_t& view, const anim::curve_t& curve);
void draw_keyframes( const anim_curves_view_t& view, const anim::curve_t& curve);

// pick
anim::keyframe_t *pick_tangent( const anim_curves_view_t& view, const Imath::V2i& p, bool& left,
				     anim::curve_t*& curve);

anim::keyframe_t *pick_keyframe( const anim_curves_view_t& view, const Imath::V2i& p);

void pick_keyframes( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::Box2i& area);

bool pick_curve( const anim_curves_view_t& view, anim::curve_t& curve, const Imath::V2i& p);

} // namespace
} // namespace

#endif

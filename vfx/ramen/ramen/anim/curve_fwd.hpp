// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_FWD_HPP
#define RAMEN_ANIM_CURVE_FWD_HPP

namespace ramen
{
namespace anim
{

class keyframe_t;

enum extrapolation_method
{
    extrapolate_constant = 0,
    extrapolate_linear,
    extrapolate_repeat
};

class curve_t;

} // namespace
} // namespace

#endif

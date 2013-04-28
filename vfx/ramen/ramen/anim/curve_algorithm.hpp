// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_ALGORITHM_HPP
#define	RAMEN_ANIM_CURVE_ALGORITHM_HPP

#include<ramen/anim/curve_fwd.hpp>

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace anim
{

void move_selected_keyframes( curve_t& c, const Imath::V2f& d);

} // namespace
} // namespace

#endif	/* _CURVE_ALGORITHM_HPP */

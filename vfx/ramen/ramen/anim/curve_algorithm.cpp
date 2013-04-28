// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/curve_algorithm.hpp>

#include<boost/foreach.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/anim/curve.hpp>

namespace ramen
{
namespace anim
{

namespace 
{

const float time_tolerance = 0.025f;

void move_selected_keys_value( curve_t& c, float offset)
{
    BOOST_FOREACH( keyframe_t& k, c.keys())
    {
	if( k.selected())
	    k.set_value( Imath::clamp( k.value() + offset, c.get_min(), c.get_max()));
    }
}

void move_selected_keys_time_left( curve_t& c, float offset)
{
    curve_t::iterator it( c.begin()), prev( c.begin());

    if( it->selected())
	it->set_time( it->time() - offset);
    ++it;

    for( ; it != c.end(); ++it, ++prev)
    {
	if( it->selected())
	    it->set_time( std::max( it->time() - offset, prev->time() + time_tolerance));
    }
}

void move_selected_keys_time_right( curve_t& c, float offset)
{
    curve_t::reverse_iterator it( c.rbegin()), prev( c.rbegin());

    if( it->selected())
	it->set_time( it->time() + offset);
    ++it;

    for( ; it != c.rend(); ++it, ++prev)
    {
	if( it->selected())
	    it->set_time( std::min( it->time() + offset, prev->time() - time_tolerance));
    }
}

} // unnamed

void move_selected_keyframes( curve_t& c, const Imath::V2f& d)
{
    if( d.x == 0.0f && d.y == 0.0f)
	return;

    move_selected_keys_value( c, d.y);

    if( d.x < 0.0f)
	move_selected_keys_time_left( c, -d.x);
    else
        move_selected_keys_time_right( c, d.x);

    c.recalc_tangents_and_coefficients();
}

} // namespace
} // namespace

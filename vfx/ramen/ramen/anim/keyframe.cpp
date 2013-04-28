// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/keyframe.hpp>

#include<adobe/algorithm/clamp.hpp>

namespace ramen
{
namespace anim
{

keyframe_t::keyframe_t() : time_(0), v0_(0), v1_(0), value_(0), selected_(false)
{
    auto_v0_ = auto_v1_ = tangent_smooth;
    tangent_cont_ = true;
}

keyframe_t::keyframe_t( float time, float value) : time_( time), value_(value), v0_(0), v1_(0), selected_(false)
{
    auto_v0_ = auto_v1_ = tangent_smooth;
    tangent_cont_ = true;
}

void keyframe_t::set_v0_auto_tangent( auto_tangent_method m)
{
    auto_v0_ = m;

    if( m == tangent_smooth || m == tangent_flat)
	set_tangent_continuity( true);
    else
    {
	if( m != tangent_fixed)
	    set_tangent_continuity( false);
    }
}

void keyframe_t::set_v1_auto_tangent( auto_tangent_method m)
{
    auto_v1_ = m;

    if( m == tangent_smooth || m == tangent_flat)
	set_tangent_continuity( true);
    else
    {
	if( m != tangent_fixed)
	    set_tangent_continuity( false);
    }
}

void keyframe_t::set_v0_tangent( float slope)
{
    v0_ = adobe::clamp( slope, min_slope(), max_slope());
    set_v0_auto_tangent( anim::tangent_fixed);

    if( tangent_cont_)
    {
	v1_ = adobe::clamp( slope, min_slope(), max_slope());
	set_v1_auto_tangent( anim::tangent_fixed);
    }
}

void keyframe_t::set_v1_tangent( float slope)
{
    v1_ = adobe::clamp( slope, min_slope(), max_slope());
    set_v1_auto_tangent( anim::tangent_fixed);

    if( tangent_cont_)
    {
	v0_ = adobe::clamp( slope, min_slope(), max_slope());
	set_v0_auto_tangent( anim::tangent_fixed);
    }
}

void keyframe_t::calc_tangents( const keyframe_t *prev, const keyframe_t *next)
{
    if( v0_auto_tangent() == tangent_fixed && v1_auto_tangent() == tangent_fixed)
	return;

    // left
    if( !prev || ( v0_auto_tangent() == tangent_step) || ( v0_auto_tangent() == tangent_flat))
	set_v0( 0);
    else
    {
	if( v0_auto_tangent() == tangent_linear)
	    set_v0( ( value() - prev->value()) / ( time() - prev->time()));
	else
	{
	    if( v0_auto_tangent() == tangent_smooth)
	    {
		if( !next)
		    set_v0( 0);
		else
		    set_v0( ( next->value() - prev->value()) / ( next->time() - prev->time()));
	    }
	}
    }

    // right
    if( !next || ( v1_auto_tangent() == tangent_step) || ( v1_auto_tangent() == tangent_flat))
	set_v1( 0);
    else
    {
	if( v1_auto_tangent() == tangent_linear)
	    set_v1( ( next->value() - value()) / ( next->time() - time()));
	else
	{
	    if( v1_auto_tangent() == tangent_smooth)
	    {
		if( !prev)
		    set_v1( 0);
		else
		    set_v1( ( next->value() - prev->value()) / ( next->time() - prev->time()));
	    }
	}
    }
}

void keyframe_t::calc_cubic_coefficients( const keyframe_t& next)
{
    if( v1_auto_tangent() == tangent_step)
    {
	coeffs_[0] = 0;
	coeffs_[1] = 0;
	coeffs_[2] = 0;
	coeffs_[3] = value();
    }
    else
    {
	float time_span = next.time() - time();
	coeffs_[0] =  ( 2 * value()) - ( 2 * next.value()) + (     time_span * v1()) + ( time_span * next.v0());
	coeffs_[1] = -( 3 * value()) + ( 3 * next.value()) - ( 2 * time_span * v1()) - ( time_span * next.v0());
	coeffs_[2] = time_span * v1();
	coeffs_[3] = value();
    }
}

float keyframe_t::evaluate_cubic( float t) const
{    
    return ((((coeffs_[0] * t) + coeffs_[1]) * t + coeffs_[2]) * t) + coeffs_[3];
}

float keyframe_t::evaluate_derivative( float t) const
{
    return ( 3.0f * coeffs_[0] * t * t) + ( 2.0f * coeffs_[1] * t) + coeffs_[2];
}

float keyframe_t::max_slope() { return 21.0f;}
float keyframe_t::min_slope() { return -max_slope();}

} // namespace
} // namespace

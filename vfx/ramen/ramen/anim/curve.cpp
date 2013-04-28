// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/curve.hpp>

#include<stdlib.h>

#include<limits>
#include<algorithm>
#include<sstream>
#include<cmath>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>
#include<adobe/algorithm/for_each_position.hpp>
#include<adobe/algorithm/remove.hpp>
#include<adobe/algorithm/find.hpp>

#include<OpenEXR/ImathFun.h>

namespace ramen
{
namespace anim
{

curve_t::curve_t()
{
    default_auto_tan_ = tangent_smooth;
    min_ = -std::numeric_limits<float>::max();
    max_ =  std::numeric_limits<float>::max();
    extrap_ = extrapolate_constant;
}

void curve_t::copy( const curve_t& other, float offset)
{
    clear();

    BOOST_FOREACH( const keyframe_t& k, other.keys())
    {
	keyframe_t new_k = k;
	new_k.set_time( k.time() + offset);
	insert( new_k, false);
    }

    recalc_tangents_and_coefficients();
}

void curve_t::copy( const curve_t& other, float offset, float start, float end)
{
    clear();

    BOOST_FOREACH( const keyframe_t& k, other.keys())
    {
	if( k.time() >= start && k.time() <= end)
	{
	    keyframe_t new_k = k;
	    new_k.set_time( k.time() + offset);
	    insert( new_k, false);
	}
    }

    recalc_tangents_and_coefficients();
}

curve_t::iterator curve_t::insert( float time, float value, bool recalc)
{
    keyframe_t k( time, value);
    return insert( k, recalc);
}

curve_t::iterator curve_t::insert( const keyframe_t& k, bool recalc)
{
    iterator it( keys().insert( k));

    if( it != begin())
    {
	it->set_v0_auto_tangent( (it - 1)->v1_auto_tangent());
	it->set_v1_auto_tangent( (it - 1)->v1_auto_tangent());
    }
    else
    {
	it->set_v0_auto_tangent( default_auto_tan_);
	it->set_v1_auto_tangent( default_auto_tan_);
    }

    if( recalc)
	recalc_tangents_and_coefficients( it);

    return it;
}

curve_t::iterator curve_t::insert( float time, bool recalc)
{
    float value = evaluate( time);
    return insert( time, value, recalc);
}

void curve_t::erase( float time) { keys().erase( time);}

void curve_t::erase_selected_keyframes()
{
    keys().erase( adobe::remove_if( keys(), boost::bind( &keyframe_t::selected, _1)), keys().end());
}

curve_t::iterator curve_t::iterator_from_keyframe_ptr( keyframe_t *k)
{
    for( curve_t::iterator it( begin()); it != end(); ++it)
    {
	if( &(*it) == k)
	    return it;
    }

    return end();
}

float curve_t::evaluate( float time) const { return Imath::clamp( do_evaluate( time), min_, max_);}

float curve_t::do_evaluate( float time) const
{
    if( empty())
	return 0;

    if( time < keys().front().time())
    {
	switch( extrapolation())
	{
	case extrapolate_constant:
	    return keys().front().value();

	case extrapolate_linear:
	{
	    float dir = keys().front().v1();
	    return keys().front().value() + (( time - keys().front().time()) * dir);
	}

	case extrapolate_repeat:
	    double ipart;
	    float new_time = std::fabs( std::modf( (double) ( time - start_time()) / ( end_time() - start_time()), &ipart));
	    new_time = ( new_time * end_time()) + ( ( 1.0f - new_time) * start_time());
	    assert( new_time >= start_time());
	    assert( new_time <= end_time());
	    return do_evaluate( new_time);
	break;
	}
    }

    if( time > keys().back().time())
    {
	switch( extrapolation())
	{
	case extrapolate_constant:
	    return keys().back().value();

	case extrapolate_linear:
	{
	    float dir = keys().back().v0();
	    return keys().back().value() + (( time - keys().back().time()) * dir);
	}

	case extrapolate_repeat:
	{
	    double ipart;
	    float new_time = std::modf( (double) ( time - start_time()) / ( end_time() - start_time()), &ipart);
	    new_time = ( new_time * end_time()) + ( ( 1.0f - new_time) * start_time());
	    assert( new_time >= start_time());
	    assert( new_time <= end_time());
	    return do_evaluate( new_time);
	}
	break;
	}
    }

    const_iterator it( keys().lower_bound( time));

    if( time == it->time())
	return it->value();

    --it;
    float t = ( time - it->time()) / ( (it+1)->time() - it->time());
    return it->evaluate_cubic( t);
}

float curve_t::derive( float time) const
{
    if( empty())
	return 0;

    if( time < keys().front().time())
    {
	// extrapolate start
	return 0;
    }

    if( time > keys().back().time())
    {
	// extrapolate end
	return 0;
    }

    const_iterator it( keys().lower_bound( time));

    --it;
    float t = ( time - it->time()) / ( (it+1)->time() - it->time());
    return it->evaluate_derivative( t);
}

float curve_t::integrate( float time1, float time2) const
{
    // TODO: we could do better
    float sum = 0.0f;

    for( float t = time1; t < time2; t += 1.0f)
	sum += evaluate( t);

    return sum;
}

void curve_t::recalc_coefficients()
{
    if( size() < 2)
	return;

    for( iterator it( keys().begin()); it != keys().end() - 1; ++it)
	it->calc_cubic_coefficients( *(it+1));
}

void curve_t::recalc_tangents_and_coefficients( iterator it)
{
    // not the best code I've ever written
    keyframe_t *ptr[5] = { 0, 0, 0, 0, 0};

    if( (it-2) >= begin())	ptr[0] = &*(it-2);
    if( (it-1) >= begin())	ptr[1] = &*(it-1);
				ptr[2] = &*it;
    if( (it+1) < end())		ptr[3] = &*(it+1);
    if( (it+2) < end())		ptr[4] = &*(it+2);

    if( ptr[1])	ptr[1]->calc_tangents( ptr[0]	, ptr[2]);
    if( ptr[2])	ptr[2]->calc_tangents( ptr[1]	, ptr[3]);
    if( ptr[3])	ptr[3]->calc_tangents( ptr[2]	, ptr[4]);

    if( ptr[0] && ptr[1]) ptr[0]->calc_cubic_coefficients( *ptr[1]);
    if( ptr[1] && ptr[2]) ptr[1]->calc_cubic_coefficients( *ptr[2]);
    if( ptr[2] && ptr[3]) ptr[2]->calc_cubic_coefficients( *ptr[3]);
    if( ptr[3] && ptr[4]) ptr[3]->calc_cubic_coefficients( *ptr[4]);
}

void curve_t::recalc_tangents_and_coefficients()
{
    adobe::for_each_position( keys(),
			    boost::bind( &curve_t::recalc_tangents_and_coefficients, this, _1));
}

void curve_t::select_all_keyframes( bool b) const
{
    adobe::for_each( keys(), boost::bind( &keyframe_t::select, _1, b));
}

bool curve_t::any_keyframe_selected() const
{
    return adobe::find_if( keys(), boost::bind( &keyframe_t::selected, _1 )) != keys().end();
}

Imath::Box2f curve_t::bounds() const
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::keyframe_t& k, keys())
    {
        bounds.extendBy( Imath::V2f( k.time(), k.value()));
    }

    return bounds;
}

Imath::Box2f curve_t::selection_bounds() const
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::keyframe_t& k, keys())
    {
	if( k.selected())
            bounds.extendBy( Imath::V2f( k.time(), k.value()));
    }

    return bounds;
}

float curve_t::start_time() const   { return keys().front().time();}
float curve_t::end_time() const	    { return keys().back().time();}

std::string curve_t::str() const
{
    std::stringstream s;

    BOOST_FOREACH( const anim::keyframe_t& k, keys())
    {
	s << k.time() << "," << k.value() << "," <<
		k.v0() << "," << k.v1() << "," << k.v0_auto_tangent() << ","
		<< k.v1_auto_tangent();
    }

    s << extrapolation();

    return s.str();
}

void swap( curve_t& a, curve_t& b)
{
    using namespace std;
    swap( a.keys_, b.keys_);
    std::swap( a.min_, b.min_);
    std::swap( a.max_, b.max_);
    std::swap( a.extrap_, b.extrap_);
    std::swap( a.default_auto_tan_, b.default_auto_tan_);
}

} // namespace
} // namespace

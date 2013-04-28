// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/point2.hpp>

#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace anim
{

point2_t::point2_t()
{
    xcurve_.set_default_auto_tangents( tangent_linear);
    ycurve_.set_default_auto_tangents( tangent_linear);
}

Imath::V2f point2_t::point_at_time( float t) const
{
    Imath::V2f p( point_);

    if( !xcurve_.empty())
	p.x = xcurve_.evaluate( t);

    if( !ycurve_.empty())
	p.y = ycurve_.evaluate( t);

    return p;
}

void point2_t::set_point( const Imath::V2f& p) { point_ = p;}

void point2_t::move( const Imath::V2f& v) { point_ += v;}

void point2_t::evaluate( float time)
{
    if( !xcurve_.empty())
	point_.x = xcurve_.evaluate( time);

    if( !ycurve_.empty())
	point_.y = ycurve_.evaluate( time);
}

void point2_t::set_keyframe( float time)
{
    xcurve_.insert( time, point_.x);
    ycurve_.insert( time, point_.y);
}

void point2_t::create_tracks( track_t *parent)
{
    std::auto_ptr<track_t> tx( new track_t( "X", &xcurve_));
    tx->changed.connect( boost::bind( &point2_t::anim_curve_changed, this, _1));
    parent->add_child( tx);

    std::auto_ptr<track_t> ty( new track_t( "Y", &ycurve_));
    ty->changed.connect( boost::bind( &point2_t::anim_curve_changed, this, _1));
    parent->add_child( ty);
}

void point2_t::anim_curve_changed( curve_t *c)
{
    // TODO: do something here
    evaluate( document_t::Instance().composition().frame());
}

void point2_t::keyframe_times( std::set<float>& times) const
{
    BOOST_FOREACH( const keyframe_t&k, xcurve_.keys()) { times.insert( k.time());}
    BOOST_FOREACH( const keyframe_t&k, ycurve_.keys()) { times.insert( k.time());}
}

void swap( point2_t& a, point2_t& b)
{
    using namespace std;
    std::swap( a.point_, b.point_);
    swap( a.xcurve_, b.xcurve_);
    swap( a.ycurve_, b.ycurve_);
}

} // namespace
} // namespace

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/triple.hpp>

#include<algorithm>

#include<ramen/GL/gl.hpp>

#include<ramen/anim/track.hpp>

namespace ramen
{

namespace qshape
{

triple_t::triple_t() : flags_( 0)
{
    select( false);
    set_corner( false);
    set_broken( false);
}

triple_t::triple_t( const Imath::V2f& p) : flags_( 0)
{
    set_p0( p);
    set_p1( p);
    set_p2( p);

    select( false);
    set_corner( true);
    set_broken( true);
}

void triple_t::select( bool b) const { selected_ = b;}

void triple_t::toggle_selection() const { select( !selected());}

void triple_t::set_corner( bool b)
{
    if( b)
	flags_ |= corner_bit;
    else
	flags_ &= ~corner_bit;
}

void triple_t::set_broken( bool b)
{
    if( b)
	flags_ |= broken_bit;
    else
	flags_ &= ~broken_bit;
}

void triple_t::move( const Imath::V2f& off)
{
    p0_.move( off);
    p1_.move( off);
    p2_.move( off);
}

void triple_t::move_left_tangent( const Imath::V2f& off)    { p0_.move( off);}
void triple_t::move_right_tangent( const Imath::V2f& off)   { p2_.move( off);}

void triple_t::adjust_left_tangent( bool c1_cont)
{
    if( broken())
	return;

    float len = ( p0_.point() - p1_.point()).length();

    if( len == 0.0f)
	return;

    Imath::V2f dir( p2_.point() - p1_.point());
    float dir_len = dir.length();

    if( dir_len == 0.0f)
	return;

    dir /= dir_len;

    if( c1_cont)
	p0_.set_point( p1_.point() - ( dir * dir_len));
    else
	p0_.set_point( p1_.point() - ( dir * len));
}

void triple_t::adjust_right_tangent( bool c1_cont)
{
    if( broken())
	return;
    
    float len = ( p2_.point() - p1_.point()).length();
    
    if( len == 0.0f)
	return;
    
    Imath::V2f dir( p0_.point() - p1_.point());
    float dir_len = dir.length();
    
    if( dir_len == 0.0f)
	return;
	
    dir /= dir_len;

    if( c1_cont)
	p2_.set_point( p1_.point() - ( dir * dir_len));
    else
	p2_.set_point( p1_.point() - ( dir * len));
}

void triple_t::convert_to_corner()
{
    p0_.set_point( p1_.point());
    p2_.set_point( p1_.point());
    set_corner( true);
    set_broken( true);
}

void triple_t::convert_to_curve()
{
    set_corner( false);
    set_broken( false);
}

void triple_t::create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( "Point"));
    p1_.create_tracks( t.get());
    p0_.create_tracks( t.get());
    p2_.create_tracks( t.get());
    parent->add_child( t);
}

void triple_t::evaluate( float time)
{
    p0_.evaluate( time);
    p1_.evaluate( time);
    p2_.evaluate( time);
}

void triple_t::set_keyframe( float time)
{
    set_p0_keyframe( time);
    set_p1_keyframe( time);
    set_p2_keyframe( time);
}

void triple_t::set_p0_keyframe( float time) { p0_.set_keyframe( time);}
void triple_t::set_p1_keyframe( float time) { p1_.set_keyframe( time);}
void triple_t::set_p2_keyframe( float time) { p2_.set_keyframe( time);}

void triple_t::keyframe_times( std::set<float>& times) const
{
    p0_.keyframe_times( times);
    p1_.keyframe_times( times);
    p2_.keyframe_times( times);
}

void swap( triple_t& a, triple_t& b)
{
   using std::swap;
   swap( a.p0_, b.p0_);
   swap( a.p1_, b.p1_);
   swap( a.p2_, b.p2_);

   std::swap( a.flags_, b.flags_);
   std::swap( a.selected_, b.selected_);
}

} // namespace
} // namespace

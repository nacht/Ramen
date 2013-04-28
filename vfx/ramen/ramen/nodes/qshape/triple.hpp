// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_TRIPLE_HPP
#define	RAMEN_QSHAPE_TRIPLE_HPP

#include<ramen/anim/point2.hpp>
#include<ramen/anim/track_fwd.hpp>

namespace ramen
{
namespace qshape
{

class triple_t
{
public:

    enum flag_bits
    {
        corner_bit      = 1 << 0,
        broken_bit      = 1 << 1
    };

    triple_t();
    triple_t( const Imath::V2f& p);

    const Imath::V2f& p0() const            { return p0_.point();}
    Imath::V2f p0_at_time( float t) const   { return p0_.point_at_time( t);}
    void set_p0( const Imath::V2f& p)       { p0_.set_point( p);}

    const Imath::V2f& p1() const            { return p1_.point();}
    Imath::V2f p1_at_time( float t) const   { return p1_.point_at_time( t);}
    void set_p1( const Imath::V2f& p)       { p1_.set_point( p);}

    const Imath::V2f& p2() const            { return p2_.point();}
    Imath::V2f p2_at_time( float t) const   { return p2_.point_at_time( t);}
    void set_p2( const Imath::V2f& p)       { p2_.set_point( p);}

    void move( const Imath::V2f& off);
    void move_left_tangent( const Imath::V2f& off);
    void move_right_tangent( const Imath::V2f& off);

    bool selected() const { return selected_;}
    void select( bool b) const;
    void toggle_selection() const;

    bool corner() const	{ return flags_ & corner_bit;}
    void set_corner( bool b);

    bool broken() const { return flags_ & broken_bit;}
    void set_broken( bool b);

    void adjust_left_tangent( bool c1_cont = false);
    void adjust_right_tangent( bool c1_cont = false);

    void convert_to_corner();
    void convert_to_curve();

    // anim
    void create_tracks( anim::track_t *parent);
    void evaluate( float time);

    void set_keyframe( float time);
    void set_p0_keyframe( float time);
    void set_p1_keyframe( float time);
    void set_p2_keyframe( float time);

    void keyframe_times( std::set<float>& times) const;

private:

    friend void swap( triple_t& a, triple_t& b);

    friend class boost::serialization::access;

    // TODO: don't serializae selected
    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "p0", p0_);
	ar & boost::serialization::make_nvp( "p1", p1_);
	ar & boost::serialization::make_nvp( "p2", p2_);
	ar & boost::serialization::make_nvp( "flags", flags_);
    }

    anim::point2_t p0_, p1_, p2_;
    boost::uint32_t flags_;
    mutable bool selected_;
};

} // namespace
} // namespace

#endif	/* _TRIPLE_HPP */

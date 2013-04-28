// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_POINT2_HPP
#define RAMEN_ANIM_POINT2_HPP

#include<set>

#include<ramen/ImathExt/serialization/ImathVec.hpp>

#include<ramen/anim/curve.hpp>
#include<ramen/anim/track_fwd.hpp>

namespace ramen
{
namespace anim
{

class point2_t
{
public:

    point2_t();

    // coordinates
    const Imath::V2f& point() const     { return point_;}
    Imath::V2f& point()                 { return point_;}

    Imath::V2f point_at_time( float t) const;

    const anim::curve_t& x_curve() const { return xcurve_;}
    const anim::curve_t& y_curve() const { return ycurve_;}

    void set_point( const Imath::V2f& p);

    void move( const Imath::V2f& v);

    // anim
    void evaluate( float time);
    void set_keyframe( float time);

    void create_tracks( track_t *parent);
    void anim_curve_changed( curve_t *c);

    void keyframe_times( std::set<float>& times) const;

private:

    friend void swap( point2_t& a, point2_t& b);

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "point", point_);
	ar & boost::serialization::make_nvp( "xcurve", xcurve_);
	ar & boost::serialization::make_nvp( "ycurve", ycurve_);
    }

    Imath::V2f point_;
    anim::curve_t xcurve_;
    anim::curve_t ycurve_;
};

} // namespace
} // namespace

#endif

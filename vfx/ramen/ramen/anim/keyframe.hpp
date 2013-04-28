// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_KEYFRAME_HPP
#define RAMEN_KEYFRAME_HPP

#include<boost/array.hpp>

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>

namespace ramen
{
namespace anim
{

enum auto_tangent_method
{
    tangent_step = 0,
    tangent_linear,
    tangent_smooth,
    tangent_flat,
    tangent_fixed
};

class keyframe_t
{
public:

    typedef float value_type;

    keyframe_t();
    keyframe_t( float time, float value);

    float time() const	    { return time_;}
    void set_time( float t) { time_ = t;}

    float  value() const  { return value_;}
    float& value()        { return value_;}

    void set_value( float x)    { value_ = x;}

    bool selected() const           { return selected_;}
    void select( bool b) const      { selected_ = b;}
    void toggle_selection() const   { selected_ = !selected_;}

    bool operator<( const keyframe_t& other) const { return time_ < other.time_;}

    // tangents
    float v0() const        { return v0_;}
    void set_v0( float v)   { v0_ = v;}
    float v1() const        { return v1_;}
    void set_v1( float v)   { v1_ = v;}

    auto_tangent_method v0_auto_tangent() const        { return auto_v0_;}
    void set_v0_auto_tangent( auto_tangent_method m);

    auto_tangent_method v1_auto_tangent() const        { return auto_v1_;}
    void set_v1_auto_tangent( auto_tangent_method m);

    bool tangent_continuity() const         { return tangent_cont_;}
    void set_tangent_continuity( bool b)    { tangent_cont_ = b;}

    void calc_tangents( const keyframe_t *prev, const keyframe_t *next);

    void set_v0_tangent( float slope);
    void set_v1_tangent( float slope);

    // cubic
    void calc_cubic_coefficients( const keyframe_t& next);
    float evaluate_cubic( float t) const;

    float evaluate_derivative( float t) const;

    const boost::array<float,4>& cubic_polynomial() const { return coeffs_;}

    // constants
    static float max_slope();
    static float min_slope();

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "time", time_);
	ar & boost::serialization::make_nvp( "value", value_);
	ar & boost::serialization::make_nvp( "selected", selected_);

	ar & boost::serialization::make_nvp( "auto_v0", auto_v0_);
	ar & boost::serialization::make_nvp( "auto_v1", auto_v1_);
        ar & boost::serialization::make_nvp( "tangent_cont", tangent_cont_);
	ar & boost::serialization::make_nvp( "v0", v0_);
	ar & boost::serialization::make_nvp( "v1", v1_);
    }
    
    float time_;
    float value_;

    float v0_, v1_; // tangents
    auto_tangent_method auto_v0_, auto_v1_;
    bool tangent_cont_;

    mutable bool selected_;

    boost::array<float,4> coeffs_;
};

} // namespace
} // namespace

#endif

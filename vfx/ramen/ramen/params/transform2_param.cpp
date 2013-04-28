// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/transform2_param.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>

namespace ramen
{

transform2_param_t::transform2_param_t() {}

transform2_param_t::transform2_param_t( const std::string& name, const std::string& id) : composite_param_t( name)
{
    set_id( id);

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
    p->set_id( id + std::string( "_center"));
    p->set_default_value( Imath::V2f( 0.5, 0.5));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Pos"));
    p->set_id( id + std::string( "_pos"));
    p->set_default_value( Imath::V2f( 0, 0));
    add_param( p);

    p.reset( new float2_param_t( "Scale"));
    p->set_id( id + std::string( "_scale"));
    p->set_default_value( Imath::V2f( 1, 1));
    p->set_step( 0.05);
    p->set_proportional( true);
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Rotation"));
    q->set_id( id + std::string( "_rot"));
    q->set_default_value( 0);
    q->set_step( 0.5);
    add_param( q);
}

transform2_param_t::transform2_param_t( const transform2_param_t& other) : composite_param_t( other) {}

const param_t& transform2_param_t::center_param() const { return params()[0];}
param_t& transform2_param_t::center_param()             { return params()[0];}

const param_t& transform2_param_t::translate_param() const  { return params()[1];}
param_t& transform2_param_t::translate_param()              { return params()[1];}

const param_t& transform2_param_t::scale_param() const      { return params()[2];}
param_t& transform2_param_t::scale_param()                  { return params()[2];}

const param_t& transform2_param_t::rotate_param() const     { return params()[3];}
param_t& transform2_param_t::rotate_param()                 { return params()[3];}

boost::optional<Imath::M33d> transform2_param_t::matrix_at_time( float time, int subsample) const
{
    const double deg2rad = 0.01745329251;

    Imath::V2f s = get_value_at_time<Imath::V2f>( scale_param(), time);

    if( s.x == 0.0f && s.y == 0.0f)
        return boost::optional<Imath::M33d>();

    Imath::V2f c = get_absolute_value_at_time<Imath::V2f>( center_param(), time);
    Imath::V2f t = get_value_at_time<Imath::V2f>( translate_param(), time) / (float) subsample;
    float angle  = get_value_at_time<float>( rotate_param(), time);

    Imath::M33d m = Imath::M33d().setTranslation( -c) *
                    Imath::M33d().setScale( s) *
                    Imath::M33d().setRotation( angle * deg2rad) *
                    Imath::M33d().setTranslation( c + t);

    return boost::optional<Imath::M33d>( m);
}

boost::optional<Imath::M33d> transform2_param_t::xform_blur_matrix_at_time( float time, float t, int subsample) const
{
    const double deg2rad = 0.01745329251f;

    Imath::V2f s = get_value_at_time<Imath::V2f>( scale_param(), time);

    if( s.x == 0.0f && s.y == 0.0f)
        return boost::optional<Imath::M33d>();

    Imath::V2f c = get_absolute_value_at_time<Imath::V2f>( center_param(), time);
    Imath::V2f d = get_value_at_time<Imath::V2f>( translate_param(), time) / (float) subsample;
    float angle  = get_value_at_time<float>( rotate_param(), time);

    s = Imath::lerp( Imath::V2f( 1, 1), s, t);
    d = Imath::lerp( Imath::V2f( 0, 0), d, t);
    angle = Imath::lerp( 0.0f, angle, t);

    Imath::M33d m = Imath::M33d().setTranslation( -c) *
                    Imath::M33d().setScale( s) *
                    Imath::M33d().setRotation( angle * deg2rad) *
                    Imath::M33d().setTranslation( c + d);

    return boost::optional<Imath::M33d>( m);
}

void transform2_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

void transform2_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::transform2_param_t)

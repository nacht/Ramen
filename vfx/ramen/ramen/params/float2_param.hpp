// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLOAT2_PARAM_HPP
#define	RAMEN_FLOAT2_PARAM_HPP

#include<ramen/params/proportional_param.hpp>

#include<iostream>

#include<OpenEXR/ImathVec.h>

#include<QPointer>

class QrDoubleSpinBox;

namespace ramen
{

class float2_param_t : public proportional_param_t
{
    Q_OBJECT
    
public:

    float2_param_t();
    explicit float2_param_t( const std::string& name);

    void set_default_value( const Imath::V2f& x);

    void set_value( const Imath::V2f& x);
    void set_value_at_time( const Imath::V2f& x, float time);

    Imath::V2f derive( float time) const;
    Imath::V2f integrate( float time1, float time2) const;

protected:

    float2_param_t( const float2_param_t& other);
    void operator=( const float2_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new float2_param_t( *this);}

private:

    virtual std::string do_str() const { return string_for_type<Imath::V2f>();}

    virtual void do_create_tracks( anim::track_t *parent);
    virtual void do_evaluate( float time);

    virtual adobe::any_regular_t do_get_value_at_time( float time) const;

    virtual void do_domain_changed( const Imath::Box2i& new_domain);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    void calc_proportional_factors();

    QPointer<QrDoubleSpinBox> input0_, input1_;

private Q_SLOTS:

    virtual void value_changed( double value);
    virtual void spinbox_pressed();
    virtual void spinbox_moved( double value);
    virtual void spinbox_released();
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "V2f::Imath", Imath::V2f);

#endif

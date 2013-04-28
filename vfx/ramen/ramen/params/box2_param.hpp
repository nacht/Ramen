// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BOX_PARAM_HPP
#define	RAMEN_BOX_PARAM_HPP

#include<ramen/params/numeric_param.hpp>

#include<utility>
#include<iostream>

#include<QPointer>

class QrDoubleSpinBox;

namespace ramen
{

class box2_param_t : public numeric_param_t
{
    Q_OBJECT

public:

    box2_param_t();
    explicit box2_param_t( const std::string& name);

    void set_default_value( const Imath::Box2f& x);

    void set_value( const Imath::Box2f& x);
    void set_value_at_time( const Imath::Box2f& x, float time);

protected:

    box2_param_t( const box2_param_t& other);
    void operator=( const box2_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const;

private:

    virtual std::string do_str() const;

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

    QPointer<QrDoubleSpinBox> input0_, input1_, input2_, input3_;

private Q_SLOTS:

    virtual void value_changed( double value);
    virtual void spinbox_pressed();
    virtual void spinbox_moved( double value);
    virtual void spinbox_released();
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "Imath::Box2f", Imath::Box2f);

#endif	/* _box2_param_HPP */

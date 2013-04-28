// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLOAT_PARAM_HPP
#define	RAMEN_FLOAT_PARAM_HPP

#include<ramen/params/numeric_param.hpp>

#include<QPointer>

class QrDoubleSpinBox;

namespace ramen
{

class float_param_t : public numeric_param_t
{
    Q_OBJECT

public:

    float_param_t();
    explicit float_param_t( const std::string& name);

    void set_default_value( float x);

    void set_value( float x);
    void set_value_at_time( float x, float time);

    float derive( float time) const;
    float integrate( float time1, float time2) const;

protected:

    float_param_t( const float_param_t& other);
    void operator=( const float_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new float_param_t( *this);}

    virtual void do_create_tracks( anim::track_t *parent);
    virtual void do_evaluate( float time);

    virtual adobe::any_regular_t do_get_value_at_time( float time) const;

    virtual std::string do_str() const { return string_for_type<float>();}

    virtual void do_domain_changed( const Imath::Box2i& new_domain);

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QrDoubleSpinBox> input_;

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_moved( double value);
    void spinbox_released();
};

} // namespace

#endif	/* _FLOAT_PARAM_HPP */

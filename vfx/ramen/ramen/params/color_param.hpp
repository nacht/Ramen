// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLOR_PARAM_HPP
#define	RAMEN_COLOR_PARAM_HPP

#include<ramen/params/animated_param.hpp>

#include<OpenEXR/ImathColor.h>

#include<QPointer>

#include<ramen/ui/eyedropper_button.hpp>

class QrDoubleSpinBox;
class QrColorButton;

namespace ramen
{

class color_param_t : public animated_param_t
{
    Q_OBJECT

public:

    color_param_t();
    explicit color_param_t( const std::string& name);

    void set_default_value( const Imath::Color4f& x);

    void set_value( const Imath::Color4f& x);
    void set_value_at_time( const Imath::Color4f& x, float time);

    bool is_rgba() const	{ return is_rgba_;}
    void set_is_rgba( bool b)	{ is_rgba_ = b;}

protected:

    color_param_t( const color_param_t& other);
    void operator=( const color_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new color_param_t( *this);}

    virtual void do_create_tracks( anim::track_t *parent);

    virtual void do_evaluate( float time);

    virtual adobe::any_regular_t do_get_value_at_time( float time) const;

    virtual std::string do_str() const { return string_for_type<Imath::Color4f>();}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    bool is_rgba_;
    QPointer<QrDoubleSpinBox> input0_, input1_, input2_, input3_;
    QPointer<QrColorButton> button_;
    QPointer<ui::eyedropper_button_t> eyedropper_;

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_moved( double value);
    void spinbox_released();

    void eyedropper_color_picked( const QrColor& c);

    void color_button_pressed();
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "Color4f::Imath", Imath::Color4f);

#endif

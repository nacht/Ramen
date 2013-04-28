// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/box2_param.hpp>

#include<limits>

#include<boost/bind.hpp>
#include<boost/serialization/utility.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/serialization/ImathBox.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/app/document.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

box2_param_t::box2_param_t() : numeric_param_t() { private_init();}

box2_param_t::box2_param_t( const std::string& name) : numeric_param_t( name) { private_init();}

box2_param_t::box2_param_t( const box2_param_t& other) : numeric_param_t( other)
{
    input0_ = input1_ = input2_ = input3_ = 0;
}

param_t *box2_param_t::do_clone() const { return new box2_param_t( *this);}

std::string box2_param_t::do_str() const { return string_for_type<Imath::Box2f>();}

void box2_param_t::private_init()
{
    add_curves( 4);
    set_default_value( Imath::Box2f( Imath::V2f( 0, 0), Imath::V2f( 0, 0)));
}

void box2_param_t::set_default_value( const Imath::Box2f& x)
{
    value().assign( x);
    default_value().assign( x);
}

void box2_param_t::set_value( const Imath::Box2f& x)
{
    float time = 1.0f;

    if( composition())
	time = composition()->frame();

    set_value_at_time( x, time);
}

void box2_param_t::set_value_at_time( const Imath::Box2f& x, float time)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);

    if( !is_static() && ( param_set()->autokey() || !curves()[0].empty()))
	curves()[0].insert( time, x.min.x);

    if( !is_static() && ( param_set()->autokey() || !curves()[1].empty()))
	curves()[1].insert( time, x.min.y);

    if( !is_static() && ( param_set()->autokey() || !curves()[2].empty()))
	curves()[1].insert( time, x.max.x);

    if( !is_static() && ( param_set()->autokey() || !curves()[3].empty()))
	curves()[1].insert( time, x.max.y);

    evaluate( time);
    param_set()->param_changed( this);
}

void box2_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));

    std::auto_ptr<anim::track_t> tx( new anim::track_t( "Left", &( curves()[0])));
    tx->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tx);

    std::auto_ptr<anim::track_t> ty( new anim::track_t( "Top", &( curves()[1])));
    ty->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( ty);

    std::auto_ptr<anim::track_t> tz( new anim::track_t( "Right", &( curves()[2])));
    ty->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tz);

    std::auto_ptr<anim::track_t> tw( new anim::track_t( "Bottom", &( curves()[3])));
    ty->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tw);

    parent->add_child( t);
}

void box2_param_t::do_evaluate( float time)
{
    value() = do_get_value_at_time( time);
}

adobe::any_regular_t box2_param_t::do_get_value_at_time( float time) const
{
    Imath::Box2f v( get_value<Imath::Box2f>( *this));

    if( !curves()[0].empty())
	v.min.x = curves()[0].evaluate( time);

    if( !curves()[1].empty())
	v.min.y = curves()[1].evaluate( time);

    if( !curves()[0].empty())
	v.max.x = curves()[2].evaluate( time);

    if( !curves()[1].empty())
	v.max.y = curves()[3].evaluate( time);

    return adobe::any_regular_t( v);
}

QWidget *box2_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    input0_ = new QrDoubleSpinBox( top);
    input1_ = new QrDoubleSpinBox( top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    int xpos = ui::inspector_t::Instance().left_margin();

    float low = absolute_min();
    float high = absolute_max();

    Imath::Box2f v = relative_to_absolute( get_value<Imath::Box2f>( *this));
    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setRange( low, high);
    input0_->setStep( step());
    input0_->setValue( v.min.x);
    input0_->setEnabled( enabled());
    connect( input0_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setRange( low, high);
    input1_->setStep( step());
    input1_->setValue( v.min.y);
    input1_->setEnabled( enabled());
    connect( input1_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input2_->move( xpos, 0);
    input2_->resize( s.width(), s.height());
    input2_->setRange( low, high);
    input2_->setStep( step());
    input2_->setValue( v.max.x);
    input2_->setEnabled( enabled());
    connect( input2_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input2_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input2_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input2_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input3_->move( xpos, 0);
    input3_->resize( s.width(), s.height());
    input3_->setRange( low, high);
    input3_->setStep( step());
    input3_->setValue( v.max.y);
    input3_->setEnabled( enabled());
    connect( input3_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input3_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input3_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input3_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void box2_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);
        input2_->blockSignals( true);
        input3_->blockSignals( true);

        Imath::Box2f x = relative_to_absolute( get_value<Imath::Box2f>( *this));
        input0_->setValue( x.min.x);
        input1_->setValue( x.min.y);
        input2_->setValue( x.max.x);
        input3_->setValue( x.max.y);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
        input2_->blockSignals( false);
        input3_->blockSignals( false);
    }
}

void box2_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);
        input2_->setEnabled( e);
        input3_->setEnabled( e);
    }
}

void box2_param_t::value_changed( double value)
{
    param_set()->begin_edit();
    set_value( absolute_to_relative( Imath::Box2f(  Imath::V2f( input0_->value(), input1_->value()),
                                                    Imath::V2f( input2_->value(), input3_->value()))));
    param_set()->end_edit();
}

void box2_param_t::spinbox_pressed()
{
    param_set()->begin_edit();
}

void box2_param_t::spinbox_moved( double value)
{
    set_value( absolute_to_relative( Imath::Box2f( Imath::V2f( input0_->value(), input1_->value()),
                                                   Imath::V2f( input2_->value(), input3_->value()))));
    if( track_mouse())
        param_set()->notify_parent();

    ui::anim_editor_t::Instance().update();
}

void box2_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());
}

void box2_param_t::do_domain_changed( const Imath::Box2i& new_domain)
{
    if( input0_)
    {
	input0_->blockSignals( true);
	input1_->blockSignals( true);
	input2_->blockSignals( true);
	input3_->blockSignals( true);

	input0_->setRange( absolute_min(), absolute_max());
	input1_->setRange( absolute_min(), absolute_max());
	input2_->setRange( absolute_min(), absolute_max());
	input3_->setRange( absolute_min(), absolute_max());

	input0_->blockSignals( false);
	input1_->blockSignals( false);
	input2_->blockSignals( false);
	input3_->blockSignals( false);

	update_widgets();
    }
}

void box2_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

void box2_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<Imath::Box2f>();

} // namespace

BOOST_CLASS_EXPORT( ramen::box2_param_t)

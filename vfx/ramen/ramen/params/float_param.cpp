// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/float_param.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

float_param_t::float_param_t() : numeric_param_t() { private_init();}
float_param_t::float_param_t( const std::string& name) : numeric_param_t( name) { private_init();}

float_param_t::float_param_t( const float_param_t& other) : numeric_param_t( other)
{ 
    input_ = 0;
}

void float_param_t::private_init()
{
    add_curve();
    set_default_value( 0);
}

void float_param_t::set_default_value( float x)
{
    value().assign( x);
    default_value().assign( x);
}

void float_param_t::set_value( float x)
{
    float time = 1.0f;

    if( param_set() && composition())
	time = composition()->frame();

    set_value_at_time( x, time);
}

void float_param_t::set_value_at_time( float x, float time)
{
    if( can_undo())
	param_set()->add_command( this);

    if( !is_static() && ( param_set()->autokey() || !curves()[0].empty()))
    {
	curves()[0].insert( time, x);
	evaluate( time);
	param_set()->param_changed( this);
    }
    else
    {
	value().assign( x);
	param_set()->param_changed( this);
    }
}

float float_param_t::derive( float time) const
{
    if( !curves()[0].empty())
	return curves()[0].derive( time);
    else
	return 0;
}

float float_param_t::integrate( float time1, float time2) const
{
    if( !curves()[0].empty())
	return curves()[0].integrate( time1, time2);
    else
    {
	float v = get_value<float>( *this);
	return (time2 - time1) * v;
    }
}

void float_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name(), &( curves()[0])));
    t->changed.connect( boost::bind( &numeric_param_t::anim_curve_changed, this, _1));
    parent->add_child( t);
}

void float_param_t::do_evaluate( float time)
{
    value() = do_get_value_at_time( time);
}

adobe::any_regular_t float_param_t::do_get_value_at_time( float time) const
{
    if( !curves()[0].empty())
        return adobe::any_regular_t( curves()[0].evaluate( time));

    return value();
}

QWidget *float_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    input_ = new QrDoubleSpinBox( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    float low = absolute_min();
    float high = absolute_max();

    input_->setRange( low, high);
    input_->setValue( relative_to_absolute( get_value<float>( *this)));
    input_->setStep( step());
    input_->move( ui::inspector_t::Instance().left_margin(), 0);
    input_->resize( s.width(), s.height());
    input_->setEnabled( enabled());
    connect( input_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void float_param_t::value_changed( double value)
{
    param_set()->begin_edit();
    set_value( absolute_to_relative( round( value)));
    param_set()->end_edit();
}

void float_param_t::spinbox_pressed()
{
    param_set()->begin_edit();
}

void float_param_t::spinbox_moved( double value)
{
    set_value( absolute_to_relative( round( value)));

    if( track_mouse())
        param_set()->notify_parent();

    ui::anim_editor_t::Instance().update();
}

void float_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());
}

void float_param_t::do_update_widgets()
{
    if( input_)
    {
        input_->blockSignals( true);
        input_->setValue( relative_to_absolute( get_value<float>( *this)));
        input_->blockSignals( false);
    }
}

void float_param_t::do_enable_widgets( bool e)
{
    if( input_)
        input_->setEnabled( e);
}

void float_param_t::do_domain_changed( const Imath::Box2i& new_domain)
{
    if( input_)
    {
	input_->blockSignals( true);
	input_->setRange( absolute_min(), absolute_max());
	input_->blockSignals( false);
	update_widgets();
    }
}

void float_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

void float_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::float_param_t)

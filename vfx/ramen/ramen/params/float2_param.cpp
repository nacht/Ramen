// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/float2_param.hpp>

#include<limits>

#include<boost/bind.hpp>
#include<boost/serialization/utility.hpp>

#include<ramen/ImathExt/serialization/ImathVec.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>

namespace ramen
{

float2_param_t::float2_param_t() : proportional_param_t() { private_init();}

float2_param_t::float2_param_t( const std::string& name) : proportional_param_t( name) { private_init();}

float2_param_t::float2_param_t( const float2_param_t& other) : proportional_param_t( other)
{
    input0_ = input1_ = 0;
}

void float2_param_t::private_init()
{
    add_curves( 2);
    set_default_value( Imath::V2f( 0, 0));
}

void float2_param_t::set_default_value( const Imath::V2f& x)
{
    value().assign( x);
    default_value().assign( x);
}

void float2_param_t::set_value( const Imath::V2f& x)
{
    float time = 1.0f;

    if( composition())
	time = composition()->frame();

    set_value_at_time( x, time);
}

void float2_param_t::set_value_at_time( const Imath::V2f& x, float time)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);

    if( !is_static() && ( param_set()->autokey() || !curves()[0].empty()))
	curves()[0].insert( time, x.x);

    if( !is_static() && ( param_set()->autokey() || !curves()[1].empty()))
	curves()[1].insert( time, x.y);

    evaluate( time);
    param_set()->param_changed( this);
}

Imath::V2f float2_param_t::derive( float time) const
{
    Imath::V2f result( 0.0f, 0.0f);

    if( !curves()[0].empty())
	result.x    = curves()[0].derive( time);

    if( !curves()[1].empty())
	result.y   = curves()[1].derive( time);

    return result;
}

Imath::V2f float2_param_t::integrate( float time1, float time2) const
{
    Imath::V2f result = get_value<Imath::V2f>( *this);
    result.x  = (time2 - time1) * result.x;
    result.y  = (time2 - time1) * result.y;

    if( !curves()[0].empty())
	result.x    = curves()[0].integrate( time1, time2);

    if( !curves()[1].empty())
	result.y   = curves()[1].integrate( time1, time2);

    return result;
}

void float2_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));

    std::auto_ptr<anim::track_t> tx( new anim::track_t( "X", &( curves()[0])));
    tx->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tx);
    
    std::auto_ptr<anim::track_t> ty( new anim::track_t( "Y", &( curves()[1])));
    ty->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( ty);

    parent->add_child( t);
}

void float2_param_t::do_evaluate( float time)
{
    value() = do_get_value_at_time( time);
}

adobe::any_regular_t float2_param_t::do_get_value_at_time( float time) const
{
    Imath::V2f v( get_value<Imath::V2f>( *this));

    if( !curves()[0].empty())
	v.x = curves()[0].evaluate( time);

    if( !curves()[1].empty())
	v.y = curves()[1].evaluate( time);

    return adobe::any_regular_t( v);
}

QWidget *float2_param_t::do_create_widgets()
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

    Imath::V2f v = relative_to_absolute( get_value<Imath::V2f>( *this));
    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setRange( low, high);
    input0_->setStep( step());
    input0_->setValue( v.x);
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
    input1_->setValue( v.y);
    input1_->setEnabled( enabled());
    connect( input1_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 2);

    if( proportional())
    {
        create_proportional_button( top);

        if( prop_button_)
            prop_button_->move( xpos, 0);
    }

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void float2_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);

        Imath::V2f pair = relative_to_absolute( get_value<Imath::V2f>( *this));
        input0_->setValue( pair.x);
        input1_->setValue( pair.y);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
    }
}

void float2_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);

        if( prop_button_)
            prop_button_->setEnabled( e);
    }
}

void float2_param_t::calc_proportional_factors()
{
    proportional_factor.x = 1;
    proportional_factor.y = 1;

    Imath::V2f v = get_value<Imath::V2f>( *this);

    if( v.x != 0 && v.y != 0)
    {
        if( sender() == input0_)
            proportional_factor.y = v.y / v.x;
        else
            proportional_factor.x = v.x / v.y;
    }
}

void float2_param_t::value_changed( double value)
{
    param_set()->begin_edit();

    if( proportional_checked())
    {
        calc_proportional_factors();
        Imath::V2f v = get_value<Imath::V2f>( *this);
        
        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
        }
        else
        {
            float inc = value - v.y;
            v.y = value;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
        }        

        set_value( absolute_to_relative( Imath::V2f( round ( v.x), round( v.y))));
        update_widgets();
    }
    else
        set_value( absolute_to_relative( Imath::V2f( round (input0_->value()), round( input1_->value()))));

    param_set()->end_edit();
}

void float2_param_t::spinbox_pressed()
{
    param_set()->begin_edit();

    if( proportional_checked())
        calc_proportional_factors();
}

void float2_param_t::spinbox_moved( double value)
{
    if( proportional_checked())
    {
        Imath::V2f v = get_value<Imath::V2f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
        }
        else
        {
            float inc = value - v.y;
            v.y = value;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
        }

        set_value( absolute_to_relative( Imath::V2f( round ( v.x), round( v.y))));
        update_widgets();
    }
    else
        set_value( absolute_to_relative( Imath::V2f( round (input0_->value()), round( input1_->value()))));

    if( track_mouse())
        param_set()->notify_parent();

    ui::anim_editor_t::Instance().update();
}

void float2_param_t::spinbox_released() { param_set()->end_edit( !track_mouse());}

void float2_param_t::do_domain_changed( const Imath::Box2i& new_domain)
{
    if( input0_)
    {
	input0_->blockSignals( true);
	input1_->blockSignals( true);

	input0_->setRange( absolute_min(), absolute_max());
	input1_->setRange( absolute_min(), absolute_max());

	input0_->blockSignals( false);
	input1_->blockSignals( false);

	update_widgets();
    }
}

void float2_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( proportional_param_t);
}

void float2_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( proportional_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<Imath::V2f>();

} // namespace

BOOST_CLASS_EXPORT( ramen::float2_param_t)

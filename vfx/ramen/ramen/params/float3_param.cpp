// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/float3_param.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<ramen/ImathExt/serialization/ImathVec.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/app/document.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

float3_param_t::float3_param_t() : proportional_param_t() { private_init();}
float3_param_t::float3_param_t( const std::string& name) : proportional_param_t( name) { private_init();}

float3_param_t::float3_param_t( const float3_param_t& other) : proportional_param_t( other)
{
    input0_ = input1_ = input2_ = 0;
}

void float3_param_t::private_init()
{
    add_curves( 3);
    set_default_value( Imath::V3f( 0, 0, 0));
}

void float3_param_t::set_default_value( const Imath::V3f& x)
{
    value().assign( x);
    default_value().assign( x);
}

void float3_param_t::set_value( const Imath::V3f& x)
{
    float time = 1.0f;

    if( composition())
	time = composition()->frame();

    set_value_at_time( x, time);
}

void float3_param_t::set_value_at_time( const Imath::V3f& x, float time)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);

    if( !is_static() && ( param_set()->autokey() || !curves()[0].empty()))
	curves()[0].insert( time, x.x);

    if( !is_static() && ( param_set()->autokey() || !curves()[1].empty()))
	curves()[1].insert( time, x.y);

    if( !is_static() && ( param_set()->autokey() || !curves()[2].empty()))
	curves()[2].insert( time, x.z);

    evaluate( time);
    param_set()->param_changed( this);
}

Imath::V3f float3_param_t::derive( float time) const
{
    Imath::V3f result( 0.0f, 0.0f, 0.0f);

    if( !curves()[0].empty())
	result.x = curves()[0].derive( time);

    if( !curves()[1].empty())
	result.y = curves()[1].derive( time);

    if( !curves()[2].empty())
	result.z = curves()[2].derive( time);

    return result;
}

Imath::V3f float3_param_t::integrate( float time1, float time2) const
{
    Imath::V3f result = get_value<Imath::V3f>( *this);
    result.x  = (time2 - time1) * result.x;
    result.y = (time2 - time1) * result.y;
    result.z  = (time2 - time1) * result.z;

    if( !curves()[0].empty())
	result.x    = curves()[0].integrate( time1, time2);

    if( !curves()[1].empty())
	result.y = curves()[1].integrate( time1, time2);

    if( !curves()[2].empty())
	result.z = curves()[2].integrate( time1, time2);

    return result;
}

void float3_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));

    std::auto_ptr<anim::track_t> tx( new anim::track_t( "X", &( curves()[0])));
    tx->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tx);

    std::auto_ptr<anim::track_t> ty( new anim::track_t( "Y", &( curves()[1])));
    ty->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( ty);

    std::auto_ptr<anim::track_t> tz( new anim::track_t( "Z", &( curves()[2])));
    tz->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tz);

    parent->add_child( t);
}

void float3_param_t::do_evaluate( float time)
{
    value() = do_get_value_at_time( time);
}

adobe::any_regular_t float3_param_t::do_get_value_at_time( float time) const
{
    Imath::V3f v( get_value<Imath::V3f>( *this));

    if( !curves()[0].empty())
	v.x = curves()[0].evaluate( time);

    if( !curves()[1].empty())
	v.y = curves()[1].evaluate( time);

    if( !curves()[2].empty())
	v.z = curves()[2].evaluate( time);

    return adobe::any_regular_t( v);
}

QWidget *float3_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    input0_ = new QrDoubleSpinBox( top);
    input1_ = new QrDoubleSpinBox( top);
    input2_ = new QrDoubleSpinBox( top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    Imath::V3f triple = get_value<Imath::V3f>( *this);

    int xpos = ui::inspector_t::Instance().left_margin();

    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setRange( get_min(), get_max());
    input0_->setStep( step());
    input0_->setValue( triple.x);
    input0_->setEnabled( enabled());
    connect( input0_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setRange( get_min(), get_max());
    input1_->setStep( step());
    input1_->setValue( triple.y);
    input1_->setEnabled( enabled());
    connect( input1_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input2_->move( xpos, 0);
    input2_->resize( s.width(), s.height());
    input2_->setRange( get_min(), get_max());
    input2_->setStep( step());
    input2_->setValue( triple.z);
    input2_->setEnabled( enabled());
    connect( input2_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input2_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input2_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input2_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
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

void float3_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);
        input2_->blockSignals( true);

        Imath::V3f triple = get_value<Imath::V3f>( *this);
        input0_->setValue( triple.x);
        input1_->setValue( triple.y);
        input2_->setValue( triple.z);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
        input2_->blockSignals( false);
    }
}

void float3_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);
        input2_->setEnabled( e);

        if( prop_button_)
            prop_button_->setEnabled( e);
    }
}

void float3_param_t::calc_proportional_factors()
{
    proportional_factor.x = 1;
    proportional_factor.y = 1;
    proportional_factor.z = 1;
    Imath::V3f v = get_value<Imath::V3f>( *this);

    if( sender() == input0_)
    {
        if( v.x != 0)
        {
            if( v.y) proportional_factor.y = v.y / v.x;
            if( v.z) proportional_factor.z = v.z / v.x;
        }
    }

    if( sender() == input1_)
    {
        if( v.y != 0)
        {
            if( v.x) proportional_factor.y = v.x / v.y;
            if( v.z) proportional_factor.z = v.z / v.y;
        }
    }

    if( sender() == input2_)
    {
        if( v.z != 0)
        {
            if( v.x) proportional_factor.y = v.x / v.z;
            if( v.y) proportional_factor.z = v.y / v.z;
        }
    }
}

void float3_param_t::value_changed( double value)
{
    param_set()->begin_edit();

    if( proportional_checked())
    {
        calc_proportional_factors();
        Imath::V3f v = get_value<Imath::V3f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
            v.z = clamp( v.z + ( inc * proportional_factor.z));
        }

        if( sender() == input1_)
        {
            float inc = value - v.y;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
            v.y = value;
            v.z = clamp( v.z + ( inc * proportional_factor.z));
        }

        if( sender() == input1_)
        {
            float inc = value - v.z;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
            v.y = clamp( v.y + ( inc * proportional_factor.y));
            v.z = value;
        }

        set_value( Imath::V3f( round ( v.x), round( v.y), round( v.z)));
        update_widgets();
    }
    else
        set_value( Imath::V3f( round( input0_->value()), round( input1_->value()), round( input2_->value())));

    param_set()->end_edit();
}

void float3_param_t::spinbox_pressed()
{
    param_set()->begin_edit();

    if( proportional_checked())
        calc_proportional_factors();
}

void float3_param_t::spinbox_moved( double value)
{
    if( proportional_checked())
    {
        Imath::V3f v = get_value<Imath::V3f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
            v.z = clamp( v.z + ( inc * proportional_factor.z));
        }

        if( sender() == input1_)
        {
            float inc = value - v.y;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
            v.y = value;
            v.z = clamp( v.z + ( inc * proportional_factor.z));
        }

        if( sender() == input1_)
        {
            float inc = value - v.z;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
            v.y = clamp( v.y + ( inc * proportional_factor.y));
            v.z = value;
        }

        set_value( Imath::V3f( round ( v.x), round( v.y), round( v.z)));
        update_widgets();
    }
    else
        set_value( Imath::V3f( round( input0_->value()), round( input1_->value()), round( input2_->value())));

    if( track_mouse())
        param_set()->notify_parent();

    ui::anim_editor_t::Instance().update();
}

void float3_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());
}

void float3_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( proportional_param_t);
}

void float3_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( proportional_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<Imath::V3f>();

} // namespace

BOOST_CLASS_EXPORT( ramen::float3_param_t)

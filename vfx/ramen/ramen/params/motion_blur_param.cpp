// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/motion_blur_param.hpp>

#include<OpenEXR/ImathFun.h>

#include<QComboBox>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>
#include<ramen/Qr/QrSpinBox.hpp>

#include<ramen/params/param_set.hpp>

namespace ramen
{

bool motion_blur_info_t::operator==( const motion_blur_info_t& other) const
{
    return samples == other.samples &&
            shutter == other.shutter &&
            shutter_offset == other.shutter_offset &&
            filter == other.filter;
}

bool motion_blur_info_t::operator!=( const motion_blur_info_t& other) const { return !( *this == other);}

std::ostream& operator<<( std::ostream& os, const motion_blur_info_t& mb)
{
    return os << mb.samples << ", " << mb.shutter << ", " << mb.shutter_offset << ", " << mb.filter;
}

motion_blur_param_t::loop_data_t::loop_data_t( float time, int samples, float shutter, float offset, motion_blur_info_t::filter_type f)
{
    if( shutter != 0)
        num_samples = 2 * samples + 1;
    else
        num_samples = 1;

    start_time  = time - ( shutter / 2.0f) + ( offset * shutter / 2.0f);
    end_time    = time + ( shutter / 2.0f) + ( offset * shutter / 2.0f);
    center_time = ( end_time + start_time) / 2.0f;
    time_step   = ( end_time - start_time) / num_samples;

    filter = f;
}

float motion_blur_param_t::loop_data_t::weight_for_time( float t) const
{
    switch( filter)
    {
    case motion_blur_info_t::box_filter:
        return 1.0f;
    break;

    case motion_blur_info_t::triangle_filter:
    {
        float x = Imath::abs( t - center_time);
        float b = (end_time - center_time) + ( time_step / 2.0);
        return Imath::lerpfactor( x, b, 0.0f);
    }
    break;

    case motion_blur_info_t::cubic_filter:
    {
        float x = Imath::abs( t - center_time);
        float b = (end_time - center_time) + ( time_step / 2.0);
        float t = Imath::lerpfactor( x, 0.0f, b);
        return ( 2.0f * t * t * t) - ( 3.0f * t * t) + 1.0f;
    }
    break;

    default:
        assert( 0 && "Motion blur, unknown filter type");
        return 1.0f; // to make MSCV happy
    }
}

motion_blur_param_t::motion_blur_param_t() : static_param_t()
{
    value().assign( motion_blur_info_t());
    default_value().assign( motion_blur_info_t());
    samples_ = 0;
    shutter_ = 0;
    shutter_off_ = 0;
    filter_ = 0;
}

motion_blur_param_t::motion_blur_param_t( const std::string& name) : static_param_t( name)
{
    value().assign( motion_blur_info_t());
    default_value().assign( motion_blur_info_t());
    samples_ = 0;
    shutter_ = 0;
    shutter_off_ = 0;
    filter_ = 0;
}

motion_blur_param_t::motion_blur_param_t( const motion_blur_param_t& other) : static_param_t( other)
{
    samples_ = 0;
    shutter_ = 0;
    shutter_off_ = 0;
    filter_ = 0;
}

motion_blur_param_t::loop_data_t motion_blur_param_t::loop_data( float time, int extra_samples, float shutter_factor) const
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    return loop_data_t( time, info.samples + extra_samples, info.shutter * shutter_factor, info.shutter_offset, info.filter);
}

param_t *motion_blur_param_t::do_clone() const { return new motion_blur_param_t( *this);}

std::string motion_blur_param_t::do_str() const { return string_for_type<motion_blur_info_t>();}

QWidget *motion_blur_param_t::do_create_widgets()
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));

    QWidget *top = new QWidget();

    samples_ = new QrSpinBox( top);
    QSize s = samples_->sizeHint();

    int h = 0;

    QrLabel *label = new QrLabel( top);
    label->move( 0, h);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Motion Blur Samples");

    samples_->setRange( 1, 64);
    samples_->setValue( info.samples);
    samples_->move( ui::inspector_t::Instance().left_margin(), h);
    samples_->resize( s.width(), s.height());
    samples_->setEnabled( enabled());
    connect( samples_, SIGNAL( valueChanged( int)), this, SLOT( samples_changed( int)));
    h += s.height() + 5;

    label = new QrLabel( top);
    label->move( 0, h);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Shutter");

    shutter_ = new QrDoubleSpinBox( top);
    shutter_->setRange( 0, 1);
    shutter_->setValue( info.shutter);
    shutter_->setStep( 0.05);
    shutter_->move( ui::inspector_t::Instance().left_margin(), h);
    shutter_->resize( s.width(), s.height());
    shutter_->setEnabled( enabled());
    connect( shutter_, SIGNAL( valueChanged( double)), this, SLOT( shutter_changed( double)));
    h += s.height() + 5;

    label = new QrLabel( top);
    label->move( 0, h);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Shutter Offset");

    shutter_off_ = new QrDoubleSpinBox( top);
    shutter_off_->setRange( -1, 1);
    shutter_off_->setValue( info.shutter_offset);
    shutter_off_->setStep( 0.05);
    shutter_off_->move( ui::inspector_t::Instance().left_margin(), h);
    shutter_off_->resize( s.width(), s.height());
    shutter_off_->setEnabled( enabled());
    connect( shutter_off_, SIGNAL( valueChanged( double)), this, SLOT( shutter_changed( double)));
    h += s.height() + 5;

    label = new QrLabel( top);
    label->move( 0, h);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Filter");

    filter_ = new QComboBox( top);
    s = filter_->sizeHint();
    filter_->setFocusPolicy( Qt::NoFocus);
    filter_->addItem( "Box");
    filter_->addItem( "Triangle");
    filter_->addItem( "Cubic");
    filter_->setCurrentIndex( (int) info.filter);
    filter_->move( ui::inspector_t::Instance().left_margin(), h);
    filter_->resize( s.width(), s.height());
    filter_->setEnabled( enabled());
    connect( filter_, SIGNAL( currentIndexChanged( int)), this, SLOT( filter_changed( int)));
    h += s.height() + 5;

    top->setMinimumSize( ui::inspector_t::Instance().width(), h);
    top->setMaximumSize( ui::inspector_t::Instance().width(), h);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void motion_blur_param_t::do_update_widgets()
{
    if( samples_)
    {
        motion_blur_info_t info( get_value<motion_blur_info_t>( *this));

        samples_->blockSignals( true);
        shutter_->blockSignals( true);
        shutter_off_->blockSignals( true);
        filter_->blockSignals( true);

        samples_->setValue( info.samples);
        shutter_->setValue( info.shutter);
        shutter_off_->setValue( info.shutter_offset);
        filter_->setCurrentIndex( (int) info.filter);

        samples_->blockSignals( false);
        shutter_->blockSignals( false);
        shutter_off_->blockSignals( false);
        filter_->blockSignals( false);
    }
}

void motion_blur_param_t::do_enable_widgets( bool e)
{
    if( samples_)
    {
        samples_->setEnabled( e);
        shutter_->setEnabled( e);
        shutter_off_->setEnabled( e);
        filter_->setEnabled( e);
    }
}

void motion_blur_param_t::samples_changed( int v)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.samples = v;

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void motion_blur_param_t::shutter_changed( double v)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.shutter = shutter_->value();
    info.shutter_offset = shutter_off_->value();

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void motion_blur_param_t::filter_changed( int index)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.filter = (motion_blur_info_t::filter_type) index;

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void motion_blur_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

void motion_blur_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<motion_blur_info_t>();

} // namespace

BOOST_CLASS_EXPORT( ramen::motion_blur_param_t)

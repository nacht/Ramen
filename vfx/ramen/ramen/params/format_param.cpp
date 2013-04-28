// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/format_param.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrSpinBox.hpp>

namespace ramen
{

format_param_t::format_param_t() : static_param_t() {}

format_param_t::format_param_t( const std::string& name) : static_param_t( name) {}

format_param_t::format_param_t( const format_param_t& other) : static_param_t( other)
{
    width_input_ = 0;
    height_input_ = 0;
}

void format_param_t::set_value( format_t format)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( format);
    param_set()->param_changed( this);
}

param_t *format_param_t::do_clone() const { return new format_param_t( *this);}

void format_param_t::do_init()
{
    if( composition())
	value().assign( composition()->default_format());
    else
	value().assign( format_t());

    default_value() = value();
}

std::string format_param_t::do_str() const { return string_for_type<format_t>();}

QWidget *format_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    width_input_ = new QrSpinBox( top);
    width_input_->setRange( 16, 8192);
    width_input_->setMouseTracking( false);
    width_input_->setValue( value().cast<format_t>().width);

    height_input_ = new QrSpinBox( top);
    height_input_->setRange( 16, 8192);
    height_input_->setValue( value().cast<format_t>().height);
    height_input_->setMouseTracking( false);
    height_input_->setEnabled( enabled());

    QSize s = width_input_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    width_input_->move( ui::inspector_t::Instance().left_margin(), 0);
    width_input_->resize( s.width(), s.height());
    width_input_->setValue( get_value<format_t>( *this).width);
    width_input_->setEnabled( enabled());
    connect( width_input_, SIGNAL( valueChanged( int)), this, SLOT( widget_changed( int)));

    height_input_->move( ui::inspector_t::Instance().left_margin() + s.width() + 5, 0);
    height_input_->resize( s.width(), s.height());
    height_input_->setValue( get_value<format_t>( *this).height);
    connect( height_input_, SIGNAL( valueChanged( int)), this, SLOT( widget_changed( int)));

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void format_param_t::do_update_widgets()
{
    if( width_input_)
    {
        width_input_->blockSignals( true);
        height_input_->blockSignals( true);

        width_input_->setValue( get_value<format_t>( *this).width);
        height_input_->setValue( get_value<format_t>( *this).height);

        width_input_->blockSignals( false);
        height_input_->blockSignals( false);
    }
}

void format_param_t::do_enable_widgets( bool e)
{
    if( width_input_)
    {
        width_input_->setEnabled( e);
        height_input_->setEnabled( e);
    }
}

void format_param_t::widget_changed( int v)
{
    int w = width_input_->value();
    int h = height_input_->value();
    format_t new_format( w, h);

    param_set()->begin_edit();
    set_value( new_format);
    param_set()->end_edit();
}

void format_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

void format_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<format_t>();

} // namespace

BOOST_CLASS_EXPORT( ramen::format_param_t)

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/bool_param.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{

bool_param_t::bool_param_t() : static_param_t()
{
    set_default_value( false);
}

bool_param_t::bool_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( false);
}

bool_param_t::bool_param_t( const bool_param_t& other) : static_param_t( other)
{
    button_ = 0;
}

void bool_param_t::set_default_value( bool x)
{
    value().assign( x);
    default_value().assign( x);
}

void bool_param_t::set_value( bool x)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);
    param_set()->param_changed( this);
}

param_t *bool_param_t::do_clone() const { return new bool_param_t( *this);}

QWidget *bool_param_t::do_create_widgets()
{
    QWidget *w = new QWidget();

    button_ = new QCheckBox( w);
    button_->setText( name().c_str());
    QSize s = button_->sizeHint();

    w->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    w->setMaximumSize( ui::inspector_t::Instance().width(), s.height());

    button_->move( ui::inspector_t::Instance().left_margin(), 0);
    button_->resize( s.width(), s.height());

    button_->setChecked( get_value<bool>( *this));
    button_->setEnabled( enabled());

    connect( button_, SIGNAL( stateChanged( int)), this, SLOT( button_checked( int)));

    return w;
}

void bool_param_t::do_update_widgets()
{
    if( button_)
    {
        button_->blockSignals( true);
        button_->setChecked( get_value<bool>( *this));
        button_->blockSignals( false);
    }
}

void bool_param_t::do_enable_widgets( bool e)
{
    if( button_)
        button_->setEnabled( e);
}

void bool_param_t::button_checked( int state)
{
    param_set()->begin_edit();
    set_value( state);
    param_set()->end_edit();
}

void bool_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

void bool_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::bool_param_t)

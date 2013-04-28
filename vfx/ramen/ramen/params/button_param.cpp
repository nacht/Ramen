// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/button_param.hpp>

#include<QPushButton>

#include<ramen/params/param_set.hpp>

namespace ramen
{

button_param_t::button_param_t() : param_t() { set_static( true);}

button_param_t::button_param_t( const std::string& name) : param_t( name) { set_static( true);}

button_param_t::button_param_t( const button_param_t& other) : param_t( other)
{
    button_ = 0;
}

QWidget *button_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();

    button_ = new QPushButton( top);
    button_->setText( name().c_str());
    connect( button_, SIGNAL( pressed()), this, SLOT( button_pressed()));

    QSize s = button_->sizeHint();

    button_->move( ui::inspector_t::Instance().left_margin(), 0);
    button_->resize( s.width(), s.height());
    button_->setEnabled( enabled());
    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void button_param_t::do_enable_widgets( bool e)
{
    if( button_)
        button_->setEnabled( e);
}

void button_param_t::button_pressed() { param_set()->param_changed( this);}

void button_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

void button_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::button_param_t)
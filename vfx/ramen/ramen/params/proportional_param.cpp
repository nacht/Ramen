// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/proportional_param.hpp>

namespace ramen
{

Imath::V3f proportional_param_t::proportional_factor = Imath::V3f( 0, 0, 0);

proportional_param_t::proportional_param_t() : numeric_param_t() {}
proportional_param_t::proportional_param_t( const std::string& name) : numeric_param_t(name) {}

proportional_param_t::proportional_param_t( const proportional_param_t& other) : numeric_param_t( other)
{
    prop_button_ = 0;
}

void proportional_param_t::set_proportional( bool p)
{
    if( p)
	set_flags( flags() | proportional_bit);
    else
	set_flags( flags() & ~dirty_bit);
}

bool proportional_param_t::proportional_checked() const
{
    if( prop_button_)
        return prop_button_->isChecked();

    return false;
}

void proportional_param_t::create_proportional_button( QWidget *parent)
{
    prop_button_ = new QrSmallToolButton( parent);
    prop_button_->setIcon( QIcon( ":icons/link.png"));
    prop_button_->setCheckable( true);
    prop_button_->setChecked( true);
    prop_button_->setEnabled( enabled());

    #ifdef __APPLE__
	prop_button_->resize( 20, 20);
    #endif
}

void proportional_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

void proportional_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( numeric_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::proportional_param_t)

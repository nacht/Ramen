// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/group_param.hpp>

#include<QGroupBox>

namespace ramen
{

group_param_t::group_param_t() : composite_param_t() {}

group_param_t::group_param_t( const std::string& name) : composite_param_t( name) {}

group_param_t::group_param_t( const group_param_t& other) : composite_param_t( other)
{
    group_ = 0;
}

QWidget *group_param_t::do_create_widgets()
{
    group_ = new QGroupBox();
    group_->setTitle( name().c_str());
    group_->setAlignment( Qt::AlignLeft);
    create_widgets_inside_widget( group_);
    return group_;
}

void group_param_t::do_enable_widgets( bool e)
{
    if( group_)
        group_->setEnabled( e);
}

void group_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

void group_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::group_param_t)
